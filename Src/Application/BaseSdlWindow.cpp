// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "BaseSdlWindow.h"

#include <memory>

BaseSdlWindow::BaseSdlWindow()
    : m_window( nullptr )
    , m_bCloseWindow( false )
    , m_bMouseEnabled( true )
    , m_bTouchEnabled( true )
    , m_bKeyEnabled( true )
{
    m_keyState = SDL_GetKeyboardState( NULL );
}

BaseSdlWindow::~BaseSdlWindow()
{
    if (m_windowInfo.openGLContext)
        SDL_GL_DeleteContext( m_windowInfo.openGLContext );

    if (m_window)
        SDL_DestroyWindow( m_window );
    m_window = nullptr;

    SDL_Quit();
}

const WindowInfo& BaseSdlWindow::GetWindowInfo()
{
    return m_windowInfo;
}

void BaseSdlWindow::SetMouseEnabled( bool enabled )
{
    m_bMouseEnabled = enabled;
}

void BaseSdlWindow::SetTouchEnabled( bool enabled )
{
    m_bTouchEnabled = enabled;
}

void BaseSdlWindow::SetKeyEnabled( bool enabled )
{
    m_bKeyEnabled = enabled;
}

int BaseSdlWindow::Init( int width, int height, bool bUseGlES )
{
    int errCode;
    if ((errCode = InitSDL( width, height, bUseGlES )) != 0)
        return errCode;

    if (!InitGL())
        return 1;

    // Activate V-Sync
    SDL_GL_SetSwapInterval( 1 );

    OnResize();

    return 0;
}

int BaseSdlWindow::GetWidth() const
{
    return m_windowInfo.width;
}

int BaseSdlWindow::GetHeight() const
{
    return m_windowInfo.height;
}

SDL_Window* BaseSdlWindow::GetSDL_Window() const
{
    return m_window;
}

void BaseSdlWindow::HandleEvents()
{
    m_activeFingers.ClearOld( [&]( SDL_FingerID fingerId, const CaptureInfo& captureInfo ) {
        auto x = captureInfo.lastKnownPosition.x;
        auto y = captureInfo.lastKnownPosition.y;

        // OnMapTouch( gem::ETouchEvent::TE_Up, fingerId, x, y );
        } );

    SDL_Event event;

    while (SDL_PollEvent( &event ))
    {
        if (IsQuitEvent( event, m_window ))
        {
            Close();
            break;
        }

        if (IsFingerEvent( event ) && m_bTouchEnabled)
        {
            // update current position
            if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION)
            {
                m_currentPos.x = GetWidth() * event.tfinger.x;
                m_currentPos.y = GetHeight() * event.tfinger.y;
            }

            touch_handler( &event );
        }

        if (IsMouseEvent( event ) && m_bMouseEnabled)
        {
            // update current position
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION)
            {
                m_currentPos.x = event.motion.x;
                m_currentPos.y = event.motion.y;
            }

            mouse_handler( &event );
        }


        if (IsKeyEvent( event ) && m_bKeyEnabled)
            key_handler( &event );

        if (IsResizeEvent( event ))
        {
            // Update size
            m_windowInfo.width = event.window.data1;
            m_windowInfo.height = event.window.data2;

            // Resize viewport
            int renderWidth, renderHeight;
            SDL_GL_GetDrawableSize( m_window, &renderWidth, &renderHeight );

            m_windowInfo.pixelRatio = (float)renderWidth / m_windowInfo.width;

            glViewport( 0, 0, renderWidth, renderHeight );

            OnResize();
        }

        if (IsWindowFocusedOrExposedEvent( event ))
            OnWindowFocusedOrExposed();
    }
}

bool BaseSdlWindow::IsDone() const
{
    return m_bCloseWindow;
}

void BaseSdlWindow::Close()
{
    m_bCloseWindow = true;
}

bool BaseSdlWindow::GetKeyState( SDL_Scancode key ) const
{
    return m_keyState[key];
}

SDL_Event BaseSdlWindow::ConvertFingerToMouseEvent( SDL_Event event ) const
{
    SDL_Event customEvent;

    switch (event.type)
    {
    case SDL_FINGERDOWN:
        customEvent.type = SDL_MOUSEBUTTONDOWN;
        customEvent.button.button = SDL_BUTTON_LEFT;
        break;
    case SDL_FINGERMOTION:
        customEvent.type = SDL_MOUSEMOTION;
        customEvent.button.button = SDL_BUTTON_LEFT;
        customEvent.button.x = event.tfinger.x * GetWidth();
        customEvent.button.y = event.tfinger.y * GetHeight();
        break;
    case SDL_FINGERUP:
        customEvent.type = SDL_MOUSEBUTTONUP;
        customEvent.button.button = SDL_BUTTON_LEFT;
        break;
    }

    return customEvent;
}

int BaseSdlWindow::InitSDL( int width, int height, bool bUseGlES )
{
    if (SDL_Init( SDL_INIT_VIDEO ) != 0)
        return -1;

#ifdef __WIN32__
    SDL_SetHint( SDL_HINT_OPENGL_ES_DRIVER, "1" );
    SDL_SetHint( SDL_HINT_VIDEO_WIN_D3DCOMPILER, "none" );
#endif

    SDL_SetHint( SDL_HINT_TOUCH_MOUSE_EVENTS, "0" );
    SDL_SetHint( "SDL_TOUCH_MOUSE_EVENTS", "0" );

    // Decide GL+GLSL versions
    if (bUseGlES)
    {
        // GL ES 2.0 + GLSL 100
        m_windowInfo.glVersion = const_cast<char*>("#version 100");

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
    }
    else
    {
        // GL 3.0 + GLSL 130
        m_windowInfo.glVersion = const_cast<char*>("#version 130");

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
    }

    float hdpi, vdpi;
    int intErr = SDL_GetDisplayDPI( 0, &m_windowInfo.ddpi, &hdpi, &vdpi );

    // Compute dpi ratio (divide with 72.0f for Apple, can be platform dependent)
    m_windowInfo.dpi = m_windowInfo.ddpi / 96.0f;

    if (width == 0 || height == 0)
    {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode( 0, &DM );

        width = DM.w;
        height = DM.h;
    }
    else
    {
        width = static_cast<int>(width * m_windowInfo.dpi + 0.5f);
        height = static_cast<int>(height * m_windowInfo.dpi + 0.5f);
    }

    m_window = SDL_CreateWindow( "BikeNav Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

    SDL_GetWindowSize( m_window, &m_windowInfo.width, &m_windowInfo.height );

    return 0;
}

bool BaseSdlWindow::InitGL()
{
    if ((m_windowInfo.openGLContext = SDL_GL_CreateContext( m_window )) != NULL)
    {
        m_windowInfo.updateOpenGLRenderContextFunc = [&]()
        {
            return SDL_GL_MakeCurrent( GetSDL_Window(), m_windowInfo.openGLContext ) >= 0;
        };

        return true;
    }

    return false;
}

bool BaseSdlWindow::IsQuitEvent( SDL_Event event, SDL_Window* window )
{
    if (event.type == SDL_QUIT)
        return true;

    if (event.window.windowID != SDL_GetWindowID( window ))
        return false;

    return event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE;
}

bool BaseSdlWindow::IsMouseEvent( SDL_Event event )
{
    return event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN
        || event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL;
}

bool BaseSdlWindow::IsFingerEvent( SDL_Event event )
{
    return event.type == SDL_FINGERUP || event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION;
}

bool BaseSdlWindow::IsKeyEvent( SDL_Event event )
{
    return event.type == SDL_KEYDOWN || event.type == SDL_KEYUP;
}

bool BaseSdlWindow::IsResizeEvent( SDL_Event event )
{
    return event.window.event == SDL_WINDOWEVENT_RESIZED
        || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED;
}

bool BaseSdlWindow::IsWindowFocusedOrExposedEvent( SDL_Event event )
{
    if (event.type != SDL_WINDOWEVENT)
        return false;

    switch (event.window.event)
    {
#if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
    case SDL_WINDOWEVENT_EXPOSED:
        return true;
    }

    return false;
}
