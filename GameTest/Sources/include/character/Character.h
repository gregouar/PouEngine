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
#include "PouEngine/utils/Stream.h"
#include "assets/CharacterModelAsset.h"

struct CharacterAttributes
{
    bool operator==(const CharacterAttributes& rhs)
    {
        if(life != rhs.life) return (false);
        if(walkingSpeed != rhs.walkingSpeed) return (false);
        return (true);
    }


    float life;
    float walkingSpeed;
};

class Character : public pou::SceneNode
{
    //friend class CharacterModelAsset;

    public:
        Character();
        virtual ~Character();

        virtual bool loadModel(const std::string &path);
        virtual bool setModel(CharacterModelAsset *model);
        virtual bool addSkeleton(std::unique_ptr<pou::Skeleton> skeleton, const std::string &name);

        //virtual bool addLimb(std::unique_ptr<pou::SpriteEntity> limb);
        virtual pou::SceneEntity *addLimb(LimbModel *limbModel); //return nullptr if error
        virtual pou::SoundObject *addSound(SoundModel *soundModel); //return nullptr if error

        virtual bool addLimbToSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool removeLimbFromSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool addSoundToSkeleton(SoundModel *soundModel, const std::string &skeleton);
        virtual bool removeSoundFromSkeleton(SoundModel *soundModel, const std::string &skeleton);
        //virtual bool removeLimbFromSkeleton(pou::SpriteEntity limb*, const std::string &skeleton);

        virtual void setWalkingSpeed(float speed);
        void setRotationRadius(float radius);

        void disableAutoLookingDirection(bool disable = true);
        void setLookingDirection(glm::vec2 direction);
        void setDestination(glm::vec2 destination);
        virtual void walk(glm::vec2 direction);
        virtual bool attack(glm::vec2 direction = glm::vec2(0), const std::string &animationName = "attack");
        virtual bool stopAttacking();

        virtual bool damage(float damages, glm::vec2 direction = glm::vec2(0));
        virtual bool interrupt(float amount);
        virtual bool resurrect();

        void startAnimation(const std::string &name, bool forceStart = false);

        void addToNearbyCharacters(Character *character);

        virtual void update(const pou::Time &elapsedTime, float localTime = -1);

        bool isAlive() const;

        virtual const std::list<Hitbox> *getHitboxes() const;
        virtual const std::list<Hitbox> *getHurtboxes() const;

        CharacterModelAsset *getModel() const;

        const CharacterAttributes &getAttributes() const;
        const CharacterModelAttributes &getModelAttributes() const;

        void serializeCharacter(pou::Stream *stream, float clientTime = -1);
        bool syncFromCharacter(Character *srcCharacter);

        virtual void    setSyncAndLocalTime(float syncTime);
        void            setLastCharacterUpdateTime(float time, bool force = false);
        float           getLastModelUpdateTime();
        float           getLastCharacterUpdateTime();

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
        //glm::vec2 m_walkingDirection;
        pou::SyncedAttribute<glm::vec2> m_walkingDirection;

        bool      m_autoLookingDirection;
        glm::vec2 m_lookingDirection;

        pou::Timer m_attackDelayTimer;

        glm::vec2   m_pushVelocity;
        pou::Timer  m_pushTimer;
        pou::Timer  m_interruptTimer;

        //float m_walkingSpeed;
        pou::SyncedAttribute<CharacterModelAttributes> m_modelAttributes;
        pou::SyncedAttribute<CharacterAttributes> m_attributes;

        std::set<Character*>    m_nearbyCharacters;
        std::set<Character*>    m_alreadyHitCharacters;
        std::map<std::string, std::unique_ptr<pou::Skeleton> > m_skeletons;

        float m_lastCharacterSyncTime;
        float m_lastCharacterUpdateTime;
        float m_lastModelUpdateTime;
        //float m_lastAttributesUpdateTime;
        //float m_lastLookingDirectionUpdateTime;

    private:
        CharacterModelAsset *m_model;

        std::map<LimbModel*, std::unique_ptr<pou::SceneEntity> >    m_limbs;
        std::map<SoundModel*, std::unique_ptr<pou::SoundObject> >   m_sounds;
        //std::map<std::string, pou::SoundObject*> m_soundsMap;

        float m_rotationRadius;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        static const float DEFAULT_INTERRUPT_DELAY;
};

#endif // CHARACTER_H
