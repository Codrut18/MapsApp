// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "MapView.h"

// for bike could be something like the following:
// (std::int64_t(gem::CT_ViewStyleHighRes) << 32) | 2538
const LargeInteger DEFAULT_MAP_STYLE_ID = 0;

MapView::MapView( gem::StrongPointer<gem::Screen> screen, RectF area, float dpi )
    : m_bRenderFps( false )
    , m_dpi( 1 )
    , m_defaultCoordinates( 45.65119, 25.60480 )
    , m_defaultZoom( 70 )
{
    gem::RectF gemArea( area.x, area.y, area.width, area.height );

    m_pView = gem::MapView::produce( screen, gemArea, this );
    m_pView->preferences().enableCursorRender( false );

    m_pScreen = screen;

    m_dpi = dpi / 96;

    SetRenderFps( m_bRenderFps );

    CenterOnDefaultCoordinates();

    SetMapStyleById( DEFAULT_MAP_STYLE_ID );
}

Coordinates MapView::GetCursorWgsPosition() const
{
    auto coords = m_pView->getCursorWgsPosition();

    return Coordinates( coords.getLatitude(), coords.getLongitude() );
}

Coordinates MapView::GetXyWgsPosition( Xy xy ) const
{
    auto coords = m_pView->transformScreenToWgs( gem::Xy( xy.x, xy.y ) );
    return Coordinates( coords.getLatitude(), coords.getLongitude() );
}

bool MapView::CenterOnCoordinates( Coordinates coordinates, int zoomLevel /*= -1 */ )
{
    gem::Coordinates coords( coordinates.latitude, coordinates.longitude );

    if (!coords.isDefault())
    {
        m_pView->centerOnCoordinates( coords, zoomLevel, gem::Xy(), gem::Animation(), 0.0, 0.0 );

        return true;
    }

    return false;
}

bool MapView::CenterOnDefaultCoordinates()
{
    return CenterOnCoordinates( m_defaultCoordinates, m_defaultZoom );
}

bool MapView::IsFpsRender() const
{
    return m_bRenderFps;
}

void MapView::SetRenderFps( bool renderFps )
{
    m_bRenderFps = renderFps;

    if (m_pScreen && m_pView)
    {
        if (renderFps)
        {
            int height = m_pScreen->getViewport().height;
            m_pView->preferences().setDrawFPS( true, gem::Xy( 0, height / 2 ) );
        }
        else
            m_pView->preferences().setDrawFPS( false, gem::Xy() );
    }
}

void MapView::SetMapStyleById(LargeInteger styleId, bool smoothTransition /*= false*/)
{
    m_pView->preferences().setMapStyleById(styleId, smoothTransition);
}

#include "API/GEM_Debug.h"

void MapView::HandleTouch( ETouchEvent touchEvent, LargeInteger touchId, Xy xy )
{
    switch (touchEvent)
    {
    case ETouchEvent::TE_Down:
        gem::Debug().log( gem::LogInfo, "MapView", __FUNCTION__, __FILE__, __LINE__, "MapView::TOUCH_DOWN" );
        break;
    case ETouchEvent::TE_Move:
        gem::Debug().log( gem::LogInfo, "MapView", __FUNCTION__, __FILE__, __LINE__, "MapView::TOUCH_MOVE" );
        break;
    case ETouchEvent::TE_Up:
        gem::Debug().log( gem::LogInfo, "MapView", __FUNCTION__, __FILE__, __LINE__, "MapView::TOUCH_UP" );
        break;
    }

    m_pScreen->handleTouchEvent( gem::ETouchEvent(touchEvent), touchId, gem::Xy( xy.x, xy.y ) );
}

bool MapView::HandleKey( EKey key, EKeyAction action )
{
    return m_pView->handleKeyboardEvent( gem::EKey( key ), gem::EKeyAction( action ) );
}

void MapView::HandleScroll( int delta, Xy xy )
{
    m_pScreen->scrollEvent( delta, gem::Xy( xy.x, xy.y ) );
}

void MapView::DisplayRoutes( gem::RouteList routes )
{
    m_pView->deactivateAllHighlights();

    m_pView->preferences().routes().clear();
    m_pView->preferences().routes().add( routes[0], true );

    for ( int i = 1; i < routes.size(); i++ )
        m_pView->preferences().routes().add( routes[i], false );

    // compute routes rectangle
    int width = m_pScreen->getViewport().width;
    int height = m_pScreen->getViewport().height;

    int margin = m_dpi * 40;
    gem::Rect routesRect( margin, margin, width - margin, height - margin );

    // actually display the routes
    m_pView->centerOnRoutes( routes, gem::ERouteDisplayMode::RDM_Full, routesRect );
}

void MapView::DisplayLandmarks( gem::LandmarkList landmarks, bool center )
{
    m_pView->deactivateAllHighlights();

    gem::HighlightRenderSettings renderSettings( gem::HO_ShowLandmark );
    renderSettings.setImageSize( GetLandmarkImageSize() ).setTextSize( GetLandmarkFontSize() );

    m_pView->activateHighlight( landmarks, renderSettings );

    if ( center )
    {
        auto viewport = m_pView->getOptimalHighlightCenterViewport();
        viewport.inflate( -50, -50 );
        m_pView->centerOnArea( m_pView->getHighlightArea(), -1, viewport );
    }
}

void MapView::DisplayLandmark( gem::Landmark landmark, bool center )
{
    m_pView->deactivateAllHighlights();

    gem::HighlightRenderSettings renderSettings( gem::HO_ShowLandmark );
    renderSettings.setImageSize( GetLandmarkImageSize() ).setTextSize( GetLandmarkFontSize() );

    m_pView->activateHighlight( { landmark }, renderSettings );

    if ( center )
    {
        gem::Animation animation( gem::AnimationLinear, gem::ProgressListener(), 3000 );

        if ( landmark.getGeographicArea() )
            m_pView->centerOnArea( landmark.getGeographicArea(), -1, gem::Xy(), animation );
        else
            m_pView->centerOnCoordinates( landmark.getCoordinates(), 85, gem::Xy(), animation );
    }
}

gem::LandmarkList MapView::SelectLandmarks( gem::Xy xy )
{
    auto prevPos = m_pView->getCursorScreenPosition();

    m_pView->setCursorScreenPosition( xy );

    gem::LandmarkList result = m_pView->cursorSelectionLandmarks();

    m_pView->setCursorScreenPosition( prevPos );

    return result;
}

gem::Route MapView::SelectRoute( gem::Xy xy )
{
    auto prevPos = m_pView->getCursorScreenPosition();

    m_pView->setCursorScreenPosition( xy );

    // need to use gem::RouteList not auto (which will be gem::RouteListRef)
    // because result is updated when setCursorScreenPosition() is called
    gem::RouteList result = m_pView->cursorSelectionRoutes();

    m_pView->setCursorScreenPosition( prevPos );

    if ( !result.empty() )
        m_pView->preferences().routes().setMainRoute( result.front() );

    return result.empty() ? gem::Route() : result.front();
}

gem::Landmark* MapView::SelectWaypoint( gem::LandmarkList waypoints, gem::Xy xy )
{
    const gem::Landmark* selectedLandmark = nullptr;

    gem::LandmarkList landmarks = SelectLandmarks( xy );

    for ( auto& resItem : landmarks )
    {
        for ( auto& pLandmark : waypoints )
            if ( pLandmark.getName() == resItem.getName() )
                selectedLandmark = &pLandmark;

        if ( selectedLandmark )
            break;
    }

    return const_cast<gem::Landmark*>( selectedLandmark );
}

void MapView::ClearRoutes()
{
    m_pView->preferences().routes().clear();
}

void MapView::DeactivateAllHighlights()
{
    m_pView->deactivateAllHighlights();
}

gem::Route MapView::KeepOnlyMainRoute()
{
    gem::Route mainRoute = m_pView->preferences().routes().getMainRoute();

    m_pView->preferences().routes().clear();
    m_pView->preferences().routes().add( mainRoute, true );

    return mainRoute;
}

bool MapView::IsFollowPosition() const
{
    return m_pView->isFollowingPosition();
}

void MapView::SetFollowPosition( bool followPosition )
{
    if ( followPosition )
        m_pView->startFollowingPosition( gem::Animation( gem::EAnimation::AnimationLinear, gem::ProgressListener(), 1600 ), -1, 25 );
    else
        m_pView->stopFollowingPosition();
}

void MapView::onLongDown( const gem::Xy& xy )
{
    
}

int MapView::GetLandmarkImageSize() const
{
    return m_dpi * 32;
}

int MapView::GetLandmarkFontSize() const
{
    return m_dpi * 12;
}
