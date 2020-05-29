#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <memory>
#include <thread>

#include "PouEngine/Types.h"
#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"

///Need to keep track of clients connected to the server somehow

struct GameClientInfos
{
    size_t  world_id;
    size_t  player_id;
    float   localTime;

    bool playerCreated;
    bool isLocalPlayer;

    pou::Timer syncTimer;
};

class GameServer
{
    public:
        GameServer();
        virtual ~GameServer();

        bool create(unsigned short port = 0, bool allowLocalPlayers = false, bool launchInThread = false);
        void shutdown();

        void update(const pou::Time &elapsedTime);
        void syncClients(const pou::Time &elapsedTime);
        void render(pou::RenderWindow *renderWindow, size_t clientNbr);

        int addLocalPlayer(); //ClientNbr
        void playerWalk(size_t clientNbr, glm::vec2 direction, float localTime = -1);

        size_t generateWorld();

        //const pou::NetAddress *getAddress() const;
        unsigned short getPort() const;

        void sendMsgTest(bool reliable, bool forceSend);

    protected:
        void cleanup();

        void processMessage(int clientNbr, std::shared_ptr<pou::NetMessage> msg);
        void processPlayerActions(int clientNbr, std::shared_ptr<NetMessage_PlayerAction> msg);
        void updateClientSync(int clientNbr, std::shared_ptr<NetMessage_AskForWorldSync> msg);

        void addClient(int clientNbr, bool isLocalClient = false);
        void disconnectClient(int clientNbr);

        void updateWorlds(const pou::Time &elapsedTime);

        void threading();

    private:
        std::atomic<bool> m_serverIsRunning;
        std::unique_ptr<pou::AbstractServer> m_server;


        std::map<size_t, GameClientInfos> m_clientInfos;
        std::map<size_t, GameWorld> m_worlds;
        size_t m_curWorldId;

        pou::Time m_remainingTime;


        bool        m_isInThread;
        std::thread m_serverThread;
        std::mutex  m_serverMutex;

        bool        m_allowLocalPlayers;


    public:
        static const int TICKRATE;
        static const float SYNCDELAY;
};

#endif // GAMESERVER_H
