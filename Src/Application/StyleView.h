#pragma once

#include "BaseView.h"

class IMainWindow;
class StyleViewModel;

class StyleView : public BaseView
{
public:
    StyleView(IMainWindow* parent);

    // IView methods
    IViewModel* GetViewModel() override;
    void SetViewModel(IViewModel* viewModel) override;

    void Render() override;

    // IViewModelListener methods
    void OnEvent(EVmEvent event) override;

private:
    StyleViewModel* m_viewModel;

    int m_mapFilterIndex;
};
