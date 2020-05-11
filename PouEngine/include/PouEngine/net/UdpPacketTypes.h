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

struct UdpPacket_Header
{
    int crc32;
    int type;

    template <typename Stream> int Serialize(Stream &stream)
    {
        return stream.computeBytes(
            stream.serializeBits(crc32, 32) +
            stream.serializeInteger(type, 0, NBR_PacketTypes)
                                   );
    }
};

}


#endif // UDPPACKETTYPES_H


