#pragma once

#include "message-queue.h"

namespace mq {
template<typename Type, class Sequence>
MessageQueue<Type, Sequence>::MessageQueue(unsigned int size,
                                           unsigned short lwm,
                                           unsigned short hwm) :
    mSize(size),
    mCurrentSize(0),
    mLwm(conversionWaterMark(lwm, false)),
    mHwm(conversionWaterMark(hwm, true)),
    mMutex(),
    mSubscribeMutex(),
    mCv(),
    mState(QueueState::Start),
    mSubscribers(),
    mQueue(MessageCompare)
{
    static_assert(std::is_same<std::pair<Type, Identifier>,
                               typename Sequence::value_type>::value,
                               "Type for MessageQueue must be the same as the container");
    assert (fixedSize() > 2 && "Size MessageQueue less than 3");
}

template<typename Type, class Sequence>
RetCodes MessageQueue<Type, Sequence>::put(const Type &message) {
    if (mCurrentSize < mHwm) {
        std::lock_guard<std::mutex> guard(mMutex);
        mQueue.emplace(std::make_pair(std::move(message),
                                      std::chrono::steady_clock::now()));
        mCurrentSize = mQueue.size();
        if(mCurrentSize == 1)
            mCv.notify_all();
    } else {
        notifySubscribers(QueueState::Hwm);
        return RetCodes::HWM;
    }
    if(mState == QueueState::Stop) {
        return RetCodes::STOPPED;
    }
    return RetCodes::OK;
}

template<typename Type, class Sequence>
RetCodes MessageQueue<Type, Sequence>::get(Type &type) {
    if(mState == QueueState::Stop)
        return RetCodes::STOPPED;
    std::unique_lock<std::mutex> lk(mMutex);
    mCv.wait(lk, [this] {
        return (!mQueue.empty() || mState == QueueState::Stop);
    });
    if (mQueue.empty())
        return RetCodes::STOPPED;
    type = mQueue.top().first;
    mQueue.pop();
    mCurrentSize = mQueue.size();
    if(mCurrentSize <= mLwm)
        notifySubscribers(QueueState::Lwm);
    return RetCodes::OK;
}

template<typename Type, class Sequence>
void MessageQueue<Type, Sequence>::setEvent(IMessageQueueEvents *writer) {
    std::lock_guard<std::mutex> guard(mSubscribeMutex);
    mSubscribers.push_back(writer);
}

template<typename Type, class Sequence>
void MessageQueue<Type, Sequence>::stop() {
    notifySubscribers(QueueState::Stop);
    mCv.notify_all();
    mState = QueueState::Stop;
}

template<typename Type, class Sequence>
void MessageQueue<Type, Sequence>::setLwm(unsigned short lwmThreshold) {
    mLwm = conversionWaterMark(lwmThreshold, false);
}

template<typename Type, class Sequence>
void MessageQueue<Type, Sequence>::setHwm(unsigned short hwmThreshold) {
    mHwm = conversionWaterMark(hwmThreshold, true);
}

template<typename Type, class Sequence>
double MessageQueue<Type, Sequence>::percentFullnessQueue() const {
    const size_t curSize {mCurrentSize};

    if (curSize < fixedSize())
        return ((curSize * 100.0) / fixedSize());

    return 100.0;
}

template<typename Type, class Sequence>
unsigned int MessageQueue<Type, Sequence>::getPercentWaterMark(bool inHwm) const {
    const auto factor = inHwm ? static_cast<double>(mHwm * 100.):
                                static_cast<double>(mLwm * 100.);
    if (inHwm) {
        return std::ceil(factor / fixedSize());
    } else {
        return std::floor(factor / fixedSize());
    }
}

template<typename Type, class Sequence>
size_t MessageQueue<Type, Sequence>::fixedSize() const {
    return mSize;
}

template<typename Type, class Sequence>
unsigned int MessageQueue<Type, Sequence>::conversionWaterMark(unsigned short wmPercent, bool inHwm) const {
    assert(wmPercent >= 0 && wmPercent <= 100);
    const auto factor = static_cast<double>(wmPercent / 100.);
    if (inHwm) {
        return std::ceil(fixedSize() * factor);
    } else {
        return std::floor(fixedSize() * factor);
    }
}

template<typename Type, class Sequence>
void MessageQueue<Type, Sequence>::notifySubscribers(QueueState state) {
    std::lock_guard<std::mutex> guard(mSubscribeMutex);
    std::for_each(mSubscribers.begin(),
                  mSubscribers.end(),
                  [state](IMessageQueueEvents *subscriber) {
        switch (state) {
        case QueueState::Start:
            subscriber->on_start();
            break;
        case QueueState::Hwm:
            subscriber->on_hwm();
            break;
        case QueueState::Lwm:
            subscriber->on_lwm();
            break;
        default:
            subscriber->on_stop();
        }
    });
}
}
