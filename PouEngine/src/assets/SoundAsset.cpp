#include "PouEngine/assets/SoundAsset.h"

#include "PouEngine/audio/AudioEngine.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"


namespace pou
{

SoundAsset::SoundAsset() : SoundAsset(-1)
{
}

SoundAsset::SoundAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile = true;
    m_allowLoadFromMemory = false;
    m_soundId = 0;

    m_volume    = 1.0f;

    m_is3D      = false;
    m_isLoop    = false;
    m_isStream  = false;
}

SoundAsset::~SoundAsset()
{
    if(m_soundId != 0)
        AudioEngine::instance()->destroySound(m_soundId);
}


bool SoundAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        Logger::error("Cannot load sound from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool SoundAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    if(hdl->FirstChildElement("name").Element() != nullptr)
        m_name = hdl->FirstChildElement("name").Element()->GetText();


    if(hdl->FirstChildElement("path").Element() != nullptr)
        m_soundPath = m_fileDirectory + hdl->FirstChildElement("path").Element()->GetText();

    if(hdl->FirstChildElement("volume").Element() != nullptr)
        m_volume = Parser::parseFloat(hdl->FirstChildElement("volume").Element()->GetText());

    if(hdl->FirstChildElement("spatial").Element() != nullptr)
        m_is3D = Parser::parseBool(hdl->FirstChildElement("spatial").Element()->GetText());

    if(hdl->FirstChildElement("loop").Element() != nullptr)
        m_isLoop = Parser::parseBool(hdl->FirstChildElement("loop").Element()->GetText());

    if(hdl->FirstChildElement("stream").Element() != nullptr)
        m_isStream = Parser::parseBool(hdl->FirstChildElement("stream").Element()->GetText());

    if(m_soundPath == std::string())
        Logger::warning("SoundXML without sound path loaded");
    else
        m_soundId = AudioEngine::instance()->loadSound(m_soundPath,m_is3D, m_isLoop, m_isStream);

    if(loaded)
        Logger::write("Sound loaded from file: "+m_filePath);

    return (loaded);
}

SoundTypeId SoundAsset::getSoundId() const
{
    return (m_soundId);
}

float SoundAsset::getVolume() const
{
    return (m_volume);
}


}
