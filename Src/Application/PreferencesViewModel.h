#pragma once

#include "BaseViewModel.h"

#include "IView.h"

class PreferencesViewModel : public BaseViewModel
{
public:
    PreferencesViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener );

    // specific methods
    bool IsRenderFps() const;
    void SetRenderFps( bool renderFps );
};
