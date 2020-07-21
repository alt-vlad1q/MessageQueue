#include "writer.h"
#include "message_impl.h"
#include "message-queue_impl.h"

#include <random>

namespace mq {
Writer::Writer(MessageQueue<MessageType> &rqueue) :
    mMessageQueue(rqueue)
{
    mMessageQueue.setEvent(this);
    mMessage.generate();
}

void Writer::run()
{
    mThread = ThreadWrapper([&](const BlockingToken &token){write(token);});
}

void Writer::on_start() {
    mState = WriterState::Start;
}

void Writer::on_stop() {
    mState = WriterState::Stop;
}

void Writer::on_hwm() {
    mState = WriterState::Hwm;
}

void Writer::on_lwm() {
    mState = WriterState::Lwm;
}

void Writer::write(const BlockingToken &token)
{
    while(token) {
        if(mMessage.empty())
            mMessage.generate();

        if(mState == WriterState::Stop)
            break;
        if(mState == WriterState::Hwm)
            continue;

        const auto retVal = mMessageQueue.put(mMessage);
        switch (retVal) {
            case mq::RetCodes::STOPPED:
                break;
            default:
                mMessage.clear();
                continue;
        }
        break;
    }
}
}
