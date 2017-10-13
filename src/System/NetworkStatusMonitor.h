//
// NetworkStatusMonitor.h
//
#ifndef __NETWORK_STATUS_MONITOR_H__
#define __NETWORK_STATUS_MONITOR_H__

#include "SystemEventRegistrar.h"

struct NetworkStatusMonitor : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    NetworkStatus _status = NetworkStatus::Off;

public:
    static NetworkStatusMonitor* theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

public:
    NetworkStatusMonitor()
    {
        theInstance = this;
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~NetworkStatusMonitor()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System Events
    //----------------------------------------------------------------------------------------------

    NetworkStatus status()
    {
        return _status;
    }

    void onNetworkStatus(NetworkStatus netStatus) override
    {
        _status = (NetworkStatus)netStatus;
    }

    void onCloudStatus(int cloudStatus) override
    {
    }

    void onTimeChanged(int timeStatus) override
    {
    }

};

#endif
