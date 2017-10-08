//
// ArtnetPollPacket.h
//

//==================================================================================================
// ArtPollPacketData
//==================================================================================================

struct PACKED ArtPollPacketData : ArtnetPacketHeaderData
{
    int8_t  _protVerHi;
    int8_t  _protVerLo;
    int8_t  _talkToMe;
    int8_t  _priority;
};

//==================================================================================================
// ArtPollPacket
//==================================================================================================

struct ArtPollPacket : ArtnetPacketHeader
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArtPollPacketData* pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ArtPollPacket() : ArtnetPacketHeader(sizeof(ArtPollPacketData), ArtnetOpMode::Poll)
    {
        pData = reinterpret_cast<ArtPollPacketData*>(ArtnetPacketHeader::pData);
    }

    ArtPollPacket(void* pb, int cb) : ArtnetPacketHeader(pb, cb)
    {
        pData = reinterpret_cast<ArtPollPacketData*>(ArtnetPacketHeader::pData);
    }

    void initialize()
    {
        ArtnetPacketHeader::initialize(ArtnetOpMode::Poll);
        setProtocolVersion(ARTNET_PROTOCOL_VERSION);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    static int cb()
    {
        return sizeof(ArtPollPacketData);
    }

    bool validate()
    {
        return (cbData >= cb()) && (protocolVersion() >= ARTNET_PROTOCOL_VERSION);
    }

    int protocolVersion()
    {
        return TypeConversion::bytesToInt(&pData->_protVerHi, 2, BigEndian);
    }
    void setProtocolVersion(int protocolVersion)
    {
        TypeConversion::intToBytes(protocolVersion, &pData->_protVerHi, 2, BigEndian);
    }

    void pollReply(const IPAddress&, ArtnetDevice&);
};
