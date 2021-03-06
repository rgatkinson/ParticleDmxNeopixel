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
    static InstanceHolder<NetworkStatusMonitor> theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

public:
    NetworkStatusMonitor()
    {
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

decltype(NetworkStatusMonitor::theInstance) SELECTANY NetworkStatusMonitor::theInstance;


#endif
