#ifndef SOUNDENTITY_H
#define SOUNDENTITY_H

#include "PouEngine/scene/SceneObject.h"

#include "PouEngine/audio/AudioEngine.h"


namespace pou
{


class SoundObject : public SceneObject
{
    public:
        SoundObject();
        virtual ~SoundObject();

        bool play();
        //bool pause();
        //bool stop();

        bool setSoundEventModel(const std::string &eventPath);

        virtual void notify(NotificationSender* , NotificationType,
                            size_t dataSize = 0, char* data = nullptr) override;

    protected:
        void updateSoundPosition();

    protected:
        SoundTypeId m_soundId;
        bool        m_isEvent;

    private:

};

}

#endif // SOUNDENTITY_H
