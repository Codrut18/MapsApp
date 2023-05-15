// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include <API/GEM_RenderContext.h>

class BitmapImpl : public gem::IBitmap
{
public:
    BitmapImpl( int width, int height );
    ~BitmapImpl();

    // gem::IRenderContext methods
    gem::EImagePixelFormat encoding() const override;

    const gem::Rect& viewport() const override;

    // gem::IBitmap methods
    void* begin() override;
    void end() override;

    gem::Size size() const override;
    
    int alignment() const override;

    void clear();

    void resize( int w, int h );

private:
    gem::Size m_size;
    gem::Rect m_viewport;
    unsigned char* m_buffer;
};
