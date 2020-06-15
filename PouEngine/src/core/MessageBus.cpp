#include "PouEngine/core/MessageBus.h"

namespace pou
{

MessageBus::MessageBus()
{
    //ctor
}

MessageBus::~MessageBus()
{
    //dtor
}


void MessageBus::postMessage(int msgType, void *msgData)
{
    MessageBus::instance()->sendNotification(msgType, msgData);
}

void MessageBus::addListener(NotificationListener *listener, int msgType)
{
    MessageBus::instance()->addToNotificationList(listener, msgType);
}

void MessageBus::removeListener(NotificationListener *listener)
{
    MessageBus::instance()->removeFromAllNotificationList(listener);
}

void MessageBus::removeListener(NotificationListener *listener, int msgType)
{
    MessageBus::instance()->removeFromNotificationList(listener, msgType);
}

}
