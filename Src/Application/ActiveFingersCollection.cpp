// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "ActiveFingersCollection.h"

#include <cstdint>
#include <chrono>

static std::int64_t GetCurrentTimeMillis();

CaptureInfo::CaptureInfo( const Pos& xy ) 
    : destination( EFingerCapture::None )
    , initialPosition( xy )
    , lastKnownPosition( xy )
    , lastUsed( GetCurrentTimeMillis() )
    , captures( 1 )
{

}

void CaptureInfo::UpdatePosition( const Pos& xy )
{
    lastKnownPosition = xy;
    lastUsed = GetCurrentTimeMillis();
    captures++;
}

static std::int64_t GetCurrentTimeMillis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock().now().time_since_epoch()).count();
}

void ActiveFingersCollection::ClearOld( std::function<void( SDL_FingerID, const CaptureInfo& )> callback )
{
    // remove from activeFingers events not used for >5 seconds & not about the current event
    auto now = GetCurrentTimeMillis();

    for (auto it = m_activeFingers.begin(); it != m_activeFingers.end(); )
    {
        if (now - it->second.lastUsed > 5000)
        {
            callback( it->first, it->second );

            it = m_activeFingers.erase( it );
        }
        else
            it++;
    }
}

CaptureInfo* ActiveFingersCollection::UpdateFingerInfo( SDL_Event* event, const Pos& xy )
{
    // retrieve active finger information if any
    CaptureInfo* captureInfo = nullptr;

    // updated active finger events map
    if (event->type == SDL_FINGERDOWN)
        captureInfo = Add( event->tfinger.fingerId, xy );
    else
    {
        captureInfo = Get( event->tfinger.fingerId );
        captureInfo->UpdatePosition( xy );
    }

    return captureInfo;
}

CaptureInfo* ActiveFingersCollection::Get( SDL_FingerID fingerId )
{
    auto it = m_activeFingers.find( fingerId );

    if (it == m_activeFingers.end())
        return nullptr;

    return &it->second;
}

void ActiveFingersCollection::Remove( SDL_FingerID fingerId )
{
    m_activeFingers.erase( fingerId );
}

CaptureInfo* ActiveFingersCollection::Add( SDL_FingerID fingerId, const Pos& xy )
{
    auto it = m_activeFingers.insert( std::make_pair<>( fingerId, CaptureInfo( xy ) ) );

    return &it.first->second;
}
