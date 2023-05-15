#include "PreferencesViewModel.h"

#include "PreferencesView.h"

PreferencesViewModel::PreferencesViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : BaseViewModel( mapService, navigationService, listener )
{
    auto func1 = [&]() { m_navigationService->GoToView( EView::Main ); };

    SetMenuItems( { { "Main menu", func1 } } );
}

bool PreferencesViewModel::IsRenderFps() const
{
    return m_mapService->IsRenderFps();
}

void PreferencesViewModel::SetRenderFps( bool renderFps )
{
    m_mapService->SetRenderFps( renderFps );    
    if (m_mapView)
        m_mapView->SetRenderFps( renderFps );
}
