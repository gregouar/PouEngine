#ifndef TERRAINGENERATOR_PATHGRAPH_H
#define TERRAINGENERATOR_PATHGRAPH_H

#include "PouEngine/tools/RNGesus.h"
#include "PouEngine/math/AstarGrid.h"
#include "PouEngine/system/XMLLoader.h"

#include "PouEngine/Types.h"
#include <vector>

class TerrainGenerator;
struct TerrainGenerator_GroundLayer;

struct TerrainGenerator_PathGraph_GridCell
{
    TerrainGenerator_PathGraph_GridCell() : empty(true){};

    bool empty;
    std::vector<size_t> pathNodes;
};

struct TerrainGenerator_PathGraph_Parameters
{
    TerrainGenerator_PathGraph_Parameters() : width(1), cycleFactor(2){};

    int width;
    float cycleFactor;
};

enum TerrainGenerator_PathGraph_NoiseType
{
    TerrainGenerator_PathGraph_Noise_Perlin,
    TerrainGenerator_PathGraph_Noise_None,
};

struct TerrainGenerator_PathGraph_Noise
{
    TerrainGenerator_PathGraph_Noise();

    TerrainGenerator_PathGraph_NoiseType type;
    float intensity;
    int kernel;
};

class TerrainGenerator_PathGraph
{
    public:
        TerrainGenerator_PathGraph();
        virtual ~TerrainGenerator_PathGraph();

        void addNode(glm::vec2 nodePosition);
        void generatesEdges(TerrainGenerator *terrain, pou::RNGenerator *rng);
        void generatesOnTerrain(TerrainGenerator *terrain, pou::RNGenerator *rng);

        void loadParameters(TiXmlElement *element, pou::HashedString pathName);

        void setGroundLayer(const TerrainGenerator_GroundLayer* groundLayer);
        const std::list<std::vector<glm::ivec2> > *getAllRasterizedPaths();

    protected:
        std::vector<glm::ivec2> rasterizesEdge(const std::pair<glm::vec2, glm::vec2> &edge,
                                              TerrainGenerator *terrain, pou::RNGenerator *rng);

        void loadNoise(TiXmlElement *element);

    private:
        pou::HashedString m_pathName;

        const TerrainGenerator_GroundLayer* m_groundLayer;
        std::vector<glm::vec2> m_nodes;
        //std::vector< std::pair<size_t, size_t> > m_edges;
        std::vector< std::pair<glm::vec2, glm::vec2> > m_edges;
        std::list<std::vector<glm::ivec2> > m_rasterizedEdges;

        pou::AstarGrid_HeuristicType m_heuristicType;
        std::vector<TerrainGenerator_PathGraph_Noise> m_noises;
        std::vector<float>  m_weightGrid;
        std::vector<bool>   m_unreachableGrid;

        TerrainGenerator_PathGraph_Parameters m_parameters;
        /*float m_cellSize;

        glm::ivec2 m_gridSize;
        std::vector<TerrainGenerator_PathGraph_GridCell> m_grid;*/
};

#endif // TERRAINGENERATOR_PATHGRAPH_H
