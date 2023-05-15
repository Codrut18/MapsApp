#pragma once

#include "IMapService.h"

#include "IViewModel.h"
#include "IViewModelListener.h"

#include "Entities.h"

#include "INavigationService.h"

#include <functional>

class IView;

class BaseViewModel : public IViewModel, public IMapServiceListener
{
public:
    BaseViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );
    ~BaseViewModel();

    // IViewModel methods
    void Init( const ViewModelInitParams& params ) override;

    void Scroll( int delta, Xy xy ) override;
    void Touch( ETouchEvent event, LargeInteger fingerId, Xy xy ) override;
    void Key( EKey key, EKeyAction action ) override;

    void Resize( int width, int height, float pixelRatio ) override;

    void BeforeViewRender() override;
    void AfterViewRender() override;

    const std::vector<std::string>& GetMenuItems() const override;
    void MenuItemSelected( int index ) override;

    bool IsConnected() const override;

    void UpdateMaps() override;

    // IMapServiceListener methods
    void OnMapServiceEvent( EMapServiceEvent event ) override;

protected:
    void SetMenuItems( const MenuItems& items );

    virtual IMapService* GetMapService();

protected:
    IMapService* m_mapService;
    INavigationService* m_navigationService;

    IViewModelListener* m_listener;

    IMapViewPtr m_mapView;

    MenuItems m_menuItems;
    std::vector<std::string> m_menuCaptions;

    Action m_action;
};
