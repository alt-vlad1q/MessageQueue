#pragma once

#include "message.h"
#include "raii-thread.h"
#include "message-queue.h"

namespace mq {
/**
 * @brief Интерфейс читателя
 */
class ReaderInterface
{
public:
    ReaderInterface() = delete;
    explicit ReaderInterface(MessageQueue<MessageType> &rqueue) :
        mMessageQueue(rqueue) {};
    virtual ~ReaderInterface() = default;

    /**
     * @brief run
     * @details Инициализация и запуск потока
     */
    void run ();

protected:
    /**
     * @brief handle_message
     * @param message
     * @details Абстрактный метод для обработки сообщения
     */
    virtual void handle_message (const MessageType &message) = 0;

private:

    /**
     * @brief read
     * @details Метод запрашивает сообщение у очереди
     * и выполняет его обработку. Завершается при получении кода - STOPPED
     */
    void read (const BlockingToken &token);

private:
    MessageQueue<MessageType> &mMessageQueue;
    ThreadWrapper mThread {};
};
}
