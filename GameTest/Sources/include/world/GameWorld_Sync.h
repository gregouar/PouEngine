#ifndef GAMEWORLD_SYNC_H
#define GAMEWORLD_SYNC_H

#include "PouEngine/scene/Scene.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/tools/IdAllocator.h"
#include "PouEngine/tools/IdPtrAllocator.h"
#include "PouEngine/renderers/RenderWindow.h"

#include "character/Character.h"
#include "character/Player.h"
#include "net/NetMessageTypes.h"
#include "world/WorldGrid.h"
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
        void syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT);

        void createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                                 int player_id, uint32_t lastSyncTime);
        void syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg, size_t clientPlayerId, float RTT);
        void processPlayerEvent(std::shared_ptr<NetMessage_PlayerEvent> playerEventMsg, size_t clientPlayerId);

        size_t syncElement(std::shared_ptr<WorldNode> node, uint32_t forceId = 0);
        size_t syncElement(pou::SpriteSheetAsset *spriteSheet, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<WorldSprite> spriteEntity, uint32_t forceId = 0);
        size_t syncElement(CharacterModelAsset *characterModel, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<Character> character, uint32_t forceId = 0);
        size_t syncElement(ItemModelAsset *characterModel, uint32_t forceId = 0);
        size_t syncElement(std::shared_ptr<Player> player, uint32_t forceId = 0);

        void desyncElement(WorldNode* node, bool noDesyncInsert = false);
        void desyncElement(Character *character, bool noDesyncInsert = false);
        void desyncElement(Player *player, bool noDesyncInsert = false);

        uint32_t getLocalTime();
        uint32_t getLastSyncTime();

        std::vector<Character*>    *getUpdatedCharacters();
        std::shared_ptr<Character>  getCharacter(int character_id);
        std::shared_ptr<Player>     getPlayer(int player_id);

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;

        void createWorldSyncMsg_Node(WorldNode *node, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Sprite(WorldSprite *sprite, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);
        void createWorldSyncMsg_Character(Character *character, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                                    int player_id, uint32_t lastSyncTime);

        void removeFromUpdatedCharacters(Character *character);

    private:
        uint32_t m_curLocalTime;
        uint32_t m_syncTime;
        uint32_t m_lastSyncTime;
        uint32_t m_deltaRTT;

        std::vector<WorldNode*>     m_updatedNodes;
        std::vector<WorldSprite*>   m_updatedSprites;
        bool m_updatedCharactersBuffer;
        std::vector<Character*>     m_updatedCharacters[2];

        pou::IdPtrAllocator<WorldNode>              m_syncNodes;
        pou::IdAllocator<pou::SpriteSheetAsset*>    m_syncSpriteSheets;
        pou::IdPtrAllocator<WorldSprite>            m_syncSpriteEntities;
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

    public:
        static const int        NODEID_BITS;
        static const int        SPRITESHEETID_BITS;
        static const int        SPRITEENTITYID_BITS;
        static const int        CHARACTERMODELSID_BITS;
        static const int        CHARACTERSID_BITS;
        static const int        ITEMMODELSID_BITS;
};

#endif // GAMEWORLD_SYNC_H
