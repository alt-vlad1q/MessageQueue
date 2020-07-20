#pragma once

#include <reader-interface.h>
#include <message-queue.h>
#include <message.h>

#include <iostream>

/**
 * @brief Читатель
 * @details Выводит сообщение в std::ostream
 */
class Reader final : public mq::ReaderInterface
{
public:
    explicit Reader(mq::MessageQueue<mq::MessageType> &rqueue, std::ostream &rstream):
        mq::ReaderInterface(rqueue),
        mOutputStream(rstream) {};

    void handle_message(const mq::MessageType &message) override;

private:
    std::ostream &mOutputStream;
};

