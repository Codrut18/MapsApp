// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <functional>

using UpdateOpenGLContextFunc = std::function<bool( void )>;

struct WindowInfo
{
    WindowInfo()
        : width( 0 )
        , height( 0 )
        , openGLContext( nullptr )
        , dpi( 1 )
        , ddpi( 120.f )
        , pixelRatio( 1 )
        , glVersion( nullptr )
    {

    }

    void* openGLContext;

    int width;
    int height;
    
    float dpi;
    float ddpi;
    float pixelRatio;

    const char* glVersion;

    UpdateOpenGLContextFunc updateOpenGLRenderContextFunc;
};
