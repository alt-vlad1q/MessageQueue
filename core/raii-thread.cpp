#include "raii-thread.h"

namespace mq {
mq::BlockingToken::operator bool() const {
    return !mToken;
}

void BlockingToken::cancel() {
    mToken = true;
}

ThreadWrapper::ThreadWrapper(ThreadWrapper &&other) noexcept :
    mThread(std::move(other.mThread)),
    mToken(std::move(other.mToken)) {
}

ThreadWrapper::~ThreadWrapper() {
    reset();
}

ThreadWrapper& ThreadWrapper::operator =(ThreadWrapper &&other) noexcept {
    ThreadWrapper tmp(std::move(other));
    this->swap(tmp);
    return *this;
}

void ThreadWrapper::swap(ThreadWrapper &other) noexcept {
    mThread.swap(other.mThread);
    mToken.swap(other.mToken);
}

bool ThreadWrapper::joinable() const noexcept {
    return mThread.joinable();
}

void ThreadWrapper::join() {
    mThread.join();
}

void ThreadWrapper::reset() {
    if (!joinable())
        return;
    mThread.join();
    mToken->cancel();
    *this = ThreadWrapper();
    std::clog << "Thread ended correctly" << std::endl;
}
}
