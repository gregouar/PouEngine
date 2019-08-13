#ifndef SkeletonModelAsset_H
#define SkeletonModelAsset_H

#include "PouEngine/assets/Asset.h"
#include "PouEngine/utils/SimpleNode.h"

#include "tinyxml/tinyxml.h"

namespace pou
{

class SkeletonModelAsset : public Asset
{
    public:
        SkeletonModelAsset();
        SkeletonModelAsset(const AssetTypeId);
        virtual ~SkeletonModelAsset();

        bool loadFromFile(const std::string &filePath);

        const SimpleNode *getRootNode() const;
        std::map<std::string, SimpleNode*> getNodesByName();


    protected:
        bool loadFromXML(TiXmlHandle *);
        void loadNode(SimpleNode* rootNode, TiXmlElement *element);

    private:
        SimpleNode m_rootNode;

        std::map<std::string, SimpleNode*> m_nodesByName;
};

}

#endif // SkeletonModelAsset_H
