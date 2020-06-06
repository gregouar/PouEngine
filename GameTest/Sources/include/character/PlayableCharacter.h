#ifndef PLAYABLECHARACTER_H
#define PLAYABLECHARACTER_H

#include <character/Character.h>

#include "assets/ItemModelAsset.h"

enum PlayerActionType
{
    PlayerActionType_CursorMove,
    PlayerActionType_Walk,
    PlayerActionType_Look,
    PlayerActionType_Attack,
    PlayerActionType_Dash,
    PlayerActionType_UseItem,
    NBR_PLAYERACTIONTYPES,
};

struct PlayerAction
{
    int actionType;

    glm::vec2   direction;
    int         value;
};


class PlayableCharacter : public Character
{
    public:
        PlayableCharacter();
        virtual ~PlayableCharacter();

        //virtual bool loadModel(const std::string &path);
        virtual bool setModel(CharacterModelAsset *model);
        //virtual bool loadItem(const std::string &path);
        virtual ItemModelAsset* removeGear(GearType type);
        virtual ItemModelAsset* useGear(ItemModelAsset *itemModel);
        virtual ItemModelAsset* useGear(size_t itemNbr);

        virtual ItemModelAsset* addItemToInventory(ItemModelAsset *itemModel, size_t itemNbr); //itemNbr is position inside inventory
        virtual ItemModelAsset* getItemFromInventory(size_t itemNbr);
        virtual ItemModelAsset* removeItemFromInventory(size_t itemNbr);

        virtual void askToWalk(glm::vec2 direction);
        virtual void askToAttack(glm::vec2 direction = glm::vec2(0,0));
        virtual void askToDash(glm::vec2 direction = glm::vec2(0,0));

        virtual void walk(glm::vec2 direction);
        virtual bool attack(glm::vec2 direction = glm::vec2(0,0), const std::string &animationName = "attack");
        virtual void lookAt(glm::vec2 position);
        virtual void forceAttackMode(bool force = true);
        virtual bool dash(glm::vec2 direction = glm::vec2(0,0));

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        virtual void rewind(uint32_t time);

        virtual const std::list<Hitbox> *getHitboxes() const;
        ItemModelAsset *getItemModel(GearType type);
        size_t getInventorySize() const;


        void serializePlayer(pou::Stream *stream, uint32_t clientTime = -1);
        bool syncFromPlayer(PlayableCharacter *srcPlayer);

        virtual void    setTimeShift(int shift);
        virtual void    setSyncAndLocalTime(uint32_t syncTime);
        void            setLastPlayerUpdateTime(uint32_t time, bool force = false);
        uint32_t        getLastPlayerUpdateTime();
        uint32_t        getLastGearUpdateTime(bool useSyncDelay = true);
        uint32_t        getLastInventoryUpdateTime(bool useSyncDelay = true);

    protected:
        virtual void updateGearsAttributes();

    protected:
        std::list<Hitbox>             m_hitboxes;
        std::vector<ItemModelAsset *> m_gearsModel;
        std::vector<ItemModelAsset *> m_inventory;

        uint32_t m_lastPlayerSyncTime;
        uint32_t m_lastPlayerUpdateTime;
        uint32_t m_lastInventoryUpdateTime;
        uint32_t m_lastGearUpdateTime;
        pou::SyncedAttribute<int> m_timeShift;


    private:
        //bool        m_isInCombatMode;
        //float   m_combatModeDelay;
        pou::SyncedTimer  m_combatModeTimer;

        //float       m_wantToAttackDelay;
        pou::SyncedTimer  m_wantToAttackTimer;
        pou::SyncedAttribute<glm::vec2>   m_wantToAttackDirection;

        pou::SyncedTimer  m_dashDelayTimer, m_dashTimer;
        //bool        m_isDashing;
        pou::SyncedAttribute<glm::vec2>   m_dashDirection;
        //float       m_dashDelay, m_dashTime;

        glm::vec2   m_wantToWalkDirection;
        //float       m_normalWalkingSpeed;

        pou::SyncedTimer  m_wantToDashTimer;
        //float       m_wantToDashDelay;
        pou::SyncedAttribute<glm::vec2>   m_wantToDashDirection;

        bool        m_isLateralWalking;
        glm::vec2   m_lookingAt;

        static const float DEFAULT_COMBATMODE_DELAY;
        static const float DEFAULT_WANTTOATTACK_DELAY;
        static const float DEFAULT_DASH_DELAY;
        static const float DEFAULT_DASH_TIME;
        static const float DEFAULT_DASH_SPEED;
        static const float DEFAULT_WANTTODASH_DELAY;
};

#endif // PLAYABLECHARACTER_H
