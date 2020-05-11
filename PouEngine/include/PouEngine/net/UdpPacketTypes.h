#ifndef UDPPACKETTYPES_H
#define UDPPACKETTYPES_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

namespace pou
{

enum PacketType
{
    PacketType_Unkwnow,
    PacketType_Connection,
    PacketType_Diconnection,
    PacketType_CustomData,
    NBR_PacketTypes,
};

struct UdpPacket_AskConnection
{
    int crc32;

    PacketType type;

    bool test;
    char charTest;

    template <typename Stream> int Serialize(Stream &stream)
    {
        return stream.computeBytes(
            stream.serializeBits(crc32, 32) +
            stream.serializeBool(test) +
            stream.serializeChar(charTest)
                                   );

    }
};

}


#endif // UDPPACKETTYPES_H


