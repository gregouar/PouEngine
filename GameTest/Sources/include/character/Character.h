#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/SoundObject.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/utils/SyncedTimer.h"
#include "PouEngine/utils/Stream.h"

#include "assets/CharacterModelAsset.h"
#include "character/CharacterState.h"

struct CharacterAttributes
{
    CharacterAttributes() : life(0), walkingSpeed(0){}

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
    friend class CharacterModelAsset;
    friend class CharacterState;

    public:
        Character();
        Character(std::shared_ptr<CharacterInput> characterInput);
        virtual ~Character();

        virtual bool loadModel(const std::string &path);
        virtual bool setModel(CharacterModelAsset *model);

        virtual pou::SceneEntity *addLimb(LimbModel *limbModel); //return nullptr if error
        virtual pou::SoundObject *addSound(SoundModel *soundModel); //return nullptr if error

        virtual bool addLimbToSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool removeLimbFromSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool addSoundToSkeleton(SoundModel *soundModel, const std::string &skeleton);
        virtual bool removeSoundFromSkeleton(SoundModel *soundModel, const std::string &skeleton);

        void setTeam(int team);

        virtual bool damage(float damages, glm::vec2 direction = glm::vec2(0));
        virtual void interrupt(float amount = 0);
        virtual bool kill(float amount = 0);
        virtual bool resurrect();

        void startAnimation(const std::string &name, bool forceStart = true);

        void addToNearbyCharacters(Character *character);
        void removeFromNearbyCharacters(Character *character);
        std::set<Character*> &getNearbyCharacters();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        virtual void rewind(uint32_t time);

        bool isAlive() const;

        virtual const std::list<Hitbox> *getHitboxes() const;
        virtual const std::list<Hitbox> *getHurtboxes() const;

        CharacterModelAsset *getModel() const;
        pou::Skeleton       *getSkeleton(const std::string &skeletonName);

        const CharacterAttributes       &getAttributes() const;
        const CharacterModelAttributes  &getModelAttributes() const;
        int getTeam() const;

        void serializeCharacter(pou::Stream *stream, uint32_t clientTime = -1);
        bool syncFromCharacter(Character *srcCharacter);

        virtual void    setSyncDelay(uint32_t delay);
        uint32_t        getLastModelUpdateTime(bool useSyncDelay = true);
        uint32_t        getLastCharacterUpdateTime(bool useSyncDelay = true);

        void disableDeath(bool disable = true); //Prevent to kill character before server approval
        void disableInputSync(bool disable = true); //<= maybe I should use some kind of inputComponent or syncComponent ?

    protected:
        void cleanup();

        virtual bool addSkeleton(std::unique_ptr<pou::Skeleton> skeleton, const std::string &name);

        virtual void switchState(CharacterStateTypes stateType);

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

        void  setLastCharacterUpdateTime(uint32_t time, bool force = false);

    protected:
        std::shared_ptr<CharacterInput> m_input;

        pou::SyncedAttribute<bool> m_isDead;

        pou::SyncedAttribute<CharacterModelAttributes>  m_modelAttributes;
        pou::SyncedAttribute<CharacterAttributes>       m_attributes;

        std::set<Character*>    m_nearbyCharacters;
        std::map<std::string, std::unique_ptr<pou::Skeleton> > m_skeletons;

        uint32_t m_lastCharacterSyncTime;
        uint32_t m_lastCharacterUpdateTime;
        uint32_t m_lastModelUpdateTime;

        bool m_disableDeath;
        bool m_disableInputSync;

        int m_team;

    private:
        CharacterModelAsset*    m_model;
        std::unique_ptr<CharacterState> m_states[NBR_CharacterStateTypes];
        CharacterState*         m_curState;

        std::map<LimbModel*, std::unique_ptr<pou::SceneEntity> >    m_limbs;
        std::map<SoundModel*, std::unique_ptr<pou::SoundObject> >   m_sounds;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        pou::SyncedAttribute<std::string> m_curAnimation; ///I should find a better way to manage animations (like id, but then I need to list all possible animations in the XML beh)

    public:
};

#endif // CHARACTER_H
