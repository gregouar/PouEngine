#include "ai/AutoLightAiComponent.h"

#include "PouEngine/tools/Hasher.h"
#include "character/Player.h"

AutoLightAiComponent::AutoLightAiComponent(Character *character) :
    AiComponent(character),
    m_activated(false),
    ANIMATION_NAME_ACTIVATE(pou::Hasher::unique_hash("activate"))
{
    //ctor
}

AutoLightAiComponent::~AutoLightAiComponent()
{
    //dtor
}

void AutoLightAiComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    AiComponent::update(elapsedTime, localTime);
    //m_pathfindingTimer.update(elapsedTime);

    if(elapsedTime == pou::Time(0))
        return;

    if(m_activated)
        return;

    auto nearbyPlayers = m_character->getNearbyPlayers();
    if(!nearbyPlayers)
        return;

    float sqMinDistance = 350.0f;
    sqMinDistance *= sqMinDistance;

    for(auto player : *nearbyPlayers)
    {
        auto charPos = m_character->transform()->getGlobalXYPosition();
        auto destination = player->transform()->getGlobalXYPosition();

        auto delta = destination - charPos;
        auto sqDistance = glm::dot(delta, delta);

        if(sqDistance > sqMinDistance)
            continue;

        auto collisionImpact = pou::PhysicsEngine::castCollisionDetectionRay(charPos, destination - 20.0f * glm::normalize(destination - charPos),
                                                                             0.0f, -1);

        if(collisionImpact.detectImpact)
            continue;

        m_activated = true;
        m_character->startAnimation(ANIMATION_NAME_ACTIVATE);
        break;
    }

    /*auto input = m_character->getInput();
    if(!input)
        return;

    input->reset();*/

    /**if(m_target && m_target->isAlive())
    {
        glm::vec2 direction = m_target->transform()->getGlobalXYPosition() - m_character->transform()->getGlobalXYPosition();


        float distance = glm::dot(direction,direction);
        if(distance < 100.0f)
        {
            m_character->startAnimation(ANIMATION_NAME_ACTIVATE);
        }
           // m_character->switchState();
    }

    this->lookForTarget(300.0f);**/


}

///
///Protected
///


/*void AutoLightAiComponent::lookForTarget(float maxDistance)
{
    Character *closestEnemy = nullptr;
    float closestEnemyDistance = -1;

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
}*/

