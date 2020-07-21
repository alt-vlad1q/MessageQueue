#include "reader.h"

#include <message_impl.h>
#include <message-queue_impl.h>

Reader::Reader(mq::MessageQueue<mq::MessageType> &rqueue, std::ostream &rstream):
    mq::ReaderInterface(rqueue),
    mOutputStream(rstream) {}

void Reader::handle_message(const mq::MessageType &rmessage)
{
    mOutputStream << rmessage;
}
