// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "API/GEM_Images.h"

enum class EIconType
{
    MenuButton,
    CurrentPosition
};

class ITextureRepository
{
public:
    virtual unsigned int GetIconTexture( EIconType iconType, int width, int height ) = 0;
    virtual void UnloadIconTexture( EIconType iconType ) = 0;

    virtual unsigned int GetTexture( unsigned int imageId, int w, int h, bool bSync = true ) = 0;
    virtual unsigned int GetTexture( gem::Image image, int w, int h, bool bSync = true ) = 0;
    virtual unsigned int GetTexture( const gem::AbstractGeometryImage& image, const gem::AbstractGeometryImageRenderSettings& settings, int w, int h, bool bSync = true ) = 0;

    virtual void UnloadAllTextures() = 0;
    virtual void UnloadTexture( unsigned int textureId ) = 0;

    virtual ~ITextureRepository() = default;
};

