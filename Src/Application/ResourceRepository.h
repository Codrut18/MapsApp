// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "IResourceRepository.h"

#include "ContentUpdateListener.h"

#include <API/GEM_ContentStore.h>

#include <map>
#include <set>
#include <mutex>
#include <vector>

const gem::EContentType MAP_TYPE = gem::EContentType::CT_RoadMap;
const gem::EContentType STYLE_TYPE = gem::EContentType::CT_ViewStyleHighRes;

enum class EResourceState
{
    Unavailable,
    Downloading,
    Available
};

class ResourceRepository : public IResourceRepository
{
public:
    ResourceRepository();
    ~ResourceRepository();

    void AddListener( IResourceRepositoryListener* listener ) override;
    void RemoveListener( IResourceRepositoryListener* listener ) override;

    // IResourceProvider methods
    bool IsResourceAvailable( EResourceType type ) override;

    gem::ContentStoreItemList GetMaps () override;
    gem::ContentStoreItemList GetStyles ();

    bool DownloadAsync( gem::ContentStoreItem& item ) override;

    EItemState GetItemState( const gem::ContentStoreItem& item ) const;

    bool IsMapUpdateRunning() const override;
    void SetCanApplyMapUpdate( bool canApplyMapUpdate ) override;

    void UpdateMaps() override;
    void UpdateStyles() override;

    void SetConnected ( bool connected );

    gem::Image GetFlagImage( const gem::String& iso ) override;

private:
    gem::ContentStoreItemList GetContentStoreItems( EResourceType type ) override;

    void ResumeExistingUpdates();

    void UpdateOfflineContentStores();
    void UpdateOnlineContentStores();

    void UpdateOnlineResource( gem::EContentType type );

    EResourceState GetContentTypeState( gem::EContentType contentType ) const;
    void SetContentTypeState( gem::EContentType contentType, EResourceState contentTypeState );

    void FillCountriesIsoToImageUids();

    unsigned int GetFlagImageUid( const gem::String& iso );

    int IsoToInt( const gem::String& iso );

private:
    std::map<gem::EContentType, EResourceState> m_contentTypesState;
    std::map<gem::EContentType, gem::ContentStoreItemList> m_offlineContentStores;
    std::map<gem::EContentType, gem::ContentStoreItemList> m_onlineContentStores;

    std::map<gem::LargeInteger, gem::StrongPointer<gem::IProgressListener>> m_downloads;
    std::map<gem::EContentType, gem::StrongPointer<gem::IProgressListener>> m_progressListeners;

    std::mutex m_contentUpdatersSync;

    gem::StrongPointer<gem::ContentUpdater> m_mapUpdater;
    gem::StrongPointer<ContentUpdateListener> m_mapUpdateListener;

    gem::StrongPointer<gem::ContentUpdater> m_mapStylesUpdater;
    gem::StrongPointer<ContentUpdateListener> m_mapStylesUpdateListener;

    bool m_bConnected;
    bool m_bCanApplyMapUpdate;

    std::vector<IResourceRepositoryListener*> m_listeners;

    std::map<int, unsigned int> m_countriesIsoToImageUids;
};
