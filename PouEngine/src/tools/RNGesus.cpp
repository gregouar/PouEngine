#include "PouEngine/tools/RNGesus.h"

#include <chrono>

namespace pou
{

///
///RNGenerator
///

RNGenerator::RNGenerator()
{
    m_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

RNGenerator::~RNGenerator()
{

}

void RNGenerator::seed(int seed)
{
    m_generator.seed(seed);
}

std::mt19937 *RNGenerator::getRNG()
{
    return &m_generator;
}

///
///RNGesus
///

RNGesus::RNGesus()
{
    //ctor
}

RNGesus::~RNGesus()
{
    //dtor
}

void RNGesus::seed(int seed)
{
    instance()->m_defaultGenerator.seed(seed);
}

int RNGesus::rand(RNGenerator *generator)
{
    if(!generator)
        generator = &instance()->m_defaultGenerator;
    auto rng = generator->getRNG();
    return (*rng)();
}

int RNGesus::uniformInt(int min, int max, RNGenerator *generator)
{
    if(!generator)
        generator = &instance()->m_defaultGenerator;

    auto rng = generator->getRNG();

    std::uniform_int_distribution<> distrib(min, max);
    return distrib(*rng);
}

float RNGesus::uniformFloat(float min, float max, RNGenerator *generator)
{
    if(!generator)
        generator = &instance()->m_defaultGenerator;

    auto rng = generator->getRNG();

    std::uniform_real_distribution<> distrib(min, max);
    return distrib(*rng);
}


}
