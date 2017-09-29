//
// MorseCode.h
//
#ifndef __MORSE_CODE_H__
#define __MORSE_CODE_H__

#include <initializer_list>
#include <vector>
#include "Util/Misc.h"

struct MorseCode
{
    //----------------------------------------------------------------------------------------------
    // Internal
    //----------------------------------------------------------------------------------------------
protected:

    // https://en.wikipedia.org/wiki/Morse_code
    // The length of a dot is one unit
    // The length of a dash is three units
    // The space between parts of the same letter is one unit
    // The space between letters is three units
    // The space between words i seven units
    enum class Duration
    {
        Dot = 1,
        Dash = 3,
        IntraLetter = 1,
        InterLetter = 3,
        InterWord = 7,
    };

    enum Pip { Dot, Dash };

    struct Symbol
    {
        const Pip*  _pips;
        int         _count;

        Symbol(std::initializer_list<Pip> pips)
        {
            _pips = pips.begin();
            _count = pips.size();
        }
    };

    static const Symbol A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
    static const Symbol Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine;

    static const Symbol* letters[];
    static const Symbol* digits[];

    static void appendUnits(std::vector<bool>& result, Duration duration, bool value)
    {
        for (int i = 0; i < int(duration); i++)
        {
            result.push_back(value);
        }
    }

    static void appendSymbol(bool atStartOfWord, std::vector<bool>& result, const Symbol* pSymbol)
    {
        if (!atStartOfWord)
        {
            appendUnits(result, Duration::InterLetter, false);
        }
        for (int iPip = 0; iPip < pSymbol->_count; iPip++)
        {
            if (iPip > 0)
            {
                appendUnits(result, Duration::IntraLetter, false);
            }
            switch (pSymbol->_pips[iPip])
            {
                case Dot:  appendUnits(result, Duration::Dot, true);  break;
                case Dash: appendUnits(result, Duration::Dash, true); break;
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

public:
    static std::vector<bool> encode(LPCSTR sz)
    {
        std::vector<bool> result;
        bool atStartOfWord = true;
        for (const char* pch = sz; *pch; pch++)
        {
            const char ch = *pch;
            if ('a' <= ch && ch <= 'z')
            {
                appendSymbol(atStartOfWord, result, letters[ch - 'a']);
                atStartOfWord = false;
            }
            else if ('A' <= ch && ch <= 'Z')
            {
                appendSymbol(atStartOfWord, result, letters[ch - 'Z']);
                atStartOfWord = false;
            }
            else if ('0' <= ch && ch <= '9')
            {
                appendSymbol(atStartOfWord, result, digits[ch - '0']);
                atStartOfWord = false;
            }
            else if (' ' == ch)
            {
                appendUnits(result, Duration::InterWord, false);
                atStartOfWord = true;
            }
        }
        return result;
    }

};


#endif
