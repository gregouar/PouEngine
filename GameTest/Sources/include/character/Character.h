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

class AiComponent;

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

        virtual bool createFromModel(const std::string &path);
        virtual bool createFromModel(CharacterModelAsset *model);

        virtual void setAiComponent(std::shared_ptr<AiComponent> aiComponent);

        virtual std::shared_ptr<pou::SceneEntity> addLimb(LimbModel *limbModel); //return nullptr if error
        virtual std::shared_ptr<pou::SoundObject> addSound(SoundModel *soundModel); //return nullptr if error

        virtual bool addLimbToSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool removeLimbFromSkeleton(LimbModel *limbModel, const std::string &skeleton);
        virtual bool addSoundToSkeleton(SoundModel *soundModel, const std::string &skeleton);
        virtual bool removeSoundFromSkeleton(SoundModel *soundModel, const std::string &skeleton);

        void setSyncId(int id);
        void setTeam(int team);

        virtual bool damage(float damages, glm::vec2 direction = glm::vec2(0), bool onlyCosmetic = false);
        virtual void interrupt(float amount = 0);
        virtual bool kill(float amount = 0);
        virtual bool resurrect();

        void startAnimation(const std::string &name, bool forceStart = true);

        void addToNearbyCharacters(std::shared_ptr<Character> character);
        void removeFromNearbyCharacters(std::shared_ptr<Character> character);
        std::set< std::shared_ptr<Character> > &getNearbyCharacters();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

        bool isAlive() const;

        virtual const std::list<Hitbox> *getHitboxes() const;
        virtual const std::list<Hitbox> *getHurtboxes() const;

        CharacterModelAsset *getModel() const;
        pou::Skeleton       *getSkeleton(const std::string &skeletonName);
        CharacterInput      *getInput();

        const CharacterAttributes       &getAttributes() const;
        const CharacterModelAttributes  &getModelAttributes() const;
        int getTeam() const;
        uint32_t getSyncId() const;

        void serializeCharacter(pou::Stream *stream, uint32_t clientTime = -1);
        virtual void syncFromCharacter(Character *srcCharacter);

        virtual void    setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        uint32_t        getLastModelUpdateTime();
        uint32_t        getLastCharacterUpdateTime();

        void disableDeath(bool disable = true); //Could be used to prevent to kill character before server approval
        void disableInputSync(bool disable = true);
        void disableDamageDealing(bool disable = true); //Used to only show cosmetic effect of damages

        bool areDamagesOnlyCosmetic();

    protected:
        void cleanup();

        virtual bool addSkeleton(std::shared_ptr<pou::Skeleton> skeleton, const std::string &name);

        virtual void switchState(CharacterStateTypes stateType);

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

        void  setLastCharacterUpdateTime(uint32_t time, bool force = false);

    protected:
        std::shared_ptr<CharacterInput> m_input;

        pou::SyncedAttribute<bool> m_isDead;

        pou::SyncedAttribute<CharacterModelAttributes>  m_modelAttributes;
        pou::SyncedAttribute<CharacterAttributes>       m_attributes;

        std::set<std::shared_ptr<Character> >    m_nearbyCharacters;
        std::map<std::string, std::shared_ptr<pou::Skeleton> > m_skeletons;

        uint32_t m_lastCharacterSyncTime;
        uint32_t m_lastCharacterUpdateTime;
        uint32_t m_lastModelUpdateTime;

        bool m_disableDeath;
        bool m_disableInputSync;
        bool m_disableDamageDealing;

        int m_team;

    private:
        uint32_t m_syncId;
        CharacterModelAsset*    m_model;
        std::unique_ptr<CharacterState> m_states[NBR_CharacterStateTypes];
        CharacterState*         m_curState;

        std::shared_ptr<AiComponent> m_aiComponent;

        std::map<LimbModel*, std::shared_ptr<pou::SceneEntity> >    m_limbs;
        std::map<SoundModel*, std::shared_ptr<pou::SoundObject> >   m_sounds;

        bool m_isDestinationSet;
        glm::vec2 m_destination;

        pou::SyncedAttribute<std::string> m_curAnimation; ///I should find a better way to manage animations (like id, but then I need to list all possible animations in the XML beh)

    public:
};

#endif // CHARACTER_H
