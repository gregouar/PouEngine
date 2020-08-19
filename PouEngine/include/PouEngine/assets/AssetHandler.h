#ifndef ASSETHANDLER_H
#define ASSETHANDLER_H

#include "PouEngine/Types.h"
#include "PouEngine/assets/Asset.h"
#include "PouEngine/core/Singleton.h"
#include "PouEngine/tools/Hasher.h"

#include <map>
#include <vector>
#include <mutex>
#include <thread>


namespace pou
{

template<class AssetType> class AssetHandler : public Singleton<AssetHandler<AssetType> >
{
    public:
        friend class Singleton<AssetHandler<AssetType> >;


        static AssetType* getAsset(const AssetTypeId assetId);

        static AssetType* loadAssetFromFile(const std::string &, AssetLoadType = LoadType_Now);
        static AssetType* loadAssetFromMemory(void *data, std::size_t dataSize, AssetLoadType = LoadType_Now);
        ///static AssetType* loadAssetFromStream(sf::InputStream *stream, AssetLoadType = LoadType_Now);

        static AssetType* loadAssetFromFile(const AssetTypeId id,const std::string &, AssetLoadType = LoadType_Now);
        static AssetType* loadAssetFromMemory(const AssetTypeId id, void *data, std::size_t dataSize, AssetLoadType = LoadType_Now);
       /// static AssetType* loadAssetFromStream(const AssetTypeId& id,sf::InputStream *stream, AssetLoadType = LoadType_Now);

        static AssetType* addAsset(bool plannedObsolescence=false, int lifeSpan=1);
        static AssetType* dummyAsset();

        void addToLoadingThread(AssetType*);
        void removeFromLoadingThread(AssetType*);

        AssetTypeId generateId();

        void addToObsolescenceList(const AssetTypeId assetId,int lifeSpan = 1);
        void removeFromObsolescenceList(const AssetTypeId assetId);

        void descreaseObsolescenceLife();
        void deleteAsset(AssetType* );
        void deleteAsset(const AssetTypeId assetId);
        void cleanAll();

        void enableDummyAsset(bool enable = true);
        void setDummyAsset(AssetType);
        AssetType* getDummyAsset();

    protected:
        AssetHandler();
        virtual ~AssetHandler();

        AssetType* getAssetImpl(const AssetTypeId assetId);

        AssetType* addAssetImpl(const AssetTypeId assetId, bool plannedObsolescence=false, int lifeSpan=1);

        AssetType* loadAssetFromFileImpl(const AssetTypeId id,const std::string &, AssetLoadType = LoadType_Now);
        AssetType* loadAssetFromMemoryImpl(const AssetTypeId id, void *data, std::size_t dataSize, AssetLoadType = LoadType_Now);
       /// AssetType* loadAssetFromStreamImpl(const AssetTypeId& id,sf::InputStream *stream, AssetLoadType = LoadType_Now);


        void loadInThread();

        void lockLoadMutex();
        void unlockLoadMutex();
        void waitForLoadingThread(AssetType *assetToWaitFor);

    private:
        std::unordered_map<AssetTypeId, AssetType*> m_assets;
        std::unordered_map<AssetTypeId, int> m_obsolescenceList;
        std::unordered_map<HashedString, AssetTypeId> m_filesList;

        std::thread m_loadThread;
        std::mutex m_loadMutex;
        std::mutex m_loadingCurAssetMutex;
        std::mutex m_assetAccessMutex;

        std::list<AssetType*> m_assetsToLoadInThread;
        AssetType* m_assetLoadingInThread;

        int m_curNewId;
        bool m_enableDummyAsset;
        AssetType m_dummyAsset;
};

}


#include "../src/assets/AssetHandler.tpp"


#endif // ASSETHANDLER_H
