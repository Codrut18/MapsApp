#pragma once

#include "BaseViewModel.h"

#include "API/GEM_NavigationInstruction.h"

class NavigationViewModel : public BaseViewModel
{
public:
    NavigationViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );

    void Init( const ViewModelInitParams& params ) override;

    ITextureRepository* GetTextureRepository();

    gem::NavigationInstruction GetNavigationInstruction();

private:
    void BeforeViewRender();

    void UpdateMenuItems();

    void StartSimulation();
    void StopSimulation();

    bool m_bIsFollowPosition;
};
