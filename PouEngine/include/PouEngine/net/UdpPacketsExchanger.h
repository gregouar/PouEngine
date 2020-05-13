#ifndef UDPPACKETSEXCHANGER_H
#define UDPPACKETSEXCHANGER_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/net/UdpSocket.h"
#include "PouEngine/utils/Timer.h"


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

/*struct SequenceBuffer
{
    static const int MAX_ENTRIES = 256;

    SequenceBuffer() : sequences(auto (MAX_ENTRIES)), entries(auto (MAX_ENTRIES))

    std::vector<uint32_t> sequences;
    std::vector<FragmentedPacket> entries;
};*/

class UdpPacketsExchanger
{
    public:
        UdpPacketsExchanger();
        virtual ~UdpPacketsExchanger();

        bool createSocket(unsigned short port = 0);
        void destroy();

        virtual void update(const Time &elapsedTime);

        virtual void sendPacket(UdpBuffer &packetBuffer, bool forceNonFragSend = false);
        virtual void receivePackets(std::vector<UdpBuffer> &packetBuffers);

        uint32_t hashPacket(std::vector<uint8_t> *data = nullptr);

        PacketType readPacketType(UdpBuffer &packetBuffer);
        unsigned short getPort() const;

    protected:
        void fragmentPacket(UdpBuffer &packetBuffer);
        bool reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer);

        int getMaxPacketSize();

    private:
        UdpSocket m_socket;

        uint16_t    m_curSequence;
        float       m_curLocalTime;
        std::map< NetAddress, std::pair<float, std::vector<FragmentedPacket> > > m_fragPacketsBuffer;

    public:
        static const int MAX_FRAGBUFFER_ENTRIES;
        static const float MAX_FRAGPACKET_LIFESPAN;
        static const float MAX_KEEPFRAGPACKETSPERCLIENT_TIME;

};

}

#endif // UDPPACKETSEXCHANGER_H
