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

size_t AbstractServer::connectLocalClient()
{
    for(size_t i = 0 ; i < m_clients.size() ; ++i)
        if(m_clients[i].status == ConnectionStatus_Disconnected)
        {
            m_clients[i].status = ConnectionStatus_Connected;
            m_clients[i].isLocalClient = true;
            return i;
        }

    return m_clients.size();
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

bool AbstractServer::isClientConnected(uint16_t clientId) const
{
    if(clientId > getMaxNbrClients())
        return (false);
    return (m_clients[clientId].status == ConnectionStatus_Connected);
}



}
