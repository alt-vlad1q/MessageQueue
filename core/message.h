#pragma once

#include <random>
#include <iostream>
#include <string>

namespace mq {
enum class MessagePriority {
    Undefined = 0,
    Height = 1,
    Medium = 2,
    Low = 3
};

/**
 * @brief Структура сообщения
 * @details Шаблонный тип Type может быть строкой,
 * интегральным типом или типом с плавающей точкой.
 * По умолчанию тип - std::string
 * В данной структуре содержатся операторы сравнения и
 * оператор вывода в поток
 */
template<typename Type>
struct Message {
    Message();
    Message(const Type &value, MessagePriority priority);

    bool empty() const;
    void clear();

    /**
     * @brief generate
     * @details  Равномерно распредёленно генерирует
     * случайное сообщение и его приорит на интервале [a, b]
     */
    void generate();

    template<typename T>
    friend bool operator< (const Message<T> &lhs, const Message<T> &rhs);

    template<typename T>
    friend bool operator> (const Message<T> &lhs, const Message<T> &rhs);

    template<typename T>
    friend std::ostream& operator<< (std::ostream &out, const Message<T> &message);

private:
    Type mValue;
    MessagePriority mPriority;
};

using MessageType = Message<std::string>;
}
