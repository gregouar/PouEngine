#include "PouEngine/net/AbstractServer.h"

namespace pou
{

AbstractServer::AbstractServer() : m_isRunning(false), m_port(0), m_curLocalTime(0)
{
    //ctor
}

AbstractServer::~AbstractServer()
{
    //dtor
}

void AbstractServer::update(const Time &elapsedTime)
{
    m_curLocalTime += elapsedTime.count();
    //this->receivePackets();
    //this->processMessages();
}


bool AbstractServer::isRunning() const
{
    return (m_isRunning);
}

/*const NetAddress &AbstractServer::getAddress() const
{
    return m_address;
}*/
unsigned short AbstractServer::getPort()  const
{
    return m_port;
}

uint16_t AbstractServer::getMaxNbrClients() const
{
    return m_clients.size();
}

bool AbstractServer::isClientConnected(uint16_t clientNbr) const
{
    if(clientNbr > getMaxNbrClients())
        return (false);
    return (m_clients[clientNbr].status == ConnectionStatus_Connected);
}



}
