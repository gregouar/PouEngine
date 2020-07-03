#include "PouEngine/core/EventsManager.h"

#include <string>
#include <locale>
#include <codecvt>

EventsManager::EventsManager()
{
    for(auto i = 0 ; i <= GLFW_KEY_LAST ; ++i)
    {
        m_keyIsPressed[i]   = false;
        m_keyPressed[i]     = false;
        m_keyRepeated[i]    = false;
        m_keyReleased[i]    = false;
    }
    m_keyMod = 0;

    for(auto i = 0 ; i <= GLFW_MOUSE_BUTTON_LAST ; ++i)
    {
        m_mouseButtonPressed[i]     = false;
        m_mouseButtonIsPressed[i]   = false;
        m_mouseButtonReleased[i]    = false;
    }

    m_mousePosition = glm::vec2(0,0);
    m_normalizedMousePosition = glm::vec2(0,0);
    m_centeredMousePosition = glm::vec2(0,0);
    m_mouseScroll   = glm::vec2(0,0);

    m_askingToClose = false;
    m_resizedWindow = false;
}

EventsManager::~EventsManager()
{
    //dtor
}

void EventsManager::init(GLFWwindow *window)
{
    m_window = window;

    glfwSetWindowUserPointer(m_window, this);

    glfwSetCharCallback         (m_window, EventsManager::char_callback);
    glfwSetKeyCallback          (m_window, EventsManager::key_callback);
    glfwSetMouseButtonCallback  (m_window, EventsManager::mouse_button_callback);
    glfwSetScrollCallback       (m_window, EventsManager::scroll_callback);
    glfwSetCursorPosCallback    (m_window, EventsManager::cursor_position_callback);
    glfwSetFramebufferSizeCallback(m_window, EventsManager::resize_callback);
}

void EventsManager::update()
{
    m_textEntered.clear();

    while(!m_justPressedKeys.empty())
    {
        m_keyPressed[m_justPressedKeys.top()] = false;
        m_justPressedKeys.pop();
    }

    while(!m_justReleasedKeys.empty())
    {
        m_keyReleased[m_justReleasedKeys.top()] = false;
        m_justReleasedKeys.pop();
    }

    while(!m_justPressedMouseButtons.empty())
    {
        m_mouseButtonPressed[m_justPressedMouseButtons.top()] = false;
        m_justPressedMouseButtons.pop();
    }

    while(!m_justReleasedMouseButtons.empty())
    {
        m_mouseButtonReleased[m_justReleasedMouseButtons.top()] = false;
        m_justReleasedMouseButtons.pop();
    }

    m_mouseScroll = glm::vec2(0,0);

    m_askingToClose = glfwWindowShouldClose(m_window);
    m_resizedWindow = false;

    glfwPollEvents();
}

void EventsManager::waitForEvents()
{
    glfwWaitEvents();
}

const std::u32string &EventsManager::getTextEntered() const
{
    return m_textEntered;
}

std::string EventsManager::getTextEnteredAsUtf8() const
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::string textEntered = converter.to_bytes(m_textEntered);

    return /*std::move(*/textEntered/*)*/;
}

std::string EventsManager::getClipBoard() const
{
    return std::string(glfwGetClipboardString(m_window));
}

void EventsManager::setClipBoard(const std::string &text) const
{
    glfwSetClipboardString(NULL, text.c_str());
}

bool EventsManager::keyPressed(int key, bool allowRepeat) const
{
    if(key >= 0 && key <= GLFW_KEY_LAST)
        return (m_keyPressed[key] & !m_keyRepeated[key])
        | (m_keyPressed[key] & m_keyRepeated[key] & allowRepeat) ;
    return (false);
}

bool EventsManager::keyIsPressed(int key) const
{
    if(key >= 0 && key <= GLFW_KEY_LAST)
        return m_keyIsPressed[key] | m_keyPressed[key];
    return (false);
}

bool EventsManager::keyReleased(int key) const
{
    if(key >= 0 && key <= GLFW_KEY_LAST)
        return m_keyReleased[key];
    return (false);
}

bool EventsManager::keyMod(int mod) const
{
    return (m_keyMod & mod);
}

bool EventsManager::mouseButtonPressed(int button) const
{
    if(button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
        return m_mouseButtonPressed[button];
    return (false);
}

bool EventsManager::mouseButtonIsPressed(int button) const
{
    if(button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
        return m_mouseButtonIsPressed[button] | m_mouseButtonPressed[button];
    return (false);
}
bool EventsManager::mouseButtonReleased(int button) const
{
    if(button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
        return m_mouseButtonReleased[button];
    return (false);
}

glm::vec2 EventsManager::mousePosition() const
{
    return m_mousePosition;
}

glm::vec2 EventsManager::normalizedMousePosition() const
{
    return m_normalizedMousePosition;
}

glm::vec2 EventsManager::centeredMousePosition() const
{
    return m_centeredMousePosition;
}

glm::vec2 EventsManager::mouseScroll() const
{
    return m_mouseScroll;
}

bool EventsManager::isAskingToClose() const
{
    return m_askingToClose;
}

bool EventsManager::resizedWindow() const
{
    return m_resizedWindow;
}

glm::vec2 EventsManager::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return glm::vec2(width, height);
}


void EventsManager::updateKey(int key, int action)
{
    if(key >= 0 && key <= GLFW_KEY_LAST)
    {
        if(action == GLFW_PRESS)
        {
            m_keyPressed[key]   = true;
            m_keyIsPressed[key] = true;
            m_justPressedKeys.push(key);

            if(key == GLFW_KEY_LEFT_SHIFT)
                m_keyMod |= GLFW_MOD_SHIFT;
            if(key == GLFW_KEY_LEFT_CONTROL)
                m_keyMod |= GLFW_MOD_CONTROL;
            if(key == GLFW_KEY_LEFT_ALT)
                m_keyMod |= GLFW_MOD_ALT;
        }
        else if(action == GLFW_REPEAT)
        {
            m_keyPressed[key]   = true;
            m_keyRepeated[key]  = true;
            m_justPressedKeys.push(key);
        }
        else if(action == GLFW_RELEASE)
        {
            m_keyReleased[key]  = true;
            m_keyIsPressed[key] = false;
            m_keyRepeated[key]  = false;
            m_justReleasedKeys.push(key);

            if(key == GLFW_KEY_LEFT_SHIFT)
                m_keyMod &= ~GLFW_MOD_SHIFT;
            if(key == GLFW_KEY_LEFT_CONTROL)
                m_keyMod &= ~GLFW_MOD_CONTROL;
            if(key == GLFW_KEY_LEFT_ALT)
                m_keyMod &= ~GLFW_MOD_ALT;
        }
    }
}

void EventsManager::updateMouseButton(int button, int action)
{
    if(button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
    {
        if(action == GLFW_PRESS)
        {
            m_mouseButtonPressed[button] = true;
            m_mouseButtonIsPressed[button] = true;
            m_justPressedMouseButtons.push(button);
        }
        else if(action == GLFW_RELEASE)
        {
            m_mouseButtonReleased[button]  = true;
            m_mouseButtonIsPressed[button] = false;
            m_justReleasedMouseButtons.push(button);
        }
    }
}

void EventsManager::updateMouseScroll(double xoffset, double yoffset)
{
    m_mouseScroll = glm::vec2(xoffset, yoffset);
}

void EventsManager::updateMousePosition(double xpos, double ypos, int width, int height)
{
    m_mousePosition = glm::vec2(xpos, ypos);
    m_normalizedMousePosition.x = xpos/(double)width;
    m_normalizedMousePosition.y = ypos/(double)height;
    m_centeredMousePosition = glm::vec2(xpos - (double)width/2, ypos - (double)height/2);
}

/*void EventsManager::updateWindowSize(int width, int height)
{
    m_resizedWindow = true;
}*/

void EventsManager::char_callback(GLFWwindow* window, unsigned int codepoint)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    eventsManager->m_textEntered.push_back(codepoint);
}

void EventsManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    if(eventsManager != nullptr)
        eventsManager->updateKey(key, action);
}

void EventsManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    if(eventsManager != nullptr)
        eventsManager->updateMouseButton(button, action);
}

void EventsManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    if(eventsManager != nullptr)
        eventsManager->updateMouseScroll(xoffset, yoffset);
}

void EventsManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    if(eventsManager != nullptr)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        eventsManager->updateMousePosition(xpos, ypos, width, height);
    }
}

void EventsManager::resize_callback(GLFWwindow* window, int width, int height)
{
    EventsManager *eventsManager =
      static_cast<EventsManager*>(glfwGetWindowUserPointer(window));

    if(eventsManager != nullptr)
        eventsManager->m_resizedWindow = true;
    /*{
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        eventsManager->updateWindowSize(w, h);
    }*/
}

