#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"
#include "PouEngine/renderers/RenderWindow.h"

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
        void render(pou::RenderWindow *renderWindow);

        //const pou::NetAddress &getAddress() const;
        unsigned short getPort() const;

        void playerWalk(glm::vec2 direction);
        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();
        void disconnectionCleanup();

        void processMessage(std::shared_ptr<pou::NetMessage> msg);

        void updateWorld(const pou::Time &elapsedTime);

    private:
        std::unique_ptr<pou::AbstractClient> m_client;

        glm::vec2   m_lastPlayerWalkDirection;

        GameWorld   m_world;
        uint16_t    m_curWorldId;
        uint16_t    m_curPlayerId;
        bool        m_isWaitingForWorldSync;
        //pou::Timer  m_syncWorldTimer;

        pou::Time m_remainingTime;

        pou::Timer m_syncTimer;

        //static const float CLIENTWORLD_SYNC_DELAY;
    public:
        static const int        TICKRATE;
        static const pou::Time  TICKDELAY;
        static const int        SYNCRATE;
        static const pou::Time  SYNCDELAY;
        static const float      INTERPOLATIONDELAY;
};

#endif // GAMECLIENT_H
