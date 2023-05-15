// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "IOpenGLContext.h"

#include <functional>

using UpdateRenderOpenGLContextFunc = std::function<bool ( void )>;

class OpenGLContextImpl : public IOpenGLContext
{
public:
    OpenGLContextImpl ( void* openGLContext, int width, int height, int dpi, float pixelRatio, UpdateRenderOpenGLContextFunc func = {} );

    // gem::IOpenGLContext methods
    bool initialize() override;
    bool isInitialized() const override;

    bool makeCurrent() override;
    bool doneCurrent() override;

    int getDpi() const override;
    virtual float getPixelRatio() const override;

    void needsRender() override;

    gem::EImagePixelFormat encoding() const override;

    gem::Rect const& viewport() const override;

    void* getSystemContext() override;

    // specific IOpenGLContext
    void SetPixelRatio( float pixelRatio ) override;

private:
    void* m_pContext;

    UpdateRenderOpenGLContextFunc m_makeCurrentFunc;

    gem::Rect m_viewport;

    int m_dpi;
    float m_pixelRatio;
};
