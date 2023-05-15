#include "MapsViewModel.h"

#include "MainView.h"

MapsViewModel::MapsViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : BaseViewModel( mapService, navigationService, listener )
{
    UpdateMenuItems();
}

void MapsViewModel::Init( const ViewModelInitParams& params )
{
    auto it = params.find( "mapView" );
    if (it != params.end())
        m_mapView = std::any_cast<IMapViewPtr>(it->second);
    else
        m_mapView = GetMapService()->GetMapView();
}

void MapsViewModel::UpdateMenuItems()
{
    auto func1 = [&]()
    {
        ViewModelInitParams params;
        params["mapView"] = m_mapView;

        m_navigationService->GoToView( EView::Main, params );
    };

    SetMenuItems( {
        { "Main", func1 },
        } );
}

IResourceRepository* MapsViewModel::GetResourceRepository()
{
    return GetMapService()->GetResourceRepository();
}

ITextureRepository* MapsViewModel::GetTextureRepository()
{
    return GetMapService()->GetTextureRepository();
}
