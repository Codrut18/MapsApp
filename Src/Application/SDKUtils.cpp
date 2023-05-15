// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "SDKUtils.h"

#include <API/GEM_Sdk.h>
#include <API/GEM_Traffic.h>

#include <string>
#include <Windows.h>
#include <shlwapi.h>

namespace
{
    // Points to <SDK/AppData> dir

    std::string GetResourcesPath()
	{
		char appPath[260];
		GetModuleFileNameA( 0, appPath, 260 );

		std::string dest = appPath + std::string("\\..\\..\\..\\..\\..\\..\\SDK\\AppData");
        
        char resourcesPath[260];
		PathCanonicalizeA(resourcesPath, dest.c_str());
		if (PathFileExistsA(resourcesPath))
			return resourcesPath;

		return std::string();
	}

	std::string GetCachePath()
    {
		return GetResourcesPath();
	}
}

SDKUtils::~SDKUtils()
{
    ReleaseSDK();
}

int SDKUtils::InitSDK( std::string logFile )
{
    // Initialize SDK
    std::string resPath = GetResourcesPath();
    if (resPath.empty())
        return gem::error::KNotFound;

    std::string cachePath = GetCachePath();
    if (cachePath.empty())
        return gem::error::KNotFound;

    apiTimer = gem::StrongPointerFactory<TimerServiceImpl>();
    apiLogger = gem::StrongPointerFactory<ApiCallLoggerImpl>( logFile );

    int errCode = gem::Sdk::initialize( 0, resPath, cachePath, apiTimer, u"eng", gem::String(), gem::EDataSavePolicy::UseExternalOnly, apiLogger );
    if( errCode != gem::KNoError )
        return errCode;

    gem::Traffic().preferences().setUseTraffic( gem::UseNone );

    return gem::KNoError;
}

void SDKUtils::Tick()
{
    apiTimer->Tick();
}

void SDKUtils::ReleaseSDK()
{
    gem::Sdk::release();
}
