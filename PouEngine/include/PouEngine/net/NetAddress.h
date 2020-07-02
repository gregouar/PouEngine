#ifndef NETADDRESS_H
#define NETADDRESS_H

#include <string>

namespace pou
{

class NetAddress
{
    public:
        NetAddress();
        NetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);

        NetAddress(unsigned int address, unsigned short port);
        NetAddress(const std::string &addressAndPort);
        NetAddress(const std::string &address, const std::string &port);

        virtual ~NetAddress();

        unsigned int getAddress() const;

        unsigned char getA() const;
        unsigned char getB() const;
        unsigned char getC() const;
        unsigned char getD() const;

        unsigned short getPort() const;

        std::string getPortString() const;
        std::string getIpAddressString() const;
        std::string getAddressString() const;

        bool operator<(const NetAddress &address) const;
        bool operator==(const NetAddress &address) const;
        //bool operator() (const NetAddress& lhs, const NetAddress& rhs) const;

    private:

        unsigned int    m_address;
        unsigned short  m_port;
};

}

#endif // NETADDRESS_H

