#pragma once

#include "IView.h"

#include "EView.h"

#include <memory>

class IViewFactory
{
public:
    virtual IView* BuildView( EView view ) = 0;

    virtual ~IViewFactory() = default;
};
