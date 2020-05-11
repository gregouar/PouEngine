#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "PouEngine/net/AbstractClient.h"
#include "PouEngine/net/UdpSocket.h"
#include "PouEngine/utils/Timer.h"

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

        virtual void update(const Time &elapsedTime);


    protected:
        virtual void receivePackets();
        virtual void processMessages();

        virtual void tryToConnect();

    protected:
        UdpSocket m_socket;

    private:
        Timer m_connectingAttemptTimer;
        Timer m_connectingTimer;

    static const float CONNECTING_ATTEMPT_DELAY;
    static const float CONNECTING_MAX_TIME;

    static const int DEFAULT_MAX_PACKETSIZE;
};

}

#endif // UDPCLIENT_H
