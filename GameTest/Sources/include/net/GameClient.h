#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "PouEngine/net/NetEngine.h"

class GameClient
{
    public:
        GameClient();
        virtual ~GameClient();

        bool create();

        bool connect(const pou::NetAddress &address);
        bool disconnect();

        //const pou::NetAddress &getAddress() const;
        unsigned short getPort() const;

    protected:
        void cleanup();

    private:
        std::unique_ptr<pou::AbstractClient> m_client;
};

#endif // GAMECLIENT_H
