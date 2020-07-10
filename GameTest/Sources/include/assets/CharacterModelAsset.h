#ifndef CHARACTERMODELASSET_H
#define CHARACTERMODELASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonmodelAsset.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/physics/PhysicsEngine.h"

#include "tinyxml/tinyxml.h"

#include "Types.h"
#include "character/Hitbox.h"

#include <vector>
#include <list>
#include <map>

class Character;

struct LimbModel
{
    std::string node;
    std::string state;
    pou::SpriteModel    *spriteModel;
    pou::MeshAsset      *mesh;
    const pou::LightModel *lightModel;
};

struct SoundModel
{
    SoundModel() : isEvent(true){}

    std::string name;
    std::string path;
    bool isEvent;
};

class AssetsForSkeletonModel
{
    public:
        AssetsForSkeletonModel(const std::map<std::string, pou::SpriteSheetAsset*> *spriteSheets,
                               const std::map<std::string, pou::LightModel> *lightModels,
                               const std::string &fileDirectory);

        bool loadFromXML(TiXmlElement *element);

        std::list<LimbModel> *getLimbs();
        std::list<SoundModel> *getSounds();

    protected:
        std::list<LimbModel>  m_limbs;
        std::list<SoundModel> m_sounds;

        const std::map<std::string, pou::SpriteSheetAsset*> *m_spriteSheets;
        const std::map<std::string, pou::LightModel> *m_lightModels;
        const std::string &m_fileDirectory;
};

struct SkeletonModelWithAssets
{
    pou::SkeletonModelAsset* skeleton;
    AssetsForSkeletonModel   assetsModel;
};

struct CharacterModelAttributes
{
    CharacterModelAttributes() : walkingSpeed(0), attackDelay(0),
         maxLife(0), attackDamages(0), immovable(0), rotationRadius(0)
        {}

    bool operator==(const CharacterModelAttributes& rhs)
    {
        if(walkingSpeed != rhs.walkingSpeed) return (false);
        if(attackDelay != rhs.attackDelay) return (false);
        if(maxLife != rhs.maxLife) return (false);
        if(attackDamages != rhs.attackDamages) return (false);
        if(immovable != rhs.immovable) return (false);
        if(rotationRadius != rhs.rotationRadius) return (false);
        return (true);
    }

    float walkingSpeed;
    float attackDelay;
    float maxLife;
    float attackDamages;
    bool  immovable;
    float rotationRadius;
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

        const CharacterModelAttributes &getAttributes() const;
        const std::vector<Hitbox> *getHitboxes() const;
        const std::vector<Hitbox> *getHurtboxes() const;

        //Could remove this
        //const std::vector<pou::BoxBody> *getCollisionboxes() const;

        /** ADD GET SKELETON ID AND GET NODE ID => use them in hitbox etc **/

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadAiScript(TiXmlElement *element);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadLightModel(TiXmlElement *element);
        bool loadSoundBank(TiXmlElement *element);
        bool loadSkeleton(TiXmlElement *element);
        bool loadHitboxes(TiXmlElement *element, std::vector<Hitbox> &boxList);
        bool loadCollisionBoxes(TiXmlElement *element, std::vector<pou::BoxBody> &boxList);
        bool loadCollisionDisks(TiXmlElement *element, std::vector<pou::DiskBody> &diskList);
        bool loadAttributes(TiXmlElement *element);

    private:
        AiScriptModelAsset *m_aiScriptModel;

        std::map<std::string, pou::SpriteSheetAsset*>   m_spriteSheets;
        std::map<std::string, pou::LightModel>          m_lightModels;
        std::map<std::string, SkeletonModelWithAssets>  m_skeletonModels;

        std::vector<Hitbox> m_hitboxes, m_hurtboxes;
        std::vector<pou::BoxBody> m_collisionBoxes;
        std::vector<pou::DiskBody> m_collisionDisks;

        CharacterModelAttributes m_attributes;
};


#endif // CHARACTERMODELASSET_H
