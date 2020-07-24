#ifndef CLOCK_H
#define CLOCK_H

#include "PouEngine/Types.h"

namespace pou
{

Time TimeZero();

class Clock
{
    public:
        Clock();
        virtual ~Clock();

        Time restart();
        Time elapsedTime();

    private:
        std::chrono::time_point<std::chrono::steady_clock > m_lastTime;
};

}

#endif // CLOCK_H
