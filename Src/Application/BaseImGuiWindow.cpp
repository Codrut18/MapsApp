// Copyright (C) 2019-2023, Magic Lane B.V.
// All rights reserved.
//
// This software is confidential and proprietary information of Magic Lane
// ("Confidential Information"). You shall not disclose such Confidential
// Information and shall use it only in accordance with the terms of the
// license agreement you entered into with Magic Lane.

#include "BaseImGuiWindow.h"

#include "imgui_internal.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl.h"

#include <algorithm>

extern unsigned int roboto_compressed_size;
extern unsigned int roboto_compressed_data[];

BaseImGuiWindow::BaseImGuiWindow()
    : BaseSdlWindow()
    , m_bDisplayMainMenu ( false )
{

}

BaseImGuiWindow::~BaseImGuiWindow()
{
    m_fonts.clear();
}

int BaseImGuiWindow::Init( int width, int height )
{
    bool bUseOpenGLES2;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    bUseOpenGLES2 = true;
#else
    bUseOpenGLES2 = false;
#endif

    int errCode;
    if ( ( errCode = BaseSdlWindow::Init( width, height, bUseOpenGLES2 ) ) != 0 )
        return errCode;

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backend
    if ( !ImGui_ImplSDL2_InitForOpenGL( GetSDL_Window(), &m_windowInfo.openGLContext ) )
        return 1;

    if ( !ImGui_ImplOpenGL3_Init( m_windowInfo.glVersion ) )
        return 1;

    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
    ImGui::GetIO().IniFilename = nullptr;

    SetupFonts();

    return 0;
}

void BaseImGuiWindow::Show()
{
    while (!IsDone())
    {
        HandleEvents();

        OnBeforeRender();

        // render frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        glClear( GL_COLOR_BUFFER_BIT );
        
        BuildUI();

        if (IsPopupModalActive())
            DisplayPopupModal();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        // swap frames
        SDL_GL_SwapWindow( GetSDL_Window() );

        OnAfterRender();
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void BaseImGuiWindow::Close()
{
    BaseSdlWindow::Close();
}

float BaseImGuiWindow::DPI( int size ) const
{
    return m_windowInfo.dpi * size;
}

void BaseImGuiWindow::BeginView( const char* name )
{
    BeginWindow( name, GetDefaultViewSize(), GetDefaultViewPos() );
}

void BaseImGuiWindow::EndView()
{
    EndWindow();
}

void BaseImGuiWindow::BeginWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags /*= DEFAULT_WIN_FLAGS */ )
{
    ImGui::SetNextWindowSize( size );
    ImGui::SetNextWindowPos( pos );

    ImGui::Begin( name, nullptr, windowFlags );
}

void BaseImGuiWindow::EndWindow()
{
    ImGui::End();
}

void BaseImGuiWindow::BeginNoPaddingWindow( const char* name, const ImVec2& size, const ImVec2& pos, ImGuiWindowFlags windowFlags /*= DEFAULT_WIN_FLAGS */ )
{
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
    BeginWindow( name, size, pos, DEFAULT_WIN_FLAGS | ImGuiWindowFlags_NoBackground );
    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
}

void BaseImGuiWindow::EndNoPaddingWindow()
{
    ImGui::PopStyleVar();
    EndWindow();
    ImGui::PopStyleVar( 1 );
}

bool BaseImGuiWindow::IsDisplayMenu() const
{
    return m_bDisplayMainMenu;
}

void BaseImGuiWindow::Menu( const WindowMenuItems& items )
{
    // compute menu sizes
    int numberOfButtons = 6;
    int buttonsPerRow = 3;
    int rows = 2;

    ImVec2 menuButtonSize;
    ImVec2 menuButtonSpacing( 3, 3 );

    buttonsPerRow = GetWidth() > GetHeight() ? 3 : 2;
    rows = (numberOfButtons + buttonsPerRow - 1) / buttonsPerRow;

    menuButtonSize.x = std::min( GetWidth() / (buttonsPerRow + 0.5), GetHeight() / (rows + 0.5) );
    menuButtonSize.y = menuButtonSize.x;

    ImVec2 windowSize;
    windowSize.x = buttonsPerRow * menuButtonSize.x + (buttonsPerRow - 1) * menuButtonSpacing.x;
    windowSize.y = rows * menuButtonSize.x + (rows - 1) * menuButtonSpacing.y;

    ImVec2 windowPos( (GetWidth() - windowSize.x) / 2, (GetHeight() - windowSize.y) / 2 );

    // Actually display the menu
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, menuButtonSpacing );

    BeginWindow( "Window_MainMenu", windowSize, windowPos );

    if (ImGui::IsWindowAppearing())
        ImGui::SetWindowFocus();

    std::string adjustedText;

    PushFontSize( EFontSize::Menu );

    for (int i = 0; i < numberOfButtons; i++)
    {
        if (i < items.size()) // active button
        {

            auto textSize = ImGui::CalcTextSize( items[i].c_str() );

            if (textSize.x + 10 > menuButtonSize.x)
            {
                adjustedText = items[i];
                std::replace( adjustedText.begin(), adjustedText.end(), ' ', '\n' );
            }

            if (ImGui::Button( adjustedText.empty() ? items[i].c_str() : adjustedText.c_str(), menuButtonSize ))
            {
                m_bDisplayMainMenu = false;
                OnMenuItem( i );
            }

            adjustedText.clear();

        }
        else // inactive button
            ImGui::Button( "##empty_button", menuButtonSize );

        if ((i + 1) % buttonsPerRow != 0)
            ImGui::SameLine();
    }

    PopFontSize();

    EndWindow();

    ImGui::PopStyleVar( 2 );
}

void BaseImGuiWindow::DisplayMenuButton()
{
    auto func = [&]() { m_bDisplayMainMenu = !m_bDisplayMainMenu; };

    TextButtonW( "Menu", GetLeftMenuButtonSize(), GetLeftMenuButtonOffset(), func );
}

void BaseImGuiWindow::TextButtonW( const char* text, const ImVec2& size, const ImVec2& position, ButtonAction action )
{
    char windowName[50], buttonName[50];
    sprintf( windowName, "Window_%s", text );
    sprintf( buttonName, "%s##Button_%s", text, text );

    BeginNoPaddingWindow( windowName, size, position, DEFAULT_WIN_FLAGS | ImGuiWindowFlags_NoBackground );

    if (ImGui::Button( buttonName, size ))
        action();

    EndNoPaddingWindow();
}

void BaseImGuiWindow::TextureButtonW( unsigned int textureId, const ImVec2& size, const ImVec2& position, ButtonAction action )
{
    char windowName[50];
    sprintf( windowName, "Window_Texture_%d", textureId );

    BeginNoPaddingWindow( windowName, size, position, DEFAULT_WIN_FLAGS | ImGuiWindowFlags_NoBackground );

    if (ImGui::ImageButton( (void*)textureId, size ))
        action();

    EndNoPaddingWindow();
}

void BaseImGuiWindow::ShowMessage( const char* messageType, char const* fmt, ... )
{
    char buffer[256];

    va_list va;
    va_start( va, fmt );
    vsprintf( buffer, fmt, va );
    va_end( va );

    m_messageType = messageType;
    m_popupMessage = buffer;
}

void BaseImGuiWindow::ShowMessage( const char* messageType, const char* message, const ButtonActionItemList& items )
{
    std::vector<std::string> buttons;
    std::vector<std::function<void( void )>> buttonsActions;

    for (const auto& it : items)
    {
        buttons.push_back( it.first );
        buttonsActions.push_back( it.second );
    }

    m_messageType = messageType;
    m_popupMessage = message;

    m_buttons = buttons;
    m_buttonsActions = buttonsActions;
}

void BaseImGuiWindow::PushFontSize( EFontSize fontSize )
{
    ImGui::PushFont( m_fonts.find( fontSize )->second );
}

void BaseImGuiWindow::PopFontSize()
{
    ImGui::PopFont();
}

int BaseImGuiWindow::GetWindowWidth() const
{
    return GetWidth();
}

int BaseImGuiWindow::GetWindowHeight() const
{
    return GetHeight();
}

ImVec2 BaseImGuiWindow::GetDefaultViewSize() const
{
    float MARGIN = DPI( 10 );

    float width = std::min( DPI( 1000 ), GetWidth() - GetLeftMenuButtonSize().x - GetLeftMenuButtonOffset().x - 2 * MARGIN );
    float height = (float)GetHeight() - 2 * MARGIN;

    return ImVec2( width, height );
}

ImVec2 BaseImGuiWindow::GetDefaultViewPos() const
{
    auto winWidth = GetWidth();
    auto viewSize = GetDefaultViewSize();

    float MARGIN = DPI( 10 );

    auto leftSpace = GetLeftMenuButtonSize().x + GetLeftMenuButtonOffset().x + MARGIN;

    if (winWidth - leftSpace - MARGIN > DPI( 1000 ))
        leftSpace += (winWidth - leftSpace - viewSize.x) / 2;

    return ImVec2( leftSpace, MARGIN );
}

ImVec2 BaseImGuiWindow::GetLeftMenuButtonSize() const
{
    if (GetWidth() < DPI( 400 ))
        return ImVec2( DPI( 40 ), DPI( 40 ) );

    if (GetWidth() < DPI( 800 ))
        return ImVec2( DPI( 50 ), DPI( 50 ) );

    return ImVec2( DPI( 80 ), DPI( 80 ) );
}

ImVec2 BaseImGuiWindow::GetLeftMenuButtonOffset() const
{
    return ImVec2( DPI( 2 ), DPI( 2 ) );
}

void BaseImGuiWindow::Combo( const char* name, const char** filter, int filterSize, int& filterIndex, std::function<void( void )> func )
{
    if (ImGui::BeginCombo( name, filter[filterIndex] ))
    {
        for (int i = 0; i < filterSize; i++)
        {
            if (ImGui::Selectable( filter[i], filterIndex == i ))
            {
                filterIndex = i;
                func();
            }
            if (filterIndex == i)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void BaseImGuiWindow::LoadingWindow( const char* text, const ImVec4& color1, const ImVec4& color2 )
{
    ImVec2 textSize = ImGui::CalcTextSize( text );

    ImVec2 textPos( ImGui::GetCursorPos().x + (ImGui::GetWindowWidth() - textSize.x) / 2, 20 );
    ImGui::SetCursorPos( textPos );
    ImGui::TextUnformatted( text );

    const ImVec2 windowSize = ImGui::GetWindowSize();

    const float INDICATOR_RADIUS = std::min( windowSize.x / 3, windowSize.y / 3 );

    ImVec2 loadingPos( (windowSize.x - 2 * INDICATOR_RADIUS) / 2, (windowSize.y - 2 * INDICATOR_RADIUS) / 2 );
    ImGui::SetCursorPos( loadingPos );

    LoadingIndicatorCircle( text, INDICATOR_RADIUS, color1, color2, 10, 2 );
}

void BaseImGuiWindow::LoadingIndicatorCircle( const char* label, const float indicator_radius, const ImVec4& main_color, const ImVec4& backdrop_color, const int circle_count, const float speed )
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID( label );

    const float circle_radius = indicator_radius / 10.0f;

    // cursor position in window
    const ImVec2 pos( window->DC.CursorPos.x, window->DC.CursorPos.y );
    const ImRect bb( pos, ImVec2( pos.x + indicator_radius * 2.0f, pos.y + indicator_radius * 2.0f ) );

    ImGui::ItemSize( bb/*, g.Style.FramePadding.y*/ );
    if (!ImGui::ItemAdd( bb, id ))
        return;

    const float t = g.Time;
    const auto degree_offset = 2.0f * IM_PI / circle_count;

    for (int i = 0; i < circle_count; ++i)
    {
        const auto x = indicator_radius * std::sin( degree_offset * i );
        const auto y = indicator_radius * std::cos( degree_offset * i );
        const auto growth = std::max( 0.0f, std::sin( t * speed - i * degree_offset ) );

        ImVec4 circleColor;
        circleColor.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
        circleColor.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
        circleColor.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
        circleColor.w = 1.0f;

        ImVec2 circleCenter( pos.x + indicator_radius + x, pos.y + indicator_radius - y );
        window->DrawList->AddCircleFilled( circleCenter, circle_radius + growth * circle_radius, ImGui::GetColorU32( circleColor ) );
    }
}

void BaseImGuiWindow::SetupFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;

    builder.AddRanges( io.Fonts->GetGlyphRangesDefault() );
    // see https://www.thoughtco.com/html-codes-romanian-characters-4062226
    builder.AddChar( 0x15e ); // Ş
    builder.AddChar( 0x15f );
    builder.AddChar( 0x162 ); // Ț
    builder.AddChar( 0x163 );
    builder.AddChar( 0x102 ); // Ă
    builder.AddChar( 0x103 );

    builder.AddRanges( io.Fonts->GetGlyphRangesChineseFull() );
    builder.AddRanges( io.Fonts->GetGlyphRangesCyrillic() );

    builder.BuildRanges( &ranges );

    m_fonts.insert( std::make_pair<>( EFontSize::Normal, io.Fonts->AddFontFromMemoryCompressedTTF( roboto_compressed_data, roboto_compressed_size, DPI( 17.0f ), 0, ranges.Data ) ) );
    m_fonts.insert( std::make_pair<>( EFontSize::Small, io.Fonts->AddFontFromMemoryCompressedTTF( roboto_compressed_data, roboto_compressed_size, DPI( 15.0f ), 0, ranges.Data ) ) );
    m_fonts.insert( std::make_pair<>( EFontSize::Big, io.Fonts->AddFontFromMemoryCompressedTTF( roboto_compressed_data, roboto_compressed_size, DPI( 19.0f ), 0, ranges.Data ) ) );
    m_fonts.insert( std::make_pair<>( EFontSize::Menu, io.Fonts->AddFontFromMemoryCompressedTTF( roboto_compressed_data, roboto_compressed_size, DPI( 23.0f ), 0, ranges.Data ) ) );

    io.Fonts->Build();
}

bool BaseImGuiWindow::IsPopupModalActive() const
{
    return !m_popupMessage.empty();
}

void BaseImGuiWindow::DisplayPopupModal()
{
    char controlName[100];
    sprintf( controlName, "%s##mymessagectrl", m_messageType.c_str() );

    if( !ImGui::IsPopupOpen( controlName ) )
        ImGui::OpenPopup( controlName );

    ImGui::SetNextWindowSize( { DPI( 400 ), DPI( 110 ) } );

    if (m_messageType.empty() && m_popupMessage.empty())
        return;

    if (ImGui::BeginPopupModal( controlName, nullptr, ImGuiWindowFlags_NoResize ))
    {
        ImGui::TextUnformatted( m_popupMessage.c_str() );
        ImGui::NewLine();
        if (m_buttons.empty())
        {
            if (ImGui::Button( "Ok", ImVec2(int(DPI(80)), int(DPI(25)))))
            {
                m_messageType.clear();
                m_popupMessage.clear();

                ImGui::CloseCurrentPopup();
            }
        }
        else
        {
            for (int i = 0; i < m_buttons.size(); i++)
            {
                if (ImGui::Button( m_buttons[i].c_str(), ImVec2( int( DPI( 80 ) ), int( DPI( 25 ) ) ) ))
                {
                    m_messageType.clear();
                    m_popupMessage.clear();

                    ImGui::CloseCurrentPopup();

                    if (m_buttonsActions[i])
                        m_buttonsActions[i]();

                    m_buttons.clear();
                    m_buttonsActions.clear();
                }
                ImGui::SameLine();
            }
        }
        
        ImGui::EndPopup();
    }
}

void BaseImGuiWindow::mouse_handler( SDL_Event* event )
{
    static bool mouseDownOnMap = false;

    bool mapCapturedEvent = mouseDownOnMap || event->type == SDL_MOUSEWHEEL;

    if (UIWantCaptureEvent( event ) && !mapCapturedEvent)
        return;

    // send event to map (if not ImGui event)
    if (event->type == SDL_MOUSEWHEEL)
    {
        if( !UIWantCaptureEvent( event ) )
            OnNonUiScroll( event->wheel.y, m_currentPos );
    }
    else
    {
        // OnNonUiMouse( (SDL_EventType)event->type, m_currentPos.x, m_currentPos.y );

        // convert mouse event into touch event
        SDL_EventType newEvType = SDL_EventType::SDL_MOUSEBUTTONDOWN;
        switch (event->type)
        {
        case SDL_MOUSEBUTTONDOWN:
            newEvType = SDL_FINGERDOWN;
            break;
        case SDL_MOUSEBUTTONUP:
            newEvType = SDL_FINGERUP;
            break;
        case SDL_MOUSEMOTION:
            newEvType = SDL_FINGERMOTION;
            break;
        }

        OnNonUiTouch( newEvType, 0, m_currentPos.x, m_currentPos.y );

        if (event->type == SDL_MOUSEBUTTONDOWN)
            mouseDownOnMap = true;

        if (event->type == SDL_MOUSEBUTTONUP)
            mouseDownOnMap = false;
    }
}

void BaseImGuiWindow::touch_handler( SDL_Event* event )
{
    // update current position
    UIUpdateMousePos( m_currentPos.x, m_currentPos.y );

    CaptureInfo* captureInfo = m_activeFingers.UpdateFingerInfo( event, Pos( m_currentPos.x, m_currentPos.y ) );

    // on finger down set up who captures the finger
    if (event->type == SDL_FINGERDOWN)
        captureInfo->destination = UIWantCaptureEvent( event ) ? EFingerCapture::Ui : EFingerCapture::Map;

    if (captureInfo->destination == EFingerCapture::Ui)
    {
        SDL_Event mouseEvent = ConvertFingerToMouseEvent( *event );

        ImGui_ImplSDL2_ProcessEvent( &mouseEvent );
    }
    else // Forward event to the map
    {
        OnNonUiTouch( (SDL_EventType) event->type, event->tfinger.fingerId, m_currentPos.x, m_currentPos.y );
    }

    if (event->type == SDL_FINGERUP)
        m_activeFingers.Remove( event->tfinger.fingerId );
}

void BaseImGuiWindow::key_handler( SDL_Event* event )
{
    // send event to map (if not ImGui event)
    if (!UIWantCaptureEvent( event ))
    {
        if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
            OnNonUiKey( event->key.keysym.scancode, (SDL_EventType)event->type );
    }
}

bool BaseImGuiWindow::UIWantCaptureEvent( SDL_Event* event )
{
    ImGui_ImplSDL2_ProcessEvent( event );
    return ImGui::GetIO().WantCaptureMouse;
}

void BaseImGuiWindow::UIUpdateMousePos( float x, float y )
{
    ImGui::GetIO().AddMousePosEvent( x, y );

    ImGui::UpdateInputEvents( true );
    ImGui::UpdateHoveredWindowAndCaptureFlags();
}
