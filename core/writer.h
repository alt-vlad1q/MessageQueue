#pragma once

#include "message.h"
#include "raii-thread.h"
#include "message-queue.h"

namespace mq {
/**
 * @brief Писатель
 * @details Принимает ссылку на очередь и
 * при создании объекта подписывается на рассылку событий очереди.
 * Генерирует сообщение, добавляет его в очередь
 */

enum class WriterState {
    Start,
    Stop,
    Hwm,
    Lwm
};

class Writer final : public IMessageQueueEvents
{
public:
    Writer() = delete;
    explicit Writer(MessageQueue<MessageType> &rqueue);

    /**
     * @brief run
     * @details Инициализация и запуск потока
     */
    void run ();

    void on_start() override;
    void on_stop() override;
    void on_hwm() override;
    void on_lwm() override;


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
    std::atomic<WriterState> mState;
};
}
