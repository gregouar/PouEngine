#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "PouEngine/net/AbstractClient.h"
#include "PouEngine/net/UdpSocket.h"

namespace pou
{

class UdpClient : public AbstractClient
{
    public:
        UdpClient();
        virtual ~UdpClient();

        virtual bool create(unsigned short port = 0);
        virtual bool destroy();

        virtual bool connect(const NetAddress &serverAddress);
        virtual bool disconnect();

    protected:
        virtual void receivePackets();

    protected:
        UdpSocket m_socket;
};

}

#endif // UDPCLIENT_H
