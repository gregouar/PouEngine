#ifndef NOTIFICATIONSENDER_H
#define NOTIFICATIONSENDER_H

#include <set>
#include <map>
#include <mutex>

#include "PouEngine/Types.h"

namespace pou
{

class NotificationListener;

class NotificationSender
{
    friend class MessageBus;

    public:
        NotificationSender();
        virtual ~NotificationSender();

        void addToAllNotificationList(NotificationListener *);
        void addToNotificationList(NotificationListener *, int notificationType);

        void removeFromNotificationList(NotificationListener *, int notificationType);
        void removeFromAllNotificationList(NotificationListener *);

    protected:
        void sendNotification(int notificationType, void* data = nullptr);

    private:
        std::unordered_set<NotificationListener*> m_listenerToNotifyEverything;
        std::unordered_map<int, std::unordered_set<NotificationListener*> > m_listenerToNotify;

        std::mutex m_mutex;
};

}

#endif // NOTIFICATIONSENDER_H
