#pragma once

#include "BaseViewModel.h"

class MainViewModel : public BaseViewModel
{
public:
    MainViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );

    void OnMapServiceEvent( EMapServiceEvent event );

    void UpdateMenuItems();
};
