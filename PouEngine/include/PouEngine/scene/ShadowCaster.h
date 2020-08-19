#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

#include "PouEngine/scene/SceneEntity.h"

namespace pou
{

class LightEntity;
class SceneRenderer;

class ShadowCaster : public SceneEntity
{
    public:
        ShadowCaster();
        virtual ~ShadowCaster();

        //void addLightSource();

        void setShadowCastingType(ShadowCastingType type);

        ShadowCastingType getShadowCastingType();

        virtual glm::vec2 castShadow(SceneRenderer *renderer, LightEntity* light) = 0;

        //virtual void notify(NotificationSender*, NotificationType);

    protected:

    protected:
        ShadowCastingType m_shadowCastingType;

       // std::unordered_set<Light*> m_lightSources;
};

}

#endif // SHADOWCASTER_H
