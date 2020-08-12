#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/renderers/RenderWindow.h"

#include "character/Character.h"
#include "character/Player.h"
#include "character/PlayerSave.h"
#include "net/NetMessageTypes.h"
#include "world/WorldGrid.h"
#include "world/GameWorld_Sync.h"
#include "generators/WorldGenerator.h"

#include <thread>

class GameWorld : public pou::NotificationListener
{
    public:
        GameWorld(bool renderable);
        virtual ~GameWorld();

        void update(const pou::Time elapsed_time/*, bool isRewinding = false*/);
        void render(pou::RenderWindow *renderWindow);

        void init();
        void generate(bool generateInThread = true);
        ///void rewind(uint32_t time, bool simulate = true);
        void destroy();

        void createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg);

        void generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg, bool useLockStepMode, bool generateInThread = true);

        size_t  askToAddPlayer(std::shared_ptr<PlayerSave> playerSave
                               /*const std::string &playerName = std::string()*/, bool isLocalPlayer, bool useLockStepMode = false);
        bool    askToRemovePlayer(size_t player_id);
        bool    isPlayerCreated(size_t player_id);

        void addPlayerAction(int player_id, const PlayerAction &playerAction);

        glm::vec2 convertScreenToWorldCoord(glm::vec2 p);

        bool isReady();

        uint32_t getLocalTime();

        Character   *getCharacter(int character_id);
        Player      *getPlayer(int player_id);

        GameWorld_Sync *getSyncComponent();

    protected:
        void createScene();
        void createPlayerCamera(Player *player);

        void generateImpl();
        void generateFromMsgImpl(std::shared_ptr<NetMessage_WorldInit> worldInitMsg, bool useLockStepMode);

        bool    initPlayer(size_t player_id, std::shared_ptr<PlayerSave> playerSave);
        bool    removePlayer(size_t player_id);

        void updateSunLight(const pou::Time elapsed_time);

        //void processPlayerActions(const pou::Time elapsed_time);
        void processPlayerActions();

        virtual void notify(pou::NotificationSender* sender, int notificationType, void* data) override;

    private:
        std::atomic<bool> m_worldReady;
        std::thread m_generatingThread;
        //std::mutex  m_serverMutex;

        //TerrainGenerator m_terrainGenerator;
        WorldGenerator m_worldGenerator;

        pou::Scene *m_scene;
        std::shared_ptr<WorldNode> m_worldRootNode;
        std::shared_ptr<WorldGrid> m_worldGrid;
        bool m_isRenderable;

        std::shared_ptr<pou::CameraObject> m_camera;
        std::shared_ptr<pou::LightEntity>  m_sunLight;
        float              m_dayTime; //Between 0 and 360

        std::list< std::pair<int, std::shared_ptr<PlayerSave> > > m_addedPlayersList;
        std::list<int> m_removedPlayersList;

        GameWorld_Sync m_syncComponent;

        std::vector< std::pair<size_t, PlayerAction> > m_playerActions;
        ///std::multimap<uint32_t, std::pair<size_t, PlayerAction> > m_playerActions;

        //pou::SoundTypeId m_musicEvent;

    public:
        static const int        MAX_NBR_PLAYERS;


};

#endif // GAMEWORLD_H
