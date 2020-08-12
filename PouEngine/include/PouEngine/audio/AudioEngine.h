#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/assets/SoundAsset.h"
#include "PouEngine/audio/AbstractAudioImpl.h"

#include <memory>

namespace pou
{

class AudioEngine : public Singleton<AudioEngine>
{
    friend class Singleton<AudioEngine>;

    public:
        bool init(std::unique_ptr<AbstractAudioImpl> impl);
        bool cleanup();

        void update();

        static bool set3DSettings(float dopplerscale = 1.0f,  float distancefactor = 1.0f,  float rolloffscale = 1.0f);

        static SoundTypeId add3DListener();
        static bool remove3DListener(SoundTypeId id);
        static bool set3DListenerOrientation(SoundTypeId id, const glm::vec3 &up, const glm::vec3 &forwrd);
        static bool set3DListenerPosition(SoundTypeId id, const glm::vec3 &pos);

        static SoundTypeId loadSound(const std::string &path, bool is3D = false, bool isLooping = false,
                       bool isStream = false);
        static bool destroySound(SoundTypeId id);
        static bool playSound(SoundTypeId id, float volume = 1.0f, const glm::vec3 &pos = glm::vec3(0));
        static bool playSound(const SoundAsset *soundAsset, float volumeFactor = 1.0f, const glm::vec3 &pos = glm::vec3(0));

        static SoundTypeId loadBank(const std::string &path);
        static bool destroyBank(SoundTypeId id);

        static SoundTypeId createEvent(const std::string &eventName);
        static bool destroyEvent(SoundTypeId id);
        static bool playEvent(SoundTypeId id);
        static bool setEvent3DPosition(SoundTypeId id, const glm::vec3 &pos);

        static void updateMasterVolumes();

    protected:
        AudioEngine();
        virtual ~AudioEngine();

    private:
        std::unique_ptr<AbstractAudioImpl> m_impl;

        std::set<std::string> m_erroredEvents;
};

}

#endif // AUDIOENGINE_H
