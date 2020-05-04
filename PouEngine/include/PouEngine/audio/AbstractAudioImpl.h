#ifndef ABSTRACTAUDIOIMPL_H
#define ABSTRACTAUDIOIMPL_H

#include "PouEngine/Types.h"

#include <string>

namespace pou
{

class AbstractAudioImpl
{
    public:
        AbstractAudioImpl() : m_curId(0) {};
        virtual ~AbstractAudioImpl(){};

        virtual void update() = 0;

        virtual bool loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream) = 0;
        virtual bool destroySound(SoundTypeId id) = 0;
        virtual bool playSound(SoundTypeId id, float volume, const glm::vec3 &pos) = 0;


    protected:
        SoundTypeId m_curId;

    private:
};

}

#endif // AUDIOENGINE_H

