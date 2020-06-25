#ifndef PLAYABLECHARACTER_H
#define PLAYABLECHARACTER_H

#include "character/Character.h"
#include "character/CharacterInput.h"

#include "assets/ItemModelAsset.h"

class Player : public Character
{
    public:
        Player(bool userControlled = true);
        virtual ~Player();

        virtual bool setModel(CharacterModelAsset *model);
        virtual ItemModelAsset* removeGear(GearType type);
        virtual ItemModelAsset* useGear(ItemModelAsset *itemModel);
        virtual ItemModelAsset* useGear(size_t itemNbr);

        virtual ItemModelAsset* addItemToInventory(ItemModelAsset *itemModel, size_t itemNbr); //itemNbr is position inside inventory
        virtual ItemModelAsset* getItemFromInventory(size_t itemNbr);
        virtual ItemModelAsset* removeItemFromInventory(size_t itemNbr);

        void processAction(const PlayerAction &playerAction);

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        virtual const std::list<Hitbox> *getHitboxes() const;
        ItemModelAsset *getItemModel(GearType type);
        size_t getInventorySize() const;

        void serializePlayer(pou::Stream *stream, uint32_t clientTime = -1);

        virtual void syncFromPlayer(Player *srcPlayer);

        ///virtual void    setTimeShift(int shift);
        uint32_t        getLastPlayerUpdateTime();
        uint32_t        getLastGearUpdateTime();
        uint32_t        getLastInventoryUpdateTime();

    protected:
        virtual void updateGearsAttributes();

        void setLastPlayerUpdateTime(uint32_t time, bool force = false);

    protected:
        std::list<Hitbox>             m_hitboxes;
        std::vector<ItemModelAsset *> m_gearsModel;
        std::vector<ItemModelAsset *> m_inventory;

        uint32_t m_lastPlayerSyncTime;
        uint32_t m_lastPlayerUpdateTime;
        uint32_t m_lastInventoryUpdateTime;
        uint32_t m_lastGearUpdateTime;
        ///pou::SyncAttribute<int> m_timeShift;

        bool m_userControlled;

    private:
        //pou::SyncTimer  m_dashDelayTimer, m_dashTimer;
        //bool        m_isDashing;
        //pou::SyncAttribute<glm::vec2>   m_dashDirection;
        //float       m_dashDelay, m_dashTime;

        /*static const float DEFAULT_DASH_DELAY;
        static const float DEFAULT_DASH_TIME;
        static const float DEFAULT_DASH_SPEED;*/
};

#endif // PLAYABLECHARACTER_H
