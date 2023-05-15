#pragma once

#include "BaseView.h"

class IMainWindow;
class PreferencesViewModel;

class PreferencesView : public BaseView
{
public:
    PreferencesView( IMainWindow* parent );

    // IView methods
    IViewModel* GetViewModel() override;
    void SetViewModel( IViewModel* viewModel ) override;

    void Render() override;

    // IViewModelListener methods
    void OnEvent( EVmEvent event ) override;

private:
    PreferencesViewModel* m_viewModel;

    bool m_bRenderFps;
};
