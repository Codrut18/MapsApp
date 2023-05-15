#pragma once

#include "BaseViewModel.h"

class MapsViewModel : public BaseViewModel
{
public:
    MapsViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );

    void Init( const ViewModelInitParams& params ) override;

    void UpdateMenuItems();

    IResourceRepository* GetResourceRepository();
    ITextureRepository* GetTextureRepository();
};
