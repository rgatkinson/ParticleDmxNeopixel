//
// Colorizer.cpp
//
#include "Particle.h"
#include "Colorizer.h"

//==================================================================================================
// Functions
//==================================================================================================

void Colorizer::setPixelColor(uint16_t iPixel, COLOR_INT color)
{
    pColorizeable->setPixelColor(iPixel, color);
}
