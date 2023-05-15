#pragma once

#include "BaseView.h"

class IMainWindow;
class NavigationViewModel;

class NavigationView : public BaseView
{
public:
    NavigationView( IMainWindow* parent );

    // IView methods
    IViewModel* GetViewModel() override;
    void SetViewModel( IViewModel* viewModel ) override;

    void Render() override;

    // IViewModelListener methods
    void OnEvent( EVmEvent event ) override;

private:
    NavigationViewModel* m_viewModel;

    int m_mapFilterIndex;
};
