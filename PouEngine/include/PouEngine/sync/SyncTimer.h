#ifndef SyncTimer_H
#define SyncTimer_H

#include "PouEngine/Types.h"
#include "PouEngine/sync/SyncAttribute.h"

namespace pou
{

class SyncTimer
{
    public:
        SyncTimer();
        virtual ~SyncTimer();

        virtual void reset(float time, bool looping = false);
        virtual void reset(const Time &time, bool looping = false);
        virtual int update(float elapsedTime, uint32_t localTime = -1);
        virtual int update(const Time &elapsedTime, uint32_t localTime = -1);

        ///virtual void rewind(uint32_t time);

        bool isActive();

    protected:

    private:
        SyncAttribute<Time>   m_maxTime;
        SyncAttribute<Time>   m_elapsedTime;
        bool        m_isLooping;
};

}

#endif // SyncTimer_H
