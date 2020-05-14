#include "PouEngine/net/UdpPacketsExchanger.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/core/VApp.h"

#include <string>

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
    m_maxPacketSize = getMaxPacketSize()*1.5;
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

void UdpPacketsExchanger::receivePackets(std::list<UdpBuffer> &packetBuffers,
                                         std::list<std::pair<ClientAddress, std::shared_ptr<NetMessage> > > &netMessages)
{
    UdpBuffer tempBuffer;
    tempBuffer.buffer.resize(m_maxPacketSize);

    while(true)
    {
        int bytes_read = m_socket.receive(tempBuffer.address,
                                          tempBuffer.buffer.data(),
                                          tempBuffer.buffer.size());

        if (bytes_read <= 0)
            break;

        //std::cout<<"Received packet from"<<tempBuffer.address.getAddressString()<<std::endl;

        auto packetType = retrieveMessagesAndAck(tempBuffer, netMessages);

        if(packetType == PacketType_Fragment)
        {
            UdpBuffer reassembledBuffer;
            if(this->reassemblePacket(tempBuffer, reassembledBuffer))
                packetBuffers.push_back(std::move(reassembledBuffer));
        }
        else if(packetType != PacketCorrupted)
            packetBuffers.push_back(std::move(tempBuffer));
    }

    for(auto &it : m_reliableMsgBuffers)
    {
        auto &reliableMsgBuffer = it.second;
        if(!reliableMsgBuffer.msgMap.empty())
        while(reliableMsgBuffer.last_id+1 == reliableMsgBuffer.msgMap.begin()->first)
        {
            netMessages.push_back({it.first, reliableMsgBuffer.msgMap.begin()->second});
            reliableMsgBuffer.msgMap.erase(reliableMsgBuffer.msgMap.begin());
            reliableMsgBuffer.last_id++;
        }
    }
}

uint32_t UdpPacketsExchanger::hashPacket(std::vector<uint8_t> *data)
{
    uint32_t s = 0;
    if(data)
        s = Hasher::crc32(data->data(),data->size(),s);
    return Hasher::crc32((std::string)VApp::APP_VERSION,s);
}

void UdpPacketsExchanger::sendPacket(NetAddress &address, UdpPacket &packet, bool forceNonFragSend)
{
    this->generatePacketHeader(packet, (PacketType)packet.type);

    UdpBuffer buffer;
    WriteStream stream;

    auto curPacketSize = packet.serialize(&stream);

    //if(!forceNonFragSend) //Probably don't need this since frag packet are already max size (and otherwise maybe we want to add reliable msg
    {
        auto &netMsgList = m_netMsgLists[{address, packet.salt}];

        packet.last_ack = netMsgList.last_ack;
        packet.ack_bits = netMsgList.ack_bits;

        for(auto &it : netMsgList.reliableMsgMap)
        {
            auto msgSize = it.second->serialize(&stream) + 2;
            if(curPacketSize + msgSize > MAX_PACKETSIZE && forceNonFragSend)
                break;
            curPacketSize += msgSize;

            packet.netMessages.push_back(it.second);
            packet.nbrNetMessages++;

            netMsgList.msgPerPacket.insert({packet.sequence, it.first});

            if(curPacketSize > MAX_PACKETSIZE)
                break;
        }

        //for(auto &it : netMsgList.nonReliableMsgList)
        for(auto it = netMsgList.nonReliableMsgList.begin() ;
            it != netMsgList.nonReliableMsgList.end() ; ++it)
        {
            auto msgSize = (*it)->serialize(&stream) + 2;

            if(curPacketSize + msgSize > MAX_PACKETSIZE && forceNonFragSend)
                break;

            curPacketSize += msgSize;

            packet.netMessages.push_back((*it));
            packet.nbrNetMessages++;

            it = netMsgList.nonReliableMsgList.erase(it);

            if(curPacketSize > MAX_PACKETSIZE)
                break;
        }
    }

    buffer.buffer.resize(packet.serialize(&stream));
    stream.setBuffer(buffer.buffer.data(), buffer.buffer.size());
    packet.serialize(&stream);
    buffer.address = address;

    this->sendPacket(buffer, forceNonFragSend);
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

void UdpPacketsExchanger::sendMessage(ClientAddress &address, std::shared_ptr<NetMessage> msg, bool forceSend)
{
    auto &netMsgList = m_netMsgLists[address];

    if(msg->isReliable)
    {
        msg->id = netMsgList.curId++;
        std::cout<<"Sending reliable message with id: "<<msg->id<<std::endl;
        netMsgList.reliableMsgMap.insert({msg->id, msg});
    } else {
        msg->id = -1;
        if(forceSend)
            netMsgList.nonReliableMsgList.push_front(msg);
        else
            netMsgList.nonReliableMsgList.push_back(msg);
    }

    if(forceSend)
    {
        /** build empty dataPacket and send it, at least one message will be added ! **/
        /** need to be careful how we choose the message ; add bool in sendPacket for that ? for the moment we send at least one of each if possible **/
        UdpPacket packet;
        packet.type = PacketType_Data;
        packet.salt = address.salt;
        this->sendPacket(address.address,packet);
    }
}

PacketType UdpPacketsExchanger::readPacketType(UdpBuffer &packetBuffer)
{
    UdpPacket receivedPacket;
    ReadStream stream;
    stream.setBuffer(packetBuffer.buffer.data(), packetBuffer.buffer.size());
    receivedPacket.serializeHeader(&stream);

    return (PacketType)receivedPacket.type;
}

bool UdpPacketsExchanger::readPacket(UdpPacket &packet, UdpBuffer &packetBuffer)
{
    ReadStream stream;

    //if((int)packetBuffer.buffer.size() < packet.serialize(&stream))
    //    return (false);

    stream.setBuffer(packetBuffer.buffer.data(), packetBuffer.buffer.size());
    packet.serialize(&stream);

    return (this->verifyPacketIntegrity(packet));
}


bool UdpPacketsExchanger::verifyPacketIntegrity(UdpPacket &packet)
{
    if((uint32_t)packet.crc32 != hashPacket())
    {
        //std::cout<<"Crc32 check failed !"<<std::endl;
        packet.type = PacketCorrupted;
    }

    if((uint32_t)packet.serial_check != Hasher::crc32(&SERIAL_CHECK,1))
    {
        //std::cout<<"Serial check failed !"<<std::endl;
        packet.type = PacketCorrupted;
    }

    return (packet.type != PacketCorrupted);
}

void UdpPacketsExchanger::generatePacketHeader(UdpPacket &packet, PacketType packetType)
{
    if(packetType >= NBR_PacketTypes)
        return;

    packet.crc32    = this->hashPacket();

    packet.sequence = m_curSequence;
    packet.last_ack = -1;
    packet.ack_bits = 0;

    packet.type     = packetType;
    packet.serial_check = Hasher::crc32(&SERIAL_CHECK,1);
    packet.nbrNetMessages = 0;
}

unsigned short UdpPacketsExchanger::getPort() const
{
    return m_socket.getPort();
}

void UdpPacketsExchanger::fragmentPacket(UdpBuffer &packetBuffer)
{
    int nbr_frags = (packetBuffer.buffer.size() / MAX_PACKETSIZE) + 1;
    assert(nbr_frags < MAX_PACKETFRAGS);

    //std::cout<<"Big packet split into "<<nbr_frags<<" parts from size:"<<packetBuffer.buffer.size()<<std::endl;

    for(auto i = 0 ; i < nbr_frags ; ++i)
    {
        UdpPacket_Fragment packet_frag;
        packet_frag.type        = PacketType_Fragment;
        packet_frag.frag_id     = (uint8_t)i;
        packet_frag.nbr_frags   = (uint8_t)nbr_frags;

        auto startIt    = packetBuffer.buffer.begin()+ i*MAX_PACKETSIZE;
        auto endIt      = packetBuffer.buffer.end();
        if((i+1)*MAX_PACKETSIZE < (int)packetBuffer.buffer.size())
            endIt = startIt+MAX_PACKETSIZE+1;

        packet_frag.frag_data.assign(startIt,endIt);

        this->sendPacket(packetBuffer.address, packet_frag, true);
    }
}

bool UdpPacketsExchanger::reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer)
{
    UdpPacket_Fragment packet_fragment;
    if(!this->readPacket(packet_fragment, fragBuffer))
        return (false);

   // std::cout<<"Packet fragment received : Seq="<<packet_fragment.sequence<<" ("<<packet_fragment.frag_id+1<<"/"<<packet_fragment.nbr_frags<<")"<<std::endl;

    ClientAddress clientAddress = {fragBuffer.address,packet_fragment.salt};
    auto fragPacketsVectorIt = m_fragPacketsBuffer.find(clientAddress);
    if(fragPacketsVectorIt == m_fragPacketsBuffer.end())
        fragPacketsVectorIt = m_fragPacketsBuffer.insert({clientAddress,
                                                         {m_curLocalTime,
                                                         std::vector<FragmentedPacket> (MAX_FRAGBUFFER_ENTRIES)}}).first;

    auto &fragmentedPacket = fragPacketsVectorIt->second.second[packet_fragment.sequence%MAX_FRAGBUFFER_ENTRIES];
    fragPacketsVectorIt->second.first = m_curLocalTime;

    if(fragmentedPacket.needToBeCleaned ||
       fragmentedPacket.sequence != packet_fragment.sequence ||
       fragmentedPacket.birthday + MAX_FRAGPACKET_LIFESPAN < m_curLocalTime)
    {
        fragmentedPacket.needToBeCleaned = false;
        fragmentedPacket.nbr_receivedFrags = 0;
        fragmentedPacket.receivedFrags = std::vector<bool>(fragmentedPacket.receivedFrags.size(), false);
        fragmentedPacket.sequence = packet_fragment.sequence;
        fragmentedPacket.nbr_frags = packet_fragment.nbr_frags;
        fragmentedPacket.birthday = m_curLocalTime;
    }

    if(!fragmentedPacket.receivedFrags[packet_fragment.frag_id])
    {
        fragmentedPacket.nbr_receivedFrags++;
        fragmentedPacket.receivedFrags[packet_fragment.frag_id] = true;
        fragmentedPacket.fragmentBuffers[packet_fragment.frag_id] = std::move(packet_fragment.frag_data);
    }

    if(fragmentedPacket.nbr_receivedFrags == fragmentedPacket.nbr_frags)
    {
        fragmentedPacket.needToBeCleaned = true;
        destBuffer.address = fragBuffer.address;
        destBuffer.buffer.resize(0);
        destBuffer.buffer.reserve(fragmentedPacket.fragmentBuffers[0].size()*fragmentedPacket.nbr_frags);

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

PacketType UdpPacketsExchanger::retrieveMessagesAndAck(UdpBuffer &packetBuffer,
                                std::list<std::pair<ClientAddress, std::shared_ptr<NetMessage> > > &netMessages)
{
    UdpPacket packet;
    ReadStream stream;
    stream.setBuffer(packetBuffer.buffer.data(), packetBuffer.buffer.size());
    packet.serializeHeaderAndMessages(&stream);

    if(!verifyPacketIntegrity(packet))
        return PacketCorrupted;

    ClientAddress clientAddress = {packetBuffer.address, packet.salt};
    auto &netMsgList = m_netMsgLists[clientAddress];

    //We remove from the msgList the messages that have been ack
    for(auto i = 0 ; i < 32 ; ++i)
    if(packet.ack_bits & ((int)1 << i))
    {
        auto packet_seq = packet.last_ack - i;
        auto msg_ids = netMsgList.msgPerPacket.equal_range(packet_seq);
        //for(auto id : msg_ids)
        for(auto id = msg_ids.first ; id != msg_ids.second ; ++id)
            netMsgList.reliableMsgMap.erase(id->second);
        netMsgList.msgPerPacket.erase(msg_ids.first, msg_ids.second);
    }

    //We update the last_ack received and ack_bit
    auto seq = packet.sequence;
    if(netMsgList.last_ack < seq)
    {
        netMsgList.ack_bits <<= seq - netMsgList.last_ack;
        netMsgList.last_ack = seq;
    }
    netMsgList.ack_bits |= (((int)1) << (netMsgList.last_ack - seq));

    //We retrieve the messages
    auto &reliableMsgBuffer = m_reliableMsgBuffers[clientAddress];
    for(auto msg : packet.netMessages)
    if(msg)
    {
        if(msg->isReliable)
        {
            //std::cout<<"I have got a reliable message dude, id:"<<msg->id<<std::endl;
            if(reliableMsgBuffer.last_id < msg->id)
                reliableMsgBuffer.msgMap.try_emplace(msg->id,msg);
        } else {
            netMessages.push_back({clientAddress, msg});
        }
    }

    return (PacketType)packet.type;
}



}

