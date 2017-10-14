//
// KridaDimmerChannel.cpp
//
#include "Particle.h"
#include "KridaDimmerChannel.h"
#include "KridaDimmer.h"

void KridaDimmerChannel::setValue(Value value)
{
    if (_value != value)
    {
        _value = value;
        _pParent->noteChannelValueChange(this);
        // INFO("KridaDimmerChannel: value=%d", _value);
    }
}
