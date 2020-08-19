#ifndef SCENEGRID_H
#define SCENEGRID_H

#include "PouEngine/scene/SceneNode.h"


namespace pou
{

struct GridProbe
{
    //std::shared_ptr<pou::SimpleNode> node;
    SceneNode *node;
    float radius;
};

class SceneGrid :  public SceneNode
{
    public:
        SceneGrid();
        virtual ~SceneGrid();

        virtual void addChildNode(std::shared_ptr<SceneNode> childNode);
        virtual bool removeChildNode(SceneNode *childNode);
        virtual bool removeChildNode(SceneNode *childNode, glm::ivec2 gridPos);
        virtual void removeAllChilds();

        //virtual bool containsChildNode(std::shared_ptr<SceneNode> childNode); //Need to implement this
        virtual bool containsChildNode(SceneNode *childNode, glm::ivec2 gridPos);

        virtual std::shared_ptr<SceneNode> extractChildNode(SceneNode *childNode);
        virtual std::shared_ptr<SceneNode> extractChildNode(SceneNode *childNode, glm::ivec2 gridPos);

        void moveChildNode(SceneNode *childNode, glm::ivec2 oldGridPos);

        void setQuadSize(float s);
        void resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize);
        void enlargeForPosition(glm::vec2 pos);

        void addUpdateProbe(SceneNode *node, float radius);
        void removeUpdateProbe(SceneNode *node);
        void setRenderProbe(SceneNode *node, float radius);

        void probesZones(std::unordered_set< std::vector<std::shared_ptr<SceneNode> > *> &zonesToUpdate, GridProbe &probe);

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        virtual void generateRenderingData(pou::SceneRenderingInstance *renderingInstance);

        glm::ivec2 getGridPos(glm::vec2 worldPos);

    protected:
        virtual void notify(pou::NotificationSender* , int notificationType,
                            void* data = nullptr) override;

        void addChildNodeToGrid(std::shared_ptr<SceneNode> childNode);


    protected:
        std::list<GridProbe> m_updateProbes;
        GridProbe m_renderProbe;

        ///I could try to replace std::vector< std::shared_ptr<pou::SimpleNode> >  by a node (representing a square on the grid)
        ///But then I need to somehow sync these nodes
        std::vector< std::vector< std::vector< std::shared_ptr<SceneNode> > > >
            m_grid;

    private:
        float m_quadSize;
        glm::ivec2 m_minPos;
        glm::ivec2 m_gridSize;

        std::list< std::pair<glm::ivec2, SceneNode*> > m_nodesToMove;
};

}


#endif // SCENEGRID_H

