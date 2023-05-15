// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "API/GEM_ProgressListener.h"

#include <functional>

using StatusChangedFunc = std::function<void( int )>;

class ContentUpdateListener : public gem::IProgressListener
{
public:
    ContentUpdateListener( StatusChangedFunc func )
        : m_statusChangedFunc( func )
    {
    }
    void notifyStart( bool hasProgress ) override {}
    void notifyComplete( int reason, gem::String hint ) override {}
    void notifyStatusChanged( int status ) override
    {
        if ( m_statusChangedFunc )
        {
            m_statusChangedFunc( status );
        }
    }

private:
    StatusChangedFunc m_statusChangedFunc;
};
