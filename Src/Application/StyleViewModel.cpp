#include "StyleViewModel.h"

#include "MainView.h"

StyleViewModel::StyleViewModel(IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener)
    : BaseViewModel(mapService, navigationService, listener)
{
    UpdateMenuItems();
}

void StyleViewModel::Init(const ViewModelInitParams& params)
{
    auto it = params.find("mapView");
    if (it != params.end())
        m_mapView = std::any_cast<IMapViewPtr>(it->second);
    else
        m_mapView = GetMapService()->GetMapView();
}

void StyleViewModel::UpdateMenuItems()
{
    auto func1 = [&]()
    {
        ViewModelInitParams params;
        params["mapView"] = m_mapView;

        m_navigationService->GoToView(EView::Main, params);
    };

    SetMenuItems({
        { "Main", func1 },
        });
}

void StyleViewModel::SetStyleId(LargeInteger id)
{
    m_action = [&, id]() {
        m_mapView->SetMapStyleById(id, true); 
        m_navigationService->GoToView(EView::Main);

    };
    
}

IResourceRepository* StyleViewModel::GetResourceRepository()
{
    return GetMapService()->GetResourceRepository();
}

ITextureRepository* StyleViewModel::GetTextureRepository()
{
    return GetMapService()->GetTextureRepository();
}
