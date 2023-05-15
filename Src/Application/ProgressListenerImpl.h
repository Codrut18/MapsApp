// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <API/GEM_ProgressListener.h>

#include <functional>

using ProgressCompleteFunc = std::function<void( int, gem::String )>;

class ProgressListenerImpl : public gem::IProgressListener
{
public:
    ProgressListenerImpl( ProgressCompleteFunc func )
        : m_func( func )
    {

    }

    virtual void notifyStart( bool hasProgress ) override
    {

    }

    virtual void notifyComplete( int reason, gem::String hint = gem::String() ) override
    {
        m_func( reason, hint );
    }

private:
    ProgressCompleteFunc m_func;
};
