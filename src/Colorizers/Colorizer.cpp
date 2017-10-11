//
// Colorizer.cpp
//
#include "Particle.h"
#include "Colorizer.h"

//==================================================================================================
// Functions
//==================================================================================================

void Colorizer::setPixelColor(uint16_t iPixel, Color color)
{
    _pColorizeable->setPixelColor(iPixel, color);
}
