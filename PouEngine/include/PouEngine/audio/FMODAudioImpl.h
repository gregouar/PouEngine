#ifndef FMODAUDIOIMPL_H
#define FMODAUDIOIMPL_H

#include <PouEngine/audio/AbstractAudioImpl.h>

#include "fmod_studio.h"
#include "fmod.h"

#include <map>
#include <vector>


namespace pou
{

class FMODAudioImpl : public AbstractAudioImpl
{
    public:
        FMODAudioImpl();
        virtual ~FMODAudioImpl();

        virtual void update();

        virtual bool loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream);
        virtual bool destroySound(SoundTypeId id);
        virtual bool playSound(SoundTypeId id, float volume, const glm::vec3 &pos);

    protected:
        bool init();
        bool cleanup();

        FMOD_VECTOR VectorToFmod(const glm::vec3& v);

    private:
        FMOD_STUDIO_SYSTEM  *m_studioSystem;
        FMOD_SYSTEM         *m_system;

        int m_nbrChannels;
        int m_currentChannelId;

        std::map<SoundTypeId, FMOD_SOUND*>  m_sounds;
        std::vector<FMOD_CHANNEL*>          m_channels;

    static const int DEFAULT_NBR_CHANNELS;
};

}

#endif // FMODAUDIOIMPL_H
