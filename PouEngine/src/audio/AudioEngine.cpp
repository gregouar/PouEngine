#include "PouEngine/audio/AudioEngine.h"

#include "PouEngine/tools/Logger.h"

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
    this->updateMasterVolumes();

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
        m_impl->update();
}

bool AudioEngine::set3DSettings(float dopplerscale,  float distancefactor,  float rolloffscale)
{
    if(instance()->m_impl == nullptr)
        return (false);
    if(distancefactor <= 0)
        return (false);
    return instance()->m_impl->set3DSettings(dopplerscale, distancefactor, rolloffscale);
}

SoundTypeId AudioEngine::add3DListener()
{
    if(instance()->m_impl == nullptr)
        return (0);
    return instance()->m_impl->add3DListener();
}

bool AudioEngine::remove3DListener(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->remove3DListener(id);
}

bool AudioEngine::set3DListenerOrientation(SoundTypeId id, const glm::vec3 &up, const glm::vec3 &forwrd)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->set3DListenerOrientation(id,up,forwrd);
}

bool AudioEngine::set3DListenerPosition(SoundTypeId id, const glm::vec3 &pos)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->set3DListenerPosition(id,pos);
}

SoundTypeId AudioEngine::loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream)
{
    if(instance()->m_impl == nullptr)
        return (0);

    auto r =  instance()->m_impl->loadSound(path,is3D,isLooping,isStream);
    if(r == 0)
        Logger::error("Could not load sound: "+path);

    return r;
}

bool AudioEngine::destroySound(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->destroySound(id);
}

bool AudioEngine::playSound(SoundTypeId id, float volume, const glm::vec3 &pos)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->playSound(id,volume,pos);
}

bool AudioEngine::playSound(const SoundAsset *soundAsset, float volumeFactor, const glm::vec3 &pos)
{
    return AudioEngine::playSound(soundAsset->getSoundId(),soundAsset->getVolume(), pos);
}

SoundTypeId AudioEngine::loadBank(const std::string &path)
{
    if(instance()->m_impl == nullptr)
        return (0);

    auto r =  instance()->m_impl->loadBank(path);
    if(r == 0)
        Logger::error("Could not load sounds bank: "+path);

    return r;
}

bool AudioEngine::destroyBank(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->destroyBank(id);
}

SoundTypeId AudioEngine::createEvent(const std::string &eventName)
{
    if(instance()->m_impl == nullptr)
        return (0);

    auto r =  instance()->m_impl->createEvent(eventName);
    if(r == 0)
        Logger::error("Could not load sound event: "+eventName);

    return r;
}

bool AudioEngine::destroyEvent(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->destroyEvent(id);
}

bool AudioEngine::playEvent(SoundTypeId id)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->playEvent(id);
}

bool AudioEngine::setEvent3DPosition(SoundTypeId id, const glm::vec3 &pos)
{
    if(instance()->m_impl == nullptr)
        return (false);
    return instance()->m_impl->setEvent3DPosition(id, pos);
}

void AudioEngine::updateMasterVolumes()
{
    if(instance()->m_impl)
        instance()->m_impl->updateMasterVolumes();
}

}
