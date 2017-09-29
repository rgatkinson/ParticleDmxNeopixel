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
        First,
        DavidByrne = First,
        QueensTrunk,            // p34
        Parrots,                // p34
        Pirates,                // p34
        HardISound,             // p34
        DontLetMeDown,          // p35
        IWill,                  // p36
        Last = DavidByrne,
        Default = DavidByrne,
    };

    // https://en.wikipedia.org/wiki/Morse_code#Speed_in_words_per_minute
    static constexpr int msDotLengthDefault = 200;
    static constexpr float onLevel = 1.0f;
    static constexpr float offLevel = 0.0f;

    int                 _msDotLength     = msDotLengthDefault;
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
        _timer = Deadline(_msDotLength);
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

    #define MESSAGE_LOOP "    "
    static LPCSTR messageString(Message message)
    {
        switch (message)
        {
            case Message::DavidByrne:   return "Hello, David Byrne! How are you today?" MESSAGE_LOOP;
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

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);
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
            if (_encodingIndex < _encodedMessage.size())
            {
                setCurrentLevel(_encodedMessage[_encodingIndex] ? onLevel : offLevel);
            }

            // Advance
            _encodingIndex++;
            if (_encodingIndex >= _encodedMessage.size())
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
