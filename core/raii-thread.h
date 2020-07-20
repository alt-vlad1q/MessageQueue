#pragma once

#include <thread>
#include <atomic>
#include <iostream>

namespace mq {

/**
 * @brief Блокирующий токен
 */
class BlockingToken
{
public:
    explicit operator bool() const {
        return !mToken;
    }

    void cancel() {
        mToken = true;
    }

private:
    std::atomic_bool mToken {false};
};

/**
 * @brief RAII Thread
 * @details Обёртка над std::thread, обеспечивающая
 * корректное завершение работы потока
 */
class ThreadWrapper
{
public:
    ThreadWrapper() noexcept = default;
    ThreadWrapper(const ThreadWrapper &) = delete;
    ThreadWrapper(ThreadWrapper &&) noexcept;

    template<class Function, class... Args>
    explicit ThreadWrapper(Function &&func, Args&&... args) :
        mToken(new BlockingToken()) {
        mThread = std::thread(std::forward<Function>(func),
                            std::forward<Args>(args)...,
                            std::cref(*mToken));
    }

    ~ThreadWrapper();

    ThreadWrapper& operator = (ThreadWrapper &other) = delete;
    ThreadWrapper& operator = (ThreadWrapper &&other) noexcept;

    void swap(ThreadWrapper &other) noexcept;
    bool joinable() const noexcept;

    void join();
    void reset();

private:
    std::thread mThread {};
    std::unique_ptr<BlockingToken> mToken;
};
}
