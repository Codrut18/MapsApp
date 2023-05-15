#pragma once

#include "IViewModelFactory.h"

class IMapService;
class INavigationService;
class IViewModelListener;

class ViewModelFactory : public IViewModelFactory
{
public:
    ViewModelFactory( IMapService* mapService, INavigationService* navigationService );

    IViewModel* BuildViewModel( EView viewType, IViewModelListener* listener ) override;

private:
    IMapService* m_mapService;
    INavigationService* m_navigationService;
};
