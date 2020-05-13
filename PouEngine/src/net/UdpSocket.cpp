#include "PouEngine/net/UdpSocket.h"

#include "PouEngine/Types.h"
#include "PouEngine/utils/Logger.h"

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC ||
      PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

/*#if PLATFORM == PLATFORM_WINDOWS
#pragma comment( lib, "wsock32.lib" )
#endif*/

namespace pou
{

UdpSocket::UdpSocket() : m_isOpen(false), m_handle(0), m_port(0)
{
    //ctor
}

UdpSocket::~UdpSocket()
{
    //dtor
}

bool UdpSocket::open(unsigned short port)
{
    this->close();

    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (handle <= 0)
    {
        #if PLATFORM == PLATFORM_WINDOWS
            Logger::warning("Failed to create udp socket: error "+std::to_string(WSAGetLastError()));
        #else
            Logger::warning("Failed to create udp socket");
        #endif
        return (false);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0)
    {
        #if PLATFORM == PLATFORM_WINDOWS
            Logger::warning("Failed to bind udp socket: error "+std::to_string(WSAGetLastError()));
        #else
            Logger::warning("Failed to bind udp socket");
        #endif
        return (false);
    }

    struct sockaddr_in sin;
    int len = sizeof(sin);
    getsockname(handle, (struct sockaddr *)&sin, &len);
    m_port = ntohs(sin.sin_port);
   // m_address = NetAddress(ntohl(address.sin_addr.s_addr), ntohs(sin.sin_port));

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;
        if(fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
        {
            Logger::error("Failed to set udp socket to non-blocking");
            return (false);
        }
    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        if(ioctlsocket(handle, FIONBIO, &nonBlocking) != 0)
        {
            Logger::error("Failed to set udp socket to non-blocking: error "+std::to_string(WSAGetLastError()));
            return (false);
        }
    #endif

    m_isOpen    = true;
    m_handle    = handle;

    return (true);
}

void UdpSocket::close()
{
    if(m_isOpen)
    {
        #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            close(m_handle);
        #elif PLATFORM == PLATFORM_WINDOWS
            closesocket(m_handle);
        #endif
    }
    m_isOpen = false;
}

bool UdpSocket::isOpen()
{
    return m_isOpen;
}

bool UdpSocket::send(const NetAddress &address, const void *data, size_t data_size)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(address.getAddress());
    addr.sin_port = htons(address.getPort());

    int sent_bytes =
        sendto(m_handle, (const char*)data, data_size, 0, (const sockaddr*)&addr, sizeof(sockaddr_in));

    if ((size_t)sent_bytes != data_size)
    {
        #if PLATFORM == PLATFORM_WINDOWS
            Logger::warning("Failed to send packet: error "+std::to_string(WSAGetLastError()));
        #else
            Logger::warning("Failed to send packet");
        #endif
        return (false);
    }

    return (true);
}

int UdpSocket::receive(NetAddress &address, const void *packet_data, size_t data_size)
{
    //unsigned char packet_data[256];
    //unsigned int max_packet_size = sizeof(packet_data);

    #if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
    #endif

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);

    int bytes = recvfrom(m_handle, (char*)packet_data, data_size/*max_packet_size*/, 0, (sockaddr*)&from, &fromLength);
    address = NetAddress(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));

    #if PLATFORM == PLATFORM_WINDOWS
        if(bytes < 0 && WSAGetLastError() != 10035) //Error 10035 always triggers because we use non-blocking sockets
            Logger::warning("Failed to receive packet: error "+std::to_string(WSAGetLastError()));
    #endif

    return (bytes);
}

/*const NetAddress &UdpSocket::getAddress() const
{
    return m_address;
}*/
unsigned short UdpSocket::getPort() const
{
    return m_port;
}

bool UdpSocket::initSocketsLayer()
{
    #if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        return WSAStartup( MAKEWORD(2,2),
                           &WsaData )
            == NO_ERROR;
    #else
        return true;
    #endif
}

bool UdpSocket::shutdownSicketsLayer()
{
    #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
    #endif

    return (true);
}

}
