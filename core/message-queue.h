#pragma once

#include "imessage-queue-events.h"
#include "return-codes.h"

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

enum class QueueState {
    Start,
    Stop,
    Hwm,
    Lwm
};

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
    MessageQueue(unsigned int size,
                 unsigned short lwm = 10,
                 unsigned short hwm = 90);

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
    RetCodes put(const Type &message);

    /**
     * @brief get
     * @details Блокирующая функция, достаёт сообщение из очереди.
     * Уведомляет "писателей" при достижении порога lwm
     * Учитывает события и коды возврата.
     * Функция потокобезопасна.
     */
    RetCodes get(Type &type);

    /**
     * @brief setEvent
     * @details Функция подписки. Использует интерфейс IMessageQueueEvents
     * для передачи события.
     */
    void setEvent(IMessageQueueEvents *event);

    /**
     * @brief stop
     * @details Уведомляет писателей о прекращении работы
     */
    void stop();

    /**
     * @brief setLwm - необходимо для демонстрации работы
     * @details установка значения Lwm.
     * @param lwmThreshold - значение в процентах
     */
    void setLwm(unsigned short lwmThreshold = 10);

    /**
     * @brief setHwm - необходимо для демонстрации работы
     * @details установка значения Hwm
     * @param hwmThreshold - значение в процентах
     */
    void setHwm(unsigned short hwmThreshold = 90);

    /**
     * @brief percentFullnessQueue - возвращает заполненность очереди
     * необходимо для демонстрации работы
     */
    double percentFullnessQueue() const;

    /**
     * @brief getPercentWaterMark - возвращает процент WaterMark
     * необходимо для демонстрации работы
     */
    unsigned int getPercentWaterMark(bool inHwm) const;

    /**
     * @brief fixedSize - возвращает размер очереди
     * необходимо для демонстрации работы
     */
    size_t fixedSize() const;
private:

    /**
     * @brief conversionWaterMark - переводит hwm/lwm
     */
    unsigned int conversionWaterMark(unsigned short wmPercent, bool inHwm) const;

    /**
     * @brief notifySubscribers
     * @details Уведомление подписчиков о пришедшем событии
     */
    void notifySubscribers(QueueState state);

private:
    const size_t mSize;
    std::atomic_size_t mCurrentSize;
    std::atomic_uint mLwm, mHwm;
    std::mutex mMutex, mSubscribeMutex;
    std::condition_variable mCv;
    std::atomic<QueueState> mState;
    std::vector<IMessageQueueEvents *> mSubscribers;
    std::priority_queue<std::pair<Type, Identifier>,
                        Sequence,
                        decltype (MessageCompare)> mQueue;
};
}
