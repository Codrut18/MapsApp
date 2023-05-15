#pragma once

#include "IView.h"

class IMainWindow;

class BaseView : public IView
{
public:
    BaseView( IMainWindow* window );

    void OnEvent( EVmEvent event );

protected:
    float DPI( int size ) const;

protected:
    IMainWindow* m_parentWindow;
};
