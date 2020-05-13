#ifndef UDPPACKETTYPES_H
#define UDPPACKETTYPES_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

#include <vector>

namespace pou
{

static const int MAX_PACKETSIZE = 1024;
static const int MAX_PACKETFRAGS = 256;
static const int SERIAL_CHECK = 69420;
static const int SALT_SIZE = 8;

enum PacketType
{
    PacketType_Fragment,
    PacketType_ConnectionMsg,
    PacketType_Data,
    NBR_PacketTypes,
    PacketCorrupted,
};

enum ConnectionMessage
{
    ConnectionMessage_ConnectionRequest,
    ConnectionMessage_Challenge,
    ConnectionMessage_ConnectionAccepted,
    ConnectionMessage_ConnectionDenied,
    ConnectionMessage_Disconnection,
    ConnectionMessage_Ping,
    NBR_ConnectionMessages,
};

struct UdpPacket
{
    int crc32;
    int sequence;
    int type;
    int serial_check;

    virtual void serializeImpl(Stream *stream){}

    int serializeHeader(Stream *stream, bool flush = true)
    {
        stream->serializeBits(crc32, 32);
        stream->serializeBits(sequence, 16);
        stream->serializeInt(type, 0, NBR_PacketTypes-1);

        if(flush)
        {
            stream->flush();
            return stream->computeBytes();
        }
        return 0;
    }

    int serialize(Stream *stream)
    {
        this->serializeHeader(stream, false);
        this->serializeImpl(stream);
        stream->serializeBits(serial_check, 32);

        if(stream->isWriting()) stream->flush();
        return stream->computeBytes();
    }
};


struct UdpPacket_Fragment : UdpPacket
{
    int frag_id;
    int nbr_frags;

    std::vector<uint8_t> frag_data;

    void serializeImpl(Stream *stream)
    {
        frag_data.resize(MAX_PACKETSIZE);

        stream->serializeBits(frag_id, 8);
        stream->serializeBits(nbr_frags, 8);

        stream->memcpy(frag_data.data(), frag_data.size());
    }
};

struct UdpPacket_ConnectionMsg : UdpPacket
{
    int connectionMessage;
    int salt;

    //int connectionData;

    void serializeImpl(Stream *stream)
    {
        //for(auto i = 0 ; i < 600 ; ++i)
        //stream->serializeBits(connectionData, 16);

        stream->serializeInt(connectionMessage, ConnectionMessage_ConnectionRequest,
                                                NBR_ConnectionMessages-1);
        stream->serializeBits(salt, SALT_SIZE);

        //for(auto i = 0 ; i <  500 ; ++i)
        //stream->serializeBits(connectionData, 16);
    }
};



struct UdpPacket_BigPacketTest : UdpPacket
{
    char ctest;
    char dummy;

    void serializeImpl(Stream *stream)
    {
        for(auto i = 0 ; i < 1800 ; ++i)
                    stream->serializeChar(ctest);

        stream->serializeChar(dummy);

        for(auto i = 0 ; i < 1800 ; ++i)
                    stream->serializeChar(ctest);
    }
};

}

#endif // UDPPACKETTYPES_H


