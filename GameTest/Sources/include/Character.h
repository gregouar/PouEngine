#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/Types.h"
#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/Skeleton.h"

/*enum CharacterParts
{
    BODY_PART,
    HEAD_PART,
    BAG_PART,
    SHOULDER_L_PART,
    SHOULDER_R_PART,
    HAND_L_PART,
    HAND_R_PART,
    FOOT_L_PART,
    FOOT_R_PART,
    WEAPON_PART,
    TOTAL_PARTS,
};*/

class Character : public pou::SceneNode
{
    public:
        Character();
        virtual ~Character();

        bool loadModel(const std::string &path);

        void walk(glm::vec2 direction);

        void startAnimation(const std::string &name, bool forceStart = false);

        virtual void update(const pou::Time &elapsedTime);

    protected:
        void cleanup();

    private:
        /*pou::SpriteEntity   *m_partsEntity[TOTAL_PARTS];
        std::unique_ptr<pou::Skeleton> m_skeleton;*/

        std::list<std::unique_ptr<pou::SpriteEntity> > m_limbs;
        std::list<std::unique_ptr<pou::Skeleton> > m_skeletons;

        glm::vec2 m_walkingDirection;
        bool m_isWalking;

};

#endif // CHARACTER_H
