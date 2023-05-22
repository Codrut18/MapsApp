#include "MainViewModel.h"

#include "MainView.h"

MainViewModel::MainViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : BaseViewModel( mapService, navigationService, listener )
{
    UpdateMenuItems();
}

void MainViewModel::OnMapServiceEvent( EMapServiceEvent event )
{
    BaseViewModel::OnMapServiceEvent( event );

    if ( event == EMapServiceEvent::Connected || event == EMapServiceEvent::Disconnected )
        UpdateMenuItems();
}

void MainViewModel::UpdateMenuItems()
{
    auto func1 = [&]() { m_navigationService->GoToView( EView::Maps ); };
    auto func6 = [&]() { m_navigationService->GoToView( EView::Styles ); };
    auto func25 = [&]() { m_navigationService->GoToView( EView::Routes ); };
    auto func3 = [&]() { m_navigationService->GoToView( EView::Preferences ); };
    auto func4 = [&]() { m_mapService->SetAllowConnection( !GetMapService()->IsConnected() ); };
    auto func5 = [&]() { m_listener->OnEvent( EVmEvent::Generic_AppClose ); };

    const char* connectStr = GetMapService()->IsConnected() ? "Disconnect" : "Connect";
    SetMenuItems( {
        { "Maps", func1 },
        { "Routes", func25 },
        { "Preferences", func3 },
        { connectStr, func4 },
        { "Exit", func5 },
        {"Styles", func6},
        } );
}
