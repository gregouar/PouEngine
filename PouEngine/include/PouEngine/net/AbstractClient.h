#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/ReliableMessagesFactory.h"
#include "PouEngine/Types.h"

#include <memory>

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

        virtual void sendReliableMessage(std::shared_ptr<ReliableMessage> msg) = 0;

        bool isConnected() const;
        const NetAddress &getServerAddress() const;
        unsigned short getPort() const;
        //const NetAddress &getAddress() const;

    protected:
        virtual void receivePackets() = 0;
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
