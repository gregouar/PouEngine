#include "PouEngine/math/Graph.h"

#include <list>
#include <map>

namespace pou
{

UnorientedGraph::UnorientedGraph()
{
    //ctor
}

UnorientedGraph::~UnorientedGraph()
{
    //dtor
}

void UnorientedGraph::addEdge(const UnorientedGraphEdge &edge)
{
    m_edges.push_back(edge);
}

void UnorientedGraph::addNode(size_t nodeId)
{
    m_nodes.insert(nodeId);
}

bool UnorientedGraph::mergeWith(const UnorientedGraph *otherGraph)
{
    for(auto node : otherGraph->m_nodes)
        if(!m_nodes.insert(node).second)
            return (false);


    for(auto edge : otherGraph->m_edges)
        m_edges.push_back(edge);

    return (true);
}

UnorientedGraph UnorientedGraph::computeMinimalSpanningTree()
{
    std::list<UnorientedGraph> forest;
    std::map<size_t, UnorientedGraph*> nodeToTreeMap;
    for(auto node : m_nodes)
    {
        forest.emplace_back();
        forest.back().addNode(node);
        nodeToTreeMap.insert({node, &forest.back()});
    }

    std::multimap<float, UnorientedGraphEdge> remainingEdges;
    for(auto edge : m_edges)
        remainingEdges.insert({edge.weight, edge});

    UnorientedGraph resultingGraph;

    while(!remainingEdges.empty())
    {
        auto curEdgeIt = remainingEdges.extract(remainingEdges.begin());
        auto curEdge = curEdgeIt.mapped();

        auto tree1 = nodeToTreeMap[curEdge.startNode];
        auto tree2 = nodeToTreeMap[curEdge.endNode];

        if(tree1->mergeWith(tree2))
        {
            for(auto &nodeToTreeMapIt : nodeToTreeMap)
                if(nodeToTreeMapIt.second == tree2)
                nodeToTreeMapIt.second = tree1;
            tree1->addEdge(curEdge);
        }

        if(tree1->getNbrNodes() == this->getNbrNodes())
        {
            std::swap(resultingGraph, *tree1);
            break;
        }
    }

    return resultingGraph;
}

size_t UnorientedGraph::getNbrNodes()
{
    return m_nodes.size();
}

size_t UnorientedGraph::getNbrEdges()
{
    return m_edges.size();
}

const std::unordered_set<size_t> &UnorientedGraph::getNodes()
{
    return m_nodes;
}

const std::vector<UnorientedGraphEdge> &UnorientedGraph::getEdges()
{
    return m_edges;
}

}
