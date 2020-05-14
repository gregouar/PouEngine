#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

///Need to keep track of clients connected to the server somehow

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create(unsigned short port = 0);

        void update(const pou::Time &elapsedTime);

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();

        void processMessage(int clientNbr, std::shared_ptr<pou::NetMessage> msg);

    private:
        std::unique_ptr<pou::AbstractServer> m_server;
};

#endif // GAMESERVER_H
