#include "NavigationService.h"

#include "IViewFactory.h"
#include "IViewModelFactory.h"

#include "IView.h"
#include "IViewModel.h"


NavigationService::NavigationService()
    : m_viewModelFactory( nullptr )
    , m_viewFactory( nullptr )
    , m_currentView( nullptr )
    , m_currentViewModel( nullptr )
{

}

void NavigationService::SetViewFactory( IViewFactory* factory )
{
    m_viewFactory = factory;
}

void NavigationService::SetViewModelFactory( IViewModelFactory* factory )
{
    m_viewModelFactory = factory;
}

void NavigationService::SetViewChangeCallback( ViewChangeCallback callback )
{
	m_viewChangeCallback = callback;
}

bool NavigationService::GoToView( EView viewType, const ViewModelInitParams& params )
{
    if ( auto view = m_viewFactory->BuildView( viewType ) )
    {
        if ( auto viewModel = m_viewModelFactory->BuildViewModel( viewType, view ) )
        {
            view->SetViewModel( viewModel );

            viewModel->Init( params );

            if ( m_currentView )
                delete m_currentView;
            if ( m_currentViewModel )
                delete m_currentViewModel;

            m_currentView = view;
            m_currentViewModel = viewModel;

            if ( m_viewChangeCallback )
                m_viewChangeCallback( m_currentView );

            return true;
        }
    }

    return false;
}
