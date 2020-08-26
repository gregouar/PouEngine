#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/SoundObject.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/system/Stream.h"

#include "assets/CharacterModelAsset.h"
#include "character/CharacterState.h"
#include "sync/NodeSyncer.h"

#include "PouEngine/sync/SyncElements.h"

class AiComponent;
class GameWorld_Sync;

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

class SyncCharacterAttributes  : public pou::AbstractSyncElement
{
    public:
        SyncCharacterAttributes();
        SyncCharacterAttributes(const CharacterAttributes &v);
        virtual ~SyncCharacterAttributes();

        void setValue(const CharacterAttributes &v);
        const CharacterAttributes &getValue() const;

    protected:
        virtual void serializeImpl(pou::Stream *stream, uint32_t clientTime) override;

    private:
        pou::SyncAttribute<CharacterAttributes> m_attribute;
};


class SyncCharacterModelAttributes  : public pou::AbstractSyncElement
{
    public:
        SyncCharacterModelAttributes();
        SyncCharacterModelAttributes(const CharacterModelAttributes &v);
        virtual ~SyncCharacterModelAttributes();

        void setValue(const CharacterModelAttributes &v);
        const CharacterModelAttributes &getValue() const;

    protected:
        virtual void serializeImpl(pou::Stream *stream, uint32_t clientTime) override;

    private:
        pou::SyncAttribute<CharacterModelAttributes> m_attribute;
};

class CharacterSkeleton : public pou::Skeleton
{
    public:
        CharacterSkeleton(pou::SkeletonModelAsset *model);
        virtual ~CharacterSkeleton();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        ///void setHurtColor(const glm::vec4 &hurtColor);

    protected:

    private:
        /**float m_hurtColorAmount;
        glm::vec4 m_hurtColor;
        pou::Timer m_hurtColorTimer;

    public:
        static const float DEFAULT_HURTCOLOR_DELAY;
        static const float DEFAULT_HURTCOLOR_FADEOUTSPEED;**/
};

class HurtNode
{
    public:
        HurtNode(pou::SceneNode *node);
        virtual ~HurtNode();

        bool update(const pou::Time &elapsedTime);

        void setHurtColor(const glm::vec4 &hurtColor);

    protected:

    private:
        pou::SceneNode *m_node;

        float m_hurtColorAmount;
        glm::vec4 m_hurtColor;
        pou::Timer m_hurtColorTimer;

    public:
        static const float DEFAULT_HURTCOLOR_DELAY;
        static const float DEFAULT_HURTCOLOR_FADEOUTSPEED;
};

class Character : //public pou::SceneObject, public pou::NotificationSender
    public pou::SceneNode
{
    friend class CharacterModelAsset;
    friend class CharacterState;

    public:
        Character();
        Character(std::shared_ptr<CharacterInput> characterInput);
        virtual ~Character();

        ///std::shared_ptr<pou::SceneNode> node();

        virtual bool createFromModel(const std::string &path);
        virtual bool createFromModel(CharacterModelAsset *model);

        virtual void setAiComponent(std::shared_ptr<AiComponent> aiComponent);

        virtual std::shared_ptr<pou::SceneEntity> addLimb(LimbModel *limbModel); //return nullptr if error
        virtual std::shared_ptr<pou::SoundObject> addSound(SoundModel *soundModel); //return nullptr if error

        virtual bool addLimbToSkeleton(LimbModel *limbModel, pou::HashedString skeleton);
        virtual bool removeLimbFromSkeleton(LimbModel *limbModel, pou::HashedString skeleton);
        virtual bool addSoundToSkeleton(SoundModel *soundModel, pou::HashedString skeleton);
        virtual bool removeSoundFromSkeleton(SoundModel *soundModel, pou::HashedString skeleton);

        void setSyncData(GameWorld_Sync *worldSync, int id);
        void setTeam(int team);

        virtual bool damage(float damages, glm::vec2 direction = glm::vec2(0),
                            bool onlyCosmetic = false);
        virtual void setHurtNodeColor(pou::SceneNode *hurtNode, const glm::vec4 &hurtColor);
        //virtual void setSkeletonHurtColor(pou::Skeleton *skeleton, const glm::vec4 &color); ///Maybe replace naked ptr by ID

        virtual void interrupt(float amount = 0);
        virtual bool kill(float amount = 0);
        virtual bool resurrect();

        void startAnimation(pou::HashedString name, bool forceStart = true);

        virtual void generateRenderingData(pou::SceneRenderingInstance *renderingInstance); ///For debugging purposes, remove later

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        ///virtual void rewind(uint32_t time);

        bool isAlive() const;

        virtual const std::vector<Hitbox> *getHitboxes() const;
        virtual const std::vector<Hitbox> *getHurtboxes() const;

        std::vector<Character*> *getNearbyCharacters();

        CharacterModelAsset *getModel() const;
        CharacterSkeleton   *getSkeleton(pou::HashedString skeletonName);
        CharacterInput      *getInput();
        AiComponent         *getAi();

        const CharacterAttributes       &getAttributes() const;
        const CharacterModelAttributes  &getModelAttributes() const;
        int getTeam() const;

        GameWorld_Sync* getWorldSync() const;
        uint32_t getCharacterSyncId() const;

        void serializeCharacter(pou::Stream *stream, uint32_t clientTime = -1);
        virtual void syncFromCharacter(Character *srcCharacter);

        virtual void    setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        uint32_t        getLastModelUpdateTime();
        uint32_t        getLastCharacterUpdateTime();

        std::shared_ptr<NodeSyncer> getNodeSyncer();

        ///pou::SyncComponent *getCharacterSyncComponent();

        void disableDeath(bool disable = true); //Could be used to prevent to kill character before server approval
        void disableSync(bool disable = true);
        void disableStateSync(bool disable = true);
        void disableInputSync(bool disable = true);
        void disableDamageDealing(bool disable = true); //Used to only show cosmetic effect of damages
        void disableDamageReceiving(bool disable = true);
        void disableAI(bool disable = true);

        bool areDamagesOnlyCosmetic();

    protected:
        void cleanup();

        virtual void updateSyncComponent(const pou::Time &elapsedTime, uint32_t localTime);
        virtual void updateHurtNodes(const pou::Time &elapsedTime);

        virtual bool addSkeleton(std::shared_ptr<CharacterSkeleton> skeleton, pou::HashedString name);

        virtual void switchState(CharacterStateTypes stateType);

        bool walkToDestination(const pou::Time& elapsedTime);
        void rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius);

        void  setLastCharacterUpdateTime(uint32_t time, bool force = false);

    protected:
        ///std::shared_ptr<pou::SceneNode> m_node;

        std::shared_ptr<CharacterInput> m_input;

        pou::BoolSyncElement m_isDead;

        SyncCharacterModelAttributes    m_modelAttributes;
        SyncCharacterAttributes         m_attributes;

        std::unordered_set<std::shared_ptr<Character> > m_nearbyCharacters;
        std::unordered_map<pou::HashedString, std::shared_ptr<CharacterSkeleton> > m_skeletons;

        uint32_t m_lastModelUpdateTime;

        bool m_disableDeath;
        bool m_disableDamageDealing;
        bool m_disableDamageReceiving;
        bool m_disableAI;

        int m_team;

    private:
        GameWorld_Sync *m_worldSync;
        uint32_t m_characterSyncId;

        CharacterModelAsset*    m_model;
        std::unique_ptr<CharacterState> m_states[NBR_CharacterStateTypes];
        CharacterState*         m_curState;
        pou::IntSyncElement     m_curStateId;

        std::shared_ptr<AiComponent> m_aiComponent;

        std::unordered_map<LimbModel*, std::shared_ptr<pou::SceneEntity> >    m_limbs;
        std::unordered_map<SoundModel*, std::shared_ptr<pou::SoundObject> >   m_sounds;

        std::unordered_map<pou::SceneNode*, HurtNode> m_hurtNodes;

       /* bool m_isDestinationSet;
        glm::vec2 m_destination; ///This should probably be moved somewhere in AIComponent*/

        pou::SyncComponent m_characterSyncComponent;
        //NodeSyncer *m_nodeSyncer;
        std::shared_ptr<NodeSyncer> m_nodeSyncer;

    public:
};

#endif // CHARACTER_H
