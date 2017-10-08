//
// ArtnetConstants.h
//
#ifndef __ARNET_CONSTANTS_H__
#define __ARNET_CONSTANTS_H__

//==================================================================================================
// Constants
//==================================================================================================

// Default DMX address for our Artnet devices
const int DMX_ADDRESS_DEFAULT = 300;

const int CCH_ARTNET_SHORT_NAME = 18;
const int CCH_ARTNET_LONG_NAME = 64;

typedef char ARTNET_SHORT_NAME[CCH_ARTNET_SHORT_NAME];
typedef char ARTNET_LONG_NAME[CCH_ARTNET_LONG_NAME];
typedef int DMX_ADDRESS;    // one-based

enum class ArtnetOpMode
{
    Poll           = 0x2000,
    PollReply      = 0x2100,
    DiagData       = 0x2300,
    Command        = 0x2400,
    Dmx            = 0x5000,
    Nzs            = 0x5100,
    Address        = 0x6000,
    Input          = 0x7000,
    TodRequest     = 0x8000,
    TodData        = 0x8100,
    TodControl     = 0x8200,
    Rdm            = 0x8300,
    RdmSub         = 0x8400,
    VideoSetup     = 0xa010,
    VidoPalette    = 0xa020,
    VideoData      = 0xa040,
    VideoMaster    = 0xf000,
    MacMaster      = 0xf000,
    MacSlave       = 0xf100,
    FirmwaveMaster = 0xf200,
    FirmwareReply  = 0xf300,
    FileTnMaster   = 0xf400,
    FileFnMaster   = 0xf500,
    FileFnReply    = 0xf600,
    lpProg         = 0xf800,
    lpOProgReply   = 0xf900,
    Media          = 0x9000,
    MediaPatch     = 0x9100,
    MediaControl   = 0x9200,
    MediaContrlReply = 0x9300,
    TimeCode       = 0x9700,
    TimeSync       = 0x9800,
    Trigger        = 0x9900,
    Directory      = 0x9a00,
    DirectoryReply = 0x9b00,
};

enum class ArtnetOemCode // https://t.co/KtvcgxCo1n
{
    Unknown = 0x00ff,
    Global  = 0xffff,
};

enum class ArtnetReportCode
{
    Debug        = 0x0000,
    PowerOk      = 0x0001,
    PowerFail    = 0x0002,
    SocketWr1    = 0x0003,
    ParseFail    = 0x0004,
    UdpFail      = 0x0005,
    ShNameOk     = 0x0006,
    LoNameOk     = 0x0007,
    DmxError     = 0x0008,
    DmxUdpFull   = 0x0009,
    DmxRxFull    = 0x000a,
    SwitchErr    = 0x000b,
    ConfigErr    = 0x000c,
    DmxShort     = 0x000d,
    FirmwareFail = 0x000e,
    UserFail     = 0x000f,
    FactoryRes   = 0x0010,
};

const uint16_t ARTNET_PORT = 0x1936;
const int ARTNET_PROTOCOL_VERSION = 14;
const int ARTNET_FIRMWARE_MAJOR = 1;
const int ARTNET_FIRMWARE_MINOR = 0;

#endif
