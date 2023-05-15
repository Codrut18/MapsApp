#pragma once

#include "INavigationService.h"

class IViewFactory;
class IViewModelFactory;
class IView;
class IViewModel;

class NavigationService: public INavigationService
{
public:
    NavigationService();

    void SetViewFactory( IViewFactory* factory ) override;
    void SetViewModelFactory( IViewModelFactory* factory ) override;

    void SetViewChangeCallback( ViewChangeCallback callback ) override;

    bool GoToView( EView viewType, const ViewModelInitParams& params = ViewModelInitParams() );

private:
    IViewModelFactory* m_viewModelFactory;
    IViewFactory* m_viewFactory;

    IView* m_currentView;
    IViewModel* m_currentViewModel;

    ViewChangeCallback m_viewChangeCallback;
};
