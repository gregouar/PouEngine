#include "PouEngine/audio/FMODAudioImpl.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{

const int FMODAudioImpl::DEFAULT_NBR_CHANNELS = 32;

FMODAudioImpl::FMODAudioImpl() : AbstractAudioImpl(),
    m_studioSystem(nullptr) , m_system(nullptr)
{
    m_nbrChannels = DEFAULT_NBR_CHANNELS;

    this->init();
}

FMODAudioImpl::~FMODAudioImpl()
{
    this->cleanup();
}

bool FMODAudioImpl::init()
{
    m_currentChannelId = 0;

    if(FMOD_Studio_System_Create(&m_studioSystem,FMOD_VERSION) != FMOD_OK)
    {
        m_studioSystem = nullptr;
        Logger::error("Couldn't create FMOD studio system");
        return (false);
    }

    if(FMOD_Studio_System_Initialize(m_studioSystem,m_nbrChannels, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, 0) != FMOD_OK)
    {
        Logger::error("Couldn't initialize FMOD studio system");
        return (false);
    }

    if(FMOD_Studio_System_GetCoreSystem(m_studioSystem,&m_system) != FMOD_OK)
    {
        m_system = nullptr;
        Logger::error("Couldn't get FMOD system");
        return (false);
    }

    m_channels.resize(m_nbrChannels, nullptr);

    return (true);
}

bool FMODAudioImpl::cleanup()
{
    bool r = true;

    if(m_studioSystem != nullptr)
    {
        if( FMOD_Studio_System_UnloadAll(m_studioSystem) != FMOD_OK ||
            FMOD_Studio_System_Release(m_studioSystem) != FMOD_OK)
        {
            r = false;
            Logger::error("Couldn't unload FMOD studio system");
        }
        m_studioSystem = nullptr;
    }

    return (r);
}

void FMODAudioImpl::update()
{
    if(m_studioSystem == nullptr || m_system == nullptr)
        return;

    FMOD_Studio_System_Update(m_studioSystem);
}

SoundTypeId FMODAudioImpl::loadSound(const std::string &path, bool is3D, bool isLooping, bool isStream)
{
    if(m_system == nullptr)
        return (0);

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= is3D ? FMOD_3D : FMOD_2D;
    eMode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD_SOUND *sound = nullptr;
    if(FMOD_System_CreateSound(m_system, path.c_str(), eMode, nullptr, &sound) != FMOD_OK)
        return (0);

    auto soundID = ++m_curSoundId;
    m_sounds.insert({soundID, sound});
    return (soundID);
}

bool FMODAudioImpl::destroySound(SoundTypeId id)
{
    bool r = true;

    if(m_system == nullptr)
        return (false);

    auto foundedSound = m_sounds.find(id);
    if(foundedSound == m_sounds.end())
        return (false);

    if(FMOD_Sound_Release(foundedSound->second) != FMOD_OK)
        r = false;

    m_sounds.erase(foundedSound);

    return (r);
}

bool FMODAudioImpl::playSound(SoundTypeId id, float volume, const glm::vec3 &pos)
{
    auto foundedSound = m_sounds.find(id);

    if(foundedSound == m_sounds.end())
        return (false);

    int channelId = m_currentChannelId++;
    if(m_currentChannelId >= m_nbrChannels)
        m_currentChannelId = 0;

    FMOD_CHANNEL *channel = nullptr;

    if(FMOD_System_PlaySound(m_system, foundedSound->second, nullptr, true, &channel) != FMOD_OK)
        return (false);

    if(!channel)
        return (false);

    FMOD_MODE curMode;
    FMOD_Sound_GetMode(foundedSound->second, &curMode);
    if(curMode & FMOD_3D)
    {
        FMOD_VECTOR fmodPos = vectorToFmod(pos);
        FMOD_Channel_Set3DAttributes(channel,&fmodPos,nullptr);
    }
    FMOD_Channel_SetVolume(channel, volume);
    FMOD_Channel_SetPaused(channel,false);

    m_channels[channelId] = channel;

    return (true);
}

SoundTypeId FMODAudioImpl::loadBank(const std::string &filePath)
{
    return this->loadBank(filePath, false);
}

SoundTypeId FMODAudioImpl::loadBank(const std::string &filePath, bool isStrings)
{
    if(m_system == nullptr)
        return (0);

    FMOD_STUDIO_BANK *bank;
    if(FMOD_Studio_System_LoadBankFile(m_studioSystem,filePath.c_str(),FMOD_STUDIO_LOAD_BANK_NORMAL,&bank)
       != FMOD_OK)
        return (0);

    if(!isStrings)
    {
        std::string fileExt = Parser::findFileExtension(filePath);
        std::string fileName = Parser::removeFileExtension(filePath);
        this->loadBank(fileName+".strings."+fileExt, true);
    }

    ///FMOD_Studio_Bank_LoadSampleData(bank);

    auto bankID = ++m_curBankId;
    m_banks.insert({bankID, bank});
    return (bankID);
}


bool FMODAudioImpl::destroyBank(SoundTypeId id)
{
    bool r = true;

    if(m_system == nullptr)
        return (false);

    auto foundedBank = m_banks.find(id);
    if(foundedBank == m_banks.end())
        return (false);

    if(FMOD_Studio_Bank_Unload(foundedBank->second) != FMOD_OK)
        r = false;

    m_banks.erase(foundedBank);

    return (r);
}

SoundTypeId FMODAudioImpl::createEvent(const std::string &eventName)
{
    if(m_system == nullptr)
        return (0);

    FMOD_STUDIO_EVENTDESCRIPTION    *eventDesc = nullptr;
    FMOD_STUDIO_EVENTINSTANCE       *event = nullptr;

    if(FMOD_Studio_System_GetEvent(m_studioSystem, eventName.c_str(), &eventDesc) != FMOD_OK)
        return (0);

    if(!eventDesc)
        return (0);

    if(FMOD_Studio_EventDescription_CreateInstance(eventDesc,&event) != FMOD_OK)
        return (0);

    auto eventID = ++m_curEventId;
    m_events.insert({eventID, event});
    return (eventID);
}

bool FMODAudioImpl::destroyEvent(SoundTypeId id)
{
    bool r = true;

    if(m_system == nullptr)
        return (false);

    auto foundedEvent = m_events.find(id);
    if(foundedEvent == m_events.end())
        return (false);

    if(FMOD_Studio_EventInstance_Release(foundedEvent->second) != FMOD_OK)
        r = false;

    m_events.erase(foundedEvent);

    return (r);
}

bool FMODAudioImpl::playEvent(SoundTypeId id)
{
    auto foundedEvent = m_events.find(id);

    if(foundedEvent == m_events.end())
        return (false);

    FMOD_Studio_EventInstance_Start(foundedEvent->second);

    return (true);
}


FMOD_VECTOR FMODAudioImpl::vectorToFmod(const glm::vec3& v)
{
    FMOD_VECTOR fVec;
    fVec.x = v.x;
    fVec.y = v.y;
    fVec.z = v.z;
    return fVec;
}


}



