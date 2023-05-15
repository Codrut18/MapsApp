// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "SDL_touch.h"
#include "SDL_events.h"

#include <functional>
#include <map>
#include <cstdint>

struct Pos
{
    Pos( int x = 0, int y = 0 )
        : x( x )
        , y( y )
    {

    }

    int x, y;
};

enum class EFingerCapture
{
    None,
    Ui,
    Map
};

struct CaptureInfo
{
    CaptureInfo( const Pos& xy );

    void UpdatePosition( const Pos& xy );

    EFingerCapture destination;
    Pos initialPosition;
    Pos lastKnownPosition;
    std::int64_t lastUsed;
    int captures;
};

class ActiveFingersCollection
{
public:
    void ClearOld( std::function<void( SDL_FingerID, const CaptureInfo& )> callback );

    CaptureInfo* UpdateFingerInfo( SDL_Event* event, const Pos& xy );

    CaptureInfo* Get( SDL_FingerID fingerId );

    void Remove( SDL_FingerID fingerId );

private:
    CaptureInfo* Add( SDL_FingerID fingerId, const Pos& xy );

private:
    std::map<SDL_FingerID, CaptureInfo> m_activeFingers;
};
