#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <memory>

#include "PouEngine/net/NetAddress.h"

namespace pou
{

class UdpSocket
{
    public:
        UdpSocket();
        virtual ~UdpSocket();

        bool open(unsigned short port = 0);
        void close();

        bool isOpen();

        bool send(const NetAddress &address, const void *data, size_t data_size);
        int receive(NetAddress &address, const void *data, size_t data_size);

        //const NetAddress &getAddress() const;
        unsigned short getPort() const;

        static bool initSocketsLayer();
        static bool shutdownSicketsLayer();

    protected:

    private:
        bool    m_isOpen;
        int     m_handle;
        unsigned short m_port;
        //NetAddress m_address;
};

}

#endif // UDPSOCKET_H
