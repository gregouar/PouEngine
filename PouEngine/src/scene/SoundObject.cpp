#include "PouEngine/scene/SoundObject.h"

#include "PouEngine/scene/SceneNode.h"

namespace pou
{


SoundObject::SoundObject() : m_soundId(0), m_isEvent(true)
{
    m_isASound = true;
}

SoundObject::~SoundObject()
{
    //dtor
}

std::shared_ptr<SceneObject> SoundObject::createCopy()
{
    auto newObject = std::make_shared<SoundObject>();
    newObject->m_soundId = m_soundId;
    newObject->m_isEvent = m_isEvent;
    return newObject;
}

std::shared_ptr<SoundObject> SoundObject::createSoundCopy()
{
    return std::dynamic_pointer_cast<SoundObject>(this->createCopy());
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
        AudioEngine::setEvent3DPosition(m_soundId, m_parentNode->transform()->getGlobalPosition());
}


void SoundObject::notify(NotificationSender *sender, int notificationType,
                         void* data)
{
    if(notificationType == NotificationType_NodeMoved)
        this->updateSoundPosition();

    SceneObject::notify(sender,notificationType,data);
}


}
