#include "writer.h"

#include <random>

namespace mq {
void Writer::run()
{
    mThread = ThreadWrapper([&](const BlockingToken &token){write(token);});
}

void Writer::write(const BlockingToken &token)
{
    while(token) {
        if(mMessage.empty())
            mMessage.generate();

        if(state() == Events::on_stop)
            break;
        if(state() == Events::on_hwm)
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
