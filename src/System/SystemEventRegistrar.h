//
// SystemEventRegistrar.h
//
#ifndef __SYSTEM_EVENT_REGISTRAR_H__
#define __SYSTEM_EVENT_REGISTRAR_H__

#include <vector>

struct SystemEventNotifications
{
    virtual void onNetworkStatus(system_event_t event, int eventParam) = 0;
    virtual void onCloudStatus(system_event_t event, int eventParam) = 0;
};


struct SystemEventRegistrar
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    static SystemEventRegistrar* theInstance;

protected:
    struct Event
    {
        bool seen;
        int eventParam;
        Event()
        {
            seen = false;
        }
    };

    std::vector<SystemEventNotifications*> _registrants;
    Event _networkEvent;
    Event _cloudEvent;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    SystemEventRegistrar()
    {
        theInstance = this;
        registerSystemEvents();
    }

    //----------------------------------------------------------------------------------------------
    // Registgration
    //----------------------------------------------------------------------------------------------

    void registerSystemEvents(SystemEventNotifications* registrant)
    {
        _registrants.push_back(registrant);

        // Deliver existing events if we have 'em
        if (_networkEvent.seen)
        {
            registrant->onNetworkStatus(network_status, _networkEvent.eventParam);
        }

        if (_cloudEvent.seen)
        {
            registrant->onCloudStatus(cloud_status, _cloudEvent.eventParam);
        }
        else
        {
            if (Particle.connected())
            {
                registrant->onCloudStatus(cloud_status, cloud_status_connected);
            }
        }
    }

    void unregisterSystemEvents(SystemEventNotifications* registrant)
    {
        for (auto it = _registrants.begin(); it != _registrants.end(); it++)
        {
            if (*it == registrant)
            {
                _registrants.erase(it);
                break;
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // System Events
    //----------------------------------------------------------------------------------------------

    void registerSystemEvents()
    {
        System.on(network_status, &staticOnNetworkStatus);
        System.on(cloud_status,   &staticOnCloudStatus);
    }

    static void staticOnNetworkStatus(system_event_t event, int eventParam)
    {
        theInstance->onNetworkStatus(event, eventParam);
    }
    static void staticOnCloudStatus(system_event_t event, int eventParam)
    {
        theInstance->onCloudStatus(event, eventParam);
    }

    void onNetworkStatus(system_event_t event, int eventParam)
    {
        _networkEvent.seen = true;
        _networkEvent.eventParam = eventParam;

        for (auto it = _registrants.begin(); it != _registrants.end(); it++)
        {
            (*it)->onNetworkStatus(network_status, eventParam);
        }
    }

    void onCloudStatus(system_event_t event, int eventParam)
    {
        _cloudEvent.seen = true;
        _cloudEvent.eventParam = eventParam;

        for (auto it = _registrants.begin(); it != _registrants.end(); it++)
        {
            (*it)->onCloudStatus(cloud_status, eventParam);
        }
    }

};

#endif
