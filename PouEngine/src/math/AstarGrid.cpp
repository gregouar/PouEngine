#include "PouEngine/math/AstarGrid.h"

#include <queue>
#include <map>

namespace pou
{

AstarNode::AstarNode() :
    pathToNodeCost(std::numeric_limits<float>::max()),
    estimatedTotalPathCost(std::numeric_limits<float>::max()),
    parentNode(-1)
{

}

AstarGrid::AstarGrid() :
    m_gridSize(0),
    m_weightGrid(nullptr)
{
    //ctor
}

AstarGrid::~AstarGrid()
{
    //dtor
}



void AstarGrid::setGridSize(const glm::ivec2 gridSize)
{
    m_gridSize = gridSize;
}

void AstarGrid::setWeightGrid(const std::vector<float> *weightGrid)
{
    m_weightGrid = weightGrid;
}

std::vector<glm::ivec2> AstarGrid::lookForPath(glm::ivec2 startCell, glm::ivec2 endCell, bool useWeightAsHeight)
{
    if(m_weightGrid && useWeightAsHeight)
        return this->lookForPathImplWithHeightGrid(startCell, endCell);
    else if(useWeightAsHeight)
        return this->lookForPathImplWithWeightGrid(startCell, endCell);
    else
        return this->lookForPathImplWithoutGrid(startCell, endCell);
}

///
///Protected
///


float AstarGrid::estimatesRemainingPathCostWithHeightGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell)
{
    auto cellZ = (*m_weightGrid)[cell.y * m_gridSize.x + cell.x];
    auto endCellZ = (*m_weightGrid)[endCell.y * m_gridSize.x + endCell.x];
    return glm::length(glm::vec3(endCell.x, endCell.y, endCellZ) - glm::vec3(cell.x, cell.y, cellZ));
}

std::vector<glm::ivec2> AstarGrid::lookForPathImplWithHeightGrid(glm::ivec2 startCell, glm::ivec2 endCell)
{
    std::vector<glm::ivec2> finalPath;

    //unordered_set_intpair openSet;
    ///std::priority_queue<float, glm::ivec2, std::less<float> > openSet;

    //std::unordered_map<std::pair<int, int>, glm::ivec2, IntPairHash> cameFromMap;
    std::unordered_map<std::pair<int, int>, AstarNode, IntPairHash> astarNodesMap;

    AstarNode startAstarNode;
    startAstarNode.estimatedTotalPathCost = estimatesRemainingPathCostWithHeightGrid(startCell, endCell);
    startAstarNode.pathToNodeCost = 0;
    astarNodesMap.insert({{startCell.x, startCell.y},startAstarNode});

    std::multimap<float, glm::ivec2> openSet;
    openSet.insert({startAstarNode.estimatedTotalPathCost,startCell});

    /*std::vector<glm::ivec2> neighborSamples = { {-1,-1},{0,-1},{1,-1},
                                                {-1,0},        {1,0},
                                                {-1,1} ,{0,1} ,{1,1}};*/

    std::vector<glm::ivec2> neighborSamples = { {0,-1},
                                                {-1,0},        {1,0},
                                                {0,1} };
    while(!openSet.empty())
    {
        auto openSetIt = openSet.extract(openSet.begin());
        auto curTotalCost = openSetIt.key();
        auto curCell = openSetIt.mapped();

        if(curCell == endCell)
        {
            do
            {
                finalPath.push_back(curCell);
                curCell = astarNodesMap[{curCell.x,curCell.y}].parentNode;
            }while(curCell != glm::ivec2(-1));

            return finalPath;
        }

        auto &curAstarCell = astarNodesMap[{curCell.x,curCell.y}];

        for(int i = 0 ; i < 8 ; ++i)
        {
            auto sampleCell = curCell + neighborSamples[i];
            if(sampleCell.x < 0 || sampleCell.y < 0 || sampleCell.x >= m_gridSize.x || sampleCell.y >= m_gridSize.y)
                continue;

            auto &sampleAstarCell = astarNodesMap[{sampleCell.x,sampleCell.y}];

            auto newPathToNodeCost = curAstarCell.pathToNodeCost + estimatesRemainingPathCostWithHeightGrid(curCell, sampleCell);
            if(newPathToNodeCost < sampleAstarCell.pathToNodeCost)
            {
                auto oldOpenSetIt = openSet.find(sampleAstarCell.pathToNodeCost);
                while(oldOpenSetIt != openSet.end() && oldOpenSetIt->first == sampleAstarCell.pathToNodeCost)
                    if(oldOpenSetIt->second == sampleCell)
                    {
                        openSet.erase(oldOpenSetIt);
                        break;
                    }

                sampleAstarCell.parentNode = curCell;
                sampleAstarCell.pathToNodeCost = newPathToNodeCost;
                sampleAstarCell.estimatedTotalPathCost = newPathToNodeCost + estimatesRemainingPathCostWithHeightGrid(sampleCell, endCell);
                openSet.insert({sampleAstarCell.estimatedTotalPathCost, sampleCell});
            }

        }
    }
    return finalPath;
}

float AstarGrid::estimatesRemainingPathCostWithWeightGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell)
{
    return glm::length(glm::vec2(endCell) - glm::vec2(cell));
}

std::vector<glm::ivec2> AstarGrid::lookForPathImplWithWeightGrid(glm::ivec2 startCell, glm::ivec2 endCell)
{
    std::vector<glm::ivec2> finalPath;

    return finalPath;
}

float AstarGrid::estimatesRemainingPathCostWithoutGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell)
{
    return glm::length(glm::vec2(endCell) - glm::vec2(cell));
}

std::vector<glm::ivec2> AstarGrid::lookForPathImplWithoutGrid(glm::ivec2 startCell, glm::ivec2 endCell)
{
    std::vector<glm::ivec2> finalPath;

    return finalPath;
}


}

