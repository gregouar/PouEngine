#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SceneEntity.h"
#include "PouEngine/tools/IdAllocator.h"
#include "PouEngine/tools/IdPtrAllocator.h"
#include "PouEngine/renderers/RenderWindow.h"

#include "character/Character.h"
#include "character/Player.h"
#include "net/NetMessageTypes.h"
#include "world/WorldGrid.h"


class GameWorld
{
    public:
        GameWorld(bool renderable);
        virtual ~GameWorld();

        void update(const pou::Time elapsed_time/*, bool isRewinding = false*/);
        void render(pou::RenderWindow *renderWindow);

        void generate();
        ///void rewind(uint32_t time, bool simulate = true);
        void destroy();

        void createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg);
        void createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, uint32_t lastSyncTime);

        void generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg);
        void syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT);

        //void createAskForSyncMsg(std::shared_ptr<NetMessage_AskForWorldSync> askForWorldSyncMsg,
        //                         int player_id, uint32_t lastSyncTime);

        void createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                                 int player_id, uint32_t lastSyncTime);
        void syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg, size_t clientPlayerId, float RTT);
        void processPlayerEvent(std::shared_ptr<NetMessage_PlayerEvent> playerEventMsg, size_t clientPlayerId);

        size_t  askToAddPlayer(bool isLocalPlayer = false);
        bool    askToRemovePlayer(size_t player_id);
        bool    isPlayerCreated(size_t player_id);

        //void playerWalk(int player_id, glm::vec2 direction, float clientTime = -1);

        /**void addPlayerAction(int player_id, const PlayerAction &playerAction, uint32_t clientTime = -1);
        void removeAllPlayerActions(int player_id, uint32_t time = -1);**/

        void addPlayerAction(int player_id, const PlayerAction &playerAction);

        glm::vec2 convertScreenToWorldCoord(glm::vec2 p);

        uint32_t getLocalTime();
        uint32_t getLastSyncTime();

        Player *getPlayer(int player_id);

    protected:
        //size_t addSyncNode(pou::SceneNode *node);
        //size_t addSyncEntity(pou::SceneEntity *entity);

        void createScene();
        void createPlayerCamera(size_t player_id);

        bool    initPlayer(size_t player_id);
        bool    removePlayer(size_t player_id);

        size_t syncElement(std::shared_ptr<pou::SceneNode> node);
        size_t syncElement(pou::SpriteSheetAsset *spriteSheet);
        size_t syncElement(std::shared_ptr<pou::SpriteEntity> spriteEntity);
        size_t syncElement(CharacterModelAsset *characterModel);
        size_t syncElement(std::shared_ptr<Character> character);
        size_t syncElement(ItemModelAsset *characterModel);
        size_t syncElement(std::shared_ptr<Player> player);

        void desyncElement(pou::SceneNode* node, bool noDesyncInsert = false);
        void desyncElement(Character *character, bool noDesyncInsert = false);
        void desyncElement(Player *player, bool noDesyncInsert = false);

        void updateSunLight(const pou::Time elapsed_time);

        //void processPlayerActions(const pou::Time elapsed_time);
        void processPlayerActions();

        /*size_t syncNode(pou::SceneNode *node);
        size_t syncNode(pou::SceneNode *node, size_t parentNodeId);
        size_t syncSpriteSheet(pou::SpriteSheetAsset *spriteSheet);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId);
        size_t syncSpriteEntity(pou::SpriteEntity *spriteEntity, size_t spriteSheetId, size_t nodeId);*/

       // pou::SceneNode* createNode(pou::SceneNode* parentNode, bool sync = false);
        //std::pair<size_t, pou::SpriteSheetAsset*> syncSpriteSheet(const std::string &path);
       /* std::pair<size_t, SpriteEntitySync> createSpriteEntitySync(size_t spriteSheetId, const std::string &spriteName);
        std::pair<size_t, SpriteEntitySync> createSpriteEntitySync(std::pair<size_t, pou::SpriteSheetAsset*> spriteSheetPair, const std::string &spriteName);*/
        //pou::SpriteEntity* createSpriteEntity();


    private:
        pou::Scene *m_scene;
        std::shared_ptr<WorldGrid> m_worldGrid;
        bool m_isRenderable;

        uint32_t m_curLocalTime;
        uint32_t m_syncTime;
        uint32_t m_lastSyncTime;
        uint32_t m_deltaRTT;

        std::shared_ptr<pou::CameraObject> m_camera;
        std::shared_ptr<pou::LightEntity>  m_sunLight;
        float              m_dayTime; //Between 0 and 360

        std::list<int> m_addedPlayersList;
        std::list<int> m_removedPlayersList;

        ///Move all this to some worldNetComponent ?
        pou::IdPtrAllocator<pou::SceneNode>         m_syncNodes;
        pou::IdAllocator<pou::SpriteSheetAsset*>    m_syncSpriteSheets;
        pou::IdPtrAllocator<pou::SpriteEntity>      m_syncSpriteEntities;
        pou::IdAllocator<CharacterModelAsset*>      m_syncCharacterModels;
        pou::IdPtrAllocator<Character>              m_syncCharacters;
        pou::IdAllocator<ItemModelAsset*>           m_syncItemModels;
        pou::IdPtrAllocator<Player>                 m_syncPlayers;

        std::multimap<uint32_t, int> m_syncTimeSpriteSheets;
        std::multimap<uint32_t, int> m_syncTimeCharacterModels;
        std::multimap<uint32_t, int> m_syncTimeItemModels;

        std::multimap<uint32_t, int> m_desyncNodes;
        std::multimap<uint32_t, int> m_desyncCharacters;
        std::multimap<uint32_t, int> m_desyncPlayers;

        std::vector< std::pair<size_t, PlayerAction> > m_playerActions;
        ///std::multimap<uint32_t, std::pair<size_t, PlayerAction> > m_playerActions;
        ///uint32_t m_wantedRewind;

    public:
        static const int        MAX_NBR_PLAYERS;

        static const int        NODEID_BITS;
        static const int        SPRITESHEETID_BITS;
        static const int        SPRITEENTITYID_BITS;
        static const int        CHARACTERMODELSID_BITS;
        static const int        CHARACTERSID_BITS;
        static const int        ITEMMODELSID_BITS;


};

#endif // GAMEWORLD_H
