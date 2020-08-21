#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_set>
#include <vector>

namespace pou
{

struct UnorientedGraphEdge
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

        void addEdge(const UnorientedGraphEdge &edge);
        void addNode(size_t nodeId);

        bool mergeWith(const UnorientedGraph *otherGraph);

        UnorientedGraph computeMinimalSpanningTree(); ///Kruskal's algorithm

        size_t getNbrNodes();
        size_t getNbrEdges();

        const std::unordered_set<size_t> &getNodes();
        const std::vector<UnorientedGraphEdge> &getEdges();

    protected:

    private:
        std::unordered_set<size_t> m_nodes;
        std::vector<UnorientedGraphEdge> m_edges;
};

}


#endif // GRAPH_H
