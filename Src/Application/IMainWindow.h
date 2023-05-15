#pragma once

#include "IMainUi.h"

#include <string>
#include <vector>
#include <functional>

#include "imgui.h"

enum class EFontSize
{
    Menu,
    Small,
    Normal,
    Big
};

using ButtonCaption = std::string;
using ButtonAction = std::function<void( void )>;
using ButtonActionItem = std::pair<ButtonCaption, ButtonAction>;
using ButtonActionItemList = std::vector<ButtonActionItem>;

using WindowMenuItems = std::vector<std::string>;

const ImVec4 ImGuiColor_Black = { 0.f, 0.f, 0.f, 1.f };
const ImVec4 ImGuiColor_Green = { 0.f, 1.f, 0.f, 1.f };
const ImVec4 ImGuiColor_Red = { 1.f, 0.f, 0.f, 1.f };

const ImGuiWindowFlags DEFAULT_WIN_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

class IMainWindow
{
public:
    virtual void Close() = 0;

    virtual float DPI( int size ) const = 0;

    // window with the right size (default view size)
    virtual void BeginView( const char* name ) = 0;
    virtual void EndView() = 0;

    // window with any size and position
    virtual void BeginWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags = DEFAULT_WIN_FLAGS ) = 0;
    virtual void EndWindow() = 0;

    // window with any size, position and no padding (outer and inner)
    virtual void BeginNoPaddingWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags = DEFAULT_WIN_FLAGS ) = 0;
    virtual void EndNoPaddingWindow() = 0;

    // action buttons
    virtual void TextButtonW( const char* text, const ImVec2& size, const ImVec2& position, ButtonAction action ) = 0;
    virtual void TextureButtonW( unsigned int textureId, const ImVec2& size, const ImVec2& position, ButtonAction action ) = 0;

    // messages
    virtual void ShowMessage( const char* messageType, char const* fmt, ... ) = 0;
    virtual void ShowMessage( const char* messageType, const char* message, const ButtonActionItemList& items ) = 0;

    // font management
    virtual void PushFontSize( EFontSize fontSize ) = 0;
    virtual void PopFontSize() = 0;

    virtual int GetWindowWidth() const = 0;
    virtual int GetWindowHeight() const = 0;

    virtual ImVec2 GetDefaultViewSize() const = 0;
    virtual ImVec2 GetDefaultViewPos() const = 0;

    virtual void Combo( const char* name, const char** filter, int filterSize, int& filterIndex, std::function<void( void )> func ) = 0;

    virtual void LoadingWindow( const char* text, const ImVec4& color1, const ImVec4& color2 ) = 0;

    virtual ~IMainWindow() = default;
};
