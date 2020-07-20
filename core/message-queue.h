#pragma once

#include "imessage-queue-events.h"

#include <queue>
#include <cmath>
#include <cassert>
#include <atomic>
#include <algorithm>
#include <condition_variable>

namespace mq {
using Identifier = std::chrono::time_point<std::chrono::steady_clock>;

const auto MessageCompare = [](const auto& lhs, const auto& rhs) {
    return lhs > rhs;
};

enum class RetCodes{
    OK = 0,
    HWM = -1,
    NO_SPACE = -2,
    STOPPED = -3
};

class IMessageQueueEvents;

/**
 * @brief Класс очереди сообщений
 * @details MessageQueue - блокирующая очередь с приоритетами, заданного размера.
 * Обеспечивает обмена сообщениями между несколькими потоками «писателей» и «читателей».
 * Класс MessageQueue поддерживает механизм HWM/LWM (по умолчанию 10%/90% соответственно)
 * Содержит блокирующий вызов get.
 */
template<typename Type, class Sequence = std::vector<std::pair<Type,Identifier>>>
class MessageQueue final
{
public:
    MessageQueue() = delete;
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue(const MessageQueue &&) = delete;
    MessageQueue(unsigned int size, unsigned short lwm = 10, unsigned short hwm = 90) :
        mSize(size),
        mCurrentSize(0),
        mLwm(conversionWaterMark(lwm, false)),
        mHwm(conversionWaterMark(hwm, true)),
        mMutex(),
        mSubscribeMutex(),
        mCv(),
        mState(Events::on_start),
        mSubscribers(),
        mQueue(MessageCompare)
    {
        static_assert(std::is_same<std::pair<Type, Identifier>,
                      typename Sequence::value_type>::value,
                      "Type for MessageQueue must be the same as the container");
        assert (fixedSize() > 2 && "Size MessageQueue less than 3");
    }

    MessageQueue& operator=(const MessageQueue &) = delete;
    MessageQueue& operator=(MessageQueue &&) = delete;
    ~MessageQueue() = default;

    /**
     * @brief put
     * @details Добавляеет сообщение в очередь
     * с присвоением временного монотонного идентификатора.
     * Учитывает события и коды возврата.
     * Уведомляет "читателей" о поступлении первого сообщения
     * Функция потокобезопасна.
     */
    RetCodes put(const Type &message) {
        if (currentSize() < mHwm) {
            std::lock_guard<std::mutex> guard(mMutex);
            mQueue.emplace(std::make_pair(std::move(message),
                                          std::chrono::steady_clock::now()));
            mCurrentSize = mQueue.size();
            if(currentSize() == 1)
                mCv.notify_all();
        } else {
            notifySubscribers(Events::on_hwm);
            return RetCodes::HWM;
        }
        if(mState == Events::on_stop) {
            return RetCodes::STOPPED;
        }
        return RetCodes::OK;
    }

    /**
     * @brief get
     * @details Блокирующая функция, достаёт сообщение из очереди.
     * Уведомляет "писателей" при достижении порога lwm
     * Учитывает события и коды возврата.
     * Функция потокобезопасна.
     */
    RetCodes get(Type &type) {
        if(mState == Events::on_stop)
            return RetCodes::STOPPED;
        std::unique_lock<std::mutex> lk(mMutex);
        mCv.wait(lk, [this] {
            return (!mQueue.empty() || mState == Events::on_stop);
        });
        if (mQueue.empty())
            return RetCodes::STOPPED;
        type = mQueue.top().first;
        mQueue.pop();
        mCurrentSize = mQueue.size();
        if(currentSize() <= mLwm)
            notifySubscribers(Events::on_lwm);
        return RetCodes::OK;
    }

    size_t currentSize() const {
        return mCurrentSize;
    }

    size_t fixedSize() const {
        return mSize;
    }

    /**
     * @brief setEvent
     * @details Функция установки состояния
     */
    void setEvent(Events event) {
        if (event == Events::on_stop) {
            notifySubscribers(Events::on_stop);
            mCv.notify_all();
        }
        if (mState == event)
            return;
        mState = event;
    }

    /**
     * @brief subscribe
     * @details подписка "писателей"
     */
    void subscribe(IMessageQueueEvents *writer) {
        std::lock_guard<std::mutex> guard(mSubscribeMutex);
        mSubscribers.push_back(writer);
    }

    /**
     * @brief setLwm
     * @details установка значения Lwm
     * @param lwmThreshold - значение в процентах
     */
    void setLwm(unsigned short lwmThreshold = 10) {
        mLwm = conversionWaterMark(lwmThreshold, false);
    }

    /**
     * @brief setHwm
     * @details установка значения Hwm
     * @param hwmThreshold - значение в процентах
     */
    void setHwm(unsigned short hwmThreshold = 90) {
        mHwm = conversionWaterMark(hwmThreshold, true);
    }

    double percentFullnessQueue() const {
        const auto curSize {currentSize()};
        const auto fixSize {fixedSize()};

        if (curSize < fixSize)
            return ((curSize * 100.0) / fixSize);

        return 100.0;
    }

    unsigned int getPercentWaterMark(bool inHwm) const {
        const auto factor = inHwm ? static_cast<double>(mHwm * 100.):
                                    static_cast<double>(mLwm * 100.);
        if (inHwm) {
            return std::ceil(factor / fixedSize());
        } else {
            return std::floor(factor / fixedSize());
        }
    }
private:
    unsigned int conversionWaterMark(unsigned short wmPercent, bool inHwm) const {
        assert(wmPercent >= 0 && wmPercent <= 100);
        const auto factor = static_cast<double>(wmPercent / 100.);
        if (inHwm) {
            return std::ceil(fixedSize() * factor);
        } else {
            return std::floor(fixedSize() * factor);
        }
    }

    /**
     * @brief notifySubscribers
     * @details Уведомление подписчиков о смене состояния
     */
    void notifySubscribers(Events event) {
        std::lock_guard<std::mutex> guard(mSubscribeMutex);
        std::for_each(mSubscribers.begin(),
                      mSubscribers.end(),
                      [event](IMessageQueueEvents *subscriber){
            subscriber->applyEvent(event);
        });
    }

private:
    const size_t mSize;
    std::atomic_size_t mCurrentSize;
    std::atomic_uint mLwm, mHwm;

    std::mutex mMutex, mSubscribeMutex;
    std::condition_variable mCv;

    Events mState;
    std::vector<IMessageQueueEvents *> mSubscribers;

    std::priority_queue<std::pair<Type, Identifier>,
    Sequence,
    decltype (MessageCompare)> mQueue;
};
}
