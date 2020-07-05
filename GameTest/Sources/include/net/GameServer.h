#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>
#include <thread>

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"

struct GameClientInfos
{
    size_t  world_id;
    size_t  player_id;
    uint32_t   lastSyncTime;
    uint32_t   localTime;
    uint32_t   lastActionTime;

    bool playerCreated;
    bool isLocalPlayer;

    pou::Timer syncTimer;
};

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create(unsigned short port = 0, bool allowLocalPlayers = false/*, bool launchInThread = false*/);
        void shutdown();

        void update(const pou::Time &elapsedTime);
        void syncClients(const pou::Time &elapsedTime);
        //void render(pou::RenderWindow *renderWindow, size_t clientId);

        int addLocalPlayer(std::shared_ptr<PlayerSave> playerSave/*const std::string &playerName*/); //Return clientId
        /*void playerCursor(size_t clientId, glm::vec2 cursorPos, float localTime = -1);
        void playerLook(size_t clientId, glm::vec2 direction, float localTime = -1);
        void playerWalk(size_t clientId, glm::vec2 direction, float localTime = -1);
        void playerDash(size_t clientId, glm::vec2 direction, float localTime = -1);
        void playerAttack(size_t clientId, glm::vec2 direction, float localTime = -1);
        void playerUseItem(size_t clientId, size_t itemNbr, float localTime = -1);*/
        //void addPlayerAction(size_t clientId, const PlayerAction &action/*, uint32_t localTime = -1*/);

        size_t generateWorld();
        ///void rewindWorld(size_t world_id, uint32_t time);

        Player* getPlayer(size_t clientId);
        std::pair<GameClientInfos*, GameWorld*> getClientInfosAndWorld(size_t clientId);

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

        void sendMsgTest(bool reliable, bool forceSend);


    protected:
        void cleanup();

        void processMessage(int clientId, std::shared_ptr<pou::NetMessage> msg);
        //void processPlayerActions(int clientId, std::shared_ptr<NetMessage_PlayerAction> msg);
        void updateClientSync(int clientId, std::shared_ptr<NetMessage_PlayerSync> msg);
        void processPlayerEvent(int clientId, std::shared_ptr<NetMessage_PlayerEvent> msg);

        void addClient(int clientId, bool isLocalClient = false);
        void disconnectClient(int clientId);

        void updateWorlds(const pou::Time &elapsedTime);

        //void threading();


    private:
        bool m_serverIsRunning;
        std::unique_ptr<pou::AbstractServer> m_server;


        std::map<size_t, GameClientInfos> m_clientInfos;
        std::map<size_t, std::unique_ptr<GameWorld> > m_worlds;
        size_t m_curWorldId;

        pou::Time m_remainingTime;


        /*bool        m_isInThread;
        std::thread m_serverThread;
        std::mutex  m_serverMutex;*/

        bool        m_allowLocalPlayers;


    public:
        static const int        TICKRATE;
        static const pou::Time  TICKDELAY;
        static const int        SYNCRATE;
        static const pou::Time  SYNCDELAY;
        ///static const int MAX_REWIND_AMOUNT;

        ///static const bool USEREWIND;
};

#endif // GAMESERVER_H
