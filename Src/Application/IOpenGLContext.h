// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <API/GEM_RenderContext.h>

class IOpenGLContext : public gem::IOpenGLContext
{
public:
    virtual void SetPixelRatio( float pixelRatio ) = 0;

    virtual ~IOpenGLContext() = default;
};
using IOpenGLContextPtr = std::unique_ptr<IOpenGLContext>;
