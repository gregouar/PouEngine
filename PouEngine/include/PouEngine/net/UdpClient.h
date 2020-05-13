#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "PouEngine/net/AbstractClient.h"
#include "PouEngine/net/UdpPacketsExchanger.h"
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

        virtual bool connectToServer(const NetAddress &serverAddress);
        virtual bool disconnectFromServer();

        virtual void update(const Time &elapsedTime);

    protected:
        virtual void receivePackets();
        virtual void processMessages(UdpBuffer &buffer);
        virtual void processConnectionMessages(UdpBuffer &buffer);

        virtual void sendConnectionMsg(NetAddress &address, ConnectionMessage msg);

        virtual void tryToConnect();
        virtual void tryToChallenge();

    protected:
        //UdpSocket m_socket;
        UdpPacketsExchanger m_packetsExchanger;


    private:
        Timer m_connectingAttemptTimer;
        Timer m_connectingTimer;

        float m_pingDelay;
        float m_deconnectionPingDelay;
        float m_lastServerAnswerPingTime;
        float m_lastServerPingTime;

        int m_salt;
        int m_serverSalt;

    static const float CONNECTING_ATTEMPT_DELAY;
    static const float CONNECTING_MAX_TIME;
    static const float DEFAULT_PINGDELAY;
    static const float DEFAULT_DECONNECTIONPINGDELAY;
};

}

#endif // UDPCLIENT_H
