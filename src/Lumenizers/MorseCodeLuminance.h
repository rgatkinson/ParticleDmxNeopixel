//
// MorseCodeLuminance.h
//
#ifndef __MORSE_CODE_LUMINANCE_H__
#define __MORSE_CODE_LUMINANCE_H__

#include "Lumenizer.h"
#include "MorseCode.h"
#include "Util/Deadline.h"

struct MorseCodeLuminance : Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    enum class Message
    {
        None,
        QueensTrunk,            // p34
        Parrots,                // p34
        Pirates,                // p34
        HardISound,             // p34
        DontLetMeDown,          // p35
        IWill,                  // p36
        DavidByrne,
        First   = QueensTrunk,
        Last    = DavidByrne,
        Default = DavidByrne,
    };

    // https://en.wikipedia.org/wiki/Morse_code#Speed_in_words_per_minute
    static constexpr int msDotLengthMin     = 50;
    static constexpr int msDotLengthDefault = 200;
    static constexpr int msDotLengthLast    = 500;
    static constexpr float onLevel  = 1.0f;
    static constexpr float offLevel = 0.0f;

    int                 _msDotLength     = 0;
    Message             _message         = Message::None;
    LPCSTR              _messageString   = nullptr;
    int                 _encodingIndex   = 0;
    std::vector<bool>   _encodedMessage;
    Deadline            _timer;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    MorseCodeLuminance(int msDuration) : Lumenizer(Flavor::MorseCode, msDuration)
    {
        setMessage(Message::Default);
        setDotLength(msDotLengthDefault);
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
protected:

    void setMessage(Message message)
    {
        if (_message != message)
        {
            _message = message;
            _messageString = messageString(message);
            _encodedMessage = MorseCode::encode(_messageString);
            INFO("MorseCode: message=\"%s\"", _messageString);
            resetMessage();
        }
    }

    void setDotLength(int msDotLength)
    {
        if (_msDotLength != msDotLength)
        {
            INFO("_msDotLength = %d", msDotLength);
            _msDotLength = msDotLength;
            _timer = Deadline(_msDotLength);
        }
    }

    #define MESSAGE_LOOP "    "
    static LPCSTR messageString(Message message)
    {
        switch (message)
        {
            case Message::DavidByrne:   return "Hello, David Byrne!" MESSAGE_LOOP;
            case Message::QueensTrunk:  return "Daddy, the Queen's trunk is here, on board the Neverland!" MESSAGE_LOOP;
            case Message::Parrots:      return "Parrots? A flock of parrots?" MESSAGE_LOOP;
            case Message::Pirates:      return "PIRATES! We've been taken over by pirates!" MESSAGE_LOOP;
            case Message::HardISound:   return "Pirates! Oh, that hard i sound is so tricky" MESSAGE_LOOP;
            case Message::DontLetMeDown: return
                "MOLLY! The Wasp is bearing down on the Neverland!  "
                "Soon as we catch you, steer clear of Black Stache and BRING THE TRUNK TO ME!" MESSAGE_LOOP;
            case Message::IWill:        return "I will!" MESSAGE_LOOP;
            default:                    return "unknown message" MESSAGE_LOOP;
        }
    }
    #undef MESSAGE_LOOP

public:

    void processParameterBlock(ColorLuminanceParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);

        if (parameterBlock.luminanceSpeed() == 0)
        {
            setDotLength(msDotLengthDefault);
        }
        else
        {
            float speed = parameterBlock.luminanceSpeedLevel(false); // not directional
            int msDotLength = scaleRangeDiscrete(1-speed, 0, 1, msDotLengthMin, msDotLengthLast+1);
            setDotLength(msDotLength);
        }

        if (parameterBlock.luminanceControl() == 0)
        {
            setMessage(Message::Default);
        }
        else
        {
            Message message = scaleRangeDiscrete(parameterBlock.luminanceControl(), 1, 255, Message::First, Message::Last);
            setMessage(message);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        resetMessage();
    }

    void loop() override
    {
        Lumenizer::loop();
        if (_timer.hasExpired())
        {
            // Set level accordingly
            if (_encodingIndex < (int)_encodedMessage.size())
            {
                setCurrentLevel(_encodedMessage[_encodingIndex] ? onLevel : offLevel);
            }

            // Advance
            _encodingIndex++;
            if (_encodingIndex >= (int)_encodedMessage.size())
            {
                _encodingIndex = 0;
            }
            _timer.reset();
        }
    }

    void report() override
    {
        Lumenizer::report();
    }

protected:

    void resetMessage()
    {
        _encodingIndex = 0;
        _timer.expire();
    }

};
#endif
