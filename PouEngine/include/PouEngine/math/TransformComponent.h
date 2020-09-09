#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>

#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"

namespace pou
{

class TransformComponent : public NotificationSender, public NotificationListener
{
    public:
        TransformComponent();
        virtual ~TransformComponent();

        void update(const Time &elapsedTime = Time(0)/*, uint32_t localTime = -1*/);

        void copyFrom(const TransformComponent *srcTransform);

        //void combineTransform(TransformComponent *localTransform);
        glm::vec4 apply(const glm::vec4 &srcPoint);
        glm::vec3 apply(const glm::vec3 &srcPoint);
        //void loadFromModelMatrix(const glm::mat4 &model);

        void setParent(TransformComponent *parent);

        void move(float, float);
        void move(float, float, float);
        void move(const glm::vec2 &);
        void move(const glm::vec3 &);

        void globalMove(float, float);
        void globalMove(float, float, float);
        void globalMove(const glm::vec2 &);
        void globalMove(const glm::vec3 &);

        void setPosition(float, float);
        void setPosition(float, float, float);
        void setPosition(const glm::vec2 &);
        void setPosition(const glm::vec3 &);

        void setGlobalPosition(float, float);
        void setGlobalPosition(float, float, float);
        void setGlobalPosition(const glm::vec2 &);
        void setGlobalPosition(const glm::vec3 &);

        void setRigidity(float rigidity);

        void scale(float scale);
        void scale(const glm::vec3 &scale);
        void linearScale(float, float, float);
        void linearScale(const glm::vec3 &scale);
        void setScale(float scale);
        void setScale(const glm::vec3 &scale);
        void rotateInRadians(float value, const glm::vec3 &axis);
        void rotateInDegrees(float value, const glm::vec3 &axis);
        void rotateInRadians(const glm::vec3 &values);
        void rotateInDegrees(const glm::vec3 &values);
        void setRotationInRadians(const glm::vec3 &rotation);
        void setRotationInDegrees(const glm::vec3 &rotation);

        size_t getTreeDepth() const;

        const glm::vec3 &getGlobalPosition();
        glm::vec2 getGlobalXYPosition();

        const glm::vec3 &getPosition() const;
        const glm::vec3 &getScale() const;
        const glm::vec3 &getEulerRotation()const;
        float getRigidity() const;

        const glm::mat4 &getModelMatrix();
        const glm::mat4 &getInvModelMatrix();

        //glm::vec3 getLocalXRotationVector() const;
        //static glm::vec3 convertZRotationVectorToYZRotation(const glm::vec3 &rotationVector);

    protected:
        virtual void notify(NotificationSender* , int notificationType,
                            void* data = nullptr) override;

        void askForUpdateModelMatrix();
        void updateGlobalPosition();
        void updateModelMatrix();

    private:
        TransformComponent *m_parent;
        size_t m_treeDepth;

        glm::vec3 m_lastGlobalPosition;
        glm::vec3 m_globalPosition;

        glm::vec3 m_position;
        glm::vec3 m_eulerRotations;
        glm::vec3 m_scale;

        float m_rigidity;

        float m_curFlexibleLength;
        float m_curFlexibleRotation;

        glm::mat4 m_modelMatrix;
        glm::mat4 m_invModelMatrix;

        bool m_needToUpdateModelMat;
};

}


#endif // TRANSFORMCOMPONENT_H
