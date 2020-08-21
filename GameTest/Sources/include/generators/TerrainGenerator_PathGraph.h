#ifndef TERRAINGENERATOR_PATHGRAPH_H
#define TERRAINGENERATOR_PATHGRAPH_H

#include "PouEngine/Types.h"
#include "tinyxml/tinyxml.h"
#include <vector>

class TerrainGenerator;
struct TerrainGenerator_GroundLayer;

struct TerrainGenerator_PathGraph_GridCell
{
    TerrainGenerator_PathGraph_GridCell() : empty(true){};

    bool empty;
    std::vector<size_t> pathNodes;
};

class TerrainGenerator_PathGraph
{
    public:
        TerrainGenerator_PathGraph();
        virtual ~TerrainGenerator_PathGraph();

        void addNode(glm::vec2 nodePosition);
        void generatesEdges(TerrainGenerator *terrain);
        void generatesOnTerrain(TerrainGenerator *terrain);

        void loadParameters(TiXmlElement *element);

        void setGroundLayer(const TerrainGenerator_GroundLayer* groundLayer);

    protected:
        std::vector<glm::vec2> rasterizesEdge(const std::pair<glm::vec2, glm::vec2> &edge, TerrainGenerator *terrain);

    private:
        const TerrainGenerator_GroundLayer* m_groundLayer;
        std::vector<glm::vec2> m_nodes;
        //std::vector< std::pair<size_t, size_t> > m_edges;
        std::vector< std::pair<glm::vec2, glm::vec2> > m_edges;

        int m_width;
        /*float m_cellSize;

        glm::ivec2 m_gridSize;
        std::vector<TerrainGenerator_PathGraph_GridCell> m_grid;*/
};

#endif // TERRAINGENERATOR_PATHGRAPH_H
