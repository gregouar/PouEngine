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

glm::vec2 RNGesus::uniformVec2(glm::vec2 min, glm::vec2 max, RNGenerator *generator)
{
    return {RNGesus::uniformFloat(min.x, max.x, generator),
            RNGesus::uniformFloat(min.y, max.y, generator)};
}

glm::vec2 RNGesus::uniformVec2InAnnulus(float minRadius, float maxRadius, RNGenerator *generator)
{
    float angle = RNGesus::uniformFloat(0, glm::pi<float>()*2.0f, generator);
    float squaredRadius = RNGesus::uniformFloat(minRadius*minRadius, maxRadius*maxRadius, generator);
    return (float)sqrt(squaredRadius)*glm::vec2(cos(angle), sin(angle));
}


}
