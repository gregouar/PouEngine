#include "PouEngine/math/Graph.h"

#include <list>
#include <map>
#include <iostream>

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

void UnorientedGraph::addEdge(const GraphEdge &edge)
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

UnorientedGraph UnorientedGraph::computeMinimalSpanningTree(float extraEdgeFactor)
{
    std::list<UnorientedGraph> forest;
    std::map<size_t, UnorientedGraph*> nodeToTreeMap;
    for(auto node : m_nodes)
    {
        forest.emplace_back();
        forest.back().addNode(node);
        nodeToTreeMap.insert({node, &forest.back()});
    }

    std::multimap<float, GraphEdge> remainingEdges;
    for(auto edge : m_edges)
        remainingEdges.insert({edge.weight, edge});

    UnorientedGraph resultingGraph;

    while(!remainingEdges.empty())
    {
        auto curEdgeIt = remainingEdges.extract(remainingEdges.begin());
        auto curEdge = curEdgeIt.mapped();

        auto tree1 = nodeToTreeMap[curEdge.startNode];
        auto tree2 = nodeToTreeMap[curEdge.endNode];

        bool addEdge = false;

        if(tree1->mergeWith(tree2))
            addEdge = true;
        else if(extraEdgeFactor > 0)
        {
            auto minimalPathLength = tree1->computeMinimalDistance(curEdge.startNode, curEdge.endNode);
            if(minimalPathLength > curEdge.weight * extraEdgeFactor)
                addEdge = true;
        }

        if(addEdge)
        {
            for(auto &nodeToTreeMapIt : nodeToTreeMap)
                if(nodeToTreeMapIt.second == tree2)
                nodeToTreeMapIt.second = tree1;
            tree1->addEdge(curEdge);
        }

        if(extraEdgeFactor == 0 && tree1->getNbrNodes() == this->getNbrNodes())
        {
            std::swap(resultingGraph, *tree1);
            break;
        }

        if(remainingEdges.empty())
        {
            std::swap(resultingGraph, *tree1);
            break;
        }
    }

    return resultingGraph;
}

/**struct DijkstraNode
{
    DijkstraNode() : parentNode(0), curWeight(std::numeric_limits<float>::max()){}

    size_t parentNode;
    float curWeight;
};**/

float UnorientedGraph::computeMinimalDistance(size_t startNode, size_t endNode)
{
    ///std::unordered_map<size_t, DijkstraNode> djikstraNodes;
    //std::unordered_set<size_t> unvisited;
    std::multimap<float, size_t> unvisited;

    for(auto node : m_nodes)
    {
        ///DijkstraNode newNode;

        float curWeight = std::numeric_limits<float>::max();

        if(node == startNode)
            curWeight = 0;
            ///newNode.curWeight = 0;

        ///djikstraNodes.insert({node, newNode});
        unvisited.insert({curWeight, node});
    }

    while(!unvisited.empty())
    {
        auto curNodeIt =  unvisited.extract(unvisited.begin());
        auto curWeight = curNodeIt.key();
        auto curNode = curNodeIt.mapped();
       // auto [curWeight, curNode] = unvisited.extract(unvisited.begin());

        if(curNode == endNode)
            return curWeight;

        auto neighborEdges = this->getAdjacentEdges(curNode);
        for(auto edge : neighborEdges)
        {
            auto foundNodeIt = unvisited.end();

            for(auto it = unvisited.begin() ; it != unvisited.end() ; ++it)
                if(it->second == edge.endNode)
                    foundNodeIt = it;

            if(foundNodeIt == unvisited.end())
                continue;

            auto destNode = foundNodeIt->second;

            float newWeight = curWeight + edge.weight;
            if(newWeight < foundNodeIt->first)
            {
                ///auto &destDjikstraNode = djikstraNodes[destNode];
                ///destDjikstraNode.parentNode = curNode; ///ACTUALLY DONT CARE SINCE WE ONLY WANT WEIGHT HAHAHAHAH
                unvisited.erase(foundNodeIt);
                unvisited.insert({newWeight, destNode});
            }
        }
    }

    return -1;
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

const std::vector<GraphEdge> &UnorientedGraph::getEdges()
{
    return m_edges;
}

std::vector<size_t> UnorientedGraph::getAdjacentNodes(size_t node)
{
    std::vector<size_t> adjNodes;

    for(auto edge : m_edges)
    {
        if(edge.startNode == node)
            adjNodes.push_back(edge.endNode);
        if(edge.endNode == node)
            adjNodes.push_back(edge.startNode);
    }

    return adjNodes;
}

std::vector<GraphEdge> UnorientedGraph::getAdjacentEdges(size_t node)
{
    std::vector<GraphEdge> adjEdges;

    for(auto edge : m_edges)
    {
        if(edge.startNode == node)
        {
            GraphEdge newEdge;
            newEdge.startNode = node;
            newEdge.endNode = edge.endNode;
            newEdge.weight = edge.weight;
            adjEdges.push_back(newEdge);
        }
        else if(edge.endNode == node)
        {
            GraphEdge newEdge;
            newEdge.startNode = node;
            newEdge.endNode = edge.startNode;
            newEdge.weight = edge.weight;
            adjEdges.push_back(newEdge);
        }
    }

    return adjEdges;
}

}
