#ifndef ABSTRACTAUDIOIMPL_H
#define ABSTRACTAUDIOIMPL_H

#include "PouEngine/Types.h"

#include <string>

namespace pou
{

class AbstractAudioImpl
{
    public:
        AbstractAudioImpl() : m_curSoundId(0), m_curBankId(0),m_curEventId(0) {};
        virtual ~AbstractAudioImpl(){};

        virtual void update() = 0;

        virtual SoundTypeId loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream) = 0;
        virtual bool destroySound(SoundTypeId id) = 0;
        virtual bool playSound(SoundTypeId id, float volume, const glm::vec3 &pos) = 0;

        virtual SoundTypeId loadBank(const std::string &path) = 0;
        virtual bool destroyBank(SoundTypeId id) = 0;

        virtual SoundTypeId createEvent(const std::string &eventName) = 0;
        virtual bool destroyEvent(SoundTypeId id) = 0;
        virtual bool playEvent(SoundTypeId id) = 0;

    protected:
        SoundTypeId m_curSoundId;
        SoundTypeId m_curBankId;
        SoundTypeId m_curEventId;

    private:
};

}

#endif // AUDIOENGINE_H

