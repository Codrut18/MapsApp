// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "IMainWindow.h"
#include "BaseImGuiWindow.h"

#include "IViewModel.h"
#include "IViewFactory.h"

#include "SDL.h"

class IView;

class MainUi : public BaseImGuiWindow
{
public:
    MainUi();
    ~MainUi();

    // IMainUi methods
    const WindowInfo& GetWindowInfo() override;

    void SetBeforeRenderCallback( BeforeRenderCallback callback ) override;

    void SetView( IView* view ) override;

private:
    // BaseSdlWindow methods
    void OnResize() override;

    // BaseImGuiWindow methods
    void BuildUI() override;

    void OnNonUiScroll( int delta, Pos pos ) override;
    void OnNonUiTouch( SDL_EventType event, int64_t fingerId, int x, int y ) override;
    void OnNonUiKey( SDL_Scancode code, SDL_EventType action ) override;

    void OnBeforeRender() override;
    void OnAfterRender() override;

    void OnMenuItem( int index ) override;

private:
    IView* m_currentView;

    bool m_bMouseClicked;

    BeforeRenderCallback m_beforeUiRenderCallback;
};
