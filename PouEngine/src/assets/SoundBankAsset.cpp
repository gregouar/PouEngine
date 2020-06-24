#include "PouEngine/assets/SoundBankAsset.h"

#include "PouEngine/audio/AudioEngine.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Parser.h"

namespace pou
{

SoundBankAsset::SoundBankAsset() : SoundBankAsset(-1)
{
}

SoundBankAsset::SoundBankAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_soundBankId = 0;
}

SoundBankAsset::~SoundBankAsset()
{
    if(m_soundBankId != 0)
        AudioEngine::instance()->destroyBank(m_soundBankId);
}


bool SoundBankAsset::loadFromFile(const std::string &filePath)
{
    m_soundBankId = AudioEngine::loadBank(filePath);

    if(m_soundBankId != 0)
        Logger::write("Sounds bank loaded from file: "+m_filePath);

    return (m_soundBankId != 0);
}


SoundTypeId SoundBankAsset::getSoundId() const
{
    return m_soundBankId;
}



}
