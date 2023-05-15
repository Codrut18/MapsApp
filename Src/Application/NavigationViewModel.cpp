#include "NavigationViewModel.h"

#include "MainView.h"

NavigationViewModel::NavigationViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : BaseViewModel( mapService, navigationService, listener )
    , m_bIsFollowPosition( m_mapView->IsFollowPosition() )
{
    UpdateMenuItems();
}

void NavigationViewModel::Init( const ViewModelInitParams& params )
{
    m_mapView = GetMapService()->GetMapView();

    StartSimulation();
}

ITextureRepository* NavigationViewModel::GetTextureRepository()
{
    return GetMapService()->GetTextureRepository();
}

gem::NavigationInstruction NavigationViewModel::GetNavigationInstruction()
{
    return m_mapService->GetNavigationInstruction();
}

void NavigationViewModel::BeforeViewRender()
{
    if ( m_mapView->IsFollowPosition() != m_bIsFollowPosition )
    {
        m_bIsFollowPosition = m_mapView->IsFollowPosition();
        UpdateMenuItems();
    }

    BaseViewModel::BeforeViewRender();
}

void NavigationViewModel::UpdateMenuItems()
{
    auto func1 = [&]() { StopSimulation(); };
    auto func2 = [&]() { m_mapView->SetFollowPosition( !m_mapView->IsFollowPosition() ); };

    const char* followText = m_mapView->IsFollowPosition() ? "Unfollow position" : "Follow position";

    SetMenuItems( {
        { "Stop simulation", func1 },
        { followText, func2 }
        } );
}

void NavigationViewModel::StartSimulation()
{
    gem::Route route = m_mapView->KeepOnlyMainRoute();

    auto destinationReachedFunc = [&]() {
        m_mapView->ClearRoutes();
        m_mapView->DeactivateAllHighlights();

        m_navigationService->GoToView( EView::Main );
    };

    int err = GetMapService()->StartNavigation( route, destinationReachedFunc );

    if ( err == gem::KNoError )
        m_mapView->SetFollowPosition( true );
    else
    {
        m_action = [&]() { m_navigationService->GoToView( EView::Routes ); };
        // DisplayMessage( "Error", "Navigation error: %d", err );
    }
}

void NavigationViewModel::StopSimulation()
{
    GetMapService()->StopNavigation();

    m_mapView->ClearRoutes();
    m_mapView->DeactivateAllHighlights();

    m_navigationService->GoToView( EView::Main );
}
