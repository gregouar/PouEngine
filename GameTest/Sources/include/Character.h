#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/scene/SpriteModel.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/Skeleton.h"

enum CharacterParts
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
};

class Character : public pou::SceneNode
{
    public:
        Character();
        virtual ~Character();

        bool loadResources();

    protected:
        void cleanup();

    private:
        pou::SpriteModel    *m_partsModel[TOTAL_PARTS];
        pou::SpriteEntity   *m_partsEntity[TOTAL_PARTS];
        pou::SceneNode      *m_partsNode[TOTAL_PARTS];

        std::unique_ptr<pou::Skeleton> m_skeleton;
        //pou::Skeleton       *m_skeleton;

};

#endif // CHARACTER_H
