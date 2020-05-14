#include "PouEngine/net/AbstractClient.h"

namespace pou
{

AbstractClient::AbstractClient() : m_connectionStatus(ConnectionStatus_Disconnected), m_port(0), m_curLocalTime(0)
{
    //ctor
}

AbstractClient::~AbstractClient()
{
    //dtor
}

void AbstractClient::update(const Time &elapsedTime)
{
    m_curLocalTime += elapsedTime.count();
    //this->receivePackets();
    //this->processMessages();
}


bool AbstractClient::isConnected() const
{
    return m_connectionStatus;
}

const NetAddress &AbstractClient::getServerAddress() const
{
    return m_serverAddress;
}

/*const NetAddress &AbstractClient::getAddress() const
{
    return m_address;
}*/
unsigned short AbstractClient::getPort() const
{
    return m_port;
}



}
