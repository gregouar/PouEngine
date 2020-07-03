#include "world/WorldSprite.h"

#include "logic/GameMessageTypes.h"

WorldSprite::WorldSprite() : pou::SpriteEntity(),
    m_syncId(0),
    m_lastModelUpdateTime(-1)
{
    //ctor
}

WorldSprite::~WorldSprite()
{
    //dtor
}


void WorldSprite::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_syncComponent.update(elapsedTime, localTime);

    pou::SpriteEntity::update(elapsedTime, localTime);

    GameMessage_World_SpriteUpdated msg;
    msg.sprite = this;
    pou::MessageBus::postMessage(GameMessageType_World_SpriteUpdated, &msg);
}

bool WorldSprite::syncFrom(WorldSprite* srcSprite)
{

    return (true);
}

bool WorldSprite::setSpriteModel(pou::SpriteModel* model)
{
    if(pou::SpriteEntity::setSpriteModel(model))
    {
        m_syncComponent.updateLastUpdateTime();
        m_lastModelUpdateTime = m_syncComponent.getLastUpdateTime();
        return (true);
    }
    return (false);
}

uint32_t WorldSprite::getSyncId()
{
    return m_syncId;
}

uint32_t WorldSprite::getLastModelUptateTime()
{
    return m_lastModelUpdateTime;
}

///
///Protected
///

void WorldSprite::setSyncId(uint32_t id)
{
    m_syncId = id;
}



