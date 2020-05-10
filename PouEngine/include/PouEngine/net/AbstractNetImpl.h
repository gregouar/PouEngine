#ifndef ABSTRACTNETIMPL_H_INCLUDED
#define ABSTRACTNETIMPL_H_INCLUDED

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/AbstractClient.h"


namespace pou
{

class AbstractNetImpl
{
    public:
        AbstractNetImpl(){};
        virtual ~AbstractNetImpl(){};

        virtual bool init() = 0;
        virtual bool shutdown() = 0;

        virtual std::unique_ptr<AbstractServer> createServer() = 0;
        virtual std::unique_ptr<AbstractClient> createClient() = 0;

    protected:

    private:
};

}


#endif // ABSTRACTNETIMPL_H_INCLUDED
