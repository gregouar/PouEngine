#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>

#include "PouEngine/net/NetEngine.h"

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create();

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

    protected:
        void cleanup();

    private:
        std::unique_ptr<pou::AbstractServer> m_server;
};

#endif // GAMESERVER_H
