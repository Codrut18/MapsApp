#include "MainView.h"

#include "MainViewModel.h"

MainView::MainView( IMainWindow* parent )
    : BaseView( parent )
    , m_viewModel( nullptr )
{

}

IViewModel* MainView::GetViewModel()
{
    return m_viewModel;
}

void MainView::SetViewModel( IViewModel* viewModel )
{
    m_viewModel = static_cast<MainViewModel*>( viewModel );
}

void MainView::Render()
{

}

void MainView::OnEvent( EVmEvent event )
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

