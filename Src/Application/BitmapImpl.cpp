// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "BitmapImpl.h"

#include <cstdlib>

BitmapImpl::BitmapImpl( int width, int height )
    : m_buffer( nullptr )
{
    resize( width, height );
}

BitmapImpl::~BitmapImpl()
{
    if( m_buffer )
    {
        delete[] m_buffer;
    }
}

gem::EImagePixelFormat BitmapImpl::encoding() const
{
    return gem::EImagePixelFormat::ABGR_8888;
}

const gem::Rect& BitmapImpl::viewport() const
{
    return m_viewport;
}

void* BitmapImpl::begin()
{
    return (void*)m_buffer;
}

void BitmapImpl::end()
{

}

gem::Size BitmapImpl::size() const
{
    return m_size;
}

int BitmapImpl::alignment() const
{
    return 1;
}

void BitmapImpl::clear()
{
    memset( m_buffer, 0xFFFFFFFF, (size_t)m_size.width * (size_t)m_size.height * 4 );
}

void BitmapImpl::resize( int w, int h )
{
    m_size = gem::Size( w, h );
    m_viewport = gem::Rect( 0, 0, w, h );

    if( m_buffer )
        delete[] m_buffer;

    if( w && h )
    {
        m_buffer = new unsigned char[(size_t)w * (size_t)h * 4];
        clear();
    }
}
