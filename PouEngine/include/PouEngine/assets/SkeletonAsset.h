#ifndef SKELETONASSET_H
#define SKELETONASSET_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/utils/SimpleNode.h"

#include "tinyxml/tinyxml.h"

namespace pou
{

class SkeletonAsset : public Asset
{
    public:
        SkeletonAsset();
        SkeletonAsset(const AssetTypeId);
        virtual ~SkeletonAsset();

        bool loadFromFile(const std::string &filePath);


    protected:
        bool loadFromXML(TiXmlHandle *);
        void loadNode(SimpleNode* rootNode, TiXmlElement *element);

    private:
        SimpleNode m_rootNode;

        std::map<std::string, SimpleNode*> m_nodes;
};

}

#endif // SKELETONASSET_H
