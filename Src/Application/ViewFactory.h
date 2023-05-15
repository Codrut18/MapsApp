#pragma once

#include "IViewFactory.h"

class IMainWindow;

class ViewFactory : public IViewFactory
{
public:
    ViewFactory( IMainWindow& mainWindow );

    IView* BuildView( EView viewType ) override;

private:
    IMainWindow& m_mainWindow;
};
