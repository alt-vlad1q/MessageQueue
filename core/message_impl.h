#pragma once

#include "message.h"

namespace mq {
template<typename Type>
Message<Type>::Message() :
    mValue(),
    mPriority(MessagePriority::Undefined) {}

template<typename Type>
Message<Type>::Message(const Type &value, MessagePriority priority) :
    mValue(value),
    mPriority(priority) {}

template<typename Type>
bool Message<Type>::empty() const {
    if (mValue == Type() && mPriority == MessagePriority::Undefined)
        return true;
    else
        return false;
}

template<typename Type>
void Message<Type>::clear() {
    mValue = Type();
    mPriority = MessagePriority::Undefined;
}

template<typename Type>
void Message<Type>::generate(){
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

template<typename T>
bool operator>(const Message<T> &lhs, const Message<T> &rhs){
    return !(lhs < rhs);
}

template<typename T>
bool operator<(const Message<T> &lhs, const Message<T> &rhs){
    return lhs.mPriority < rhs.mPriority;
}

template<typename Type>
std::ostream& operator<< (std::ostream &out, const Message<Type> &message) {
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
}
