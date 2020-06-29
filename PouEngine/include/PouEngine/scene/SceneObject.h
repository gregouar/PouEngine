#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "PouEngine/Types.h"
#include "PouEngine/core/NotificationListener.h"

/** Any object that can be put in a scene : instance of sprite or mesh, ligth source, camera, ambient sound, ... **/

namespace pou
{

class SceneNode;

class SceneObject : public NotificationListener
{
    friend class SceneNode;

    public:
        SceneObject();
        virtual ~SceneObject();

        SceneNode *getParentNode();

        bool isALight();
        bool isAnEntity();
        bool isAShadowCaster();

        //void setLocalTime(uint32_t time);
        uint32_t getLastUpdateTime();
        uint32_t getLastNodeUpdateTime();

        virtual void update(const Time &elapsedTime, uint32_t localTime = -1);
        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

    protected:
        void setLastUpdateTime(uint32_t time, bool force = false);
        SceneNode *setParentNode(SceneNode*);

    protected:
        SceneNode *m_parentNode;

        bool m_isALight;
        bool m_isAnEntity;
        bool m_isAShadowCaster;

        uint32_t m_curLocalTime;
        uint32_t m_lastSyncTime;
        uint32_t m_lastUpdateTime;
        uint32_t m_lastNodeUpdateTime;

    private:
};

}

#endif // SCENEOBJECT_H
