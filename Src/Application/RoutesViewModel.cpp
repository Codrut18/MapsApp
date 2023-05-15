#include "RoutesViewModel.h"

#include "MainView.h"

#include "API/GEM_ImageIDs.h"

RoutesViewModel::RoutesViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : BaseViewModel( mapService, navigationService, listener )
    , m_state( ERoutesState::PoiSelection )
{
    m_waypoints.push_back( gem::Landmark( "Departure", { 45.65119, 25.60480 } )
        .setImage( gem::image::Core::Waypoint_Start ) );

    m_waypoints.push_back( gem::Landmark( "Arrival", { 45.84494, 24.97414 } )
        .setImage( gem::image::Core::Waypoint_Finish ) );

    UpdateMenuItems();

    m_mapView->DisplayLandmarks( m_waypoints, true );
}

void RoutesViewModel::Scroll( int delta, Xy xy )
{
    if ( m_state == ERoutesState::RoutesComputed )
        return;

    BaseViewModel::Scroll( delta, xy );
}

void RoutesViewModel::UpdateMenuItems()
{
    switch ( m_state )
    {
    case ERoutesState::PoiSelection:
    {
        auto func1 = [&]() 
        {
            ComputeRoutes();
        };
        auto func2 = [&]() {
            m_mapView->DeactivateAllHighlights();
            m_navigationService->GoToView( EView::Main ); 
        };

        SetMenuItems( {
            { "Compute routes", func1 },
            { "Main", func2 },
            } );

        break;
    }
    case ERoutesState::RoutesComputing:
    {
        auto func1 = [&]() { CancelComputeRoutes(); };

        SetMenuItems( {
            { "Abort computation", func1 }
            } );

        break;
    }
    case ERoutesState::RoutesComputed:
    {
        auto func1 = [&]() { 
            m_navigationService->GoToView( EView::Navigation );
        };
        auto func2 = [&]()
        {
            m_mapView->ClearRoutes();
            m_mapView->DisplayLandmarks( m_waypoints, true );
            SetState( ERoutesState::PoiSelection );
        };
        auto func3 = [&]() { 
            m_mapView->ClearRoutes();
            m_navigationService->GoToView( EView::Main ); 
        };

        SetMenuItems( {
            { "Start simulation", func1 },
            { "Clear routes", func2 },
            { "Main", func3 },
            } );

        break;
    }
    }
}

void RoutesViewModel::Touch( ETouchEvent event, LargeInteger fingerId, Xy xy )
{
    if ( m_state == ERoutesState::PoiSelection )
    {
        switch ( event )
        {
        case ETouchEvent::TE_Down:
        {
            gem::Xy gemXy( xy.x, xy.y );

            if ( m_selectedWaypoint = m_mapView->SelectWaypoint( GetDepartureArrivalLandmarks(), gemXy ) )
                return;

            break;
        }
        case ETouchEvent::TE_Move:
        {
            if ( m_selectedWaypoint )
                return;

            break;
        }
        case ETouchEvent::TE_Up:
        {
            if ( m_selectedWaypoint )
            {
                Coordinates coordinates = m_mapView->GetXyWgsPosition( xy );
                gem::Coordinates gemCoordinates( coordinates.latitude, coordinates.longitude );

                m_selectedWaypoint->setCoordinates( gemCoordinates );
                m_selectedWaypoint = nullptr;

                m_mapView->DisplayLandmarks( GetDepartureArrivalLandmarks(), false );
                return;
            }

            break;
        }
        }
    }

    if ( m_state == ERoutesState::RoutesComputed )
    {
        if ( gem::Route route = m_mapView->SelectRoute( gem::Xy( xy.x, xy.y ) ) )
        {

        }

        return;
    }
        
    BaseViewModel::Touch( event, fingerId, xy );
}

gem::LandmarkList RoutesViewModel::GetDepartureArrivalLandmarks() const
{
    return m_waypoints;
}

void RoutesViewModel::UpdateWaypointCoordinates( int index, gem::Coordinates coordinates )
{
    m_waypoints.toStd().at( index ).setCoordinates( coordinates );
}

void RoutesViewModel::ComputeRoutes()
{
    SetState( ERoutesState::RoutesComputing );

    int err = GetMapService()->ComputeRoutes( m_waypoints, [&]( int reason, gem::String hint, gem::RouteList routes )
        {
            if ( reason == gem::KNoError )
            {
                if ( !routes.empty() )
                    m_mapView->DisplayRoutes( routes );

                SetState( routes.empty() ? ERoutesState::PoiSelection : ERoutesState::RoutesComputed );
            }
            else
            {
                SetState( ERoutesState::PoiSelection );
                // DisplayMessage( "Error", "Route calculation error: %d %s", reason, hint.toStdString().c_str() );
            }
        } );


    if ( err != gem::KNoError )
    {
        SetState( ERoutesState::PoiSelection );
        // DisplayMessage( "Error", "Route calculation error: %d", err );
    }
}

void RoutesViewModel::CancelComputeRoutes()
{
    GetMapService()->CancelComputeRoutes();
}

void RoutesViewModel::SetState( ERoutesState state )
{
    m_state = state;

    UpdateMenuItems();
}
