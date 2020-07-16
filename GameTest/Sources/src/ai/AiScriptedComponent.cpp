#include "ai/AiScriptedComponent.h"

AiScriptedComponent::AiScriptedComponent(Character *character):
    AiComponent(character),
    m_model(nullptr)
{
    //ctor
}

AiScriptedComponent::~AiScriptedComponent()
{
    //dtor
}

/// NEED TO SYNC m_target OVER NETWORK

void AiScriptedComponent::createFromModel(AiScriptModelAsset *model)
{
    m_model = model;
}

void AiScriptedComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    AiComponent::update(elapsedTime, localTime);
    m_pathfindingTimer.update(elapsedTime);

    if(!m_model)
        return;

    if(!m_character)
        return;

    auto input = m_character->getInput();
    if(!input)
        return;

    input->reset();

    if(m_target && m_target->isAlive())
    {
        glm::vec2 direction = m_target/*->node()*/->getGlobalXYPosition() - m_character/*->node()*/->getGlobalXYPosition();

        float distance = glm::dot(direction,direction);

        if(distance > 1000.0f * 1000.0f)
        {
            m_target = nullptr;
            this->stopListeningTo(m_target,pou::NotificationType_SenderDestroyed);
            m_targetId.setValue(0);
        }
        else if(distance > 70.0f*70.0f)
        {
            this->avoidCollisionsTo(m_target->getGlobalXYPosition());
            //input->setWalkingDirection(direction);
            //input->setLookingDirection(direction);
        }
        else
            input->setAttacking(true, direction);

        return;
    }

    this->lookForTarget(300.0f);
}


void AiScriptedComponent::notify(pou::NotificationSender* sender, int notificationType, void* data)
{
    AiComponent::notify(sender, notificationType, data);

    if(notificationType == pou::NotificationType_SenderDestroyed)
    {
        if(sender == m_target)
            m_target = nullptr;
    }
}



void AiScriptedComponent::lookForTarget(float maxDistance)
{
    Character *closestEnemy = nullptr;
    float closestDistance = -1;

    auto nearbyCharacters = m_character->getNearbyCharacters();
    if(!nearbyCharacters)
        return;

    for(auto enemy : *nearbyCharacters)
    {
        if(!enemy->isAlive())
            continue;
        if(enemy->getTeam() == m_character->getTeam())
            continue;

        float distance = glm::dot(enemy/*->node()*/->getGlobalPosition() - m_character/*->node()*/->getGlobalPosition(),
                                  enemy/*->node()*/->getGlobalPosition() - m_character/*->node()*/->getGlobalPosition());
        if(closestDistance == -1 || closestDistance > distance)
        {
            closestEnemy = enemy;
            closestDistance = distance;
        }
    }

    if(closestDistance != -1 && closestDistance < maxDistance*maxDistance)
    {
        m_target = closestEnemy;
        this->startListeningTo(m_target,pou::NotificationType_SenderDestroyed);
        m_targetId.setValue(m_target->getCharacterSyncId());
    }
}

void AiScriptedComponent::avoidCollisionsTo(glm::vec2 destination)
{
    auto charPos = m_character->getGlobalXYPosition();

    if(!m_pathfindingTimer.isActive())
    {
        m_pathfinder.findPath(charPos, destination, 25.0f, -1);
        m_pathfindingTimer.reset(0.1f);

        auto &path = m_pathfinder.getPath();

        std::cout<<"start:"<<charPos.x<<" "<<charPos.y<<std::endl;
        for(auto no : path)
            std::cout<<no.x<<" "<<no.y<<std::endl;
        std::cout<<"destination:"<<destination.x<<" "<<destination.y<<std::endl;
        std::cout<<std::endl;
    }

    if(m_pathfinder.pathFounded())
    {
        auto &path = m_pathfinder.getPath();

        while(glm::dot(path.front()-charPos, path.front()-charPos) < 10)
            path.pop_front();

        if(path.size() > 1)
            destination = *std::next(path.begin());
        else
            destination = *path.begin();

    }

    auto direction = destination - m_character->getGlobalXYPosition();

    auto input = m_character->getInput();

    input->setWalkingDirection(direction);
    input->setLookingDirection(direction);
}


