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

        bool isRunning() const;
        bool update(const Time &elapsedTime);

        unsigned short getPort() const;
        //const NetAddress &getAddress() const;

    protected:
        void receivePackets();
        void processMessages();

    protected:
        uint16_t    m_maxNbrClients;
        bool        m_isRunning;

        unsigned short m_port;
        //NetAddress m_address;
};

}

#endif // ABSTRACTSERVER_H
