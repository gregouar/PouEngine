#ifndef CAMERAOBJECT_H
#define CAMERAOBJECT_H

#include "PouEngine/scene/SceneObject.h"
#include "PouEngine/audio/AudioEngine.h"

namespace pou
{

///Add viewport, etc

class CameraObject : public SceneObject
{
    public:
        CameraObject();
        virtual ~CameraObject();

        void setViewport(glm::vec2 offset, glm::vec2 extent); //Normalized viewport

        void setListening(bool listening = true);

        void setZoom(float zoom);
        void zoom(float zoomFactor);

        float getZoom();
        glm::vec2 getViewportOffset();
        glm::vec2 getViewportExtent();

        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;
    protected:
        void updateListeningPosition();

    protected:
        bool        m_isListening;
        SoundTypeId m_3DListener;


        float m_zoom;
        glm::vec2 m_offset;
        glm::vec2 m_extent;

    private:

};

}

#endif // CAMERAOBJECT_H
