// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "Entities.h"

#include <memory>

#include "API/GEM_RoutingService.h"
#include "API/GEM_Landmark.h"
#include "API/GEM_ApiLists.h"

class IMapView
{
public:
    // coordinates & distances
    virtual Coordinates GetCursorWgsPosition() const = 0;
    virtual Coordinates GetXyWgsPosition( Xy xy ) const = 0;

    virtual bool CenterOnCoordinates( Coordinates coordinates, int zoomLevel = -1 ) = 0;
    virtual bool CenterOnDefaultCoordinates() = 0;

    // other stuff
    virtual bool IsFpsRender() const = 0;
    virtual void SetRenderFps( bool renderFps ) = 0;

    virtual void SetMapStyleById( LargeInteger styleId, bool smoothTransition = false ) = 0;

    // handle events
    virtual void HandleTouch( ETouchEvent touchEvent, LargeInteger touchId, Xy xy ) = 0;
    virtual bool HandleKey( EKey key, EKeyAction action ) = 0;
    virtual void HandleScroll( int delta, Xy xy ) = 0;

    // other methods
    virtual gem::Landmark* SelectWaypoint( gem::LandmarkList waypoints, gem::Xy xy ) = 0;
    virtual gem::Route SelectRoute( gem::Xy xy ) = 0;

    virtual void DisplayRoutes( gem::RouteList routes ) = 0;
    virtual void DisplayLandmarks( gem::LandmarkList landmarks, bool center ) = 0;

    virtual gem::Route KeepOnlyMainRoute() = 0;

    virtual void ClearRoutes() = 0;

    virtual void DeactivateAllHighlights() = 0;

    virtual bool IsFollowPosition() const = 0;
    virtual void SetFollowPosition( bool followPosition ) = 0;

    virtual ~IMapView() = default;
};

using IMapViewPtr = std::shared_ptr<IMapView>;
