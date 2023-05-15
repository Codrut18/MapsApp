// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "MainUi.h"

#include "IViewModel.h"

#include <algorithm>

MainUi::MainUi()
    : BaseImGuiWindow()
    , m_currentView( nullptr )
    , m_bMouseClicked( false )
{

}

MainUi::~MainUi()
{
    if ( m_currentView )
    {
        delete m_currentView->GetViewModel();
        delete m_currentView;
    }
}

const WindowInfo& MainUi::GetWindowInfo()
{
    return BaseSdlWindow::GetWindowInfo();
}

void MainUi::SetBeforeRenderCallback( BeforeRenderCallback callback )
{
    m_beforeUiRenderCallback = callback;
}

void MainUi::SetView( IView* view )
{
	m_currentView = view;
}

void MainUi::OnResize()
{
    if (m_currentView)
        m_currentView->GetViewModel()->Resize( m_windowInfo.width, m_windowInfo.height, m_windowInfo.pixelRatio );
}

void MainUi::BuildUI()
{
    m_beforeUiRenderCallback();

    m_currentView->Render();

    DisplayMenuButton();    
    if (IsDisplayMenu())
        Menu( m_currentView->GetViewModel()->GetMenuItems() );
}

void MainUi::OnNonUiScroll( int delta, Pos pos )
{
    m_currentView->GetViewModel()->Scroll( delta * 1000, Xy( pos.x, pos.y ) );
}

void MainUi::OnNonUiTouch( SDL_EventType event, int64_t fingerId, int x, int y )
{
    switch (event)
    {
    case SDL_FINGERDOWN:
    {
        m_bMouseClicked = true;

        m_currentView->GetViewModel()->Touch( ETouchEvent::TE_Down, fingerId, Xy( x, y ) );

        break;
    }
    case SDL_FINGERUP:
    {
        if (m_bMouseClicked || fingerId)
            m_currentView->GetViewModel()->Touch( ETouchEvent::TE_Up, fingerId, Xy( x, y ) );

        m_bMouseClicked = false;
        break;
    }
    case SDL_FINGERMOTION:
    {
        if (m_bMouseClicked)
            m_currentView->GetViewModel()->Touch( ETouchEvent::TE_Move, fingerId, Xy( x, y ) );
        break;
    }
    }
}

static EKey GetKey( SDL_Scancode scanCode )
{
    switch (scanCode)
    {
    case SDL_SCANCODE_0:
        return EKey::K_0;
    case SDL_SCANCODE_1:
        return EKey::K_1;
    case SDL_SCANCODE_2:
        return EKey::K_2;

    case SDL_SCANCODE_W:
        return EKey::K_W;
    case SDL_SCANCODE_A:
        return EKey::K_A;
    case SDL_SCANCODE_S:
        return EKey::K_S;
    case SDL_SCANCODE_D:
        return EKey::K_D;
    case SDL_SCANCODE_C:
        return EKey::K_C;
    case SDL_SCANCODE_M:
        return EKey::K_M;

    case SDL_SCANCODE_MINUS:
        return EKey::K_Minus;
    case SDL_SCANCODE_EQUALS:
        return EKey::K_Equal;
    case SDL_SCANCODE_INSERT:
        return EKey::K_Insert;
    case SDL_SCANCODE_DELETE:
        return EKey::K_Delete;
    case SDL_SCANCODE_HOME:
        return EKey::K_Home;
    case SDL_SCANCODE_END:
        return EKey::K_End;
    case SDL_SCANCODE_PAGEUP:
        return EKey::K_Page_Up;
    case SDL_SCANCODE_PAGEDOWN:
        return EKey::K_Page_Down;

    case SDL_SCANCODE_LEFT:
        return EKey::K_Left;
    case SDL_SCANCODE_RIGHT:
        return EKey::K_Right;
    case SDL_SCANCODE_UP:
        return EKey::K_Up;
    case SDL_SCANCODE_DOWN:
        return EKey::K_Down;

    case SDL_SCANCODE_LCTRL:
        return EKey::K_LCtrl;
    case SDL_SCANCODE_RCTRL:
        return EKey::K_RCtrl;
    case SDL_SCANCODE_LALT:
        return EKey::K_LAlt;
    case SDL_SCANCODE_RALT:
        return EKey::K_RAlt;
    case SDL_SCANCODE_LSHIFT:
        return EKey::K_LShift;
    case SDL_SCANCODE_RSHIFT:
        return EKey::K_RShift;
    }

    return EKey::K_Z;
}

void MainUi::OnNonUiKey( SDL_Scancode code, SDL_EventType action )
{
    EKey key = GetKey( code );
    EKeyAction act = action == SDL_KEYDOWN ? EKeyAction::KA_Press : EKeyAction::KA_Release;

    m_currentView->GetViewModel()->Key( key, act );
}

void MainUi::OnBeforeRender()
{
    m_currentView->GetViewModel()->BeforeViewRender();
}

void MainUi::OnAfterRender()
{
    m_currentView->GetViewModel()->AfterViewRender();
}

void MainUi::OnMenuItem( int index )
{
    m_currentView->GetViewModel()->MenuItemSelected( index );
}
