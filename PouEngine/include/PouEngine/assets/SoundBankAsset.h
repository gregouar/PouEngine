#ifndef SOUNDBANKASSET_H
#define SOUNDBANKASSET_H

#include "PouEngine/assets/Asset.h"

namespace pou
{

class SoundBankAsset : public Asset
{
    public:
        SoundBankAsset();
        SoundBankAsset(const AssetTypeId);
        virtual ~SoundBankAsset();

        bool loadFromFile(const std::string &filePath);

        SoundTypeId getSoundId() const;

    protected:

    private:
        SoundTypeId m_soundBankId;
};

}

#endif // SOUNDBANKASSET_H
