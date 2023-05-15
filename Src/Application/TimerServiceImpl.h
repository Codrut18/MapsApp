// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <API/GEM_Timer.h>

class TimerServiceImpl : public gem::ITimerService
{
public:
    TimerServiceImpl();

    void Tick();

protected:
    int onRegisterListener( gem::ITimerListener* listener );
    void onUnregisterListener();

    int onStartTimer( int intervalMs );
    int onStopTimer();

protected:
    gem::ITimerListener* m_pListener;
};
