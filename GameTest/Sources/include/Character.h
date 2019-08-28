#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/Skeleton.h"

class Character : public pou::SceneNode
{
    public:
        Character();
        virtual ~Character();

        bool loadModel(const std::string &path);

        void setWalkingSpeed(float speed);
        void setRotationRadius(float radius);

        void setDestination(glm::vec2 destination);
        void walk(glm::vec2 direction);

        void startAnimation(const std::string &name, bool forceStart = false);

        virtual void update(const pou::Time &elapsedTime);

    protected:
        void cleanup();

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

    private:
        std::list<std::unique_ptr<pou::SpriteEntity> > m_limbs;
        std::list<std::unique_ptr<pou::Skeleton> > m_skeletons;

        float m_walkingSpeed;
        float m_rotationRadius;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        bool      m_isWalking;
        glm::vec2 m_lookingDirection;
        glm::vec2 m_walkingDirection;

};

#endif // CHARACTER_H
