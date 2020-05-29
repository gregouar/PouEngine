#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/NetMessagesFactory.h"
#include "PouEngine/Types.h"

#include <list>

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
    bool                isLocalClient;
};


class AbstractServer
{
    public:
        AbstractServer();
        virtual ~AbstractServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0) = 0;
        virtual bool shutdown() = 0;

        virtual void update(const Time &elapsedTime);

        virtual void sendMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg, bool forceSend = false) = 0;
        virtual void sendReliableBigMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg) = 0;
        virtual void receivePackets(std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages) = 0;

        size_t connectLocalClient();

        bool isRunning() const;
        unsigned short getPort() const;
        virtual uint16_t getMaxNbrClients() const;
        //const NetAddress &getAddress() const;
        bool isClientConnected(uint16_t clientNbr) const;

    protected:
        //virtual void processMessages() = 0;

    protected:
        //uint16_t    m_maxNbrClients;
        bool        m_isRunning;

        unsigned short m_port;
        float m_curLocalTime;
        //NetAddress m_address;

        std::vector<ClientInfos> m_clients;
};

}

#endif // ABSTRACTSERVER_H
