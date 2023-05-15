#include "PreferencesView.h"

#include "PreferencesViewModel.h"
#include "IMainWindow.h"

PreferencesView::PreferencesView( IMainWindow* parent )
    : BaseView( parent )
    , m_viewModel( nullptr )
    , m_bRenderFps( false )
{

}

IViewModel* PreferencesView::GetViewModel()
{
    return m_viewModel;
}

void PreferencesView::SetViewModel( IViewModel* viewModel )
{
    m_viewModel = static_cast<PreferencesViewModel*>( viewModel );

    m_bRenderFps = m_viewModel->IsRenderFps();
}

void PreferencesView::Render()
{
    ImGui::SetNextWindowBgAlpha( 0.8f );

    m_parentWindow->BeginView( "Window_Preferences" );

    if ( ImGui::BeginTable( "##table_preferences", 3, ImGuiTableFlags_ScrollX ) )
    {
        float COLUMN_0_WIDTH = DPI( 150 );
        float COLUMN_1_WIDTH = ImGui::GetWindowSize().x - COLUMN_0_WIDTH - DPI( 20 );

        ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthFixed, COLUMN_0_WIDTH );
        ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthFixed, COLUMN_1_WIDTH );

        // 1st preference
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex( 0 );

        ImGui::Text( "Render FPS" );

        ImGui::TableSetColumnIndex( 1 );

        ImGui::Checkbox( "##render_fps", &m_bRenderFps );
        if ( ImGui::IsItemClicked() )
            m_viewModel->SetRenderFps( !m_bRenderFps );

        // 2nd, 3rd preferences...

        ImGui::EndTable();
    }

    m_parentWindow->EndView();
}

void PreferencesView::OnEvent( EVmEvent event )
{
    switch ( event )
    {
    case EVmEvent::Generic_ChangedConnection:
        break;
    default:
        BaseView::OnEvent( event );
        break;
    }
}
