#pragma once

#include "EView.h"

#include <memory>

class IView;
class IViewModel;
class IViewModelListener;

class IViewModelFactory
{
public:
    virtual IViewModel* BuildViewModel( EView viewType, IViewModelListener* listener ) = 0;

    virtual ~IViewModelFactory() = default;
};
