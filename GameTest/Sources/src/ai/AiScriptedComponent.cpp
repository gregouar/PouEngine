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


void AiScriptedComponent::createFromModel(AiScriptModelAsset *model)
{
    m_model = model;
}

void AiScriptedComponent::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    AiComponent::update(elapsedTime, localTime);

    if(!m_model)
        return;

    if(!m_character)
        return;

    auto input = m_character->getInput();
    if(!input)
        return;

    input->reset();

    Character *closetEnemy;
    float closestDistance = -1;

    for(auto enemy : m_character->getNearbyCharacters())
    {
        if(!enemy->isAlive())
            continue;
        if(enemy->getTeam() == m_character->getTeam())
            continue;

        float distance = glm::dot(enemy->getGlobalPosition() - m_character->getGlobalPosition(),
                                  enemy->getGlobalPosition() - m_character->getGlobalPosition());
        if(closestDistance == -1 || closestDistance > distance)
        {
            closetEnemy = enemy;
            closestDistance = distance;
        }
    }

    if(closestDistance != -1 && closestDistance < 300.0f*300.0f)
    {
        glm::vec2 direction = closetEnemy->getGlobalXYPosition() - m_character->getGlobalXYPosition();
        if(closestDistance > 70.0f*70.0f)
        {
            input->setWalkingDirection(direction);
            input->setLookingDirection(direction);
        }
        else
            input->setAttacking(true, direction);
    }

}

