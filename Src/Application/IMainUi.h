// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "IViewModelFactory.h"
#include "WindowInfo.h"
#include "IViewFactory.h"
#include "EView.h"

class IView;

using BeforeRenderCallback = std::function<void( void )>;

class IMainUi
{
public:
    virtual int Init( int width, int height ) = 0;

    virtual const WindowInfo& GetWindowInfo() = 0;

    virtual void SetBeforeRenderCallback( BeforeRenderCallback callback ) = 0;

    virtual void SetView( IView* view ) = 0;

    virtual void Show() = 0;

    // events related
    virtual void SetMouseEnabled(bool enabled) = 0;
    virtual void SetTouchEnabled(bool enabled) = 0;
    virtual void SetKeyEnabled(bool enabled) = 0;

    virtual ~IMainUi() = default;
};
