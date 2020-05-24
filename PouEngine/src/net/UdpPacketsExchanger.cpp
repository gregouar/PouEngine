#include "PouEngine/net/UdpPacketsExchanger.h"

#include "PouEngine/utils/Stream.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/core/VApp.h"

#include <string>
#include <numeric>

namespace pou
{

//const int UdpBuffer::MAX_PACKETSIZE = 1024;
//const int UdpBuffer::MAX_PACKETFRAGS = 256;

const int UdpPacketsExchanger::MAX_FRAGBUFFER_ENTRIES = 256;
const float UdpPacketsExchanger::MAX_FRAGPACKET_LIFESPAN = 60.0f*5;
const float UdpPacketsExchanger::MAX_KEEPFRAGPACKETSPERCLIENT_TIME = 60.0f*10;
const float UdpPacketsExchanger::SLICE_SENDING_DELAY = .5f;
const int UdpPacketsExchanger::NBR_SLICESPERSEND = 50;

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
   // m_chunkSendingBuffer.chunk_id = m_curChunkId;
   // m_lastReceivedChunkId = -1;
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


    for(auto &chunkSendingBufferIt : m_chunkSendingBuffers)
    {
        auto &chunkSendingBuffer = chunkSendingBufferIt.second;
        if(chunkSendingBuffer.sendingTimer.update(elapsedTime)
        ||!chunkSendingBuffer.sendingTimer.isActive())
        {
            size_t i = 0;
            for(auto it = chunkSendingBuffer.slicesToSend.begin() ; it != chunkSendingBuffer.slicesToSend.end()
            && i < NBR_SLICESPERSEND ; ++i, ++it)
            {
                chunkSendingBuffer.sendingTimer.reset(SLICE_SENDING_DELAY);
                this->sendSlice(chunkSendingBuffer, *it);
            }
        }
    }

    for(auto &chunkReceivingBufferIt : m_chunkReceivingBuffers)
    {
        auto &chunkReceivingBuffer = chunkReceivingBufferIt.second;
        if(chunkReceivingBuffer.ackTimer.update(elapsedTime)
        ||!chunkReceivingBuffer.ackTimer.isActive())
        {
            if(chunkReceivingBuffer.isReceiving)
            {
                this->ping(chunkReceivingBuffer.address);
                chunkReceivingBuffer.ackTimer.reset(SLICE_SENDING_DELAY*0.5);
            }
        }
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
            {
                packetType = retrieveMessagesAndAck(reassembledBuffer, netMessages);
                packetBuffers.push_back(std::move(reassembledBuffer));
            }
        }
        else if(packetType == PacketType_Slice)
        {
            this->reassembleChunk(tempBuffer, netMessages);
        }
        else if(packetType != PacketCorrupted)
            packetBuffers.push_back(std::move(tempBuffer));
    }

    for(auto &it : m_reliableMsgBuffers)
    {
        auto &reliableMsgBuffer = it.second;
        while(!reliableMsgBuffer.msgMap.empty())
        {
            auto founded = reliableMsgBuffer.msgMap.find((reliableMsgBuffer.last_id+1)%NetMessagesFactory::NETMESSAGEID_MAX_NBR);
            if(founded != reliableMsgBuffer.msgMap.end())
            {
                netMessages.push_back({it.first, founded->second});
                reliableMsgBuffer.msgMap.erase(founded);
                reliableMsgBuffer.last_id++;
            } else
                break;

        }
        /*while((reliableMsgBuffer.last_id+1)%NetMessagesFactory::NETMESSAGEID_MAX_NBR
              == reliableMsgBuffer.msgMap.begin()->first)
        {
            netMessages.push_back({it.first, reliableMsgBuffer.msgMap.begin()->second});
            reliableMsgBuffer.msgMap.erase(reliableMsgBuffer.msgMap.begin());
            reliableMsgBuffer.last_id++;
        }*/
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

    auto &netMsgList = m_netMsgLists[{address, packet.salt}];
    packet.last_ack = netMsgList.last_ack;
    packet.ack_bits = netMsgList.ack_bits;

    if(!forceNonFragSend)
    {
        /*auto &netMsgList = m_netMsgLists[{address, packet.salt}];

        packet.last_ack = netMsgList.last_ack;
        packet.ack_bits = netMsgList.ack_bits;*/

        auto &sendedPacketContent = netMsgList.sendedPacketContents[packet.sequence];
        sendedPacketContent.messageIds.clear();
        sendedPacketContent.sendTime = m_curLocalTime;

        //std::cout<<"ListSize:"<<netMsgList.reliableMsgMap.size()<<std::endl;
        //std::cout<<"Sending packet seq:"<<packet.sequence<<std::endl;

        for(auto &it : netMsgList.reliableMsgMap)
        {
            auto msgSize = it.second->serialize(&stream) + 2;
            if(curPacketSize + msgSize > MAX_PACKETSIZE && forceNonFragSend)
                break;
            curPacketSize += msgSize;

            packet.netMessages.push_back(it.second);
            packet.nbrNetMessages++;

            //netMsgList.msgPerPacket.insert({packet.sequence, it.first});
            sendedPacketContent.messageIds.push_back(it.first);

            //std::cout<<"Trying to send msgid:"<<it.second->id<<std::endl;

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

        if(netMsgList.curId == NetMessagesFactory::NETMESSAGEID_MAX_NBR)
            netMsgList.curId = 0;

        //std::cout<<"Sending reliable message with id: "<<msg->id<<std::endl;
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

bool UdpPacketsExchanger::sendChunk(ClientAddress &address,  std::shared_ptr<NetMessage> msg, bool initialBurst)
{
    msg->isReliable = false;

    WriteStream stream;
    int bufferSize = msg->serialize(&stream,true);
    std::vector<uint8_t> buffer(bufferSize, 0);
    stream.setBuffer(buffer);
    msg->serialize(&stream);

    return this->sendChunk(address, buffer, msg->type, initialBurst);
}


bool UdpPacketsExchanger::sendChunk(ClientAddress &clientAddress, std::vector<uint8_t> &chunk_data, int type, bool initialBurst)
{
    auto chunkSendingBufferIt = m_chunkSendingBuffers.find(clientAddress);
    if(chunkSendingBufferIt == m_chunkSendingBuffers.end())
    {
        chunkSendingBufferIt = m_chunkSendingBuffers.insert({clientAddress,ChunkSendingBuffer ()}).first;
        auto &chunkSendingBuffer = chunkSendingBufferIt->second;
        chunkSendingBuffer.address = clientAddress;
        chunkSendingBuffer.chunk_id = -1;
    }
    auto &chunkSendingBuffer = chunkSendingBufferIt->second;

    if(!chunkSendingBuffer.slicesToSend.empty())
        return (false); ///Add to queue ?

    chunkSendingBuffer.address = clientAddress;
    chunkSendingBuffer.chunk_id = (chunkSendingBuffer.chunk_id + 1) % UDPPACKET_CHUNKID_MAX;
    chunkSendingBuffer.chunk_msg_type = type;
    chunkSendingBuffer.nbr_slices = (chunk_data.size()/MAX_SLICESIZE) + (int)((chunk_data.size() % MAX_SLICESIZE == 0) ? 0 : 1);
    chunkSendingBuffer.packetSeqToSliceId.clear();
    chunkSendingBuffer.buffer = std::move(chunk_data);

    /*m_chunkSendingBuffer.slicesToSend.resize(m_chunkSendingBuffer.nbrSlices,0);
    std::iota (std::begin(m_chunkSendingBuffer.slicesToSend), std::end(m_chunkSendingBuffer.slicesToSend), 0);*/
    chunkSendingBuffer.slicesToSend.clear();
    for(int i = 0 ; i  <  chunkSendingBuffer.nbr_slices ; ++i)
    {
        chunkSendingBuffer.slicesToSend.insert(i);
        if(initialBurst)
            this->sendSlice(chunkSendingBuffer, i);
    }

    std::cout<<"Sending chunk with #slices: "<<chunkSendingBuffer.nbr_slices<<std::endl;

    return (true);
}

void UdpPacketsExchanger::sendSlice(ChunkSendingBuffer &chunkSendingBuffer, int sliceId)
{
    UdpPacket_Slice packet;
    packet.type = PacketType_Slice;
    packet.salt = chunkSendingBuffer.address.salt;
    packet.chunk_id = chunkSendingBuffer.chunk_id;
    packet.slice_id = sliceId;
    packet.nbr_slices = chunkSendingBuffer.nbr_slices;
    packet.chunk_msg_type = chunkSendingBuffer.chunk_msg_type;

    auto startIt    = chunkSendingBuffer.buffer.begin()+ sliceId*MAX_SLICESIZE;
    auto endIt      = chunkSendingBuffer.buffer.end();
    if((sliceId+1)*MAX_SLICESIZE < (int)chunkSendingBuffer.buffer.size())
        endIt = startIt+MAX_SLICESIZE+1;

    packet.slice_data.assign(startIt,endIt);

    //std::cout<<"Send slice: "<<sliceId<<std::endl;

    this->sendPacket(chunkSendingBuffer.address.address, packet, true);
    chunkSendingBuffer.packetSeqToSliceId.insert_or_assign(packet.sequence, sliceId);
    m_curSequence++;
}


bool UdpPacketsExchanger::reassembleChunk(UdpBuffer &chunkBuffer,
                                         std::list<std::pair<ClientAddress, std::shared_ptr<NetMessage> > > &netMessages)
{
    UdpPacket_Slice packetSlice;
    if(!this->readPacket(packetSlice, chunkBuffer))
        return (false);

    std::cout<<"Packet slice received : ChunkId="<<packetSlice.chunk_id<<" ("<<packetSlice.slice_id+1<<"/"<<packetSlice.nbr_slices<<")"<<std::endl;

    ClientAddress clientAddress = {chunkBuffer.address,packetSlice.salt};
    auto chunkReceivingBufferIt = m_chunkReceivingBuffers.find(clientAddress);
    if(chunkReceivingBufferIt == m_chunkReceivingBuffers.end())
    {
        chunkReceivingBufferIt = m_chunkReceivingBuffers.insert({clientAddress,ChunkReceivingBuffer ()}).first;
        auto &chunkReceivingBuffer = chunkReceivingBufferIt->second;
        chunkReceivingBuffer.address = clientAddress;
        chunkReceivingBuffer.chunk_id = packetSlice.chunk_id;
        chunkReceivingBuffer.isReceiving = false;
    }
    auto &chunkReceivingBuffer = chunkReceivingBufferIt->second;

    if(chunkReceivingBuffer.chunk_id % UDPPACKET_CHUNKID_MAX != packetSlice.chunk_id /*&& chunkReceivingBuffer.isReceiving*/)
    {
        ///Error ? Queue ?
        return (false);
    }

    if(!chunkReceivingBuffer.isReceiving)
    {
        chunkReceivingBuffer.nbr_slices = packetSlice.nbr_slices;
        chunkReceivingBuffer.chunk_msg_type = packetSlice.chunk_msg_type;
        chunkReceivingBuffer.isReceiving = true;
    }

    auto it = chunkReceivingBuffer.sliceBuffers.find(packetSlice.slice_id);
    if(it == chunkReceivingBuffer.sliceBuffers.end())
        chunkReceivingBuffer.sliceBuffers.insert({packetSlice.slice_id, std::move(packetSlice.slice_data)});

    if((int)chunkReceivingBuffer.sliceBuffers.size() == chunkReceivingBuffer.nbr_slices)
    {
        std::vector<uint8_t> reassembledBuffer;
        reassembledBuffer.reserve(MAX_SLICESIZE*chunkReceivingBuffer.nbr_slices);

        for(size_t i = 0 ; i < chunkReceivingBuffer.sliceBuffers.size() ; ++i)
            reassembledBuffer.insert(reassembledBuffer.end(),
                                     chunkReceivingBuffer.sliceBuffers[i].begin(),
                                     chunkReceivingBuffer.sliceBuffers[i].end());

        ReadStream stream;
        stream.setBuffer(reassembledBuffer);
        auto message = NetEngine::createNetMessage(chunkReceivingBuffer.chunk_msg_type);
        message->serialize(&stream);
        netMessages.push_back({clientAddress, message});

        chunkReceivingBuffer.sliceBuffers.clear();
        chunkReceivingBuffer.isReceiving = false;
        chunkReceivingBuffer.chunk_id++;
        return (true);
    }

    return (false);
}

void UdpPacketsExchanger::ping(ClientAddress &address)
{
    UdpPacket_ConnectionMsg packet;
    packet.type = PacketType_ConnectionMsg;
    packet.connectionMessage = ConnectionMessage_Ping;
    packet.salt = address.salt;
    this->sendPacket(address.address,packet);
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

    packet.sequence = m_curSequence % UDPPACKET_SEQ_MAX;
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

    std::cout<<"Big packet split into "<<nbr_frags<<" parts from size:"<<packetBuffer.buffer.size()<<std::endl;

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

    if(packet.type == PacketType_Slice)
    {
        UdpPacket_Slice packetSlice;
        if(!this->readPacket(packetSlice, packetBuffer))
            return PacketCorrupted;
    }

    ClientAddress clientAddress = {packetBuffer.address, packet.salt};
    auto &netMsgList = m_netMsgLists[clientAddress];

    /*for(int i = 0 ; i < 32 ; ++i)
        std::cout<<(bool)(packet.ack_bits & (uint32_t)(pow(2,i)));
    std::cout<<std::endl;*/

    //We remove from the msgList the messages that have been ack
    for(auto i = 0 ; i < 32 ; ++i)
    if((packet.ack_bits & ((int)1 << i)))
    {
        int packet_seq = (packet.last_ack - (int)i) % UDPPACKET_SEQ_MAX;
        if(packet_seq < 0)
            packet_seq += UDPPACKET_SEQ_MAX;

        //std::cout<<"Client has acked packet seq:"<<packet_seq<<std::endl;

        /*
       // std::cout<<"Ack packet:"<<packet_seq<<" (last_ack:"<<packet.last_ack<<std::endl;
        auto msg_ids = netMsgList.msgPerPacket.equal_range(packet_seq);
        //for(auto id : msg_ids)
        for(auto id = msg_ids.first ; id != msg_ids.second ; ++id)
            netMsgList.reliableMsgMap.erase(id->second);
        netMsgList.msgPerPacket.erase(msg_ids.first, msg_ids.second);*/

        auto &sendedPacketContent = netMsgList.sendedPacketContents[packet_seq];
        for(auto id : sendedPacketContent.messageIds)
            netMsgList.reliableMsgMap.erase(id);
        sendedPacketContent.messageIds.clear();

        auto chunkSendingBufferIt = m_chunkSendingBuffers.find(clientAddress);
        if(chunkSendingBufferIt != m_chunkSendingBuffers.end())
        {
            auto &chunkSendingBuffer = chunkSendingBufferIt->second;
            if(!chunkSendingBuffer.slicesToSend.empty())
            {
                auto sliceIt = chunkSendingBuffer.packetSeqToSliceId.find(packet_seq);
                if(sliceIt != chunkSendingBuffer.packetSeqToSliceId.end())
                {
                    chunkSendingBuffer.slicesToSend.erase(sliceIt->second);
                    chunkSendingBuffer.packetSeqToSliceId.erase(sliceIt);
                }
            }
        }

        if(sendedPacketContent.sendTime != -1)
        {
            netMsgList.avrgRTT = netMsgList.avrgRTT*0.9 + (m_curLocalTime - sendedPacketContent.sendTime)*0.1;
            //std::cout<<"Average RTT: "<<netMsgList.avrgRTT<<std::endl;
            sendedPacketContent.sendTime = -1;
        }
    }

    //We don't want to ack for frag part
    if(packet.type == PacketType_Fragment)
        return PacketType_Fragment;

    //We update the last_ack received and ack_bit
    uint32_t seq = (uint32_t)packet.sequence;

    int64_t delta = seq - netMsgList.last_ack%UDPPACKET_SEQ_MAX;
    if(abs(delta) > UDPPACKET_SEQ_MAX/2)
    {
        if(delta < 0)
            delta += UDPPACKET_SEQ_MAX;
        else
            delta -= UDPPACKET_SEQ_MAX;
    }

    if(delta > 0)
    {
        netMsgList.ack_bits <<= delta;
        netMsgList.last_ack = packet.sequence;
        delta = 0;
    }
    netMsgList.ack_bits |= (((int)1) << (-delta));

    //We retrieve the messages
    auto &reliableMsgBuffer = m_reliableMsgBuffers[clientAddress];
    for(auto msg : packet.netMessages)
    if(msg)
    {
        if(msg->isReliable)
        {
            int64_t delta = msg->id - reliableMsgBuffer.last_id % NetMessagesFactory::NETMESSAGEID_MAX_NBR;
            if(abs(delta) > NetMessagesFactory::NETMESSAGEID_MAX_NBR/2)
            {
                if(delta < 0)
                    delta += NetMessagesFactory::NETMESSAGEID_MAX_NBR;
                else
                    delta -= NetMessagesFactory::NETMESSAGEID_MAX_NBR;
            }

            if(delta > 0)
                reliableMsgBuffer.msgMap.try_emplace(msg->id,msg);
        } else {
            netMessages.push_back({clientAddress, msg});
        }
    }

    return (PacketType)packet.type;
}



}

