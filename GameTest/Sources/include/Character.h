#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/SoundObject.h"
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
        virtual bool addSkeleton(std::unique_ptr<pou::Skeleton> skeleton, const std::string &name);

        //virtual bool addLimb(std::unique_ptr<pou::SpriteEntity> limb);
        virtual pou::SpriteEntity *addLimb(LimbModel *limbModel); //return nullptr if error
        virtual pou::SoundObject *addSound(SoundModel *soundModel); //return nullptr if error

        virtual bool addLimbToSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool removeLimbFromSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool addSoundToSkeleton(SoundModel *soundModel, const std::string &skeleton);
        virtual bool removeSoundFromSkeleton(SoundModel *soundModel, const std::string &skeleton);
        //virtual bool removeLimbFromSkeleton(pou::SpriteEntity limb*, const std::string &skeleton);

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

        virtual const std::list<Hitbox> *getHitboxes() const;
        virtual const std::list<Hitbox> *getHurtboxes() const;

        const CharacterModelAsset *getModel() const;

    protected:
        void cleanup();

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

        virtual void updateWalking(const pou::Time &elapsedTime);
        virtual void updateAttacking(const pou::Time &elapsedTime);
        virtual void updateLookingDirection(const pou::Time &elapsedTime);
        //virtual void updateSounds();

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

        std::map<LimbModel*, std::unique_ptr<pou::SpriteEntity> >   m_limbs;
        std::map<SoundModel*, std::unique_ptr<pou::SoundObject> >   m_sounds;
        //std::map<std::string, pou::SoundObject*> m_soundsMap;

        float m_rotationRadius;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        static const float DEFAULT_INTERRUPT_DELAY;
};

#endif // CHARACTER_H
