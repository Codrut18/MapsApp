#include "ViewFactory.h"

#include "IMainWindow.h"

#include "MainView.h"
#include "MapsView.h"
#include "RoutesView.h"
#include "NavigationView.h"
#include "PreferencesView.h"

ViewFactory::ViewFactory( IMainWindow& mainWindow ) 
    : m_mainWindow( mainWindow )
{

}

IView* ViewFactory::BuildView( EView viewType )
{
    switch (viewType)
    {
    case EView::Main:
        return new MainView( &m_mainWindow );
    case EView::Maps:
        return new MapsView( &m_mainWindow );
    case EView::Routes:
        return new RoutesView( &m_mainWindow );
    case EView::Navigation:
        return new NavigationView( &m_mainWindow );
    case EView::Preferences:
        return new PreferencesView( &m_mainWindow );
    }

    return nullptr;
}
