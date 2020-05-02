#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/utils/Timer.h"
#include "assets/CharacterModelAsset.h"

class Character : public pou::SceneNode
{
    //friend class CharacterModelAsset;

    public:
        Character();
        virtual ~Character();

        virtual bool loadModel(const std::string &path);
        virtual bool addLimb(std::unique_ptr<pou::SpriteEntity> limb);
        virtual bool addSkeleton(std::unique_ptr<pou::Skeleton> skeleton, const std::string &name);

        virtual void setWalkingSpeed(float speed);
        void setRotationRadius(float radius);

        void setDestination(glm::vec2 destination);
        void walk(glm::vec2 direction);
        virtual bool attack(glm::vec2 direction = glm::vec2(0), const std::string &animationName = "attack");
        virtual bool stopAttacking();

        virtual bool damage(float damages, glm::vec2 direction = glm::vec2(0));
        virtual bool interrupt(float amount);
        virtual bool resurrect();

        void startAnimation(const std::string &name, bool forceStart = false);

        void addToNearbyCharacters(Character *character);

        virtual void update(const pou::Time &elapsedTime);

        bool isAlive() const;

        const std::list<Hitbox> *getHitboxes() const;
        const std::list<Hitbox> *getHurtboxes() const;

    protected:
        void cleanup();

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

        virtual void updateWalking(const pou::Time &elapsedTime);
        virtual void updateAttacking(const pou::Time &elapsedTime);
        virtual void updateLookingDirection(const pou::Time &elapsedTime);

        float computeWantedRotation(float startingRotation, glm::vec2 position);


    protected:
        bool      m_isDead;
        bool      m_isWalking;
        bool      m_isAttacking;
        glm::vec2 m_walkingDirection;
        glm::vec2 m_lookingDirection;

        pou::Timer m_attackDelayTimer;

        glm::vec2   m_pushVelocity;
        pou::Timer  m_pushTimer;
        pou::Timer  m_interruptTimer;

        //float m_walkingSpeed;
        CharacterAttributes m_attributes;

        std::list<Character*>   m_nearbyCharacters;
        std::set<Character*>    m_alreadyHitCharacters;
        std::map<std::string, std::unique_ptr<pou::Skeleton> > m_skeletons;

    private:
        CharacterModelAsset *m_model;

        std::list<std::unique_ptr<pou::SpriteEntity> > m_limbs;

        float m_rotationRadius;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        static const float DEFAULT_INTERRUPT_DELAY;
};

#endif // CHARACTER_H
