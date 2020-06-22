#ifndef SCENEGRID_H
#define SCENEGRID_H

#include "PouEngine/scene/SceneNode.h"

namespace pou
{

struct GridProbe
{
    std::shared_ptr<SimpleNode> node;
    float radius;
};

class SceneGrid : public SceneNode
{
    public:
        SceneGrid();
        virtual ~SceneGrid();

        virtual void addChildNode(std::shared_ptr<SimpleNode> childNode);
        virtual bool removeChildNode(SimpleNode *childNode);
        virtual void removeAllChilds();

        //virtual bool containsChildNode(std::shared_ptr<SimpleNode> childNode); //Need to implement this

        void setQuadSize(float s);
        void resizeQuad(glm::ivec2 minPos, glm::ivec2 gridSize);
        void enlargeForPosition(glm::vec2 pos);

        void addUpdateProbe(std::shared_ptr<SimpleNode> node, float radius);
        void removeUpdateProbe(std::shared_ptr<SimpleNode> node);
        void setRenderProbe(std::shared_ptr<SimpleNode> node, float radius);

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);

        virtual void generateRenderingData(SceneRenderingInstance *renderingInstance, bool propagateToChilds = true);

        glm::ivec2 getGridPos(glm::vec2 worldPos);

    protected:
        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;


    protected:
        std::list<GridProbe> m_updateProbes;
        GridProbe m_renderProbe;

        std::vector< std::vector< std::vector< std::shared_ptr<SimpleNode> > > >
            m_grid;

    private:
        float m_quadSize;
        glm::ivec2 m_minPos;
        glm::ivec2 m_gridSize;
};

}

#endif // SCENEGRID_H
