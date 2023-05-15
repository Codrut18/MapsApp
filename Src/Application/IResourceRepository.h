// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "API/GEM_ApiLists.h"

enum class EResourceType
{
    Map,
    Style
};

enum class EItemState
{
    Completed = 1,
    Unavailable,
    InProgress,
    Paused,
    Other
};

class IResourceRepositoryListener
{
public:
    virtual void OnResourceUpdated ( EResourceType resType ) = 0;

    virtual ~IResourceRepositoryListener () = default;
};

class IResourceRepository
{
public:
    virtual void AddListener( IResourceRepositoryListener* listener ) = 0;
    virtual void RemoveListener( IResourceRepositoryListener* listener ) = 0;

    virtual bool IsResourceAvailable( EResourceType type ) = 0;

    virtual gem::ContentStoreItemList GetMaps() = 0;
    virtual gem::ContentStoreItemList GetStyles() = 0;
    virtual gem::ContentStoreItemList GetContentStoreItems( EResourceType type ) = 0;

    virtual bool DownloadAsync( gem::ContentStoreItem& item ) = 0;

    virtual EItemState GetItemState( const gem::ContentStoreItem& item ) const = 0;

    virtual bool IsMapUpdateRunning() const = 0;

    virtual void UpdateMaps() = 0;
    virtual void UpdateStyles() = 0;

    virtual void SetCanApplyMapUpdate( bool canApplyMapUpdate ) = 0;

    virtual void SetConnected( bool connected ) = 0;

    virtual gem::Image GetFlagImage( const gem::String& iso ) = 0;

    virtual ~IResourceRepository() = default;
};
using IResourceRepositoryPtr = std::shared_ptr<IResourceRepository>;
