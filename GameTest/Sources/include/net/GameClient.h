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

        bool connectToServer(const pou::NetAddress &address, std::shared_ptr<PlayerSave> playerSave,
                             bool useLockStepMode = false);
        bool disconnectFromServer();

        void update(const pou::Time &elapsedTime);

        const pou::NetAddress &getServerAddress() const;
        unsigned short getPort() const;
        float getRTT() const;

        Player *getPlayer();

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();
        void disconnectionCleanup();

        void processMessage(std::shared_ptr<pou::NetMessage> msg);

        void updateWorld(const pou::Time &elapsedTime);

        virtual void notify(pou::NotificationSender*, int notificationType, void* data) override;

    private:
        std::unique_ptr<pou::AbstractClient> m_client;

        bool m_useLockStepMode;
        std::shared_ptr<PlayerSave> m_playerSave;

        std::unique_ptr<GameWorld>  m_world;
        uint16_t    m_curWorldId;
        uint16_t    m_curPlayerId;
        bool        m_isWaitingForWorldSync;
        bool        m_isWaitingForWorldGen;

        glm::vec2   m_curCursorPos;

        pou::Time m_remainingTime;

        pou::Timer m_syncTimer;

        uint32_t    m_lastServerAckTime;

    public:
        static const int        TICKRATE;
        static const pou::Time  TICKDELAY;
        static const int        SYNCRATE;
        static const pou::Time  SYNCDELAY;
        static const float      INTERPOLATIONDELAY;
        static const uint32_t   MAX_PLAYER_REWIND;
};

#endif // GAMECLIENT_H
