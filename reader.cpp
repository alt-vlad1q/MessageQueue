#include "reader.h"

void Reader::handle_message(const mq::MessageType &rmessage)
{
    mOutputStream << rmessage;
}
