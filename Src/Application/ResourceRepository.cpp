// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "ResourceRepository.h"

#include "ProgressListenerImpl.h"

#include <API/GEM_Debug.h>
#include <API/GEM_MapDetails.h>

#include <functional>

using ContentStoreCompleteFunc = std::function<void( int, const gem::LargeInteger )>;

class ContentStoreItemListener : public gem::IProgressListener
{
public:
    ContentStoreItemListener( ContentStoreCompleteFunc func, gem::LargeInteger id )
        : m_func( func )
        , m_id( id )
    {
    }

private:
    void notifyStart( bool hasProgress ) override {}
    void notifyComplete( int reason, gem::String ) override
    {
        m_func( reason, m_id );
    }

private:
    ContentStoreCompleteFunc m_func;
    gem::LargeInteger m_id;
};

ResourceRepository::ResourceRepository()
    : m_bConnected( false )
    , m_bCanApplyMapUpdate ( true )
{
    SetConnected( false );

    // handle map styles update
    m_mapStylesUpdater = gem::ContentStore().createContentUpdater( STYLE_TYPE ).first;

    auto handleNewMapStylesStatus = [ &, upd = gem::WeakPointer<gem::ContentUpdater>(m_mapStylesUpdater) ]( int status )
    {
        switch( auto contentUpdaterStatus = gem::EContentUpdaterStatus( status ) )
        {
        case gem::EContentUpdaterStatus::FullyReady:
        case gem::EContentUpdaterStatus::PartiallyReady:
        {
            if( auto updater = upd.lock() )
            {
                updater->apply();

                UpdateOnlineResource( STYLE_TYPE );

                for ( auto it : m_listeners )
                    it->OnResourceUpdated( EResourceType::Style );
            }
            break;
        }
        }
    };

    m_mapStylesUpdateListener = gem::StrongPointerFactory<ContentUpdateListener>( handleNewMapStylesStatus );

    // handle maps update
    m_mapUpdater = gem::ContentStore().createContentUpdater( MAP_TYPE ).first;

    auto handleNewMapStatus = [ &, upd = gem::WeakPointer<gem::ContentUpdater>( m_mapUpdater ) ](int status)
        {
            switch( auto contentUpdaterStatus = gem::EContentUpdaterStatus( status ) )
            {
            case gem::EContentUpdaterStatus::FullyReady:
            case gem::EContentUpdaterStatus::PartiallyReady:
            {
                if( auto updater = upd.lock() )
                {
                    if( m_bCanApplyMapUpdate )
                    {
                        updater->apply();

                        UpdateOnlineResource( MAP_TYPE );

                        for(auto it : m_listeners )
                            it->OnResourceUpdated(EResourceType::Map);
                    }
                }
                break;
            }
            }
        };

    m_mapUpdateListener = gem::StrongPointerFactory<ContentUpdateListener>( handleNewMapStatus );

    FillCountriesIsoToImageUids();
}

ResourceRepository::~ResourceRepository()
{
    std::lock_guard<std::mutex> guard( m_contentUpdatersSync );

    for( auto it : m_downloads )
        gem::ContentStore().cancel( it.second );

    m_downloads.clear();

    m_mapUpdateListener.reset();
    m_mapUpdater.reset();

    m_mapStylesUpdateListener.reset();
    m_mapStylesUpdater.reset();
}

void ResourceRepository::AddListener( IResourceRepositoryListener* listener )
{
    m_listeners.push_back( listener );
}

void ResourceRepository::RemoveListener( IResourceRepositoryListener* listener )
{
    for ( auto it = m_listeners.begin(); it != m_listeners.end(); )
        if ( *it == listener )
            it = m_listeners.erase( it );
        else
            it++;
}

bool ResourceRepository::IsResourceAvailable( EResourceType type )
{
    gem::EContentType contentType = gem::EContentType::CT_Unknown;

    switch (type)
    {
    case EResourceType::Map:
        contentType = MAP_TYPE;
        break;
    case EResourceType::Style:
        contentType = STYLE_TYPE;
        break;
    }

    if (contentType == gem::EContentType::CT_Unknown)
        return false;

    return GetContentTypeState( contentType ) == EResourceState::Available;
}

gem::ContentStoreItemList ResourceRepository::GetMaps ()
{
    return GetContentStoreItems ( EResourceType::Map );
}

gem::ContentStoreItemList ResourceRepository::GetStyles ()
{
    return GetContentStoreItems ( EResourceType::Style );
}

bool ResourceRepository::DownloadAsync( gem::ContentStoreItem& item )
{
    std::lock_guard<std::mutex> guard( m_contentUpdatersSync );

    auto func = [&]( int reason, gem::LargeInteger itemId )
    {
        m_downloads.erase( itemId );
    };

    gem::StrongPointer<ContentStoreItemListener> listenerPtr = gem::StrongPointerFactory<ContentStoreItemListener>( func, item.getId() );

    if (item.asyncDownload( listenerPtr ) == gem::KNoError)
    {
        m_downloads.insert( std::make_pair<>( gem::LargeInteger( item.getId() ), listenerPtr ) );
        return true;
    }

    return false;
}

EItemState ResourceRepository::GetItemState( const gem::ContentStoreItem& item ) const
{
    switch (item.getStatus())
    {
    case gem::EContentStoreItemStatus::CIS_Completed:
        return EItemState::Completed;
    case gem::EContentStoreItemStatus::CIS_Unavailable:
        return EItemState::Unavailable;
    case gem::EContentStoreItemStatus::CIS_DownloadQueued:
    case gem::EContentStoreItemStatus::CIS_DownloadWaiting:
    case gem::EContentStoreItemStatus::CIS_DownloadWaitingFreeNetwork:
    case gem::EContentStoreItemStatus::CIS_DownloadRunning:
    case gem::EContentStoreItemStatus::CIS_UpdateWaiting:
        return EItemState::InProgress;
    case gem::EContentStoreItemStatus::CIS_Paused:
        return EItemState::Paused;
    }

    return EItemState::Other;
}

bool ResourceRepository::IsMapUpdateRunning() const
{
    return m_mapUpdater->isStarted();
}

void ResourceRepository::SetCanApplyMapUpdate( bool canApplyMapUpdate )
{
    m_bCanApplyMapUpdate = canApplyMapUpdate;
}

void ResourceRepository::UpdateMaps()
{
    m_mapUpdater->update( true, m_mapUpdateListener );
}

void ResourceRepository::UpdateStyles()
{
    m_mapStylesUpdater->update( true, m_mapStylesUpdateListener );
}

void ResourceRepository::SetConnected( bool connected )
{
    if ( connected )
    {
        if ( !m_bConnected )
        {
            m_bConnected = connected;
            UpdateOnlineContentStores();
        }

        ResumeExistingUpdates();
    }
    else
    {
        m_bConnected = false;
        UpdateOfflineContentStores();
    }
}

gem::Image ResourceRepository::GetFlagImage( const gem::String& iso )
{
	unsigned int imageId = GetFlagImageUid( iso );

	return gem::Image( imageId );
}

gem::ContentStoreItemList ResourceRepository::GetContentStoreItems( EResourceType type )
{
    gem::EContentType contentType = gem::EContentType::CT_Unknown;

    switch ( type )
    {
    case EResourceType::Map:
        contentType = MAP_TYPE;
        break;
    case EResourceType::Style:
        contentType = STYLE_TYPE;
        break;
    }

    if ( contentType != gem::EContentType::CT_Unknown && GetContentTypeState( contentType ) == EResourceState::Available )
    {
        if ( m_bConnected )
        {
            auto it = m_onlineContentStores.find( contentType );
            return it == m_onlineContentStores.end() ? gem::ContentStoreItemList() : it->second;
        }
        else
        {
            auto it = m_offlineContentStores.find( contentType );
            return it == m_offlineContentStores.end() ? gem::ContentStoreItemList() : it->second;
        }
    }

    return gem::ContentStoreItemList();
}

void ResourceRepository::ResumeExistingUpdates()
{
    // resume existing updates
    if( m_mapStylesUpdater->getStatus() != gem::EContentUpdaterStatus::Idle )
        m_mapStylesUpdater->update( true, m_mapStylesUpdateListener );

    if( m_mapUpdater->getStatus() != gem::EContentUpdaterStatus::Idle )
        m_mapUpdater->update( true, m_mapUpdateListener );

}

void ResourceRepository::UpdateOfflineContentStores()
{
    m_offlineContentStores.clear();

    m_offlineContentStores.insert( std::make_pair<>( MAP_TYPE, gem::ContentStore().getLocalContentList( MAP_TYPE ) ) );
    SetContentTypeState( MAP_TYPE, EResourceState::Available );

    m_offlineContentStores.insert( std::make_pair<>( STYLE_TYPE, gem::ContentStore().getLocalContentList( STYLE_TYPE ) ) );
    SetContentTypeState( STYLE_TYPE, EResourceState::Available );
}

void ResourceRepository::UpdateOnlineContentStores()
{
    gem::Debug().log( gem::LogInfo, "ResourceRepository", __FUNCTION__, __FILE__, __LINE__, "Update online content store" );

    SetContentTypeState( STYLE_TYPE, EResourceState::Unavailable );
    SetContentTypeState( MAP_TYPE, EResourceState::Unavailable );

    UpdateOnlineResource( STYLE_TYPE );
    UpdateOnlineResource( MAP_TYPE );
}

void ResourceRepository::UpdateOnlineResource( gem::EContentType contentType )
{
    auto res = gem::ContentStore().getStoreContentList( contentType );
    if( res.second )
    {
        m_onlineContentStores.insert( std::make_pair<>( contentType, res.first ) );
        SetContentTypeState( contentType, EResourceState::Available );
    }
    else
    {
        SetContentTypeState( contentType, EResourceState::Downloading );

        auto func = [&, contentType]( int reason, gem::String hint )
        {
            if( reason == gem::KNoError )
            {
                auto res = gem::ContentStore().getStoreContentList( contentType );

                m_onlineContentStores[contentType] = res.first;

                SetContentTypeState( contentType, EResourceState::Available );
            }
        };

        auto progressListener = gem::StrongPointerFactory<ProgressListenerImpl>( func );
        m_progressListeners[contentType] = progressListener;
        gem::ContentStore().asyncGetStoreContentList( contentType, progressListener );
    }
}

EResourceState ResourceRepository::GetContentTypeState( gem::EContentType contentType ) const
{
    auto it = m_contentTypesState.find( contentType );

    return it == m_contentTypesState.end() ? EResourceState::Unavailable : it->second;
}


void ResourceRepository::SetContentTypeState( gem::EContentType contentType, EResourceState contentTypeState )
{
    gem::Debug().log( gem::LogInfo, "ResourceRepository", __FUNCTION__, __FILE__, __LINE__, "Set content type state(%d) = %d", int( contentType ), int( contentTypeState ) );

    m_contentTypesState[contentType] = contentTypeState;
}

void ResourceRepository::FillCountriesIsoToImageUids()
{
    // fill the map (iso country (int), imageUid) 
    auto collectCountries = [&]( const gem::String& name, const gem::Image& icon, const gem::String& iso )
    {
        m_countriesIsoToImageUids.insert( std::make_pair<>( IsoToInt( iso ), icon.getUid() ) );

        return true;
    };

    gem::MapDetails().iterateCountries( collectCountries );
}

unsigned int ResourceRepository::GetFlagImageUid( const gem::String& iso )
{
    return m_countriesIsoToImageUids.find( IsoToInt( iso ) )->second;
}

int ResourceRepository::IsoToInt( const gem::String& iso )
{
    return iso[0] | (iso[1] << 8) | (iso[2] << 16);
}
