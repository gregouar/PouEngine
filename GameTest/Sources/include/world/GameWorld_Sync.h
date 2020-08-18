#ifndef GAMEWORLD_SYNC_H
#define GAMEWORLD_SYNC_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SceneGrid.h"
#include "PouEngine/tools/IdAllocator.h"
#include "PouEngine/tools/IdPtrAllocator.h"
#include "PouEngine/renderers/RenderWindow.h"

#include "character/Character.h"
#include "character/Player.h"
#include "net/NetMessageTypes.h"
#include "world/WorldSprite.h"

class GameWorld_Sync : public pou::NotificationListener
{
    public:
        GameWorld_Sync();
        virtual ~GameWorld_Sync();

        void clear();

        virtual void update(const pou::Time &elapsedTime = pou::Time(0));

        //void createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg);
        void createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, uint32_t lastSyncTime);

        //void generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg);
        void syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT, bool useLockStepMode);

        void createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                                 int player_id, uint32_t lastSyncTime);
        void syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg, size_t clientPlayerId, float RTT);
        void addPlayerEvent(std::shared_ptr<NetMessage_PlayerEvent> playerEventMsg/*, size_t clientPlayerId*/);

        ///Only for lockStepMode
        void addPlayerAction(uint32_t player_id, PlayerAction &playerAction, uint32_t actionTime);
        void syncPlayerAction(uint32_t player_id, PlayerAction &playerAction);

        std::shared_ptr<NodeSyncer> syncElement(std::shared_ptr<pou::SceneNode> node, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<pou::SceneNode> node, std::shared_ptr<NodeSyncer> nodeSyncer, uint32_t forceId = 0);
        size_t syncElement(pou::SpriteSheetAsset *spriteSheet, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<WorldSprite> spriteEntity, uint32_t forceId = 0);
        size_t syncElement(pou::MeshAsset *meshModel, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<WorldMesh> meshEntity, uint32_t forceId = 0);
        size_t syncElement(CharacterModelAsset *characterModel, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<Character> character, uint32_t forceId = 0);
        size_t syncElement(ItemModelAsset *characterModel, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<Player> player, uint32_t forceId = 0);
        size_t syncElement(PrefabAsset *prefabAsset, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<PrefabInstance> prefab, uint32_t forceId = 0);

        void desyncElement(NodeSyncer* nodeSyncer, bool noDesyncInsert = false);
        void desyncElement(Character *character, bool noDesyncInsert = false);
        void desyncElement(Player *player, bool noDesyncInsert = false);

        uint32_t getLocalTime();
        uint32_t getLastWorldSyncTime();

        std::vector<Character*>    *getUpdatedCharacters();
        std::shared_ptr<Character>  getCharacter(int character_id);
        std::shared_ptr<Player>     getPlayer(int player_id);

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;

        void createWorldSyncMsg_Node(NodeSyncer *nodeSyncer, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Sprite(WorldSprite *sprite, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Mesh(WorldMesh *mesh, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Character(Character *character, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Prefab(PrefabInstance *prefab, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);

        void removeFromUpdatedCharacters(Character *character);

        void processPlayerEvents();
        void processPlayerActions();

    private:
        uint32_t m_curLocalTime;
        uint32_t m_syncTime;
        uint32_t m_lastWorldSyncTime;
        std::map<int, uint32_t> m_lastPlayerSyncTime;
        uint32_t m_deltaRTT;

        std::vector<NodeSyncer*>    m_updatedNodeSyncers;
        std::vector<WorldSprite*>   m_updatedSprites;
        std::vector<WorldMesh*>     m_updatedMeshes;
        bool m_updatedCharactersBuffer;
        std::vector<Character*>     m_updatedCharacters[2];

        pou::IdPtrAllocator<pou::SceneNode>         m_syncNodes;
        pou::IdPtrAllocator<NodeSyncer>             m_nodeSyncers;

        pou::IdAllocator<pou::SpriteSheetAsset*>    m_syncSpriteSheets;
        pou::IdPtrAllocator<WorldSprite>            m_syncSpriteEntities;
        pou::IdAllocator<pou::MeshAsset*>           m_syncMeshModels;
        pou::IdPtrAllocator<WorldMesh>              m_syncMeshEntities;

        pou::IdAllocator<CharacterModelAsset*>      m_syncCharacterModels;
        pou::IdPtrAllocator<Character>              m_syncCharacters;
        pou::IdAllocator<ItemModelAsset*>           m_syncItemModels;
        pou::IdPtrAllocator<Player>                 m_syncPlayers;

        pou::IdAllocator<PrefabAsset*>              m_syncPrefabAssets;
        pou::IdPtrAllocator<PrefabInstance>         m_syncPrefabInstances;

        std::multimap<uint32_t, int> m_syncTimeSpriteSheets;
        std::multimap<uint32_t, int> m_syncTimeMeshModels;
        std::multimap<uint32_t, int> m_syncTimeCharacterModels;
        std::multimap<uint32_t, int> m_syncTimeItemModels;
        std::multimap<uint32_t, int> m_syncTimePrefabModels;

        std::multimap<uint32_t, int> m_desyncNodes;
        std::multimap<uint32_t, int> m_desyncCharacters;
        std::multimap<uint32_t, int> m_desyncPlayers;

        std::multimap<uint32_t, std::shared_ptr<NetMessage_PlayerEvent> > m_playerEvents;
        std::multimap<uint32_t, std::pair<size_t, PlayerAction> > m_playerActions;
        std::multimap<uint32_t, std::pair<size_t, PlayerAction> > m_syncPlayerActions;

    public:
        static const int        NODEID_BITS;
        static const int        SPRITESHEETID_BITS;
        static const int        SPRITEENTITYID_BITS;
        static const int        MESHMODELID_BITS;
        static const int        MESHENTITYID_BITS;
        static const int        CHARACTERMODELSID_BITS;
        static const int        CHARACTERSID_BITS;
        static const int        ITEMMODELSID_BITS;
        static const int        PREFABASSETID_BITS;
        static const int        PREFABINSTANCEID_BITS;
};

#endif // GAMEWORLD_SYNC_H
