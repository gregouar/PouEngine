#include "generators/TerrainGenerator_PathGraph.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/math/Graph.h"

#include "generators/TerrainGenerator.h"

TerrainGenerator_PathGraph::TerrainGenerator_PathGraph() :
    m_groundLayer(nullptr),
    m_width(1)//,
   // m_cellSize(0)
{
    //ctor
}

TerrainGenerator_PathGraph::~TerrainGenerator_PathGraph()
{
    //dtor
}

void TerrainGenerator_PathGraph::addNode(glm::vec2 nodePosition)
{
    m_nodes.push_back(nodePosition);
}

void TerrainGenerator_PathGraph::generatesEdges(TerrainGenerator *terrain)
{
    pou::UnorientedGraph fullGraph;

    for(size_t i = 0 ; i < m_nodes.size() ; ++i)
    {
        fullGraph.addNode(i);
        for(size_t j = 0 ; j < m_nodes.size() ; ++j)
        {
            auto weight = glm::length(m_nodes[j] - m_nodes[i]);
            fullGraph.addEdge({i,j,weight});
        }
    }

    auto spanningTree = fullGraph.computeMinimalSpanningTree();

    for(auto edge : spanningTree.getEdges())
        m_edges.push_back({m_nodes[edge.startNode],
                           m_nodes[edge.endNode]});

    ///THINK HOW TO ADD SOME CYCLES IN ADDITION TO THE RECIPE

    /*auto terrainExtent = terrain->getExtent();
    m_gridSize = glm::ceil(terrainExtent/m_cellSize);

    m_grid.resize(gridSize.x * gridSize.y);*/

    /*for(size_t i = 0 ; i < m_nodes.size() ; ++i)
    for(size_t j = 0 ; j < m_nodes.size() ; ++j)
        m_edges.push_back({i,j});*/
}

void TerrainGenerator_PathGraph::generatesOnTerrain(TerrainGenerator *terrain)
{
    for(auto edge : m_edges)
    {
        auto rasterizedPath = this->rasterizesEdge(edge, terrain);
        for(auto cell : rasterizedPath)
            terrain->setGroundLayer(cell.x, cell.y, m_groundLayer);
    }
}


void TerrainGenerator_PathGraph::loadParameters(TiXmlElement *pathElement)
{
    auto widthAtt = pathElement->Attribute("width");
    //auto cellSizeAtt = pathElement->Attribute("cellSize");

    if(widthAtt && pou::Parser::isInt(widthAtt))
        m_width = pou::Parser::parseInt(widthAtt);

    /*if(cellSizeAtt && pou::Parser::isFloat(cellSizeAtt))
        m_cellSize = pou::Parser::parseFloat(cellSizeAtt);*/
}

void TerrainGenerator_PathGraph::setGroundLayer(const TerrainGenerator_GroundLayer* groundLayer)
{
    m_groundLayer = groundLayer;
}

///
///Protected
///

std::vector<glm::vec2> TerrainGenerator_PathGraph::rasterizesEdge(const std::pair<glm::vec2, glm::vec2> &edge, TerrainGenerator *terrain)
{
    auto startWorldPos = edge.first; //m_nodes[edge.first];
    auto endWorldPos = edge.second; //m_nodes[edge.second];
    auto deltaWorldPos = endWorldPos - startWorldPos;

    //auto cellSize = terrain->getTileSize();

    std::vector<glm::vec2> rasterizedPath;

    ///DO ACTUAL RASTERIZATION PLEASE POU
    for(auto i = 0 ; i < 1000 ; ++i)
    {
        auto worldPos = startWorldPos + deltaWorldPos * (float)i/1000.0f;
        auto cellPos = terrain->worldToGridPosition(worldPos);
        rasterizedPath.push_back(cellPos);
    }

    return rasterizedPath;
}





