#ifndef UDPPACKETSEXCHANGER_H
#define UDPPACKETSEXCHANGER_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/net/UdpSocket.h"


#include <vector>
#include <map>
#include <cstdint>

namespace pou
{

class UdpPacketsExchanger;

struct UdpBuffer
{
    UdpBuffer();

    NetAddress  sender;
    std::vector<uint8_t> buffer;
};

struct FragmentedPacket
{
    FragmentedPacket();

    uint16_t sequence;
    int nbr_frags;
    Timer timer;

    const int MAX_FRAGPACKET_LIFESPAN;

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

        unsigned short getPort() const;

        virtual void sendPacket(const UdpBuffer &packetBuffer);
        virtual void receivePackets(std::vector<UdpBuffer> &packetBuffers);

        uint32_t hashPacket(std::vector<uint8_t> *data = nullptr);

    protected:
        PacketType readPacketType(UdpBuffer &packetBuffer);
        void fragmentPacket(const UdpBuffer &packetBuffer);
        bool reassemblePacket(UdpBuffer &fragBuffer, UdpBuffer &destBuffer);

    private:
        UdpSocket m_socket;

        uint16_t m_curSequence;
        std::vector<std::map<NetAddress, FragmentedPacket> > m_fragPacketsBuffer;


        static const int MAX_FRAGBUFFER_ENTRIES;

};

}

#endif // UDPPACKETSEXCHANGER_H
