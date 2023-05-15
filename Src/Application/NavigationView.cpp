#include "NavigationView.h"

#include "IMainWindow.h"
#include "NavigationViewModel.h"

#include "IResourceRepository.h"
#include "ITextureRepository.h"

#include <API/GEM_ContentStoreItem.h>

NavigationView::NavigationView( IMainWindow* parent )
    : BaseView( parent )
    , m_viewModel( nullptr )
    , m_mapFilterIndex( 0 )
{

}

IViewModel* NavigationView::GetViewModel()
{
    return m_viewModel;
}

void NavigationView::SetViewModel( IViewModel* viewModel )
{
    m_viewModel = static_cast<NavigationViewModel*>(viewModel);
}

static gem::String FormatFileSize( gem::LargeInteger sz )
{
    if (sz < 1024 * 1024)
        return gem::String::formatString( u"%.2lf KB", sz / 1024. );

    if (sz < 1024 * 1024 * 1024)
        return gem::String::formatString( u"%.2lf MB", sz / (1024. * 1024.) );

    return gem::String::formatString( u"%.2lf GB", sz / (1024. * 1024. * 1024.) );
}

void NavigationView::Render()
{
    int fullWindowWidth = m_parentWindow->GetWindowWidth();
    int fullWindowHeight = m_parentWindow->GetWindowHeight();

    static const ImVec2 DEFAULT_WINDOW_PADDING = ImGui::GetStyle().WindowPadding;
    static const ImVec2 DEFAULT_ITEM_SPACING = ImGui::GetStyle().ItemSpacing;

    gem::NavigationInstruction instruction = m_viewModel->GetNavigationInstruction();

    m_parentWindow->PushFontSize( EFontSize::Big );

    // Instruction window
    {
        // window and columns size
        const ImVec2 INSTRUCTION_ICON_SIZE( DPI( 70 ), DPI( 70 ) );

        ImVec2 windowSize( std::min( DPI( 800 ), m_parentWindow->GetDefaultViewSize().x - DPI( 30 ) ), INSTRUCTION_ICON_SIZE.x + DPI( 10 ) );
        ImVec2 windowPos( m_parentWindow->GetDefaultViewPos().x, DPI( 5 ) );

        int firstColumnSize = INSTRUCTION_ICON_SIZE.x;
        int secondColumnSize = DPI( 70 );
        int lastColumnSize = windowSize.x - firstColumnSize - secondColumnSize - DPI( 20 );

        // prepare data
        auto distMetersToNextTurn = instruction.getTimeDistanceToNextTurn().getTotalDistance();

        gem::String instructionDist;

        if ( distMetersToNextTurn < 1000 )
            instructionDist = gem::String::formatString( u"(%d m)", distMetersToNextTurn);
        else
            instructionDist = gem::String::formatString( u"(%.2f km)", distMetersToNextTurn / 1000. );

        gem::String instructionText = instruction.getNextTurnInstruction();
        instructionText.fallbackToLegacyUnicode();

        static int lastWindowSizeX = 0;
        static gem::String lastInstructionText = "";
        static gem::String lastTransformedInstructionText = "";

        if ( instructionText == lastInstructionText && windowSize.x == lastWindowSizeX )
            instructionText = lastTransformedInstructionText;
        else
        {
            lastInstructionText = instructionText;

            std::vector<gem::String> lines;
            while ( !instructionText.empty() )
            {
                auto instructionTextSize = ImGui::CalcTextSize( instructionText.toStdString().c_str() );
                if ( instructionTextSize.x > lastColumnSize )
                {
                    if ( instructionText.find( ' ' ) == -1 )
                    {
                        lines.push_back( instructionText );
                        instructionText.clear();
                        break;
                    }

                    for ( int i = instructionText.size() - 2; i >= 0; i-- )
                    {
                        if ( instructionText[i] == ' ' )
                        {
                            gem::String str = instructionText.left( i );
                            if ( ImGui::CalcTextSize( str.toStdString().c_str() ).x < lastColumnSize )
                            {
                                lines.push_back( str );
                                instructionText = instructionText.right( instructionText.size() - i - 1 );
                                break;
                            }
                        }
                    }
                }
                else
                {
                    lines.push_back( instructionText );
                    instructionText.clear();
                    break;
                }
            }

            instructionText.clear();
            if ( !lines.empty() )
            {
                for ( int i = 0; i + 1 < lines.size(); i++ )
                    instructionText.append( lines[i] ).append( "\n" );
                instructionText.append( lines.back() );
            }

            lastTransformedInstructionText = instructionText;
        }

        lastWindowSizeX = windowSize.x;

        unsigned int imageId = instruction.getNextTurnDetails().getAbstractGeometryImage().getUid();

        // actual UI

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );

        m_parentWindow->BeginWindow( "Window_NextInstruction", windowSize, windowPos );

        if ( ImGui::BeginTable( "##table_instructions", 3 ) )
        {
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthFixed, firstColumnSize );
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthFixed, secondColumnSize );
            ImGui::TableSetupColumn( "", ImGuiTableColumnFlags_WidthFixed, lastColumnSize );

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );

            if ( instruction && !instruction.isDefault() )
            {
                auto textureRepository = m_viewModel->GetTextureRepository();

                gem::Rgba color( 100, 100, 100, 255 );
                gem::AbstractGeometryImageRenderSettings settings( gem::Rgba::white(), gem::Rgba::black(), color );

                auto textureId = textureRepository->GetTexture( instruction.getNextTurnDetails().getAbstractGeometryImage(), settings, INSTRUCTION_ICON_SIZE.x, INSTRUCTION_ICON_SIZE.x );

                ImGui::Image( (void*)textureId, INSTRUCTION_ICON_SIZE );
            }

            ImGui::TableSetColumnIndex( 1 );

            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ( windowSize.y - ImGui::GetFontSize() ) / 4 );
            if ( instruction && !instruction.isDefault() )
                ImGui::TextUnformatted( instructionDist.toStdString().c_str() );

            ImGui::TableSetColumnIndex( 2 );

            ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ( windowSize.y - ImGui::GetFontSize() ) / 4 );
            if ( instruction && !instruction.isDefault() )
                ImGui::TextUnformatted( instructionText.toStdString().c_str() );

            ImGui::EndTable();
        }

        m_parentWindow->EndWindow();

        ImGui::PopStyleVar();
    }

    // Remaining distance window
    {
        // prepare data
        int distMeters = instruction.getRemainingTravelTimeDistance().getTotalDistance();

        char distStr[20];
        if ( distMeters < 1000 )
            sprintf( distStr, "%d m", distMeters - distMeters % 50 );
        else
            sprintf( distStr, "%.2f km", ( distMeters - distMeters % 50 ) / 1000.f );

        int timeSec = instruction.getRemainingTravelTimeDistance().getTotalTime();
        char timeStr[20];
        if ( timeSec < 60 )
            sprintf( timeStr, "%d sec", timeSec + 5 - timeSec % 5 );
        else
            if ( timeSec < 3600 )
                sprintf( timeStr, "%d min", timeSec / 60 );
            else
                sprintf( timeStr, "%d:%02d hr", timeSec / 3600, ( timeSec % 3600 ) / 60 );

        // actual UI
        ImVec2 windowSize(
            std::max( ImGui::CalcTextSize( distStr ).x, ImGui::CalcTextSize( timeStr ).x ) + 2 * DEFAULT_WINDOW_PADDING.x + DPI( 5 ),
            2 * ImGui::GetTextLineHeight() + DEFAULT_ITEM_SPACING.y + 2 * DEFAULT_WINDOW_PADDING.y );
        ImVec2 windowPos( 0, fullWindowHeight - windowSize.y + 1 );

        m_parentWindow->BeginWindow( "Window_RemainingTimeDistance", windowSize, windowPos );

        ImGui::TextUnformatted( distStr );
        ImGui::TextUnformatted( timeStr );

        m_parentWindow->EndWindow();
    }

    // Speed window
    {
        // prepare data
        auto position = instruction.getCurrentPosition();
        double speedKMH = position ? position->getSpeed() * 3.6 : 0;

        char speedStr[20];
        if ( speedKMH < 1 )
            sprintf( speedStr, "%.2f km/h", speedKMH );
        else
            sprintf( speedStr, "%d km/h", (int)speedKMH );

        //actual UI
        ImVec2 windowSize( ImGui::CalcTextSize( speedStr ).x + 2 * DEFAULT_WINDOW_PADDING.x + DPI( 5 ), ImGui::GetTextLineHeight() + 2 * DEFAULT_WINDOW_PADDING.y );
        ImVec2 windowPos( fullWindowWidth - windowSize.x + 1, fullWindowHeight - windowSize.y + 1 );

        m_parentWindow->BeginWindow( "Window_Speed", windowSize, windowPos );

        ImGui::TextUnformatted( speedStr );

        m_parentWindow->EndWindow();
    }

    m_parentWindow->PopFontSize();
}

void NavigationView::OnEvent( EVmEvent event )
{
    switch (event)
    {
    case EVmEvent::Generic_ChangedConnection:
        break;
    default:
        BaseView::OnEvent( event );
        break;
    }
}

