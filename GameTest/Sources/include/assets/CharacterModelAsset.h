#ifndef CHARACTERMODELASSET_H
#define CHARACTERMODELASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonmodelAsset.h"
#include "PouEngine/scene/Skeleton.h"
#include "PouEngine/scene/SpriteEntity.h"

#include "tinyxml/tinyxml.h"

#include <vector>
#include <list>
#include <map>


struct LimbModel
{
    std::string node;
    pou::SpriteModel* spriteModel;
};

struct SkeletonWithLimbs
{
    pou::SkeletonModelAsset* skeleton;
    std::list<LimbModel> limbs;
};

struct CharacterAttributes
{
    CharacterAttributes() : walkingSpeed(0), attackDelay(0) {}
    float walkingSpeed;
    float attackDelay;
};

class CharacterModelAsset : public pou::Asset
{
    public:
        CharacterModelAsset();
        CharacterModelAsset(const pou::AssetTypeId);
        virtual ~CharacterModelAsset();

        bool loadFromFile(const std::string &filePath);

        bool generateOnNode(pou::SceneNode *parentNode,
                            std::list<std::unique_ptr<pou::Skeleton> > *skeletons,
                            std::list<std::unique_ptr<pou::SpriteEntity> > *limbs);

        const CharacterAttributes &getAttributes() const;

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadSkeleton(TiXmlElement *element);
        bool loadAttributes(TiXmlElement *element);

    private:
        std::map<std::string, pou::SpriteSheetAsset*> m_spriteSheets;
        std::list<SkeletonWithLimbs> m_skeletonModels;

        CharacterAttributes m_attributes;
};


#endif // CHARACTERMODELASSET_H
