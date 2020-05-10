#include "PouEngine/net/AbstractServer.h"

namespace pou
{

AbstractServer::AbstractServer() : m_isRunning(false), m_port(0)
{
    //ctor
}

AbstractServer::~AbstractServer()
{
    //dtor
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



}
