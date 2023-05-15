#include "BaseView.h"

#include "IMainWindow.h"
#include "IViewModel.h"

BaseView::BaseView( IMainWindow* window ) 
    : m_parentWindow( window )
{

}

void BaseView::OnEvent( EVmEvent event )
{
    switch ( event )
    {
    case EVmEvent::Generic_AppClose:
    {
        m_parentWindow->Close();
        break;
    }
    case EVmEvent::Generic_NewMaps:
    {
        auto yesFunc = [&]() { GetViewModel()->UpdateMaps(); };
        auto noFunc = ButtonAction();

        ButtonActionItemList items = {
            { "Yes", yesFunc },
            { "No", noFunc }
        };

        m_parentWindow->ShowMessage( "Question", "Maps are old. Do you want to update?", items );

        break;
    }
    case EVmEvent::Generic_MapsExpired:
    {
        auto yesFunc = [&]() { GetViewModel()->UpdateMaps(); };
        auto noFunc = ButtonAction();

        ButtonActionItemList items = {
            { "Yes", yesFunc },
            { "No", noFunc }
        };

        m_parentWindow->ShowMessage( "Question", "Maps are expired. Do you want to update?", items );
        break;
    }
    }
}

float BaseView::DPI( int size ) const
{
    return m_parentWindow->DPI( size );
}
