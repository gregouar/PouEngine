#include "PouEngine/core/NotificationSender.h"
#include "PouEngine/core/NotificationListener.h"

namespace pou
{

NotificationSender::NotificationSender()
{
    //ctor
}

NotificationSender::~NotificationSender()
{
    for(auto listener : m_listenerToNotifyEverything)
    {
        listener->notify(this, NotificationType_SenderDestroyed);
        listener->notifySenderDestruction(this);
    }

    for(auto listenerSet : m_listenerToNotify)
        for(auto listener : listenerSet.second)
        {
            listener->notify(this, NotificationType_SenderDestroyed);
            listener->notifySenderDestruction(this);
        }
}



void NotificationSender::addToAllNotificationList(NotificationListener *listener)
{
    auto ret = m_listenerToNotifyEverything.insert(listener);
    if(ret.second == true)
        listener->addSender(this);
}

void NotificationSender::addToNotificationList(NotificationListener *listener, int notificationType)
{
    auto ret = m_listenerToNotify[notificationType].insert(listener);
    if(ret.second == true)
        listener->addSender(this);
}

void NotificationSender::removeFromNotificationList(NotificationListener *listener, int notificationType)
{
    m_listenerToNotify[notificationType].erase(listener);
}

void NotificationSender::removeFromAllNotificationList(NotificationListener *listener)
{
    m_listenerToNotifyEverything.erase(listener);

    for(auto type : m_listenerToNotify)
        this->removeFromNotificationList(listener, type.first);
}


void NotificationSender::sendNotification(int notificationType, void* data)
{
    for(auto listener : m_listenerToNotify[notificationType])
        listener->notify(this, notificationType, data);

    for(auto listener : m_listenerToNotifyEverything)
        listener->notify(this, notificationType, data);
}

}
