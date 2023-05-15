#pragma once

#include "EView.h"

#include "IViewModel.h"
#include "IViewFactory.h"
#include "IViewModelFactory.h"

#include <functional>
#include <memory>

using ViewChangeCallback = std::function<void( IView* )>;

class INavigationService
{
public:
    virtual bool GoToView( EView view, const ViewModelInitParams& params = ViewModelInitParams() ) = 0;

    virtual void SetViewFactory( IViewFactory* factory ) = 0;
    virtual void SetViewModelFactory( IViewModelFactory* factory ) = 0;

    virtual void SetViewChangeCallback( ViewChangeCallback callback ) = 0;

    virtual ~INavigationService() = default;
};

using INavigationServicePtr = std::unique_ptr<INavigationService>;
