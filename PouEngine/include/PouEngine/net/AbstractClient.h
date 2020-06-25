#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/NetMessagesFactory.h"
#include "PouEngine/Types.h"

#include <list>

namespace pou
{

class AbstractClient
{
    public:
        AbstractClient();
        virtual ~AbstractClient();

        virtual bool create(unsigned short port = 0) = 0;
        virtual bool destroy() = 0;

        virtual bool connectToServer(const NetAddress &serverAddress) = 0;
        virtual bool disconnectFromServer() = 0;

        virtual void update(const Time &elapsedTime);

        virtual void sendMessage(std::shared_ptr<NetMessage> msg, bool forceSend = false) = 0;
        virtual void sendBigReliableMessage(std::shared_ptr<NetMessage> msg) = 0;
        virtual void receivePackets(std::list<std::shared_ptr<NetMessage> > &netMessages) = 0;

        bool isConnected() const;
        const NetAddress &getServerAddress() const;
        unsigned short getPort() const;
        virtual float getRTT() const = 0;
        //const NetAddress &getAddress() const;

    protected:
        //virtual void processMessages() = 0;

    protected:
        ConnectionStatus m_connectionStatus;
        NetAddress m_serverAddress;
        unsigned short m_port;
        float m_curLocalTime;
        //NetAddress m_address;

};

}


#endif // ABSTRACTCLIENT_H
