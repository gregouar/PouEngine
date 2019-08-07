#ifndef PROFILER_H
#define PROFILER_H

#include "PouEngine/utils/Clock.h"
#include "PouEngine/utils/Singleton.h"

#include <mutex>
#include <stack>
#include <list>

namespace pou
{

struct ProfilerClock
{
    Clock   clock;
    std::string text;
};

struct ProfilerTime
{
    Time    time;
    std::string text;
};

class Profiler : public Singleton<Profiler>
{
    public:
        friend class Singleton<Profiler>;

        static void resetLoop(bool print);
        static void pushClock(const std::string&);
        static void popClock();

    protected:
        Profiler();
        virtual ~Profiler();

        std::mutex m_profilerMutex;

    private:
        Clock m_loopClock;
        std::stack<ProfilerClock> m_clocks;
        std::list<ProfilerTime> m_times;

};

}

#endif // PROFILER_H


