#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create(unsigned short port = 0);

        void update(const pou::Time &elapsedTime);

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

        void sendReliableMsgTest();

    protected:
        void cleanup();

    private:
        std::unique_ptr<pou::AbstractServer> m_server;
};

#endif // GAMESERVER_H
