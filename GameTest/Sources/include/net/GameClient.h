#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

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

        void sendReliableMsgTest();

    protected:
        void cleanup();

    private:
        std::unique_ptr<pou::AbstractClient> m_client;
};

#endif // GAMECLIENT_H
