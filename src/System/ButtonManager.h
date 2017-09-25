//
// ButtonManager.h
//
#ifndef __BUTTON_MANAGER_H__
#define __BUTTON_MANAGER_H__

#include "Particle.h"

struct ButtonManager
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    typedef void (*PFN)();

protected:
    PFN _pfn;
    bool _buttonPressed;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ButtonManager(PFN pfn)
    {
        _pfn = pfn;
        _buttonPressed = false;
    }


    bool buttonIsPressed()
    {
        return System.buttonPushed() > 0;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
    }

    void loop()
    {
        if (buttonIsPressed())
        {
            if (!_buttonPressed)
            {
                _buttonPressed = true;
                _pfn();
            }
        }
        else
        {
            _buttonPressed = false;
        }
    }
};

#endif
