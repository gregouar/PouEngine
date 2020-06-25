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
        glm::vec2 direction = m_target->getGlobalXYPosition() - m_character->getGlobalXYPosition();

        float distance = glm::dot(direction,direction);

        if(distance > 1000.0f * 1000.0f)
        {
            m_target = nullptr;
            this->stopListeningTo(m_target,pou::NotificationType_SenderDestroyed);
            m_targetId.setValue(0);
        }
        else if(distance > 70.0f*70.0f)
        {
            input->setWalkingDirection(direction);
            input->setLookingDirection(direction);
        }
        else
            input->setAttacking(true, direction);

        return;
    }

    Character *closestEnemy = nullptr;
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
            closestEnemy = enemy.get();
            closestDistance = distance;
        }
    }

    if(closestDistance != -1 && closestDistance < 300.0f*300.0f)
    {
        m_target = closestEnemy;
        this->startListeningTo(m_target,pou::NotificationType_SenderDestroyed);
        m_targetId.setValue(m_target->getSyncId());
    }

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

