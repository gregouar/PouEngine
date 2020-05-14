#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/net/NetMessagesFactory.h"
#include "PouEngine/Types.h"

#include <list>

namespace pou
{

class AbstractServer
{
    public:
        AbstractServer();
        virtual ~AbstractServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0) = 0;
        virtual bool shutdown() = 0;

        virtual void update(const Time &elapsedTime);

        virtual void sendMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg, bool forceSend = false) = 0;
        virtual void receivePackets(std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages) = 0;

        bool isRunning() const;
        unsigned short getPort() const;
        virtual uint16_t getMaxNbrClients() const = 0;
        //const NetAddress &getAddress() const;

    protected:
        //virtual void processMessages() = 0;

    protected:
        //uint16_t    m_maxNbrClients;
        bool        m_isRunning;

        unsigned short m_port;
        float m_curLocalTime;
        //NetAddress m_address;
};

}

#endif // ABSTRACTSERVER_H
