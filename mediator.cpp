#include "mediator.h"

#include <imessage-queue-events.h>
#include <message_impl.h>
#include <message-queue_impl.h>

#include <iostream>

Mediator::Mediator(MessageQueue &rqueue, QObject *parent) :
    QObject(parent),
    mQueue(rqueue),
    mHwm(rqueue.getPercentWaterMark(true)),
    mLwm(rqueue.getPercentWaterMark(false)),
    mStopped(false){}

ushort Mediator::hwm() const
{
    return mHwm;
}

void Mediator::setHwm(ushort hwm)
{
    if (mHwm == hwm)
        return;

    mHwm = hwm;

    mQueue.setHwm(mHwm);
    emit hwmChanged(mHwm);
}

ushort Mediator::lwm() const
{
    return mLwm;
}

void Mediator::setLwm(ushort lwm)
{
    if (mLwm == lwm)
        return;

    mLwm = lwm;
    mQueue.setLwm(mLwm);
    emit lwmChanged(mLwm);
}

bool Mediator::stopped() const
{
    return mStopped;
}

void Mediator::setStopped(bool stopped)
{
    if (mStopped == stopped)
        return;

    mStopped = stopped;
    mQueue.stop();
    emit stoppedChanged(mStopped);
}
