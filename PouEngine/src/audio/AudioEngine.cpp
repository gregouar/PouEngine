#include "PouEngine/audio/AudioEngine.h"

#include "PouEngine/utils/Logger.h"

namespace pou
{

AudioEngine::AudioEngine() :
    m_impl(nullptr)
{
    //ctor
}

AudioEngine::~AudioEngine()
{
    this->cleanup();
}

bool AudioEngine::init(std::unique_ptr<AbstractAudioImpl> impl)
{
    this->cleanup();

    m_impl = std::move(impl);

    return (true);
}

bool AudioEngine::cleanup()
{
    m_impl.reset();

    return (true);
}

void AudioEngine::update()
{
    if(m_impl != nullptr)
        m_impl.get()->update();
}


bool AudioEngine::loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream)
{
    if(instance()->m_impl == nullptr)
        return (false);

    if(!instance()->m_impl.get()->loadSound(path,is3D,isLooping,isStream))
    {
        Logger::error("Could not load sound: "+path);
        return (false);
    }

    return (true);
}

bool AudioEngine::destroySound(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl.get()->destroySound(id);
}

bool AudioEngine::playSound(SoundTypeId id, float volume, const glm::vec3 &pos)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl.get()->playSound(id,volume,pos);
}

bool AudioEngine::playSound(const SoundAsset *soundAsset, float volumeFactor, const glm::vec3 &pos)
{
    return AudioEngine::playSound(soundAsset->getSoundId(),soundAsset->getVolume(), pos);
}

}
