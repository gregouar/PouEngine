#include "ai/AiComponent.h"

#include "world/GameWorld.h"

AiComponent::AiComponent(Character *character) :
    m_character(character),
    m_target(nullptr),
    m_targetId(0)
{
    m_targetId.useBits(GameWorld::CHARACTERSID_BITS);
    m_syncComponent.addSyncElement(&m_targetId);
}

AiComponent::~AiComponent()
{
    //dtor
}

void AiComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(!m_character)
        return;

    int oldTargetId = m_targetId.getValue();

    m_syncComponent.update(elapsedTime, localTime);

    if(oldTargetId != m_targetId.getValue())
    {
        auto *world = m_character->getWorld();
        if(world)
            m_target = world->getSyncCharacter(m_targetId.getValue());
    }
}

pou::SyncComponent *AiComponent::getSyncComponent()
{
    return &m_syncComponent;
}

/*void AiComponent::syncFrom(AiComponent *aiComponent)
{
    m_targetId.syncFrom(&aiComponent->m_targetId);
}

void AiComponent::serialize(pou::Stream *stream, uint32_t clientTime)
{
    {
        int targetId = m_targetId.getValue();
        stream->serializeBits(targetId, GameWorld::CHARACTERSID_BITS);

        if(stream->isReading())
            m_targetId.setValue(targetId, true);
    }
}*/

void AiComponent::notify(pou::NotificationSender*, int notificationType, void* data)
{

}
