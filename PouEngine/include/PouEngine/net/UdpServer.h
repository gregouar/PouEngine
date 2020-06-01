#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/UdpPacketsExchanger.h"

#include <vector>

namespace pou
{

class UdpServer : public AbstractServer
{
    public:
        UdpServer();
        virtual ~UdpServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0);
        virtual bool shutdown();

        virtual void update(const Time &elapsedTime);

        virtual void sendMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg, bool forceSend);
        virtual void sendReliableBigMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg);
        virtual void receivePackets(std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages);

        virtual float getRTT(uint16_t clientNbr) const;

        //virtual uint16_t getMaxNbrClients() const;

    protected:
        virtual void processPacket(UdpBuffer &buffer,
                                   std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages);
        //virtual int  verifyMessage(std::pair<ClientAddress, std::shared_ptr<NetMessage> > addressAndMessage);
        virtual void processConnectionMessages(UdpBuffer &buffer,
                                               std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages);

        virtual void sendConnectionMsg(uint16_t clientNbr, ConnectionMessage msg);
        virtual void sendConnectionMsg(NetAddress &address, ConnectionMessage msg, int salt);

        virtual void disconnectClient(uint16_t clientNbr, bool sendMsg = true);
        virtual void denyConnectionFrom(NetAddress &address);
        virtual void challengeConnexionFrom(uint16_t clientNbr);
        virtual void allowConnectionFrom(uint16_t clientNbr);

        uint16_t findClientIndex(NetAddress &address, int salt);

    private:
       //UdpSocket m_socket;
        UdpPacketsExchanger m_packetsExchanger;

        float m_pingDelay;
        float m_disconnectionPingDelay;

        std::list<int> m_disconnectionList;

    public:
        static const float DEFAULT_PINGDELAY;
        static const float DEFAULT_DECONNECTIONPINGDELAY;
};

}

#endif // UDPSERVER_H
