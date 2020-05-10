#ifndef NETADDRESS_H
#define NETADDRESS_H

namespace pou
{

class NetAddress
{
    public:
        NetAddress();
        NetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);

        NetAddress(unsigned int address, unsigned short port);

        virtual ~NetAddress();

        unsigned int getAddress() const;

        unsigned char getA() const;
        unsigned char getB() const;
        unsigned char getC() const;
        unsigned char getD() const;

        unsigned short getPort() const;

    private:

        unsigned int    m_address;
        unsigned short  m_port;
};

}

#endif // NETADDRESS_H

