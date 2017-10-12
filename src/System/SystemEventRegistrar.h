//
// SystemEventRegistrar.h
//
#ifndef __SYSTEM_EVENT_REGISTRAR_H__
#define __SYSTEM_EVENT_REGISTRAR_H__

#include <vector>
#include <functional>
#include "Util/Deadline.h"

//==================================================================================================
// SystemEventNotifications
//==================================================================================================

struct SystemEventNotifications
{
    virtual void onNetworkStatus(int netStatus) = 0;
    virtual void onCloudStatus(int cloudStatus) = 0;
    virtual void onTimeChanged(int timeStatus) = 0;
};

struct ButtonEventNotifications
{
    virtual void onButtonClick(int clickCount) = 0;
    virtual void onButtonFinalClick(int clickCount) = 0;
};


//==================================================================================================
// SystemEventRegistrar
//==================================================================================================

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

    struct ButtonCallable
    {
        std::function<void(int)> pfn;
        int                      token;
        ButtonCallable(std::function<void(int)> pfn, int token)
        {
            this->pfn = pfn;
            this->token = token;
        }
    };

    std::vector<SystemEventNotifications*> _systemNotifications;
    std::vector<ButtonEventNotifications*> _buttonNotifications;
    Event _networkEvent;
    Event _cloudEvent;
    Event _timeEvent;

    std::vector<ButtonCallable>            _buttonClickFunctions;
    std::vector<ButtonCallable>            _buttonFinalClickFunctions;
    int                                    _nextToken = 0;
    bool                                   _buttonPressed;
    Deadline                               _buttonUpTimer;
    int                                    _clickCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    SystemEventRegistrar()
        : _buttonUpTimer(500)
    {
        theInstance = this;
        registerSystemEvents();
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool isCloudConnected()
    {
        return _cloudEvent.seen
            ? _cloudEvent.eventParam==cloud_status_connected
            : Particle.connected();
    }

    bool isTimeSyncd()
    {
        return _timeEvent.seen && _timeEvent.eventParam==time_changed_sync;
    }

    //----------------------------------------------------------------------------------------------
    // Registgration
    //----------------------------------------------------------------------------------------------

    void registerSystemEvents(SystemEventNotifications* registrant)
    {
        _systemNotifications.push_back(registrant);

        // Deliver existing events if we have 'em
        if (_networkEvent.seen)
        {
            registrant->onNetworkStatus(_networkEvent.eventParam);
        }

        if (_cloudEvent.seen)
        {
            registrant->onCloudStatus(_cloudEvent.eventParam);
        }
        else
        {
            if (Particle.connected())
            {
                registrant->onCloudStatus(cloud_status_connected);
            }
        }
        if (_timeEvent.seen)
        {
            registrant->onTimeChanged(_timeEvent.eventParam);
        }
    }
    void unregisterSystemEvents(SystemEventNotifications* registrant)
    {
        unregister(_systemNotifications, registrant);
    }

    //-----------------------------------------

    void registerButtonEvents(ButtonEventNotifications* registrant)
    {
        _buttonNotifications.push_back(registrant);
    }
    void unregisterButtonEvents(ButtonEventNotifications* registrant)
    {
        unregister(_buttonNotifications, registrant);
    }

    //-----------------------------------------

    int registerButtonClick(std::function<void(int)> pfn)
    {
        int token = _nextToken++;
        _buttonClickFunctions.push_back(ButtonCallable(pfn, token));
        return token;
    }
    void unregisterButtonClick(int token)
    {
        for (auto it = _buttonClickFunctions.begin(); it != _buttonClickFunctions.end(); it++)
        {
            if ((*it).token == token)
            {
                _buttonClickFunctions.erase(it);
                break;
            }
        }
    }

    //-----------------------------------------

    int registerButtonFinalClick(std::function<void(int)> pfn)
    {
        int token = _nextToken++;
        _buttonFinalClickFunctions.push_back(ButtonCallable(pfn, token));
        return token;
    }
    void unregisterButtonFinalClick(int token)
    {
        for (auto it = _buttonFinalClickFunctions.begin(); it != _buttonFinalClickFunctions.end(); it++)
        {
            if ((*it).token == token)
            {
                _buttonFinalClickFunctions.erase(it);
                break;
            }
        }
    }

protected:
    template <typename T>
    static void unregister(std::vector<T>& vector, const T& value)
    {
        for (auto it = vector.begin(); it != vector.end(); it++)
        {
            if (*it == value)
            {
                vector.erase(it);
                break;
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // System Events
    //----------------------------------------------------------------------------------------------
protected:

    void registerSystemEvents()
    {
        System.on(network_status,       &staticOnNetworkStatus);
        System.on(cloud_status,         &staticOnCloudStatus);
        System.on(time_changed,         &staticOnTimeChanged);
    }

    static void staticOnNetworkStatus(system_event_t event, int eventParam)
    {
        theInstance->onNetworkStatus(event, eventParam);
    }
    static void staticOnCloudStatus(system_event_t event, int eventParam)
    {
        theInstance->onCloudStatus(event, eventParam);
    }
    static void staticOnTimeChanged(system_event_t event, int eventParam)
    {
        theInstance->onCloudTimeChanged(event, eventParam);
    }

    void onNetworkStatus(system_event_t event, int eventParam)
    {
        _networkEvent.seen = true;
        _networkEvent.eventParam = eventParam;

        for (auto it = _systemNotifications.begin(); it != _systemNotifications.end(); it++)
        {
            (*it)->onNetworkStatus(eventParam);
        }
    }

    void onCloudStatus(system_event_t event, int eventParam)
    {
        _cloudEvent.seen = true;
        _cloudEvent.eventParam = eventParam;

        for (auto it = _systemNotifications.begin(); it != _systemNotifications.end(); it++)
        {
            (*it)->onCloudStatus(eventParam);
        }
    }

    void onCloudTimeChanged(system_event_t event, int eventParam)
    {
        _timeEvent.seen = true;
        _timeEvent.eventParam = eventParam;

        for (auto it = _systemNotifications.begin(); it != _systemNotifications.end(); it++)
        {
            (*it)->onTimeChanged(eventParam);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Buttons
    //----------------------------------------------------------------------------------------------

    bool systemButtonIsPressed()
    {
        return System.buttonPushed() > 0;
    }

    void buttonBegin()
    {
        _buttonPressed = systemButtonIsPressed();
        _clickCount = 0;
    }

    void buttonLoop()
    {
        if (systemButtonIsPressed())
        {
            if (!_buttonPressed)
            {
                // Button just went down
                _buttonPressed = true;
            }
        }
        else
        {
            if (_buttonPressed)
            {
                // Button just went up
                _buttonPressed = false;
                _clickCount++;
                onButtonClick(button_click, clip(_clickCount, 0, 0x0F));
                _buttonUpTimer.reset();
            }
            else
            {
                // Button has been up a while. Have we run out of patience waiting for more clicks?
                if (_buttonUpTimer.hasExpired())
                {
                    if (_clickCount > 0)
                    {
                        onButtonFinalClick(button_click, clip(_clickCount, 0, 0x0F));
                        _clickCount = 0;
                    }
                }
            }
        }
    }

    void onButtonClick(system_event_t event, int eventParam)
    {
        int clickCount = system_button_clicks(eventParam);

        for (auto it = _buttonNotifications.begin(); it != _buttonNotifications.end(); it++)
        {
            (*it)->onButtonClick(clickCount);
        }

        for (auto it = _buttonClickFunctions.begin(); it != _buttonClickFunctions.end(); it++)
        {
            (*it).pfn(clickCount);
        }
    }

    void onButtonFinalClick(system_event_t event, int eventParam)
    {
        int clickCount = system_button_clicks(eventParam);

        for (auto it = _buttonNotifications.begin(); it != _buttonNotifications.end(); it++)
        {
            (*it)->onButtonFinalClick(clickCount);
        }

        for (auto it = _buttonFinalClickFunctions.begin(); it != _buttonFinalClickFunctions.end(); it++)
        {
            (*it).pfn(clickCount);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin()
    {
        buttonBegin();
    }

    void loop()
    {
        buttonLoop();
    }

};

#endif
