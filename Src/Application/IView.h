#pragma once

#include "IViewModelListener.h"

class IViewModel;

class IView : public IViewModelListener
{
public:
    virtual void SetViewModel( IViewModel* viewModel ) = 0;

    virtual IViewModel* GetViewModel() = 0;

    virtual void Render() = 0;

    virtual ~IView() = default;
};
