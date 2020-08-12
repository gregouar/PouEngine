#include "generators/PoissonDiskSampler.h"

PoissonDiskSampler::PoissonDiskSampler() :
    m_rng(nullptr)
{
    //ctor
}

PoissonDiskSampler::~PoissonDiskSampler()
{
    //dtor
}



const std::vector<glm::vec2> &PoissonDiskSampler::generateDistribution(glm::vec2 rectSize, float minDist, int rejectionThresold)
{
    glm::vec2 firstPoint = pou::RNGesus::uniformVec2(glm::vec2(0), glm::vec2(rectSize), m_rng);
    return this->generateDistributionFrom(firstPoint, rectSize, minDist, rejectionThresold);
}

const std::vector<glm::vec2> &PoissonDiskSampler::generateDistributionFrom(glm::vec2 firstPoint, glm::vec2 rectSize,
                                                                           float minDist, int rejectionThresold)
{
    m_pointDistribution = std::vector<glm::vec2>();

    m_cellSize = minDist/sqrt(2);
    m_backgroundGridSize = glm::ceil(rectSize/m_cellSize);
    m_backgroundGrid = std::vector(m_backgroundGridSize.x * m_backgroundGridSize.y, -1);
    m_activeList.clear();
    m_pointDistribution.reserve(m_backgroundGridSize.x * m_backgroundGridSize.y);

    float halfMinDist = minDist * 0.5;

    //glm::vec2 firstPoint = pou::RNGesus::uniformVec2(glm::vec2(0), glm::vec2(rectSize), m_rng);
    this->addPoint(firstPoint);

    while(!m_activeList.empty())
    {
        int randIndex = pou::RNGesus::uniformInt(0, m_activeList.size()-1, m_rng);

        auto startingPointIt = std::next(m_activeList.begin(), randIndex);
        auto startingPointPos = m_pointDistribution[*startingPointIt];

        for(int i = 0 ; i < rejectionThresold ; ++i)
        {
            auto newPointPos = startingPointPos + pou::RNGesus::uniformVec2InAnnulus(minDist, 2*minDist, m_rng);

            bool reject = false;

            if(this->getBackgroundGridValue(newPointPos) != -1)
                reject = true;

            if(newPointPos.x < halfMinDist || newPointPos.y < halfMinDist
            || newPointPos.x > rectSize.x || newPointPos.y > rectSize.y)
                reject = true;

            auto newPointGridPos = glm::floor(newPointPos/m_cellSize);

            if(!reject)
            for(int x = -1 ; x <= 1 ; ++x)
            for(int y = -1 ; y <= 1 ; ++y)
            {
                if(x == 0 && y == 0)
                    continue;

                auto gridValue = this->getBackgroundGridValue(x+newPointGridPos.x,
                                                              y+newPointGridPos.y);

                if(gridValue == -1)
                    continue;

                const auto otherPoint = m_pointDistribution[gridValue];

                if(glm::dot(otherPoint - newPointPos, otherPoint - newPointPos) < minDist * minDist)
                {
                    reject = true;
                    break;
                }
            }

            if(!reject)
            {
                this->addPoint(newPointPos);
                break;
            }
            else if (i + 1 == rejectionThresold)
            {
                m_activeList.erase(startingPointIt);
            }
        }
    }

    return m_pointDistribution;
}


void PoissonDiskSampler::setRng(pou::RNGenerator *rng)
{
    m_rng = rng;
}

///
///Protected
///

void PoissonDiskSampler::addPoint(glm::vec2 pos)
{
    if(!this->addToBackgroundGrid(pos, m_pointDistribution.size()))
        return;
    m_activeList.push_back(m_pointDistribution.size());
    m_pointDistribution.push_back(pos);
}

bool PoissonDiskSampler::addToBackgroundGrid(glm::vec2 pos, int value)
{
    glm::vec2 gridPos = glm::floor(pos/m_cellSize);

    if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x >= m_backgroundGridSize.x || gridPos.y >= m_backgroundGridSize.y)
        return (false);

    m_backgroundGrid[m_backgroundGridSize.x * gridPos.y + gridPos.x] = value;

    return (true);
}

int PoissonDiskSampler::getBackgroundGridValue(glm::vec2 pos)
{
    glm::ivec2 gridPos = glm::floor(pos/m_cellSize);
    return this->getBackgroundGridValue(gridPos.x, gridPos.y);
}

int PoissonDiskSampler::getBackgroundGridValue(int x, int y)
{
    if(x < 0 || y < 0 || x >= m_backgroundGridSize.x || y >= m_backgroundGridSize.y)
        return (-1);
    return m_backgroundGrid[m_backgroundGridSize.x * y + x];
}


