#ifndef WORLDSPRITE_H
#define WORLDSPRITE_H

#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/sync/SyncElements.h"


class WorldSprite : public pou::SpriteEntity
{
    friend class GameWorld_Sync;

    public:
        WorldSprite();
        virtual ~WorldSprite();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        virtual bool syncFrom(WorldSprite* srcSprite);

        virtual bool setSpriteModel(pou::SpriteModel* model);

        uint32_t getSyncId();
        uint32_t getLastModelUptateTime();

    protected:
        void setSyncId(uint32_t id);

    private:
        uint32_t m_syncId;
        uint32_t m_lastModelUpdateTime;

        pou::SyncComponent m_syncComponent;
};

#endif // WORLDSPRITE_H
