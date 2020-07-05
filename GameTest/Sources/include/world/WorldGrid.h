#ifndef WORLDGRID_H
#define WORLDGRID_H

//#include "PouEngine/scene/SceneGrid.h"
#include "world/WorldNode.h"

struct GridProbe
{
    //std::shared_ptr<pou::SimpleNode> node;
    pou::SimpleNode *node;
    float radius;
};

class WorldGrid : /**public pou::SceneGrid,**/ public WorldNode
{
    public:
        WorldGrid();
        virtual ~WorldGrid();

        virtual void addChildNode(std::shared_ptr<pou::SimpleNode> childNode);
        virtual bool removeChildNode(pou::SimpleNode *childNode);
        virtual bool removeChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos);
        virtual void removeAllChilds();

        //virtual bool containsChildNode(std::shared_ptr<SimpleNode> childNode); //Need to implement this
        virtual bool containsChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos);

        virtual std::shared_ptr<pou::SimpleNode> extractChildNode(pou::SimpleNode *childNode);
        virtual std::shared_ptr<pou::SimpleNode> extractChildNode(pou::SimpleNode *childNode, glm::ivec2 gridPos);

        void setQuadSize(float s);
        void resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize);
        void enlargeForPosition(glm::vec2 pos);

        //void addUpdateProbe(std::shared_ptr<pou::SimpleNode> node, float radius);
        void addUpdateProbe(pou::SimpleNode *node, float radius);
        void removeUpdateProbe(pou::SimpleNode *node);
        //void setRenderProbe(std::shared_ptr<pou::SimpleNode> node, float radius);
        void setRenderProbe(pou::SimpleNode *node, float radius);

        void probesZones(std::set< std::vector<std::shared_ptr<pou::SimpleNode> > *> &zonesToUpdate, GridProbe &probe);
        //void probesZones(std::set< std::shared_ptr<SimpleNode> > &nodesToUpdate, GridProbe &probe);

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        virtual void generateRenderingData(pou::SceneRenderingInstance *renderingInstance, bool propagateToChilds = true);

        glm::ivec2 getGridPos(glm::vec2 worldPos);

    protected:
        virtual void notify(pou::NotificationSender* , int notificationType,
                            void* data = nullptr) override;


    protected:
        std::list<GridProbe> m_updateProbes;
        GridProbe m_renderProbe;

        ///I could try to replace std::vector< std::shared_ptr<pou::SimpleNode> >  by a node (representing a square on the grid)
        ///But then I need to somehow sync these nodes
        std::vector< std::vector< std::vector< std::shared_ptr<pou::SimpleNode> > > >
            m_grid;

        //std::set<pou::SimpleNode*> m_needToUpdateNodes;

    private:
        float m_quadSize;
        glm::ivec2 m_minPos;
        glm::ivec2 m_gridSize;

        std::list< std::pair<glm::ivec2, pou::SimpleNode*> > m_nodesToMove;
};


#endif // WORLDGRID_H
