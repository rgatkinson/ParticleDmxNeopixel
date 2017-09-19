// Example usage for DmxNeopixel library by Robert Atkinson.

#include "DmxNeopixel.h"

// Initialize objects from the lib
DmxNeopixel dmxNeopixel;

void setup() {
    // Call functions on initialized library objects that require hardware
    dmxNeopixel.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    dmxNeopixel.process();
}
