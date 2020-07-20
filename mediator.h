#pragma once

#include <message.h>
#include <message-queue.h>

#include <QObject>

/**
 * @brief Посредник
 * @details Пробрасывает значения lwm, hwm и сигнал прекращения работы
 * между Gui и очередью
 * Необходим для демонстрации состояния очереди
 */
class Mediator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ushort hwm READ hwm WRITE setHwm NOTIFY hwmChanged)
    Q_PROPERTY(ushort lwm READ lwm WRITE setLwm NOTIFY lwmChanged)
    Q_PROPERTY(bool stopped READ stopped WRITE setStopped NOTIFY stoppedChanged)

    using MessageQueue = mq::MessageQueue<mq::MessageType>;

public:
    explicit Mediator(MessageQueue &queue, QObject *parent = nullptr);

    ushort hwm() const;
    void setHwm(ushort hvm);
    ushort lwm() const;
    void setLwm(ushort lwm);
    bool stopped() const;
    void setStopped(bool stopped);

signals:
    void hwmChanged(ushort hwm);
    void lwmChanged(ushort lwm);
    void stoppedChanged(bool stopped);

private:
    MessageQueue &mQueue;
    ushort mHwm;
    ushort mLwm;
    bool mStopped;
};
