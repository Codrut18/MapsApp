// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "TextureRepository.h"

#include "BitmapImpl.h"

#include <API/GEM_ImageIDs.h>
#include <API/GEM_MapDetails.h>
#include <API/GEM_OperationScheduler.h>

#include "GLES2/gl2.h"

#include <functional>

TextureRepository::TextureRepository()
{
    FillCountriesIsoToImageUids();
}

TextureRepository::~TextureRepository()
{
    UnloadAllTextures();
}

unsigned int TextureRepository::GetIconTexture(EIconType iconType, int width, int height)
{
    unsigned int textureId = GetIconId(iconType);

    return GetTexture(textureId, width, height);
}

void TextureRepository::UnloadIconTexture(EIconType iconType)
{
    auto textureId = GetIconId(iconType);

    UnloadTexture(textureId);
}

unsigned int TextureRepository::GetTexture(gem::Image image, int w, int h, bool bSync /* = true */)
{
    auto it = m_ImagesUidToTextureId.find(image.getUid());

    // texture is loaded
    if (it != m_ImagesUidToTextureId.end() && (!bSync || it->second != -1))
        return it->second;

    // if texture was already ordered async, wait until available
    if (bSync && it != m_ImagesUidToTextureId.end())
    {
        while (it->second == -1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            it = m_ImagesUidToTextureId.find(image.getUid());
        }

        return it->second;
    }

    if (bSync)
    {
        // load texture for the first time
        auto bitmap = gem::StrongPointerFactory<BitmapImpl>(w, h);
        image.render(*bitmap);

        unsigned int textureId = LoadTextureIntoGPU(bitmap->size().width, bitmap->size().height, bitmap->begin());

        m_ImagesUidToTextureId.insert(std::make_pair<>(image.getUid(), textureId));

        return textureId;
    }
    else
    {
        auto lambda = [&, image, w, h]()
        {
            auto bitmap = gem::StrongPointerFactory<BitmapImpl>(w, h);
            image.render(*bitmap);

            auto lambda2 = [&, bitmap, image]()
            {
                unsigned int textureId = LoadTextureIntoGPU(bitmap->size().width, bitmap->size().height, bitmap->begin());
                m_ImagesUidToTextureId[image.getUid()] = textureId;
            };

            gem::OperationScheduler().timeoutOperation(20, lambda2, gem::ProgressListener(), true);
        };

        gem::OperationScheduler().executeOperation(lambda, gem::ProgressListener(), true);

        m_ImagesUidToTextureId.insert(std::make_pair<>(image.getUid(), -1));

        return -1;
    }
}

unsigned int TextureRepository::GetTexture(const gem::AbstractGeometryImage& image, const gem::AbstractGeometryImageRenderSettings& settings, int w, int h, bool bSync /*= true */)
{
    auto it = m_ImagesUidToTextureId.find(image.getUid());

    // texture is loaded
    if (it != m_ImagesUidToTextureId.end() && (!bSync || it->second != -1))
        return it->second;

    // if texture was already ordered async, wait until available
    if (bSync && it != m_ImagesUidToTextureId.end())
    {
        while (it->second == -1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            it = m_ImagesUidToTextureId.find(image.getUid());
        }

        return it->second;
    }

    if (bSync)
    {
        // load texture for the first time
        auto bitmap = gem::StrongPointerFactory<BitmapImpl>(w, h);
        image.render(*bitmap, settings);

        unsigned int textureId = LoadTextureIntoGPU(bitmap->size().width, bitmap->size().height, bitmap->begin());

        m_ImagesUidToTextureId.insert(std::make_pair<>(image.getUid(), textureId));

        return textureId;
    }
    else
    {
        auto lambda = [&, image, w, h]()
        {
            auto bitmap = gem::StrongPointerFactory<BitmapImpl>(w, h);
            image.render(*bitmap, settings);

            auto lambda2 = [&, bitmap, image]()
            {
                unsigned int textureId = LoadTextureIntoGPU(bitmap->size().width, bitmap->size().height, bitmap->begin());
                m_ImagesUidToTextureId[image.getUid()] = textureId;
            };

            gem::OperationScheduler().timeoutOperation(20, lambda2, gem::ProgressListener(), true);
        };

        gem::OperationScheduler().executeOperation(lambda, gem::ProgressListener(), true);

        m_ImagesUidToTextureId.insert(std::make_pair<>(image.getUid(), -1));

        return -1;
    }
}

unsigned int TextureRepository::GetTexture(unsigned int imageId, int w, int h, bool bSync )
{
    return GetTexture(gem::Image(imageId), w, h);
}

void TextureRepository::UnloadAllTextures()
{
    for (auto it : m_ImagesUidToTextureId)
        UnloadTexture(it.second);
}

void TextureRepository::UnloadTexture(unsigned int textureId)
{
    m_ImagesUidToTextureId.erase(textureId);

    UnloadTextureFromGPU(textureId);
}

unsigned int TextureRepository::GetIconId(EIconType iconType)
{
    switch (iconType)
    {
    case EIconType::MenuButton:
        return gem::image::Core::Menu_MoreDotsCircle;
    case EIconType::CurrentPosition:
        return gem::image::Core::Search_Results_Pin;
    }

    return -1;
}

unsigned int TextureRepository::LoadTextureIntoGPU(int width, int height, void* data)
{
    unsigned int textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Required on WebGL for non power-of-two textures

    // Upload pixels into texture
#ifdef GL_UNPACK_ROW_LENGTH
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return textureId;
}

void TextureRepository::UnloadTextureFromGPU(unsigned int& textureId)
{
    if (textureId != -1)
        glDeleteTextures(1, &textureId);
}

void TextureRepository::FillCountriesIsoToImageUids()
{
    // fill the map (iso country (int), imageUid) 
    auto collectCountries = [&](const gem::String& name, const gem::Image& icon, const gem::String& iso)
    {
        m_countriesIsoToImageUids.insert(std::make_pair<>(IsoToInt(iso), icon.getUid()));

        return true;
    };

    gem::MapDetails().iterateCountries(collectCountries);
}

unsigned int TextureRepository::GetFlagImageUid(const gem::String& iso)
{
    return m_countriesIsoToImageUids.find(IsoToInt(iso))->second;
}

gem::Image TextureRepository::GetFlagImage(const gem::String& iso)
{
    unsigned int imageId = GetFlagImageUid(iso);

    return gem::Image(imageId);
}

int TextureRepository::IsoToInt(const gem::String& iso)
{
    return iso[0] | (iso[1] << 8) | (iso[2] << 16);
}
