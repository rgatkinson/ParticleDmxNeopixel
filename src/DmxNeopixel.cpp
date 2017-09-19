/* DmxNeopixel library by Robert Atkinson
 */

#include "DmxNeopixel.h"

/**
 * Constructor.
 */
DmxNeopixel::DmxNeopixel()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void DmxNeopixel::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void DmxNeopixel::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void DmxNeopixel::doit()
{
    Serial.println("called doit");
}
