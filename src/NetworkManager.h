//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

struct NetworkManager
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int  loopCount = 0;
    bool connectAttempted = false;
    int  connectAttemptLoopCount = 0;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    NetworkManager()
    {
        // nothing to do
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    static void startup()
    {
        WiFi.selectAntenna(ANT_INTERNAL);
    }

    void begin()
    {
        WiFi.connect();
    }

    void loop()
    {
        if (WiFi.ready() && !connectAttempted)
        {
            Log.info("calling Particle.connect()...");
            Particle.connect();
            Log.info("...done");
            connectAttempted = true;
            connectAttemptLoopCount = loopCount;
        }
        if (connectAttempted && connectAttemptLoopCount == loopCount-1)
        {
            if (!Particle.connected())
            {
                Log.warn("Particle.connect() failed");
            }
            else
            {
                Log.info("Particle.connect() succeeded");
            }
        }
        Particle.process();
        loopCount++;
    }

    void report()
    {

    }

};

#endif
