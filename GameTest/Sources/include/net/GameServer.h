#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"

///Need to keep track of clients connected to the server somehow

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create(unsigned short port = 0);
        void shutdown();

        void update(const pou::Time &elapsedTime);
        size_t generateWorld();

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();

        void processMessage(int clientNbr, std::shared_ptr<pou::NetMessage> msg);

    private:
        std::unique_ptr<pou::AbstractServer> m_server;

        std::map<size_t, GameWorld> m_worlds;
        size_t m_curWorldId;
};

#endif // GAMESERVER_H
