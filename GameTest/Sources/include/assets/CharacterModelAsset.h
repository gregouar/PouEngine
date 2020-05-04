#ifndef CHARACTERMODELASSET_H
#define CHARACTERMODELASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonmodelAsset.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/scene/SpriteEntity.h"

#include "tinyxml/tinyxml.h"

#include "Types.h"
#include "Hitbox.h"

#include <vector>
#include <list>
#include <map>

class Character;

struct LimbModel
{
    std::string node;
    pou::SpriteModel* spriteModel;
};

struct SoundModel
{
    SoundModel() : isEvent(true){}

    std::string tag;
    std::string soundName;
    bool isEvent;
};

struct SkeletonWithLimbs
{
    pou::SkeletonModelAsset* skeleton;
    std::list<LimbModel> limbs;
    std::list<SoundModel> sounds;
};

struct CharacterAttributes
{
    CharacterAttributes() : walkingSpeed(0), attackDelay(0),
        life(0), attackDamages(0), immovable(0)
        {}
    float walkingSpeed;
    float attackDelay;
    float maxLife;
    float life;
    float attackDamages;
    bool  immovable;
};


class CharacterModelAsset : public pou::Asset
{
    public:
        CharacterModelAsset();
        CharacterModelAsset(const pou::AssetTypeId);
        virtual ~CharacterModelAsset();

        bool loadFromFile(const std::string &filePath);

        /*bool generateOnNode(pou::SceneNode *parentNode,
                            std::list<std::unique_ptr<pou::Skeleton> > *skeletons,
                            std::list<std::unique_ptr<pou::SpriteEntity> > *limbs);*/

        bool generateCharacter(Character *targetCharacter);

        const CharacterAttributes &getAttributes() const;
        const std::list<Hitbox> *getHitboxes() const;
        const std::list<Hitbox> *getHurtboxes() const;

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadSoundBank(TiXmlElement *element);
        bool loadSkeleton(TiXmlElement *element);
        bool loadHitboxes(TiXmlElement *element, std::list<Hitbox> &boxList);
        bool loadAttributes(TiXmlElement *element);

    private:
        std::map<std::string, pou::SpriteSheetAsset*> m_spriteSheets;
        std::map<std::string, SkeletonWithLimbs> m_skeletonModels;

        std::list<Hitbox> m_hitboxes, m_hurtboxes;

        CharacterAttributes m_attributes;
};


#endif // CHARACTERMODELASSET_H
