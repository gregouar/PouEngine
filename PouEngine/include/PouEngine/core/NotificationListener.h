#ifndef NOTIFICATIONLISTENER_H
#define NOTIFICATIONLISTENER_H

#include <set>

#include "PouEngine/Types.h"

namespace pou
{

class NotificationSender;

class NotificationListener
{
    friend class NotificationSender;

    public:
        NotificationListener();
        virtual ~NotificationListener();

        NotificationListener (const NotificationListener &) = delete;
        NotificationListener & operator = (const NotificationListener &) = delete;

        void stopListeningTo(NotificationSender*);
        void stopListeningTo(NotificationSender*, int notificationType);
        void startListeningTo(NotificationSender*);
        void startListeningTo(NotificationSender*, int notificationType);

    protected:
        virtual void notify(NotificationSender*, int notificationType,
                            void* data = nullptr) = 0;

    private:
        void addSender(NotificationSender*);
        void notifySenderDestruction(NotificationSender*);

        std::set<NotificationSender*> m_senders;
};

}

#endif // NOTIFICATIONLISTENER_H
