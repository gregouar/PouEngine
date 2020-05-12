#include "PouEngine/net/UdpPacketsExchanger.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/core/VApp.h"

namespace pou
{

//const int UdpBuffer::MAX_PACKETSIZE = 1024;
//const int UdpBuffer::MAX_PACKETFRAGS = 256;

const int UdpPacketsExchanger::MAX_FRAGBUFFER_ENTRIES = 256;

UdpBuffer::UdpBuffer() : buffer(MAX_PACKETSIZE)
{
}

UdpPacketsExchanger::UdpPacketsExchanger() : m_fragPacketsBuffer(MAX_FRAGBUFFER_ENTRIES)
{
    //ctor
}

UdpPacketsExchanger::~UdpPacketsExchanger()
{
    this->destroy();
}

bool UdpPacketsExchanger::createSocket(unsigned short port)
{
    return m_socket.open(port);
}

void UdpPacketsExchanger::destroy()
{
    m_socket.close();
}

unsigned short UdpPacketsExchanger::getPort() const
{
    return m_socket.getPort();
}

void UdpPacketsExchanger::receivePackets(std::vector<UdpBuffer> &packetBuffers)
{
    //NetAddress sender;
    //std::vector<uint8_t> buffer(DEFAULT_MAX_PACKETSIZE);

    while(true)
    {
        UdpBuffer tempBuffer;

        int bytes_read = m_socket.receive(tempBuffer.sender,
                                          tempBuffer.buffer.data(),
                                          tempBuffer.buffer.size());

        if (bytes_read <= 0)
        //{
            //packetBuffers.pop_back();
            break;
        //}

        if(readPacketType(tempBuffer) == PacketType_Fragment)
        {
            UdpBuffer reassembledBuffer;
            if(this->reassemblePacket(tempBuffer, reassembledBuffer))
                packetBuffers.push_back(reassembledBuffer);
        }
        else
            packetBuffers.push_back(tempBuffer);

        //std::cout<<"Received packet from"<<sender.getAddressString()<<std::endl;
    }
}

uint32_t UdpPacketsExchanger::hashPacket(std::vector<uint8_t> *data)
{
    uint32_t s = 0;
    if(data)
        s = Hasher::crc32(data->data(),data->size(),s);
    return Hasher::crc32(VApp::APP_VERSION,s);
}

void UdpPacketsExchanger::sendPacket(const UdpBuffer &packetBuffer)
{
    if(packetBuffer.buffer.size() > MAX_PACKETSIZE)
        this->fragmentPacket(packetBuffer);
    else
        m_socket.send(packetBuffer.sender, packetBuffer.buffer.data(), packetBuffer.buffer.size());

    m_curSequence++;
}

PacketType UdpPacketsExchanger::readPacketType(UdpBuffer &packetBuffer)
{
    UdpPacket_Header receivedPacket;
    ReadStream stream;
    stream.setBuffer(packetBuffer.buffer.data(), packetBuffer.buffer.size());
    receivedPacket.Serialize(&stream);

    ///Maybe not good to do it here... it depends on type in general
    if((uint32_t)receivedPacket.crc32 != hashPacket())
        return (PacketCorrupted);

    return (PacketType)receivedPacket.type;
}


void UdpPacketsExchanger::fragmentPacket(const UdpBuffer &packetBuffer)
{
    std::cout<<"Big packet !"<<std::endl;

    int nbr_frags = (packetBuffer.buffer.size() / MAX_PACKETSIZE) + 1;
    assert(nbr_frags < MAX_PACKETFRAGS);

    for(auto i = 0 ; i < nbr_frags ; ++i)
    {
        UdpPacket_Fragment packet_frag;
        packet_frag.header.crc32    = hashPacket();
        packet_frag.header.sequence = m_curSequence;
        packet_frag.header.type     = PacketType_Fragment;

        packet_frag.frag_id = (uint16_t)i;
        packet_frag.nbr_frags = (uint8_t)nbr_frags;

        UdpBuffer buffer;
        //buffer.

        //this->sendPacket();
    }
}

bool UdpPacketsExchanger::reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer)
{
    return (false);
}

}

