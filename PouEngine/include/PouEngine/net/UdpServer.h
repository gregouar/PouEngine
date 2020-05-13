#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/UdpPacketsExchanger.h"

#include <vector>

namespace pou
{

struct ClientInfos
{
    ClientInfos() : address(), status(ConnectionStatus_Disconnected){};

    NetAddress          address;
    ConnectionStatus    status;
    float               lastPingTime;
    float               lastPingAnswerTime;
    int                 serverSalt;
    int                 clientSalt;
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

        virtual void sendConnectionMsg(uint16_t clientNbr, ConnectionMessage msg);
        virtual void sendConnectionMsg(NetAddress &address, ConnectionMessage msg, int salt);

        virtual void disconnectClient(uint16_t clientNbr);
        virtual void denyConnectionFrom(NetAddress &address);
        virtual void challengeConnexionFrom(uint16_t clientNbr);
        virtual void allowConnectionFrom(uint16_t clientNbr);

        uint16_t findClientIndex(NetAddress &address, int salt);

    private:
       //UdpSocket m_socket;
        UdpPacketsExchanger m_packetsExchanger;

        std::vector<ClientInfos> m_clients;

        float m_pingDelay;
        float m_deconnectionPingDelay;

    public:
        static const float DEFAULT_PINGDELAY;
        static const float DEFAULT_DECONNECTIONPINGDELAY;
};

}

#endif // UDPSERVER_H
