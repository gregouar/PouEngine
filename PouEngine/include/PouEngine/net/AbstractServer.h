#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include "PouEngine/net/NetAddress.h"
#include "PouEngine/Types.h"

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

        bool isRunning() const;
        unsigned short getPort() const;
        //const NetAddress &getAddress() const;

    protected:
        virtual void receivePackets() = 0;
        //virtual void processMessages() = 0;

    protected:
        uint16_t    m_maxNbrClients;
        bool        m_isRunning;

        unsigned short m_port;
        //NetAddress m_address;
};

}

#endif // ABSTRACTSERVER_H
