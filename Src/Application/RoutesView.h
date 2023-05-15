#pragma once

#include "BaseView.h"

class IMainWindow;
class RoutesViewModel;

class RoutesView : public BaseView
{
public:
    RoutesView( IMainWindow* parent );

    // IView methods
    IViewModel* GetViewModel() override;
    void SetViewModel( IViewModel* viewModel ) override;

    void Render() override;

    // IViewModelListener methods
    void OnEvent( EVmEvent event ) override;

private:
    RoutesViewModel* m_viewModel;
};
