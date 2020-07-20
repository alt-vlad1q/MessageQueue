#pragma once

#include "message.h"
#include "raii-thread.h"
#include "message-queue.h"
#include "imessage-queue-events.h"

namespace mq {
/**
 * @brief Писатель
 * @details Принимает ссылку на очередь и
 * при создании объекта подписывается на рассылку состояния.
 * Генерирует сообщение, добавляет его в очередь
 */
class Writer final : public IMessageQueueEvents
{
public:
    Writer() = delete;
    explicit Writer(MessageQueue<MessageType> &rqueue) :
        mMessageQueue(rqueue)
    {
        mMessageQueue.subscribe(this);
        mMessage.generate();
    };

    /**
     * @brief run
     * @details Инициализация и запуск потока
     */
    void run ();

private:
    /**
     * @brief write
     * @details Добавляет сообщение в очередь,
     * обрабатывает состояния и код возврата - STOPPED
     */
    void write (const BlockingToken &token);

private:
    MessageQueue<MessageType> &mMessageQueue;
    MessageType mMessage;
    ThreadWrapper mThread;
};
}
