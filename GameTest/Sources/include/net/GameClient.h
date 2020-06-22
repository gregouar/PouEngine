#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"
#include "PouEngine/renderers/RenderWindow.h"
#include "PouEngine/core/NotificationListener.h"

#include "world/GameWorld.h"

class GameClient : public pou::NotificationListener
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
        const pou::NetAddress &getServerAddress() const;
        unsigned short getPort() const;

        Player *getPlayer();

        /*void playerCursor(glm::vec2 direction);
        void playerLook(glm::vec2 direction);
        void playerWalk(glm::vec2 direction);
        void playerDash(glm::vec2 direction);
        void playerAttack(glm::vec2 direction);
        void playerUseItem(size_t itemNbr);*/

        void addPlayerAction(const PlayerAction &action);

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();
        void disconnectionCleanup();

        void processMessage(std::shared_ptr<pou::NetMessage> msg);

        void updateWorld(const pou::Time &elapsedTime);

        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data);

    private:
        std::unique_ptr<pou::AbstractClient> m_client;

       // uint32_t    m_lastPlayerWalkTime;
        glm::vec2   m_lastPlayerWalkDirection;

        std::unique_ptr<GameWorld>  m_world;
        uint16_t    m_curWorldId;
        uint16_t    m_curPlayerId;
        bool        m_isWaitingForWorldSync;
        //pou::Timer  m_syncWorldTimer;

        glm::vec2   m_curCursorPos;

        pou::Time m_remainingTime;

        pou::Timer m_syncTimer;

        uint32_t    m_lastServerAckTime;

        //static const float CLIENTWORLD_SYNC_DELAY;
    public:
        static const int        TICKRATE;
        static const pou::Time  TICKDELAY;
        static const int        SYNCRATE;
        static const pou::Time  SYNCDELAY;
        static const float      INTERPOLATIONDELAY;
        static const uint32_t   MAX_PLAYER_REWIND;
};

#endif // GAMECLIENT_H
