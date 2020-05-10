#ifndef UDPNETIMPL_H
#define UDPNETIMPL_H

#include <memory>

#include "PouEngine/net/AbstractNetImpl.h"

namespace pou
{

class UdpNetImpl : public AbstractNetImpl
{
    public:
        UdpNetImpl();
        virtual ~UdpNetImpl();

        virtual bool init();
        virtual bool shutdown();

        virtual std::unique_ptr<AbstractServer> createServer();
        virtual std::unique_ptr<AbstractClient> createClient();

    protected:

    private:
};

}

#endif // UDPNETIMPL_H
