#include "PouEngine/net/NetAddress.h"

namespace pou
{

NetAddress::NetAddress() : NetAddress(127,0,0,1, 0)
{
    //ctor
}

NetAddress::NetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) :
    NetAddress((a << 24) | (b << 16) | (c << 8) | d, port)
{
}

NetAddress::NetAddress(unsigned int address, unsigned short port) :
    m_address(address), m_port(port)
{

}


NetAddress::~NetAddress()
{
    //dtor
}


unsigned int NetAddress::getAddress() const
{
    return m_address;
}

unsigned char NetAddress::getA() const
{
    return static_cast<unsigned char> (m_address >> 24);
}
unsigned char NetAddress::getB() const
{
    return static_cast<unsigned char> (m_address >> 16);
}
unsigned char NetAddress::getC() const
{
    return static_cast<unsigned char> (m_address >> 8);
}
unsigned char NetAddress::getD() const
{
    return static_cast<unsigned char> (m_address);
}

unsigned short NetAddress::getPort() const
{
    return m_port;
}

std::string NetAddress::getAddressString() const
{
    return std::move(std::to_string((int)getA())+"."+
                     std::to_string((int)getB())+"."+
                     std::to_string((int)getC())+"."+
                     std::to_string((int)getD())+":"+
                     std::to_string(getPort()));
}

bool NetAddress::operator<(const NetAddress &address) const
{
    return (m_address < address.m_address) ||
        (m_address == address.m_address && m_port < address.m_port);
}

bool NetAddress::operator() (const NetAddress& lhs, const NetAddress& rhs) const
{
    return (lhs.m_address < rhs.m_address) ||
        (lhs.m_address == rhs.m_address && lhs.m_port < rhs.m_port);
}


};


