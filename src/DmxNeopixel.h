#pragma once

/* DmxNeopixel library by Robert Atkinson
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

// This is your main class that users will import into their application
class DmxNeopixel
{
public:
  /**
   * Constructor
   */
  DmxNeopixel();

  /**
   * Example method
   */
  void begin();

  /**
   * Example method
   */
  void process();

private:
  /**
   * Example private method
   */
  void doit();
};
