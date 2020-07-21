#include "reader-interface.h"
#include "message_impl.h"
#include "message-queue_impl.h"

namespace mq {
ReaderInterface::ReaderInterface(MessageQueue<MessageType> &rqueue) :
    mMessageQueue(rqueue) {}

void ReaderInterface::run()
{
    mThread = ThreadWrapper([&](const BlockingToken &token){read(token);});
}

void ReaderInterface::read(const BlockingToken &token)
{
    while(token) {
        MessageType str;
        switch (mMessageQueue.get(str)) {
            case mq::RetCodes::STOPPED: {
                break;
            }
            default: {
                handle_message(str);
                continue;
            }
        }
        break;
    }
}
}
