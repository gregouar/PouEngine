#ifndef SYNCEDTIMER_H
#define SYNCEDTIMER_H

#include "PouEngine/Types.h"
#include "PouEngine/utils/SyncedAttribute.h"

namespace pou
{

class SyncedTimer
{
    public:
        SyncedTimer();
        virtual ~SyncedTimer();

        virtual void reset(float time, bool looping = false);
        virtual void reset(const Time &time, bool looping = false);
        virtual int update(float elapsedTime, uint32_t localTime = -1);
        virtual int update(const Time &elapsedTime, uint32_t localTime = -1);

        ///virtual void rewind(uint32_t time);

        bool isActive();

    protected:

    private:
        SyncedAttribute<Time>   m_maxTime;
        SyncedAttribute<Time>   m_elapsedTime;
        bool        m_isLooping;
};

}

#endif // SYNCEDTIMER_H
