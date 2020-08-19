#include "ai/AiComponent.h"

#include "world/GameWorld.h"

#include "PouEngine/system/Clock.h"

AiComponent::AiComponent(Character *character) :
    m_character(character),
    m_target(nullptr),
    m_targetId(0)
{
    m_targetId.useBits(GameWorld_Sync::CHARACTERSID_BITS);
    m_syncComponent.addSyncElement(&m_targetId);
}

AiComponent::~AiComponent()
{
    //dtor
}

void AiComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_pathfindingTimer.update(elapsedTime);

    if(!m_character)
        return;

    int oldTargetId = m_targetId.getValue();

    m_syncComponent.update(elapsedTime, localTime);

    if(oldTargetId != m_targetId.getValue())
    {
        auto *world = m_character->getWorldSync();
        if(world)
            m_target = world->getCharacter(m_targetId.getValue()).get();
    }
}

std::list<glm::vec2> &AiComponent::getPlannedPath()
{
    return m_pathfinder.getPath();
}


void AiComponent::setTarget(Character *target)
{
    if(m_target == target)
        return;

    if(m_target)
        this->stopListeningTo(m_target, pou::NotificationType_SenderDestroyed);

    m_target = target;

    if(m_target)
    {
        this->startListeningTo(m_target, pou::NotificationType_SenderDestroyed);
        m_targetId.setValue(m_target->getCharacterSyncId());
    } else
        m_targetId.setValue(0);
}

Character *AiComponent::getTarget()
{
    return m_target;
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

///
///Protected
///

void AiComponent::notify(pou::NotificationSender* sender, int notificationType, void* data)
{
    if(notificationType == pou::NotificationType_SenderDestroyed)
    {
        if(sender == m_target)
            this->setTarget(nullptr);
    }
}

void AiComponent::avoidCollisionsTo(glm::vec2 destination)
{
    auto charPos = m_character->transform()->getGlobalXYPosition();

    ///Change 500 by some parameter depending on monster type (big monsters should avoid less obstacles)
    float minMass = 500;

    auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(charPos, destination - 20.0f * glm::normalize(destination - charPos),
                                                                         20.0f, minMass);

    if(collisionImpact.detectImpact)
    {
        if(!m_pathfindingTimer.isActive())
        {
            m_pathfinder.findPath(charPos, destination, 20.0f, 20.0f, minMass);
            m_pathfindingTimer.reset(0.5f);
        }

        if(m_pathfinder.pathFounded())
        {
            auto &path = m_pathfinder.getPath();

            while(glm::dot(path.front()-charPos, path.front()-charPos) < 100)
                path.pop_front();

            if(!path.empty())
                destination = *path.begin();
        }
    } else
        m_pathfinder.getPath().clear();


    auto direction = destination - m_character->transform()->getGlobalXYPosition();

    auto input = m_character->getInput();

    input->setWalkingDirection(direction);
    input->setLookingDirection(direction);
}


