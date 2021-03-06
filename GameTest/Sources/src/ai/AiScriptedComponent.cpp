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
    //m_pathfindingTimer.update(elapsedTime);

    if(elapsedTime == pou::Time(0))
        return;

    if(!m_model)
        return;

    if(!m_character)
        return;

    auto input = m_character->getInput();
    if(!input)
        return;

    input->reset();

    if(!m_character->isAlive())
        return;

    if(m_target && m_target->isAlive())
    {
        glm::vec2 direction = m_target->transform()->getGlobalXYPosition() - m_character->transform()->getGlobalXYPosition();

        float distance = glm::dot(direction,direction);

        if(distance > 1000.0f * 1000.0f)
            this->setTarget(nullptr);
        else if(distance > 70.0f*70.0f)
            this->avoidCollisionsTo(m_target->transform()->getGlobalXYPosition());
        else
            input->setAttacking(true, direction);

        return;
    }

    this->lookForTarget(300.0f);
}


void AiScriptedComponent::notify(pou::NotificationSender* sender, int notificationType, void* data)
{
    AiComponent::notify(sender, notificationType, data);
}



void AiScriptedComponent::lookForTarget(float maxDistance)
{
    Character *closestEnemy = nullptr;
    float closestEnemyDistance = -1;

    Character *closestFriendWithTarget = nullptr;
    float closestFriendDistance = -1;

    auto nearbyCharacters = m_character->getNearbyCharacters();
    if(!nearbyCharacters)
        return;

    auto &thisGlobalPosition = m_character->transform()->getGlobalPosition();

    for(auto otherCharacter : *nearbyCharacters)
    {
        if(!otherCharacter->isAlive())
            continue;

        float distance = glm::dot(otherCharacter->transform()->getGlobalPosition() - thisGlobalPosition,
                                  otherCharacter->transform()->getGlobalPosition() - thisGlobalPosition);

        if(otherCharacter->getTeam() == m_character->getTeam())
        {
            auto otherCharAi = otherCharacter->getAi();
            if(otherCharAi && otherCharAi->getTarget())
            if(closestFriendDistance == -1 || closestFriendDistance > distance)
            {
                closestFriendWithTarget = otherCharacter;
                closestFriendDistance = distance;
            }
            continue;
        }

        if(closestEnemyDistance == -1 || closestEnemyDistance > distance)
        {
            closestEnemy = otherCharacter;
            closestEnemyDistance = distance;
        }
    }

    if(closestEnemyDistance != -1 && closestEnemyDistance < maxDistance*maxDistance)
    {
        this->setTarget(closestEnemy);
        //std::cout<<"ClosestTarget: "<<thisGlobalPosition.x<<" "<<thisGlobalPosition.y<<" "<<closestEnemy->transform()->getGlobalPosition().x<<" "<<closestEnemy->transform()->getGlobalPosition().y<<std::endl;
    }
    else if(closestFriendDistance != -1 && closestFriendDistance < maxDistance*maxDistance)
    {
        auto otherCharAi = closestFriendWithTarget->getAi();
        this->setTarget(otherCharAi->getTarget());
    }
}

/*void AiScriptedComponent::avoidCollisionsTo(glm::vec2 destination)
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
}*/


