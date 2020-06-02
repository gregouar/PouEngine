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
        virtual void reset(const Time &time, bool looping = false);
        virtual int update(float elapsedTime);
        virtual int update(const Time &elapsedTime);

        bool isActive();

    protected:

    private:
        pou::Time   m_maxTime;
        pou::Time   m_elapsedTime;
        bool        m_isLooping;
};

}

#endif // TIMER_H
