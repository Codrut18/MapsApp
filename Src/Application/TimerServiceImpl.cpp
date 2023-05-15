// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "TimerServiceImpl.h"

#include <API/GEM_Error.h>

TimerServiceImpl::TimerServiceImpl()
    : m_pListener(nullptr)
{

}

void TimerServiceImpl::Tick()
{
    if(m_pListener)
        m_pListener->onTimer();
}

int TimerServiceImpl::onRegisterListener( gem::ITimerListener* listener )
{
    m_pListener = listener;
    return gem::KNoError;
}

void TimerServiceImpl::onUnregisterListener()
{
    m_pListener = nullptr;
}

int TimerServiceImpl::onStartTimer( int intervalMs )
{
    return gem::KNoError;
}

int TimerServiceImpl::onStopTimer()
{
    return gem::KNoError;
}
