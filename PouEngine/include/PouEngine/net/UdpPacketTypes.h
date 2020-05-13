#ifndef UDPPACKETTYPES_H
#define UDPPACKETTYPES_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

#include <vector>

namespace pou
{

static const int MAX_PACKETSIZE = 1024;
static const int MAX_PACKETFRAGS = 256;

enum PacketType
{
    PacketType_Fragment = 0,
    PacketType_Connection,
    PacketType_Diconnection,
    PacketType_CustomData,
    NBR_PacketTypes,
    PacketCorrupted,
};

struct UdpPacket
{
    virtual void serializeImpl(Stream *stream) = 0;

    int serialize(Stream *stream)
    {
        //int bits = this->SerializeImpl(stream);
        this->serializeImpl(stream);
        if(stream->isWriting()) stream->flush();
        return stream->computeBytes();
    }
};

struct UdpPacket_Header : UdpPacket
{
    int crc32;
    int sequence;
    int type;

   // char ctest;

    void serializeImpl(Stream *stream)
    {
        //int bits = (
                    stream->serializeBits(crc32, 32); // +
                    stream->serializeBits(sequence, 16); // +
                    stream->serializeInteger(type, 0, NBR_PacketTypes-1);
                   // );
        //for(auto i = 0 ; i < 1800 ; ++i)
                    //bits +=
          //          stream->serializeChar(ctest);

       // return (true);
    }
};


struct UdpPacket_BigPacketTest : UdpPacket
{
    UdpPacket_Header header;

    char ctest;

    char dummy;

    void serializeImpl(Stream *stream)
    {
        header.serializeImpl(stream);

        for(auto i = 0 ; i < 1800 ; ++i)
                    stream->serializeChar(ctest);

        stream->serializeChar(dummy);

        for(auto i = 0 ; i < 1800 ; ++i)
                    stream->serializeChar(ctest);
    }
};


struct UdpPacket_Fragment : UdpPacket
{
    UdpPacket_Header header;
    int frag_id;
    int nbr_frags;

    //uint8_t frag_data[MAX_PACKETSIZE];
    std::vector<uint8_t> frag_data;

    void serializeImpl(Stream *stream)
    {
        frag_data.resize(MAX_PACKETSIZE);
        //int bits = (
            header.serializeImpl(stream); // +
            stream->serializeBits(frag_id, 8); // +
            stream->serializeBits(nbr_frags, 8);
             //   );

        /*int zeroes = 0;
        int rem_bits = 8 - bits%8;

        bits += stream->serializeBits(zeroes, rem_bits);*/

        /*if(stream->isWriting())
        {

            memcpy(buffer,frag_data,MAX_PACKETSIZE);
        } else if(stream->isReading()) {
            memcpy(frag_data,buffer,MAX_PACKETSIZE);
        }

        bits += MAX_PACKETSIZE*8;*/
        //bits +=

        //return bits;


         stream->memcpy(frag_data.data(), frag_data.size());

    }
};

}


#endif // UDPPACKETTYPES_H


