//
// TypeConversion.h
//
#ifndef _TYPE_CONVERSION_H_
#define _TYPE_CONVERSION_H_

enum ByteOrder
{
    LittleEndian,
    BigEndian
};

struct TypeConversion
{
    template<class T>
    static int bytesToInt(T* pt, int cbIn, ByteOrder byteOrder)
    {
        int result = 0;
        int cb = cbIn;
        byte* pb = reinterpret_cast<byte*>(pt);
        pb = (byteOrder==BigEndian ? pb : pb + cb -1);  // MSB
        while (cb > 0)
        {
            result = (result << 8) | *pb;
            cb -= 1;
            pb += (byteOrder==BigEndian ? 1 : -1);
        }
        return result;
    }

    template<class T>
    static void intToBytes(int value, T* pt, int cbIn, ByteOrder byteOrder)
    {
        int cb = cbIn;
        byte* pb = reinterpret_cast<byte*>(pt);
        pb = (byteOrder==LittleEndian ? pb : pb + cb -1);  // LSB
        while (cb > 0)
        {
            *pb = byte(value);
            value = value >> 8;
            cb--;
            pb += (byteOrder==LittleEndian ? 1 : -1);
        }
    }
};

#endif
