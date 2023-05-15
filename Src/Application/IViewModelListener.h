#pragma once

enum class EVmEvent
{
    Generic_ChangedConnection, 
    Generic_AppClose,
    Generic_NewMaps,
    Generic_MapsExpired
};

class IViewModelListener
{
public:
    virtual void OnEvent( EVmEvent event ) = 0;

    virtual ~IViewModelListener() = default;
};
