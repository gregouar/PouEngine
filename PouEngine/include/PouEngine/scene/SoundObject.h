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

        virtual std::shared_ptr<SceneObject> createCopy();
        virtual std::shared_ptr<SoundObject> createSoundCopy();

        bool play();
        //bool pause();
        //bool stop();

        bool setSoundEventModel(const std::string &eventPath);

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

    protected:
        void updateSoundPosition();

    protected:
        SoundTypeId m_soundId;
        bool        m_isEvent;

    private:

};

}

#endif // SOUNDENTITY_H
