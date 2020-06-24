#ifndef NETENGINE_H
#define NETENGINE_H

#include <memory>

#include "PouEngine/core/Singleton.h"
#include "PouEngine/net/AbstractNetImpl.h"
#include "PouEngine/net/NetMessagesFactory.h"

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

        static void addNetMessageModel(std::unique_ptr<NetMessage> msgModel);
        static std::shared_ptr<NetMessage> createNetMessage(int type);
        static int getNbrNetMsgTypes();

        static void setSyncDelay(float delay);
        static float getSyncDelay();

        static void setMaxRewindAmount(size_t rewindAmount);
        static float getMaxRewindAmount();

    protected:

    private:
        std::unique_ptr<AbstractNetImpl> m_impl;
        NetMessagesFactory m_netMsgFactory;

        float   m_syncDelay;
        size_t  m_maxRewindAmount;
};

}

#endif // NETENGINE_H
