#include "PouEngine/net/UdpNetImpl.h"

#include "PouEngine/net/UdpSocket.h"
#include "PouEngine/net/UdpServer.h"
#include "PouEngine/net/UdpClient.h"

namespace pou
{

UdpNetImpl::UdpNetImpl()
{
    //ctor
}

UdpNetImpl::~UdpNetImpl()
{
    //dtor
}


bool UdpNetImpl::init()
{
    return UdpSocket::initSocketsLayer();
}

bool UdpNetImpl::shutdown()
{
    return UdpSocket::shutdownSicketsLayer();
}

std::unique_ptr<AbstractServer> UdpNetImpl::createServer()
{
    return std::move(std::make_unique<UdpServer> ());
}

std::unique_ptr<AbstractClient> UdpNetImpl::createClient()
{
    return std::move(std::make_unique<UdpClient> ());
}


}
