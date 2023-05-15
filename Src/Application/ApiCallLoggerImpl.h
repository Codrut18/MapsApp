// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <API/GEM_CallLogger.h>

#include <fstream>

class ApiCallLoggerImpl : public gem::IApiCallLogger
{
public:
    ApiCallLoggerImpl( std::string logFile = std::string() )
        : m_logStream( nullptr )
    {
        if ( !logFile.empty() )
            m_logStream = std::make_shared<std::ofstream>( logFile, std::ios::app );
    }

    ~ApiCallLoggerImpl()
    {
        if( m_logStream )
            m_logStream->close();
    }

    virtual void onLog( int logLevel, char* logText, unsigned logTextSize )
    {
        if( m_logStream )
            (*m_logStream) << std::string( logText, logTextSize ) << std::endl;
    }

    virtual int onGetLogLevel() const
    {
        return gem::ELogLevel::LogInfo;
    }

private:
    std::shared_ptr<std::ofstream> m_logStream;
};
