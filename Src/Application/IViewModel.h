#pragma once

#include "Entities.h"

#include <vector>
#include <string>
#include <map>
#include <any>
#include <functional>

class IView;
class IViewModelListener;

using Action = std::function<void( void )>;
using MenuItemText = std::string;
using MenuItem = std::pair < MenuItemText, Action >;
using MenuItems = std::vector<MenuItem>;

using ViewModelInitParams = std::map<std::string, std::any>;

class IViewModel
{
public:
    virtual void Init( const ViewModelInitParams& params ) = 0;

    virtual void Scroll( int delta, Xy xy ) = 0;
    virtual void Touch( ETouchEvent event, LargeInteger fingerId, Xy xy ) = 0;
    virtual void Key( EKey key, EKeyAction action ) = 0;

    virtual void Resize( int width, int height, float pixelRatio ) = 0;

    virtual void BeforeViewRender() = 0;
    virtual void AfterViewRender() = 0;

    virtual const std::vector<std::string>& GetMenuItems() const = 0;
    virtual void MenuItemSelected( int index ) = 0;

    virtual bool IsConnected() const = 0;
    virtual void UpdateMaps() = 0;

    virtual ~IViewModel() = default;
};
