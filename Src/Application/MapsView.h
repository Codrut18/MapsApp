#pragma once

#include "BaseView.h"

class IMainWindow;
class MapsViewModel;

class MapsView : public BaseView
{
public:
    MapsView( IMainWindow* parent );

    // IView methods
    IViewModel* GetViewModel() override;
    void SetViewModel( IViewModel* viewModel ) override;

    void Render() override;

    // IViewModelListener methods
    void OnEvent( EVmEvent event ) override;

private:
    MapsViewModel* m_viewModel;

    int m_mapFilterIndex;
};
