#include "ai/AiComponent.h"

#include "world/GameWorld.h"

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

void AiComponent::notify(pou::NotificationSender*, int notificationType, void* data)
{

}

void AiComponent::avoidCollisionsTo(glm::vec2 destination)
{
    auto charPos = m_character->getGlobalXYPosition();

    auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(charPos, destination,
                                                                         20.0f, -1);


    if(collisionImpact.detectImpact)
    {
        if(!m_pathfindingTimer.isActive())
        {
            m_pathfinder.findPath(charPos, destination, 20.0f, -1);
            m_pathfindingTimer.reset(0.5f);

            //auto &path = m_pathfinder.getPath();
            //path.push_back({collisionImpact.collisionImpact});

            /*auto &path = m_pathfinder.getPath();

            std::cout<<"start:"<<charPos.x<<" "<<charPos.y<<std::endl;
            for(auto no : path)
                std::cout<<no.x<<" "<<no.y<<std::endl;
            std::cout<<"destination:"<<destination.x<<" "<<destination.y<<std::endl;
            std::cout<<std::endl;*/
        }

        if(m_pathfinder.pathFounded())
        {
            auto &path = m_pathfinder.getPath();

            while(glm::dot(path.front()-charPos, path.front()-charPos) < 100)
                path.pop_front();

            /*if(path.size() > 1)
                destination = *std::next(path.begin());
            else*/
            if(!path.empty())
                destination = *path.begin();

        }
    } else
        m_pathfinder.getPath().clear();


    auto direction = destination - m_character->getGlobalXYPosition();

    auto input = m_character->getInput();

    input->setWalkingDirection(direction);
    input->setLookingDirection(direction);
}


