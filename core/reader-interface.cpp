#include "reader-interface.h"

namespace mq {
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
