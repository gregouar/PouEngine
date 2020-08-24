#include "generators/TerrainGenerator_PathGraph.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/math/Graph.h"
#include "PouEngine/math/AstarGrid.h"

#include "generators/TerrainGenerator.h"

TerrainGenerator_PathGraph_Noise::TerrainGenerator_PathGraph_Noise() :
    type(TerrainGenerator_PathGraph_Noise_None),
    intensity(1.0f),
    kernel(1)
{
}

TerrainGenerator_PathGraph::TerrainGenerator_PathGraph() :
    m_groundLayer(nullptr)
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

void TerrainGenerator_PathGraph::generatesEdges(TerrainGenerator *terrain, pou::RNGenerator *rng)
{
    pou::UnorientedGraph fullGraph;

    for(size_t i = 0 ; i < m_nodes.size() ; ++i)
    {
        fullGraph.addNode(i);
        for(size_t j = 0 ; j < m_nodes.size() ; ++j)
        if(i != j)
        {
            auto weight = glm::length(m_nodes[j] - m_nodes[i]);
            fullGraph.addEdge({i,j,weight});
        }
    }

    auto spanningTree = fullGraph.computeMinimalSpanningTree(m_parameters.cycleFactor);

    for(auto edge : spanningTree.getEdges())
        m_edges.push_back({m_nodes[edge.startNode],
                           m_nodes[edge.endNode]});


    std::cout<<"Generate path noise..."<<std::endl;

    auto gridSize = terrain->getGridSize();

    m_weightGrid = std::vector<float>(gridSize.x * gridSize.y, 0);

    for(auto &noise : m_noises)
    {
        if(noise.type == TerrainGenerator_PathGraph_Noise_Perlin)
            pou::MathTools::generatePerlinNoise(m_weightGrid, gridSize, noise.kernel, noise.intensity*1000.0f, rng);
    }

    m_unreachableGrid = std::vector<bool>(gridSize.x * gridSize.y);

    for(int y = 0 ; y < gridSize.y ; ++y)
    {
    for(int x = 0 ; x < gridSize.x ; ++x)
    {
        m_unreachableGrid[y * gridSize.x + x] = !terrain->containsNoPathOrPath(x,y,m_pathName);
        //std::cout<<m_unreachableGrid[y * gridSize.x + x];
    }
        //std::cout<<std::endl;
    }
}

void TerrainGenerator_PathGraph::generatesOnTerrain(TerrainGenerator *terrain, pou::RNGenerator *rng)
{
    std::cout<<"Computing paths..."<<std::endl;

    auto width = m_parameters.width;

    std::vector<glm::ivec2> samples;
    samples.reserve(width*width);
    //samples.push_back({0,0});

    auto cellShift = glm::ivec2(width/2);
    for(int y = 0 ; y < width ; ++y)
    for(int x = 0 ; x < width ; ++x)
    {
        samples.push_back(glm::ivec2(x,y) - cellShift);
    }

    for(auto edge : m_edges)
    {
        auto rasterizedPath = this->rasterizesEdge(edge, terrain, rng);
        for(auto cell : rasterizedPath)
        {
            for(auto sample : samples)
                terrain->setGroundLayer(cell.x + sample.x, cell.y + sample.y, m_groundLayer);
        }
        m_rasterizedEdges.push_back(std::move(rasterizedPath));
    }
}

void TerrainGenerator_PathGraph::loadParameters(TiXmlElement *pathElement, pou::HashedString pathName)
{
    m_pathName = pathName;

    auto widthAtt = pathElement->Attribute("width");
    auto cycleFactorAtt = pathElement->Attribute("cycleFactor");
    //auto cellSizeAtt = pathElement->Attribute("cellSize");

    if(widthAtt && pou::Parser::isInt(widthAtt))
        m_parameters.width = pou::Parser::parseInt(widthAtt);

    if(cycleFactorAtt && pou::Parser::isFloat(cycleFactorAtt))
        m_parameters.cycleFactor = pou::Parser::parseFloat(cycleFactorAtt);


    auto noiseChild = pathElement->FirstChildElement("noise");
    while(noiseChild != nullptr)
    {
        auto noiseElement = noiseChild->ToElement();
        if(noiseElement)
            this->loadNoise(noiseElement);
        noiseChild = noiseChild->NextSiblingElement("noise");
    }

    /*if(cellSizeAtt && pou::Parser::isFloat(cellSizeAtt))
        m_cellSize = pou::Parser::parseFloat(cellSizeAtt);*/
}

void TerrainGenerator_PathGraph::setGroundLayer(const TerrainGenerator_GroundLayer* groundLayer)
{
    m_groundLayer = groundLayer;
}


const std::list<std::vector<glm::ivec2> > *TerrainGenerator_PathGraph::getAllRasterizedPaths()
{
    return &m_rasterizedEdges;
}

///
///Protected
///

std::vector<glm::ivec2> TerrainGenerator_PathGraph::rasterizesEdge(const std::pair<glm::vec2, glm::vec2> &edge, TerrainGenerator *terrain, pou::RNGenerator *rng)
{
    auto startWorldPos = edge.first; //m_nodes[edge.first];
    auto endWorldPos = edge.second; //m_nodes[edge.second];
    //auto deltaWorldPos = endWorldPos - startWorldPos;

    auto startGridPos = terrain->worldToGridPosition(startWorldPos);
    auto endGridPos = terrain->worldToGridPosition(endWorldPos);

    pou::AstarGrid astar;
    astar.setGridSize(terrain->getGridSize());
    astar.setWeightGrid(&m_weightGrid);
    astar.setUnreachableGrid(&m_unreachableGrid);

    std::vector<glm::ivec2> rasterizedPath;

    rasterizedPath = astar.lookForPath(startGridPos, endGridPos, true);

    return rasterizedPath;
}


void TerrainGenerator_PathGraph::loadNoise(TiXmlElement *noiseElement)
{
    auto typeAtt = noiseElement->Attribute("type");
    if(!typeAtt)
        return;

    auto &noise = m_noises.emplace_back();

    if(std::string(typeAtt) == "perlin")
        noise.type = TerrainGenerator_PathGraph_Noise_Perlin;
    else
        noise.type = TerrainGenerator_PathGraph_Noise_None;


    auto intensityAtt = noiseElement->Attribute("intensity");
    auto kernelAtt = noiseElement->Attribute("kernel");

    if(intensityAtt && pou::Parser::isFloat(intensityAtt))
        noise.intensity = pou::Parser::parseFloat(intensityAtt);

    if(kernelAtt && pou::Parser::isInt(intensityAtt))
        noise.kernel = pou::Parser::parseInt(kernelAtt);
}



