#ifndef TIMER_H
#define TIMER_H

#include "PouEngine/Types.h"

namespace pou
{


class Timer
{
    public:
        Timer();
        virtual ~Timer();

        virtual void reset(float time, bool looping = false);
        virtual int update(float elapsedTime);
        virtual int update(const pou::Time &elapsedTime);

        bool isActive();

    protected:

    private:
        float   m_maxTime;
        float   m_elapsedTime;
        bool    m_isLooping;
};

}

#endif // TIMER_H
