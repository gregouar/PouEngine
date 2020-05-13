#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/UdpPacketsExchanger.h"

#include <vector>

namespace pou
{

struct ClientInfos
{
    NetAddress          address;
    ConnectionStatus    status;
    float               lastPingTime;
};

class UdpServer : public AbstractServer
{
    public:
        UdpServer();
        virtual ~UdpServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0);
        virtual bool shutdown();

        virtual void update(const Time &elapsedTime);

    protected:
        virtual void receivePackets();
        virtual void processMessages(UdpBuffer &buffer);
        virtual void processConnectionMessages(UdpBuffer &buffer);

        virtual void sendConnectionMsg(NetAddress &address, ConnectionMessage msg);

        virtual void disconnectClient(uint16_t clientNbr);
        virtual void denyConnectionFrom(NetAddress &address);
        virtual void allowConnectionFrom(NetAddress &address);

        uint16_t findClientIndex(NetAddress &address);

    private:
       //UdpSocket m_socket;
        UdpPacketsExchanger m_packetsExchanger;

        std::vector<ClientInfos> m_clients;

        float m_deconnectionPingDelay;

    public:
        static const float DEFAULT_DECONNECTIONPINGDELAY;
};

}

#endif // UDPSERVER_H
