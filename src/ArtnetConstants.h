//
// ArtnetConstants.h
//

//==================================================================================================
// Constants
//==================================================================================================

enum ArtnetOpMode
{
    ArtnetOpModePoll           = 0x2000,
    ArtnetOpModePollReply      = 0x2100,
    ArtnetOpModeDiagData       = 0x2300,
    ArtnetOpModeCommand        = 0x2400,
    ArtnetOpModeDmx            = 0x5000,
    ArtnetOpModeNzs            = 0x5100,
    ArtnetOpModeAddress        = 0x6000,
    ArtnetOpModeInput          = 0x7000,
    ArtnetOpModeTodRequest     = 0x8000,
    ArtnetOpModeTodData        = 0x8100,
    ArtnetOpModeTodControl     = 0x8200,
    ArtnetOpModeRdm            = 0x8300,
    ArtnetOpModeRdmSub         = 0x8400,
    ArtnetOpModeVideoSetup     = 0xa010,
    ArtnetOpModeVidoPalette    = 0xa020,
    ArtnetOpModeVideoData      = 0xa040,
    ArtnetOpModeVideoMaster    = 0xf000,
    ArtnetOpModeMacMaster      = 0xf000,
    ArtnetOpModeMacSlave       = 0xf100,
    ArtnetOpModeFirmwaveMaster = 0xf200,
    ArtnetOpModeFirmwareReply  = 0xf300,
    ArtnetOpModeFileTnMaster   = 0xf400,
    ArtnetOpModeFileFnMaster   = 0xf500,
    ArtnetOpModeFileFnReply    = 0xf600,
    ArtnetOpModelpProg         = 0xf800,
    ArtnetOpModelpOProgReply   = 0xf900,
    ArtnetOpModeMedia          = 0x9000,
    ArtnetOpModeMediaPatch     = 0x9100,
    ArtnetOpModeMediaControl   = 0x9200,
    ArtnetOpModeMediaContrlReply = 0x9300,
    ArtnetOpModeTimeCode       = 0x9700,
    ArtnetOpModeTimeSync       = 0x9800,
    ArtnetOpModeTrigger        = 0x9900,
    ArtnetOpModeDirectory      = 0x9a00,
    ArtnetOpModeDirectoryReply = 0x9b00,
};

enum OemCode // https://t.co/KtvcgxCo1n
{
    OemUnknown = 0x00ff,
    OemGlobal  = 0xffff,
};

enum ReportCode
{
    RcDebug        = 0x0000,
    RcPowerOk      = 0x0001,
    RcPowerFail    = 0x0002,
    RcSocketWr1    = 0x0003,
    RcParseFail    = 0x0004,
    RcUdpFail      = 0x0005,
    RcShNameOk     = 0x0006,
    RcLoNameOk     = 0x0007,
    RcDmxError     = 0x0008,
    RcDmxUdpFull   = 0x0009,
    RcDmxRxFull    = 0x000a,
    RcSwitchErr    = 0x000b,
    RcConfigErr    = 0x000c,
    RcDmxShort     = 0x000d,
    RcFirmwareFail = 0x000e,
    RcUserFail     = 0x000f,
    RcFactoryRes   = 0x0010,
};

const uint16_t ARTNET_PORT = 0x1936;
const int ARTNET_PROTOCOL_VERSION = 14;
const int FIRMWARE_MAJOR = 1;
const int FIRMWARE_MINOR = 0;

typedef int DMX_ADDRESS;    // one-based
