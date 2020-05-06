#include "PouEngine/scene/SoundObject.h"

#include "PouEngine/scene/SceneNode.h"

namespace pou
{


SoundObject::SoundObject() : m_soundId(0), m_isEvent(true)
{
    //ctor
}

SoundObject::~SoundObject()
{
    //dtor
}

bool SoundObject::play()
{
    if(m_soundId == 0)
        return (false);

    if(m_isEvent)
        return AudioEngine::playEvent(m_soundId);

    return (true);
}


bool SoundObject::setSoundEventModel(const std::string &eventPath)
{
    if(m_soundId != 0)
    {
        if(m_isEvent)
            AudioEngine::destroyEvent(m_soundId);
    }

    m_soundId = AudioEngine::createEvent(eventPath);
    m_isEvent = true;

    this->updateSoundPosition();

    return (m_soundId != 0);
}

void SoundObject::updateSoundPosition()
{
    if(m_parentNode == nullptr || m_soundId == 0)
        return;

    if(m_isEvent)
        AudioEngine::setEvent3DPosition(m_soundId, m_parentNode->getGlobalPosition());
}


void SoundObject::notify(NotificationSender *sender, NotificationType notification,
                             size_t dataSize, char* data)
{
    if(notification == Notification_NodeMoved)
        this->updateSoundPosition();

    SceneObject::notify(sender,notification,dataSize,data);
}


}
