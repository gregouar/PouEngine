#ifndef POISSONDISKSAMPLER_H
#define POISSONDISKSAMPLER_H

#include "PouEngine/tools/RNGesus.h"

class PoissonDiskSampler
{
    public:
        PoissonDiskSampler();
        virtual ~PoissonDiskSampler();

        const std::vector<glm::vec2> &generateDistribution(glm::vec2 rectSize, float minDist, int rejectionThresold = 30);
        const std::vector<glm::vec2> &generateDistributionFrom(glm::vec2 startingPoint, glm::vec2 rectSize,
                                                               float minDist, int rejectionThresold = 30);

        void setRng(pou::RNGenerator *rng);

    protected:
        void addPoint(glm::vec2 pos);
        bool addToBackgroundGrid(glm::vec2 pos, int value);
        int getBackgroundGridValue(glm::vec2 pos);
        int getBackgroundGridValue(int x, int y);

    private:
        pou::RNGenerator *m_rng;

        float m_cellSize;
        glm::ivec2 m_backgroundGridSize;
        std::vector<int> m_backgroundGrid;

        std::list<int> m_activeList;
        std::vector<glm::vec2> m_pointDistribution;
};

#endif // POISSONDISKSAMPLER_H
