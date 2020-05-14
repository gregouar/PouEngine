#ifndef UDPPACKETSEXCHANGER_H
#define UDPPACKETSEXCHANGER_H

#include "PouEngine/Types.h"
#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/net/UdpSocket.h"

#include <memory>
#include <vector>
#include <map>
#include <cstdint>

namespace pou
{

class UdpPacketsExchanger;

struct UdpBuffer
{
    UdpBuffer();

    NetAddress  address;
    std::vector<uint8_t> buffer;
};

struct ClientAddress
{
    bool operator<(const ClientAddress &rhs) const
    {
        return (salt < rhs.salt) ||
            (salt == rhs.salt && address < rhs.address);
    }

    NetAddress address;
    int salt;
};

struct FragmentedPacket
{
    FragmentedPacket();

    bool needToBeCleaned;
    uint16_t sequence;
    int nbr_frags;
    float birthday;

    std::vector< std::vector<uint8_t> > fragmentBuffers;
    std::vector< bool > receivedFrags;
    int nbr_receivedFrags;
};

struct ReliableMessagesList
{
    ReliableMessagesList() : curId(0), last_ack(-1), ack_bits(0){}

    std::map<int, std::shared_ptr<ReliableMessage> > msgList;
    int curId;

    int last_ack;
    int ack_bits;

    std::multimap<int, int> msgPerPacket;
};

struct ReliableMessagesBuffer
{
    ReliableMessagesBuffer() : last_id(-1){}

    int last_id;
    std::map<int, std::shared_ptr<ReliableMessage> > msgMap;
};

class UdpPacketsExchanger
{
    public:
        UdpPacketsExchanger();
        virtual ~UdpPacketsExchanger();

        bool createSocket(unsigned short port = 0);
        void destroy();

        virtual void update(const pou::Time &elapsedTime);

        virtual void sendPacket(NetAddress &address, UdpPacket &packet, bool forceNonFragSend = false);
        virtual void sendPacket(UdpBuffer &packetBuffer, bool forceNonFragSend = false);
        virtual void receivePackets(std::list<UdpBuffer> &packetBuffers,
                                    std::list<std::pair<ClientAddress, std::shared_ptr<ReliableMessage> > > &reliableMessages);

        virtual void sendReliableMessage(ClientAddress &address, std::shared_ptr<ReliableMessage> msg);

        void generatePacketHeader(UdpPacket &packet, PacketType packetType);
        PacketType readPacketType(UdpBuffer &packetBuffer);
        bool readPacket(UdpPacket &packet, UdpBuffer &packetBuffer);
        unsigned short getPort() const;

    protected:
        void fragmentPacket(UdpBuffer &packetBuffer);
        bool reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer);

        uint32_t hashPacket(std::vector<uint8_t> *data = nullptr);
        bool verifyPacketIntegrity(UdpPacket &packet);

        int getMaxPacketSize();

        PacketType checkMessagesAndAck(UdpBuffer &packetBuffer);

    private:
        int m_maxPacketSize;
        UdpSocket m_socket;

        uint16_t    m_curSequence;
        float       m_curLocalTime;
        std::map< ClientAddress, std::pair<float, std::vector<FragmentedPacket> > > m_fragPacketsBuffer;
        std::map< ClientAddress, ReliableMessagesList > m_reliableMsgLists;
        std::map< ClientAddress, ReliableMessagesBuffer > m_reliableMsgBuffers;

    public:
        static const int MAX_FRAGBUFFER_ENTRIES;
        static const float MAX_FRAGPACKET_LIFESPAN;
        static const float MAX_KEEPFRAGPACKETSPERCLIENT_TIME;

};

}

#endif // UDPPACKETSEXCHANGER_H
