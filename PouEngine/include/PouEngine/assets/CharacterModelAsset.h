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

namespace pou
{

struct LimbModel
{
    std::string node;
    SpriteModel* spriteModel;
};

struct SkeletonWithLimbs
{
    SkeletonModelAsset* skeleton;
    std::list<LimbModel> limbs;
};

class CharacterModelAsset : public Asset
{
    public:
        CharacterModelAsset();
        CharacterModelAsset(const AssetTypeId);
        virtual ~CharacterModelAsset();

        bool loadFromFile(const std::string &filePath);

        bool generateOnNode(SceneNode *parentNode,
                            std::list<std::unique_ptr<Skeleton> > *skeletons,
                            std::list<std::unique_ptr<SpriteEntity> > *limbs);

    protected:
        bool loadFromXML(TiXmlHandle *);
        bool loadSpriteSheet(TiXmlElement *element);
        bool loadSkeleton(TiXmlElement *element);

    private:
        std::map<std::string, SpriteSheetAsset*> m_spriteSheets;
        std::list<SkeletonWithLimbs> m_skeletonModels;
};

}

#endif // CHARACTERMODELASSET_H
