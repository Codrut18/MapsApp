// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "ITextureRepository.h"

#include <map>

class TextureRepository : public ITextureRepository
{
public:
    TextureRepository();
    ~TextureRepository();

    unsigned int GetIconTexture(EIconType iconType, int width, int height) override;
    void UnloadIconTexture(EIconType iconType) override;

    unsigned int GetTexture( unsigned int imageId, int w, int h, bool bSync = true ) override;
    unsigned int GetTexture(gem::Image image, int w, int h, bool bSync = true) override;
    unsigned int GetTexture(const gem::AbstractGeometryImage& image, const gem::AbstractGeometryImageRenderSettings& settings, int w, int h, bool bSync = true) override;

    void UnloadAllTextures() override;
    void UnloadTexture(unsigned int textureId) override;

private:
    static unsigned int GetIconId(EIconType iconType);

    static unsigned int LoadTextureIntoGPU(int width, int height, void* data);
    static void UnloadTextureFromGPU(unsigned int& textureId);

    void FillCountriesIsoToImageUids();

    unsigned int GetFlagImageUid(const gem::String& iso);
    gem::Image GetFlagImage(const gem::String& iso);

    static int IsoToInt(const gem::String& iso);

private:
    std::map<unsigned int, unsigned int> m_ImagesUidToTextureId;

    // Countries iso to image ids
    std::map<int, unsigned int> m_countriesIsoToImageUids;
};
