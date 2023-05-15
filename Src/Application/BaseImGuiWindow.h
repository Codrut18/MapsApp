// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#pragma once

#include "BaseSdlWindow.h"
#include "IMainWindow.h"

#include <imgui.h>

#include <map>
#include <string>
#include <vector>

class BaseImGuiWindow : public BaseSdlWindow, public IMainWindow
{
public:
    BaseImGuiWindow();
    ~BaseImGuiWindow();

    // IMainUi methods
    int Init( int width, int height ) override;

    void Show() override;

    // IMainWindow methods
    void Close() override;

    float DPI( int size ) const override;

    void BeginView( const char* name );
    void EndView();

    void BeginWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags = DEFAULT_WIN_FLAGS );
    void EndWindow();

    void BeginNoPaddingWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags = DEFAULT_WIN_FLAGS );
    void EndNoPaddingWindow();

    bool IsDisplayMenu() const;
    void Menu( const WindowMenuItems& items );
    void DisplayMenuButton();

    void TextButtonW( const char* text, const ImVec2& size, const ImVec2& position, ButtonAction action );
    void TextureButtonW( unsigned int textureId, const ImVec2& size, const ImVec2& position, ButtonAction action );

    void ShowMessage( const char* messageType, char const* fmt, ... );
    void ShowMessage( const char* messageType, const char* message, const ButtonActionItemList& items );

    // fonts management
    void PushFontSize( EFontSize fontSize );
    void PopFontSize();

    int GetWindowWidth() const override;
    int GetWindowHeight() const override;

    ImVec2 GetDefaultViewSize() const;
    ImVec2 GetDefaultViewPos() const;

    ImVec2 GetLeftMenuButtonSize() const;
    ImVec2 GetLeftMenuButtonOffset() const;

    void Combo( const char* name, const char** filter, int filterSize, int& filterIndex, std::function<void( void )> func ) override;

    void LoadingWindow( const char* text, const ImVec4& color1, const ImVec4& color2 ) override;
    void LoadingIndicatorCircle( const char* label, const float indicator_radius, const ImVec4& main_color, const ImVec4& backdrop_color, const int circle_count, const float speed );

protected:
    // hook for build Ui
    virtual void BuildUI() = 0;

    // hooks for events
    virtual void OnNonUiScroll( int delta, Pos pos ) {}
    virtual void OnNonUiMouse( SDL_EventType eventType, int x, int y ) {}
    virtual void OnNonUiTouch( SDL_EventType eventType, int64_t fingerId, int x, int y ) {}
    virtual void OnNonUiKey( SDL_Scancode code, SDL_EventType action ) {}

    virtual void OnBeforeRender() = 0;
    virtual void OnAfterRender() = 0;

    virtual void OnMenuItem( int index ) = 0;

private:
    // fonts management
    void SetupFonts();

    // popup window management
    bool IsPopupModalActive() const;
    void DisplayPopupModal();

    // events management
    void mouse_handler( SDL_Event* event );
    void touch_handler( SDL_Event* event );
    void key_handler( SDL_Event* event );

    bool UIWantCaptureEvent( SDL_Event* event );
    void UIUpdateMousePos( float x, float y );

private:
    std::map<EFontSize, ImFont*> m_fonts;

    bool m_bDisplayMainMenu;

    std::string m_messageType;
    std::string m_popupMessage;
    std::vector<std::string> m_buttons;
    std::vector <std::function<void( void )>> m_buttonsActions;
};
