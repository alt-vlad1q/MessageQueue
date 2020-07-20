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
    Message() :
        mValue(),
        mPriority(MessagePriority::Undefined) {}
    Message(const Type &value, MessagePriority priority) :
        mValue(value),
        mPriority(priority) {}

    bool empty() const {
        if (mValue == Type() && mPriority == MessagePriority::Undefined)
            return true;
        else
            return false;
    }

    void clear() {
        mValue = Type();
        mPriority = MessagePriority::Undefined;
    }

    /**
     * @brief generate
     * @details  Равномерно распредёленно генерирует
     * случайное сообщение и его приорит на интервале [a, b]
     */
    void generate(){
        std::random_device randomDevice {};
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<> distribution(static_cast<int>(MessagePriority::Height),
                                                     static_cast<int>(MessagePriority::Low));
        std::uniform_int_distribution<> distributionMessage (1, 1000);

        if constexpr (std::is_same<Type, std::string>::value)
                mValue = {std::string("Message_") + std::to_string(distributionMessage(generator))};

        if constexpr (std::is_integral<Type>::value ||
                std::is_floating_point<Type>::value)
                mValue = static_cast<Type>(distributionMessage(generator));

        mPriority = static_cast<MessagePriority>(distribution(generator));
    }

    friend bool operator< (const Message &lhs, const Message &rhs) {
        return lhs.mPriority < rhs.mPriority;
    }

    friend bool operator> (const Message &lhs, const Message &rhs) {
        return !(lhs < rhs);
    }

    friend std::ostream& operator<< (std::ostream &out, const Message &message) {
        switch (message.mPriority) {
            case MessagePriority::Height:
                out << "[Height]";
                break;
            case MessagePriority::Medium:
                out << "[Medium]";
                break;
            default:
                out << "[Low   ]";
        }
        out << " " << message.mValue << std::endl;
        return out;
    }

private:
    Type mValue;
    MessagePriority mPriority;
};

using MessageType = Message<std::string>;
}
