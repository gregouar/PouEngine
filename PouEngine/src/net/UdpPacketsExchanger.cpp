#include "PouEngine/net/UdpPacketsExchanger.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/core/VApp.h"

namespace pou
{

//const int UdpBuffer::MAX_PACKETSIZE = 1024;
//const int UdpBuffer::MAX_PACKETFRAGS = 256;

const int UdpPacketsExchanger::MAX_FRAGBUFFER_ENTRIES = 256;
const float UdpPacketsExchanger::MAX_FRAGPACKET_LIFESPAN = 60.0f*5;
const float UdpPacketsExchanger::MAX_KEEPFRAGPACKETSPERCLIENT_TIME = 60.0f*10;

UdpBuffer::UdpBuffer() : buffer(MAX_PACKETSIZE)
{

}

FragmentedPacket::FragmentedPacket() :
    needToBeCleaned(true),
    fragmentBuffers(UdpPacketsExchanger::MAX_FRAGBUFFER_ENTRIES),
    receivedFrags(UdpPacketsExchanger::MAX_FRAGBUFFER_ENTRIES)
{

}

UdpPacketsExchanger::UdpPacketsExchanger() : m_curLocalTime(0)
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

void UdpPacketsExchanger::update(const Time &elapsedTime)
{
    m_curLocalTime += elapsedTime.count();

    for(auto it = m_fragPacketsBuffer.begin() ; it != m_fragPacketsBuffer.end() ; )
    {
        if(it->second.first + MAX_KEEPFRAGPACKETSPERCLIENT_TIME < m_curLocalTime)
            it = m_fragPacketsBuffer.erase(it);
        else
            ++it;
    }
}

void UdpPacketsExchanger::receivePackets(std::vector<UdpBuffer> &packetBuffers)
{
    //NetAddress sender;
    //std::vector<uint8_t> buffer(DEFAULT_MAX_PACKETSIZE);
    UdpBuffer tempBuffer;
    tempBuffer.buffer.resize(getMaxPacketSize());

    while(true)
    {
        int bytes_read = m_socket.receive(tempBuffer.address,
                                          tempBuffer.buffer.data(),
                                          tempBuffer.buffer.size());

        if (bytes_read <= 0)
        //{
            //packetBuffers.pop_back();
            break;
        //}

        //std::cout<<"Received packet from"<<tempBuffer.address.getAddressString()<<std::endl;

        if(readPacketType(tempBuffer) == PacketType_Fragment)
        {
            UdpBuffer reassembledBuffer;
            if(this->reassemblePacket(tempBuffer, reassembledBuffer))
                packetBuffers.push_back(reassembledBuffer);
        }
        else
            packetBuffers.push_back(tempBuffer);

    }
}

uint32_t UdpPacketsExchanger::hashPacket(std::vector<uint8_t> *data)
{
    uint32_t s = 0;
    if(data)
        s = Hasher::crc32(data->data(),data->size(),s);
    return Hasher::crc32(VApp::APP_VERSION,s);
}

void UdpPacketsExchanger::sendPacket(UdpBuffer &packetBuffer, bool forceNonFragSend)
{
    if(!forceNonFragSend && packetBuffer.buffer.size() > MAX_PACKETSIZE)
        this->fragmentPacket(packetBuffer);
    else
        m_socket.send(packetBuffer.address, packetBuffer.buffer.data(), packetBuffer.buffer.size());

    if(!forceNonFragSend)
        m_curSequence++;
}

PacketType UdpPacketsExchanger::readPacketType(UdpBuffer &packetBuffer)
{
    UdpPacket_Header receivedPacket;
    ReadStream stream;
    stream.setBuffer(packetBuffer.buffer.data(), packetBuffer.buffer.size());
    receivedPacket.serialize(&stream);

    ///Maybe not good to do it here... it depends on type in general
    if((uint32_t)receivedPacket.crc32 != hashPacket())
        return (PacketCorrupted);

    return (PacketType)receivedPacket.type;
}


void UdpPacketsExchanger::fragmentPacket(UdpBuffer &packetBuffer)
{
    int nbr_frags = (packetBuffer.buffer.size() / MAX_PACKETSIZE) + 1;
    assert(nbr_frags < MAX_PACKETFRAGS);

    std::cout<<"Big packet split into "<<nbr_frags<<" parts"<<std::endl;

    for(auto i = 0 ; i < nbr_frags ; ++i)
    {
        UdpPacket_Fragment packet_frag;
        packet_frag.header.crc32    = hashPacket();
        packet_frag.header.sequence = m_curSequence;
        packet_frag.header.type     = PacketType_Fragment;

        packet_frag.frag_id     = (uint8_t)i;
        packet_frag.nbr_frags   = (uint8_t)nbr_frags;

        auto startIt = packetBuffer.buffer.begin()+ i*MAX_PACKETSIZE;
        auto endIt = packetBuffer.buffer.end();
        if((i+1)*MAX_PACKETSIZE < (int)packetBuffer.buffer.size())
            endIt = startIt+MAX_PACKETSIZE;

        packet_frag.frag_data.assign(startIt,endIt);   //= std::move(packetBuffer.buffer); // + i*MAX_PACKETSIZE;
        //packet_frag.frag_data = std::vector<uint8_t> (MAX_PACKETSIZE);

        WriteStream stream;
        UdpBuffer buffer;
        buffer.buffer.resize(packet_frag.serialize(&stream));
        stream.setBuffer(buffer.buffer.data(), buffer.buffer.size());
        packet_frag.serialize(&stream);
        buffer.address = packetBuffer.address;

        this->sendPacket(buffer, true);
    }
}

bool UdpPacketsExchanger::reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer)
{
    ReadStream stream;
    UdpPacket_Fragment packet_fragment;
    fragBuffer.buffer.resize(packet_fragment.serialize(&stream));
    stream.setBuffer(fragBuffer.buffer.data(), fragBuffer.buffer.size());
    packet_fragment.serialize(&stream);

    std::cout<<"Packet fragment received : Seq="<<packet_fragment.header.sequence<<" ("<<packet_fragment.frag_id+1<<"/"<<packet_fragment.nbr_frags<<")"<<std::endl;


    auto fragPacketsVectorIt = m_fragPacketsBuffer.find(fragBuffer.address);
    if(fragPacketsVectorIt == m_fragPacketsBuffer.end())
        fragPacketsVectorIt = m_fragPacketsBuffer.insert({fragBuffer.address,
                                                         {m_curLocalTime,
                                                         std::vector<FragmentedPacket> (MAX_FRAGBUFFER_ENTRIES)}}).first;

    auto &fragmentedPacket = fragPacketsVectorIt->second.second[packet_fragment.header.sequence%MAX_FRAGBUFFER_ENTRIES];
    fragPacketsVectorIt->second.first = m_curLocalTime;

    /*auto &fragPacketsMap = m_fragPacketsBuffer[packet_fragment.header.sequence%MAX_FRAGBUFFER_ENTRIES];
    auto fragmentedPacketIt = fragPacketsMap.find(fragBuffer.address);

    if(fragmentedPacketIt == fragPacketsMap.end())
        fragmentedPacketIt = fragPacketsMap.insert({fragBuffer.address, FragmentedPacket ()}).first;

    auto &fragmentedPacket = fragmentedPacketIt->second;*/

    if(fragmentedPacket.needToBeCleaned ||
       fragmentedPacket.sequence != packet_fragment.header.sequence ||
       fragmentedPacket.birthday + MAX_FRAGPACKET_LIFESPAN < m_curLocalTime)
    {
        fragmentedPacket.needToBeCleaned = false;
        fragmentedPacket.nbr_receivedFrags = 0;
        fragmentedPacket.receivedFrags = std::vector<bool>(fragmentedPacket.receivedFrags.size(), false);
        fragmentedPacket.sequence = packet_fragment.header.sequence;
        fragmentedPacket.nbr_frags = packet_fragment.nbr_frags;
        fragmentedPacket.birthday = m_curLocalTime;
    }

    if(!fragmentedPacket.receivedFrags[packet_fragment.frag_id])
    {
        fragmentedPacket.nbr_receivedFrags++;
        fragmentedPacket.receivedFrags[packet_fragment.frag_id] = true;
        fragmentedPacket.fragmentBuffers[packet_fragment.frag_id] = packet_fragment.frag_data;
    }

    if(fragmentedPacket.nbr_receivedFrags == fragmentedPacket.nbr_frags)
    {
        fragmentedPacket.needToBeCleaned = true;
        destBuffer.address = fragBuffer.address;
        destBuffer.buffer.resize(0);
        destBuffer.buffer.reserve(MAX_PACKETSIZE*fragmentedPacket.nbr_frags);

        for(auto i = 0 ; i < fragmentedPacket.nbr_frags ; ++i)
            destBuffer.buffer.insert(destBuffer.buffer.end(),
                                     fragmentedPacket.fragmentBuffers[i].begin(),
                                     fragmentedPacket.fragmentBuffers[i].end());

        return (true);
    }

    return (false);
}

int UdpPacketsExchanger::getMaxPacketSize()
{
    ReadStream stream;
    UdpPacket_Fragment packet_frag;
    return packet_frag.serialize(&stream);
}

}
