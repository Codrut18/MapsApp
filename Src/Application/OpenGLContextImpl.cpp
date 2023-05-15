// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "OpenGLContextImpl.h"

OpenGLContextImpl::OpenGLContextImpl ( void* openGLContext, int width, int height, int dpi, float pixelRatio, UpdateRenderOpenGLContextFunc func /*= {} */ ) : m_pContext ( openGLContext )
, m_makeCurrentFunc ( func )
, m_viewport ( 0, 0, width, height )
, m_dpi ( dpi )
, m_pixelRatio ( pixelRatio )
{

}

bool OpenGLContextImpl::initialize ()
{
    return true;
}

bool OpenGLContextImpl::isInitialized () const
{
    return true;
}

bool OpenGLContextImpl::makeCurrent ()
{
    if ( m_makeCurrentFunc )
        return m_makeCurrentFunc ();

    return true;
}

bool OpenGLContextImpl::doneCurrent ()
{
    return true;
}

int OpenGLContextImpl::getDpi () const
{
    return m_dpi;
}

float OpenGLContextImpl::getPixelRatio () const
{
    return  m_pixelRatio;
}

void OpenGLContextImpl::needsRender ()
{

}

gem::EImagePixelFormat OpenGLContextImpl::encoding () const
{
    return gem::EImagePixelFormat::ABGR_8888;
}

gem::Rect const& OpenGLContextImpl::viewport () const
{
    return m_viewport;
}

void* OpenGLContextImpl::getSystemContext ()
{
    return (void*)&m_pContext;
}

void OpenGLContextImpl::SetPixelRatio ( float pixelRatio )
{
    m_pixelRatio = pixelRatio;
}
