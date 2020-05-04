#ifndef SOUNDASSET_H
#define SOUNDASSET_H

#include "PouEngine/assets/Asset.h"

#include "tinyxml/tinyxml.h"

namespace pou
{

class SoundAsset : public Asset
{
    public:
        SoundAsset();
        SoundAsset(const AssetTypeId);
        virtual ~SoundAsset();

        bool loadFromFile(const std::string &filePath);

        SoundTypeId getSoundId() const;
        float       getVolume() const;

    protected:
        bool loadFromXML(TiXmlHandle *);

    private:
        std::string m_soundPath;
        SoundTypeId m_soundId;

        float m_volume;

        bool m_is3D;
        bool m_isLoop;
        bool m_isStream;
};

}

#endif // SOUNDASSET_H
