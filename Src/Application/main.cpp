#include "IMapService.h"
#include "NavigationService.h"

#include "MainUi.h"

#include "ViewFactory.h"
#include "ViewModelFactory.h"

const int WINDOW_WIDTH = 480;
const int WINDOW_HEIGHT = 800;

int main( int argc, char** argv )
{
    setbuf( stdout, 0 );

    std::string logFilePath = argc > 1 ? argv[1] : "";

    // Initialize UI
    MainUi ui;
    if ( ui.Init(WINDOW_WIDTH, WINDOW_HEIGHT) != 0 )
        return -1;

    ui.SetMouseEnabled( true );
    ui.SetTouchEnabled( false );
    ui.SetKeyEnabled( true );

    // Initialize map service
    IMapServicePtr mapService = IMapService::Produce( logFilePath );
    if ( !mapService )
        return -2;

    mapService->InitGLContext( ui.GetWindowInfo() );

    // Create navigation service
    NavigationService navigationService;

    // Create views & view models factories
    ViewModelFactory viewModelFactory( mapService.get(), &navigationService );
    ViewFactory viewFactory( ui );

    // Initialize navigation service components
    navigationService.SetViewFactory( &viewFactory );
    navigationService.SetViewModelFactory( &viewModelFactory );
    navigationService.SetViewChangeCallback( [&ui]( IView* view ) { ui.SetView( view ); } );

    // Go to the main view
    navigationService.GoToView( EView::Main );

    // Setup & show UI
    ui.SetBeforeRenderCallback( [mapService]() { mapService->Tick(); } );
    ui.Show();

    return 0;
}

#if defined(_WIN32)
#include <Windows.h> 
#include <Shellapi.h>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    int argc;
    LPWSTR* szArglist = CommandLineToArgvW( GetCommandLineW(), &argc );
    char** argv = new char* [argc];
    for( int i = 0; i < argc; i++ ) {
        int lgth = wcslen( szArglist[i] );
        argv[i] = new char[lgth + 1];
        for( int j = 0; j <= lgth; j++ )
            argv[i][j] = char( szArglist[i][j] );
    }
    LocalFree( szArglist );

    main( argc, argv );
}
#endif
