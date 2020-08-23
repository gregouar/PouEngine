#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_set>
#include <vector>

namespace pou
{

struct GraphEdge
{
    size_t startNode;
    size_t endNode;
    float weight;
};

class UnorientedGraph
{
    public:
        UnorientedGraph();
        virtual ~UnorientedGraph();

        void addEdge(const GraphEdge &edge);
        void addNode(size_t nodeId);

        bool mergeWith(const UnorientedGraph *otherGraph);

        UnorientedGraph computeMinimalSpanningTree(float extraEdgeFactor = 0); ///Kruskal's algorithm

        float computeMinimalDistance(size_t startNode, size_t endNode);

        size_t getNbrNodes();
        size_t getNbrEdges();

        const std::unordered_set<size_t> &getNodes();
        const std::vector<GraphEdge> &getEdges();
        std::vector<size_t> getAdjacentNodes(size_t node);
        std::vector<GraphEdge> getAdjacentEdges(size_t node);

    protected:

    private:
        std::unordered_set<size_t> m_nodes;
        std::vector<GraphEdge> m_edges;
};

}


#endif // GRAPH_H
