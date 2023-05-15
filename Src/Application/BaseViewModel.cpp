#include "BaseViewModel.h"

#include "IView.h"
#include "IResourceRepository.h"

BaseViewModel::BaseViewModel( IMapService* mapService, INavigationService* navigationService, IViewModelListener* listener )
    : m_mapService( mapService )
    , m_navigationService( navigationService )
    , m_listener( listener )
    , m_mapView( mapService->GetMapView() )
{
    m_mapService->AddListener( this );
}

BaseViewModel::~BaseViewModel()
{
    m_mapService->RemoveListener( this );
}

void BaseViewModel::Init( const ViewModelInitParams& params )
{

}

void BaseViewModel::Scroll( int delta, Xy xy )
{
    if (m_mapView)
        m_mapView->HandleScroll( delta, xy );
}

void BaseViewModel::Touch( ETouchEvent event, LargeInteger fingerId, Xy xy )
{
    if (m_mapView)
        m_mapView->HandleTouch( event, fingerId, xy );
}

void BaseViewModel::Key( EKey key, EKeyAction keyAction )
{
    if (m_mapView)
        m_mapView->HandleKey( key, keyAction );
}

void BaseViewModel::Resize( int width, int height, float pixelRatio )
{
    if (m_mapService)
        m_mapService->Resize( Size( width, height ), pixelRatio );
}

void BaseViewModel::BeforeViewRender()
{
    m_action = nullptr;
}

void BaseViewModel::AfterViewRender()
{
    if (m_action)
        m_action();
}

void BaseViewModel::MenuItemSelected( int index )
{
    m_action = m_menuItems[index].second;
}

const std::vector<std::string>& BaseViewModel::GetMenuItems() const
{
    return m_menuCaptions;
}

bool BaseViewModel::IsConnected() const
{
    return m_mapService->IsConnected();
}

void BaseViewModel::UpdateMaps()
{
    m_mapService->UpdateMaps();
}

void BaseViewModel::OnMapServiceEvent( EMapServiceEvent event )
{
    switch ( event )
    {
    case EMapServiceEvent::Connected:
        break;
    case EMapServiceEvent::Disconnected:
        break;
    case EMapServiceEvent::ExpiredMaps:
    {
        m_listener->OnEvent( EVmEvent::Generic_MapsExpired );
        break;
    }
    case EMapServiceEvent::NewMaps:
    {
        m_listener->OnEvent( EVmEvent::Generic_NewMaps );
        break;
    }
    case EMapServiceEvent::NewStyles:
        break;
    }
}

void BaseViewModel::SetMenuItems( const MenuItems& items )
{
    m_menuItems = items;

    m_menuCaptions.clear();

    for (const auto& el : items)
        m_menuCaptions.push_back( el.first );
}

IMapService* BaseViewModel::GetMapService()
{
    return m_mapService;
}
