#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"

class GameClient
{
    public:
        GameClient();
        virtual ~GameClient();

        bool create(unsigned short port = 0);

        bool connectToServer(const pou::NetAddress &address);
        bool disconnectFromServer();

        void update(const pou::Time &elapsedTime);

        //const pou::NetAddress &getAddress() const;
        unsigned short getPort() const;

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();

        void processMessage(std::shared_ptr<pou::NetMessage> msg);

    private:
        std::unique_ptr<pou::AbstractClient> m_client;

        GameWorld m_world;
};

#endif // GAMECLIENT_H
