#ifndef NETENGINE_H
#define NETENGINE_H

#include <memory>

#include "PouEngine/utils/Singleton.h"
#include "PouEngine/net/AbstractNetImpl.h"
#include "PouEngine/net/ReliableMessagesFactory.h"

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

        static void addReliableMessageModel(std::unique_ptr<ReliableMessage> msgModel);
        static std::shared_ptr<ReliableMessage> createReliableMessage(int type);
        static int getNbrReliableMsgTypes();

    protected:

    private:
        std::unique_ptr<AbstractNetImpl> m_impl;
        ReliableMessagesFactory m_reliableMsgFactory;
};

}

#endif // NETENGINE_H
