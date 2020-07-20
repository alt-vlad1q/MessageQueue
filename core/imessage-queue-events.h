#pragma once

#include <iostream>

namespace mq {
enum class Events {
    on_start = 1,
    on_stop,
    on_hwm,
    on_lwm
};

/**
 * @brief Класс содержащий состояния очереди
 */
class IMessageQueueEvents
{
public:
    IMessageQueueEvents() : mState(Events::on_start) {};
    virtual ~IMessageQueueEvents() = default;

    inline Events state() {
        return mState;
    }
    inline void applyEvent(Events _state) {
        mState = _state;
    }

private:
    Events mState;
};
}

