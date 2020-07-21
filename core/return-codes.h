#pragma once

namespace mq {
enum class RetCodes{
    OK = 0,
    HWM = -1,
    NO_SPACE = -2,
    STOPPED = -3
};
}
