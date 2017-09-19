//
// DmxAddresses.h
//
// This logically belongs in a project *using* the library, not in the library
// itself, but illustrates how using the library across multiple DMX addresses
// can work.

const int DMX_ADDRESS_FIRST = 300;
const int DMX_ADDRESS_AmuletOne = DMX_ADDRESS_FIRST;
const int DMX_ADDRESS_AmuletTwo = DMX_ADDRESS_AmuletOne + Amulet::DMX_ADDRESS_COUNT;
