#pragma once

#include "BaseViewModel.h"

#include "API/GEM_Landmark.h"
#include "API/GEM_ApiLists.h"

enum class ERoutesState
{
    PoiSelection,
    RoutesComputing,
    RoutesComputed
};

class RoutesViewModel : public BaseViewModel
{
public:
    RoutesViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );

    void Scroll( int delta, Xy xy );

    void UpdateMenuItems();

private:
    void Touch( ETouchEvent event, LargeInteger fingerId, Xy xy ) override;

    gem::LandmarkList GetDepartureArrivalLandmarks() const;
    void UpdateWaypointCoordinates( int index, gem::Coordinates coordinates );

    void ComputeRoutes();
    void CancelComputeRoutes();

private:
    void SetState( ERoutesState state );

private:
    ERoutesState m_state;

    gem::LandmarkList m_waypoints;
    gem::Landmark* m_selectedWaypoint;
};
