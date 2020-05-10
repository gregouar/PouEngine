#ifndef NETENGINE_H
#define NETENGINE_H

#include <memory>

#include "PouEngine/utils/Singleton.h"
#include "PouEngine/net/AbstractNetImpl.h"

namespace pou
{

class NetEngine  : public Singleton<NetEngine>
{
    friend class Singleton<NetEngine>;

    public:
        NetEngine();
        virtual ~NetEngine();

        static bool init(std::unique_ptr<AbstractNetImpl> impl);
        bool cleanup();

        static std::unique_ptr<AbstractServer> createServer();
        static std::unique_ptr<AbstractClient> createClient();

    protected:

    private:
        std::unique_ptr<AbstractNetImpl> m_impl;
};

}

#endif // NETENGINE_H
