#include "PouEngine/scene/SoundObject.h"


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

    return (m_soundId != 0);
}

void SoundObject::updateSoundPosition()
{

}


void SoundObject::notify(NotificationSender *sender, NotificationType notification,
                             size_t dataSize, char* data)
{
    if(notification == Notification_NodeMoved)
        this->updateSoundPosition();

    SceneObject::notify(sender,notification,dataSize,data);
}


}
