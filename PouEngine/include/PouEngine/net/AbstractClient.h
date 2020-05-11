#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/Types.h"

namespace pou
{

class AbstractClient
{
    public:
        AbstractClient();
        virtual ~AbstractClient();

        virtual bool create(unsigned short port = 0) = 0;
        virtual bool destroy() = 0;

        virtual bool connect(const NetAddress &serverAddress) = 0;
        virtual bool disconnect() = 0;

        virtual void update(const Time &elapsedTime);

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
        //NetAddress m_address;

};

}


#endif // ABSTRACTCLIENT_H
