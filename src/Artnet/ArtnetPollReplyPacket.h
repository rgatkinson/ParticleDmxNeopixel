//
// ArtnetPollReplyPacket.h
//

//==================================================================================================
// ArtPollReplyPacketData
//==================================================================================================

struct PACKED ArtPollReplyPacketData : ArtnetPacketHeaderData
{
    uint8_t     _ipAddress[4];   // 3: 10
    int16_t     _port;           // 4: 14. little endian
    int8_t      _versInfoH;      // 5: 16
    int8_t      _versInfoL;      // 6: 17
    int8_t      _netSwitch;      // 7: 18
    int8_t      _subSwitch;      // 8: 19
    int8_t      _oemHi;          // 9: 20=0x14
    int8_t      _oemLo;          // 10: 21=0x15
    int8_t      _ubeaVersion;    // 11: 22=0x16
    int8_t      _status1;        // 12: 23=0x17
    int8_t      _estaManLo;      // 13: 24=0x18
    int8_t      _estaManHi;      // 14: 25=0x19
    ARTNET_SHORT_NAME  _shortName;      // 15: 26=0x1a
    ARTNET_LONG_NAME   _longName;       // 16: 44=0x2c
    ARTNET_LONG_NAME   _nodeReport;     // 17: 108=0x6c
    int8_t      _numPortsHi;     // 18: 172=0xac
    int8_t      _numPortsLo;     // 19: 173=0xad
    int8_t      _portTypes[4];   // 20: 174=0xae
    int8_t      _goodInput[4];   // 21: 178
    int8_t      _goodOutput[4];  // 22: 182
    int8_t      _swIn[4];        // 23: 186
    int8_t      _swOut[4];       // 24: 190
    int8_t      _swVideo;        // 25: 194
    int8_t      _swMacro;        // 26: 195
    int8_t      _swRemote;       // 27: 196
    int8_t      _spare[3];       // 28-30: 197-199: 0xc5-0xc7
    int8_t      _style;          // 31: 200
    byte        _macAddress[6];  // 32-37: 201-206: 0xc9-0xce
    int8_t      _bindIp[4];      // 38: 207=0xcf
    int8_t      _bindIndex;      // 39: 212
    int8_t      _status2;        // 40: 213
    int8_t      _filler[26];     // 41: 214-239
};

//==================================================================================================
// ArtPollReplyPacket
//==================================================================================================

struct ArtPollReplyPacket : ArtnetPacketHeader
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArtPollReplyPacketData* pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ArtPollReplyPacket() : ArtnetPacketHeader(sizeof(ArtPollReplyPacketData), ArtnetOpMode::PollReply)
    {
        pData = reinterpret_cast<ArtPollReplyPacketData*>(ArtnetPacketHeader::pData);
    }

    ArtPollReplyPacket(void* pb, int cb) : ArtnetPacketHeader(pb, cb)
    {
        pData = reinterpret_cast<ArtPollReplyPacketData*>(ArtnetPacketHeader::pData);
    }

    void initialize()
    {
        ArtnetPacketHeader::initialize(ArtnetOpMode::PollReply);

        HAL_IPAddress& halIpAddress = WiFi.localIP().raw();
        memcpy(&pData->_ipAddress[0], &halIpAddress, 4);
        setPort(ARTNET_PORT);
        pData->_versInfoH = ARTNET_FIRMWARE_MAJOR;
        pData->_versInfoL = ARTNET_FIRMWARE_MINOR;
        setOemCode(ArtnetOemCode::Unknown);
        WiFi.macAddress(pData->_macAddress);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    static int cb()
    {
        return sizeof(ArtPollReplyPacketData);
    }

    IPAddress ipAddress()
    {
        return IPAddress(pData->_ipAddress);
    }
    void setIpAddress(IPAddress ipAddress)
    {
        pData->_ipAddress[0] = ipAddress[0];
        pData->_ipAddress[1] = ipAddress[1];
        pData->_ipAddress[2] = ipAddress[2];
        pData->_ipAddress[3] = ipAddress[3];
    }

    int port()
    {
        return TypeConversion::bytesToInt(&pData->_port, 2, LittleEndian);
    }
    void setPort(int port)
    {
        TypeConversion::intToBytes(port, &pData->_port, 2, LittleEndian);
    }

    ArtnetOemCode oemCode()
    {
        return (ArtnetOemCode)TypeConversion::bytesToInt(&pData->_oemHi, 2, BigEndian);
    }
    void setOemCode(ArtnetOemCode oemCode)
    {
        TypeConversion::intToBytes((int)oemCode, &pData->_oemHi, 2, BigEndian);
    }

    LPSTR shortName()
    {
        return &pData->_shortName[0];
    }
    void setShortName(LPCSTR sz)
    {
        safeStrncpy(&pData->_shortName[0], sizeof(pData->_shortName), sz);
    }

    LPSTR longName()
    {
        return &pData->_longName[0];
    }
    void setLongName(LPCSTR sz)
    {
        safeStrncpy(&pData->_longName[0], sizeof(pData->_longName), sz);
    }

    LPSTR nodeReport()
    {
        return &pData->_nodeReport[0];
    }
    void setNodeReport(LPCSTR sz)
    {
        safeStrncpy(&pData->_nodeReport[0], sizeof(pData->_nodeReport), sz);
    }
    void setNodeReport(ArtnetReportCode reportCode, int counter)
    {
        char sz[sizeof(pData->_nodeReport)];
        snprintf(&sz[0], sizeof(sz), "#%04x [%d] Detailed status not provided", reportCode, counter);
        setNodeReport(sz);
    }
};
