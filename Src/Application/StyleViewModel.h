#pragma once

#include "BaseViewModel.h"

class StyleViewModel : public BaseViewModel
{
public:
    StyleViewModel(IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener);

    void Init(const ViewModelInitParams& params) override;

    void UpdateMenuItems();

    void SetStyleId(LargeInteger id);

    IResourceRepository* GetResourceRepository();
    ITextureRepository* GetTextureRepository();
};
