#ifndef UDPPACKETTYPES_H
#define UDPPACKETTYPES_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

#include "PouEngine/net/NetEngine.h"

#include <vector>

namespace pou
{

static const int MAX_PACKETSIZE = 1024;
static const int MAX_PACKETFRAGS = 256;
static const int SERIAL_CHECK = 69420;
static const int SALT_SIZE = 8;

const int UDPPACKET_SEQ_SIZE = 16;
const int UDPPACKET_SEQ_MAX = (int)pow(2,UDPPACKET_SEQ_SIZE);

static const int MAX_SLICESIZE = 1024;
const int UDPPACKET_SLICEID_SIZE = 16;
const int UDPPACKET_SLICEID_MAX = (int)pow(2,UDPPACKET_SLICEID_SIZE);
const int UDPPACKET_CHUNKID_SIZE = 8;
const int UDPPACKET_CHUNKID_MAX = (int)pow(2,UDPPACKET_CHUNKID_SIZE);



enum PacketType
{
    PacketType_Fragment,
    PacketType_ConnectionMsg,
    PacketType_Data,
    PacketType_Slice,
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
    int last_ack;
    int ack_bits;
    int salt;
    int serial_check;

    int nbrNetMessages;
    std::vector< std::shared_ptr<NetMessage> > netMessages;

    UdpPacket() : nbrNetMessages(0){}

    virtual void serializeImpl(Stream *stream){}

    int serializeHeader(Stream *stream, bool flush = true)
    {
        stream->serializeBits(crc32, 32);
        stream->serializeBits(salt, SALT_SIZE);
        stream->serializeBits(sequence, UDPPACKET_SEQ_SIZE);
        stream->serializeBits(last_ack, UDPPACKET_SEQ_SIZE);
        stream->serializeBits(ack_bits, 32);
        stream->serializeInt(type, 0, NBR_PacketTypes-1);

        if(flush)
        {
            stream->flush();
            return stream->computeBytes();
        }
        return 0;
    }

    int serializeHeaderAndMessages(Stream *stream, bool flush = true)
    {
        this->serializeHeader(stream, false);

        stream->serializeBits(nbrNetMessages, 8);
        if(stream->isReading()) netMessages.resize(0);
        for(int i = 0 ; i < nbrNetMessages ; ++i)
        {
            int msg_type;
            if((int)netMessages.size() > i)
                msg_type = netMessages[i]->type;
            stream->serializeInt(msg_type, 0, NetEngine::getNbrNetMsgTypes());
            if(stream->isReading())
                netMessages.push_back(NetEngine::createNetMessage(msg_type));
            if(netMessages[i])
                netMessages[i]->serialize(stream, false);
        }

        stream->serializeBits(serial_check, 32);

        if(flush)
        {
            stream->flush();
            return stream->computeBytes();
        }
        return 0;
    }

    int serialize(Stream *stream)
    {
        this->serializeHeaderAndMessages(stream, false);
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

struct UdpPacket_Slice : UdpPacket
{
    int chunk_id;
    int slice_id;
    int nbr_slices;
    int chunk_msg_type;

    std::vector<uint8_t> slice_data;

    void serializeImpl(Stream *stream)
    {
        slice_data.resize(MAX_SLICESIZE);

        stream->serializeBits(chunk_id, UDPPACKET_CHUNKID_SIZE);
        stream->serializeBits(slice_id, UDPPACKET_SLICEID_SIZE);
        stream->serializeBits(nbr_slices, UDPPACKET_SLICEID_SIZE);
        stream->serializeInt(chunk_msg_type, 0, NetEngine::getNbrNetMsgTypes());

        stream->memcpy(slice_data.data(), slice_data.size());
    }
};

struct UdpPacket_ConnectionMsg : UdpPacket
{
    int connectionMessage;

    //int connectionData;

    void serializeImpl(Stream *stream)
    {
        //for(auto i = 0 ; i < 600 ; ++i)
        //stream->serializeBits(connectionData, 16);

        stream->serializeInt(connectionMessage, ConnectionMessage_ConnectionRequest,
                                                NBR_ConnectionMessages-1);

        //for(auto i = 0 ; i <  500 ; ++i)
        //stream->serializeBits(connectionData, 16);
    }
};



/*struct UdpPacket_BigPacketTest : UdpPacket
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
};*/

}

#endif // UDPPACKETTYPES_H


