#ifndef RNGESUS_H
#define RNGESUS_H

#include "PouEngine/core/Singleton.h"
#include "PouEngine/Types.h"

#include <random>

namespace pou
{

class RNGenerator
{
    friend class RNGesus;

    public:
        RNGenerator();
        virtual ~RNGenerator();

        void seed(int seed);

    protected:
        std::mt19937 *getRNG();

    private:
        std::mt19937 m_generator;
};

class RNGesus : public Singleton<RNGesus>
{
    friend class Singleton<RNGesus>;

    public:
        static void seed(int seed);
        static int rand(RNGenerator *generator = nullptr);
        static int uniformInt(int min, int max, RNGenerator *generator = nullptr);
        static float uniformFloat(float min, float max, RNGenerator *generator = nullptr);
        static glm::vec2 uniformVec2(glm::vec2 min, glm::vec2 max, RNGenerator *generator = nullptr);
        static glm::vec2 uniformVec2InAnnulus(float minRadius, float maxRadius, RNGenerator *generator = nullptr);

    protected:
        RNGesus();
        virtual ~RNGesus();

    private:
        RNGenerator m_defaultGenerator;
};

}

#endif // RNGESUS_H
