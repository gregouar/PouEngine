#include "PouEngine/core/StatesManager.h"

#include "PouEngine/core/GameState.h"
#include "PouEngine/tools/Logger.h"

namespace pou
{

StatesManager::StatesManager() :
    m_attachedApp(nullptr),
    m_switchState(false)
{
}

StatesManager::~StatesManager()
{
    //dtor
}

void StatesManager::stop()
{
    this->switchState(nullptr);
}

void StatesManager::switchState(GameState* state)
{
    m_switchState = true;
    m_nextState   = state;
}

void StatesManager::pushState(GameState* state)
{
    if(m_states.back())
        m_states.back()->obscuring();
    m_states.push_back(state);
    m_states.back()->setManager(this);
    m_states.back()->entered();
}

GameState* StatesManager::popState()
{
    if(m_states.empty())
    {
        Logger::error("Attempted to pop from an empty game state stack");
        return (nullptr);
    }

    m_states.back()->leaving();
    m_states.pop_back();

    if(m_states.empty())
        return (nullptr);

    m_states.back()->revealed();
    return m_states.back();
}

GameState* StatesManager::peekState()
{
    if(m_states.empty())
        return (nullptr);
    return m_states.back();
}


void StatesManager::handleEvents(const EventsManager *eventsManager)
{
    for(std::size_t i = 0; i < m_states.size() ; ++i)
        m_states[i]->handleEvents(eventsManager);
}

void StatesManager::update(const Time &elapsedTime)
{
    for(auto state : m_states)
        state->update(elapsedTime);
}

void StatesManager::draw(RenderWindow *renderWindow)
{
    for(auto state : m_states)
        state->draw(renderWindow);
}

void StatesManager::handleSwitchs()
{
    if(m_switchState)
    {
        this->switchStateImpl(m_nextState);
        m_switchState = false;
    }
}

void StatesManager::switchStateImpl(GameState* state)
{
    for(std::size_t i = 0; i < m_states.size() ; ++i)
        m_states[i]->leaving();

    m_states.clear();

    if(state != nullptr)
    {
        m_states.push_back(state);
        m_states.back()->setManager(this);
        m_states.back()->entered();
    }
}

void StatesManager::attachApp(VApp* app)
{
    m_attachedApp = app;
}

VApp* StatesManager::getApp()
{
    return m_attachedApp;
}

}
