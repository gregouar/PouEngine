#ifndef ASTARGRID_H
#define ASTARGRID_H

#include "PouEngine/Types.h"
#include <vector>

namespace pou
{

struct AstarNode
{
    AstarNode();

    float pathToNodeCost;
    float estimatedTotalPathCost;
    glm::ivec2 parentNode;
};

class AstarGrid
{
    public:
        AstarGrid();
        virtual ~AstarGrid();

        void setGridSize(const glm::ivec2 gridSize);
        void setWeightGrid(const std::vector<float> *weightGrid);
        void setUnreachableGrid(const std::vector<bool> *unreachableGrid);

        std::vector<glm::ivec2> lookForPath(glm::ivec2 startCell, glm::ivec2 endCell, bool useWeightAsHeight);

    protected:
        std::pair<float, bool> estimatesRemainingPathCostWithHeightGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell);
        std::vector<glm::ivec2> lookForPathImplWithHeightGrid(glm::ivec2 startCell, glm::ivec2 endCell);

        float estimatesRemainingPathCostWithWeightGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell);
        std::vector<glm::ivec2> lookForPathImplWithWeightGrid(glm::ivec2 startCell, glm::ivec2 endCell);

        float estimatesRemainingPathCostWithoutGrid(const glm::ivec2 &cell, const glm::ivec2 &endCell);
        std::vector<glm::ivec2> lookForPathImplWithoutGrid(glm::ivec2 startCell, glm::ivec2 endCell);

    private:
        glm::ivec2 m_gridSize;
        const std::vector<float> *m_weightGrid;
        const std::vector<bool>  *m_unreachableGrid;
};

}


#endif // ASTARGRID_H
