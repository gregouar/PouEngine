#include "PouEngine/core/NotificationListener.h"
#include "PouEngine/core/NotificationSender.h"


namespace pou
{

NotificationListener::NotificationListener()
{
    //ctor
}

NotificationListener::~NotificationListener()
{
    for(auto sender : m_senders)
        sender->removeFromAllNotificationList(this);
}

void NotificationListener::addSender(NotificationSender* sender)
{
    m_senders.insert(sender);
}

void NotificationListener::notifySenderDestruction(NotificationSender* sender)
{
    m_senders.erase(sender);
}

void NotificationListener::stopListeningTo(NotificationSender* sender)
{
    if(sender != nullptr)
    {
        m_senders.erase(sender);
        sender->removeFromAllNotificationList(this);
    }
}

void NotificationListener::startListeningTo(NotificationSender* sender)
{
    this->stopListeningTo(sender);
    if(sender != nullptr)
    {
        m_senders.insert(sender);
        sender->addToAllNotificationList(this);
    }
}


void NotificationListener::startListeningTo(NotificationSender* sender, int notificationType)
{
    if(sender != nullptr)
    {
        m_senders.insert(sender);
        sender->addToNotificationList(this, notificationType);
    }
}


}
