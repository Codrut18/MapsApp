// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "IMainUi.h"
#include "ActiveFingersCollection.h"
#include "WindowInfo.h"

#include "SDL.h"
#include "SDL_opengles2.h"

class BaseSdlWindow : public IMainUi
{
public:
    BaseSdlWindow();
    ~BaseSdlWindow();

    // IMainUi methods
    const WindowInfo& GetWindowInfo() override;

    void SetMouseEnabled( bool enabled ) override;
    void SetTouchEnabled( bool enabled ) override;
    void SetKeyEnabled( bool enabled ) override;

    // other methods
    int Init( int width, int height, bool bUseGlES );

    int GetWidth() const;
    int GetHeight() const;

protected:
    SDL_Window* GetSDL_Window() const;

    void HandleEvents();

    bool IsDone() const;
    void Close();

    bool GetKeyState( SDL_Scancode key ) const;

    // hooks
    virtual void mouse_handler( SDL_Event* event ) {};
    virtual void touch_handler( SDL_Event* event ) {};
    virtual void key_handler( SDL_Event* event ) {};
    
    virtual void OnResize() {}
    virtual void OnWindowFocusedOrExposed() {}

    // other utils
    SDL_Event ConvertFingerToMouseEvent( SDL_Event event ) const;

private:
    int InitSDL( int width, int height, bool bUseGlES );
    bool InitGL();

    static bool IsQuitEvent(SDL_Event event, SDL_Window* window);
    static bool IsMouseEvent( SDL_Event event );
    static bool IsFingerEvent( SDL_Event event );
    static bool IsKeyEvent( SDL_Event event );
    static bool IsResizeEvent( SDL_Event event );
    static bool IsWindowFocusedOrExposedEvent( SDL_Event event );

protected:
    WindowInfo m_windowInfo;

    Pos m_currentPos;
    ActiveFingersCollection m_activeFingers;

private:
    SDL_Window* m_window;

    bool m_bCloseWindow;

    // mouse, touch, key related
    bool m_bMouseEnabled;
    bool m_bTouchEnabled;
    bool m_bKeyEnabled;

    const Uint8* m_keyState;
};
