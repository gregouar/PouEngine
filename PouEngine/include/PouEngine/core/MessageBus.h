#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include "PouEngine/utils/Singleton.h"
#include "PouEngine/core/NotificationSender.h"

namespace pou
{

class MessageBus : public NotificationSender, public Singleton<MessageBus>
{
    friend class Singleton<MessageBus>;

    public:
        MessageBus();
        virtual ~MessageBus();

        //void addEventType();
        static void postMessage(int msgType, void *msgData = nullptr);
        static void addListener(NotificationListener *listener, int msgType);
        static void removeListener(NotificationListener *listener);
        static void removeListener(NotificationListener *listener, int msgType);

    protected:

    private:
};

}

#endif // MESSAGEBUS_H
