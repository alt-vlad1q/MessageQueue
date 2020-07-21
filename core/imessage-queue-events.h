#pragma once

#include <iostream>

namespace mq {
/**
 * @brief Интерфейс событий
 */
class IMessageQueueEvents
{
public:
    virtual ~IMessageQueueEvents() = default;

    virtual void on_start() = 0;
    virtual void on_stop() = 0;
    virtual void on_hwm() = 0;
    virtual void on_lwm() = 0;
};
}

