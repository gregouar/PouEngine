#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "PouEngine/utils/Singleton.h"
#include "PouEngine/assets/SoundAsset.h"
#include "PouEngine/audio/AbstractAudioImpl.h"

#include <memory>

namespace pou
{

class AudioEngine  : public Singleton<AudioEngine>
{

    friend class Singleton<AudioEngine>;

    public:
        AudioEngine();
        virtual ~AudioEngine();

        bool init(std::unique_ptr<AbstractAudioImpl> impl);
        bool cleanup();

        void update();

        static bool loadSound(const std::string &path, bool is3D = false, bool isLooping = false,
                       bool isStream = false);
        static bool destroySound(SoundTypeId id);
        static bool playSound(SoundTypeId id, float volume = 1.0f, const glm::vec3 &pos = glm::vec3(0));
        static bool playSound(const SoundAsset *soundAsset, float volumeFactor = 1.0f, const glm::vec3 &pos = glm::vec3(0));

    protected:

    private:
        std::unique_ptr<AbstractAudioImpl> m_impl;
};

}

#endif // AUDIOENGINE_H
