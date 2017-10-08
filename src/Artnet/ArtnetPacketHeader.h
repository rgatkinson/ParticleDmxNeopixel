//
// ArtnetPacketHeader.h
//

//==================================================================================================
// ArtnetPacketHeaderData
//==================================================================================================

struct PACKED ArtnetPacketHeaderData
{
    static const int cbId = 8;
    static uint8_t rgbIdExpected[cbId];

    uint8_t _rgbId[cbId];   // 0, 8
    int16_t _opMode;        // 8, 2
};


//==================================================================================================
// ArtnetPacketHeader
//==================================================================================================

struct ArtnetPacketHeader
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArtnetPacketHeaderData* pData;
    int cbData;
    bool dataIsAllocated;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ArtnetPacketHeader(int cb, ArtnetOpMode artnetOpMode)
    {
        dataIsAllocated = true;
        pData = reinterpret_cast<ArtnetPacketHeaderData*>(mallocNoFail(cb));
        cbData = cb;
    }

    ArtnetPacketHeader(void* pb, int cb)
    {
        dataIsAllocated = false;
        pData = reinterpret_cast<ArtnetPacketHeaderData*>(pb);
        cbData = cb;
    }

    virtual ~ArtnetPacketHeader()
    {
        if (dataIsAllocated)
        {
            free(pData);
        }
    }

    void initialize(ArtnetOpMode artnetOpMode)
    {
        memset(pData, 0, cbData);
        memcpy(&pData->_rgbId[0], &ArtnetPacketHeaderData::rgbIdExpected[0], ArtnetPacketHeaderData::cbId);
        setArtnetOpMode(artnetOpMode);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool validate();

    void report()
    {
        Log.info("ArtnetPacketHeader reporting");
    }

    ArtnetOpMode artnetOpMode()
    {
        return (ArtnetOpMode)TypeConversion::bytesToInt(&pData->_opMode, 2, LittleEndian);
    }

    void setArtnetOpMode(ArtnetOpMode artnetOpMode)
    {
        TypeConversion::intToBytes((int)artnetOpMode, &pData->_opMode, 2, LittleEndian);
    }
};
