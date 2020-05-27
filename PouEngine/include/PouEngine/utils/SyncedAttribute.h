#ifndef SYNCEDATTRIBUTE_H_INCLUDED
#define SYNCEDATTRIBUTE_H_INCLUDED

#include "PouEngine/Types.h"

namespace pou
{

template<typename T>
class SyncedAttribute
{
    public:
        SyncedAttribute();
        SyncedAttribute(const T &t, float curTime);
        virtual ~SyncedAttribute();

        void syncFrom(const SyncedAttribute<T> &t);
        bool update(const Time &elapsed_time, float curTime);

        void setValue(const T &t);
        void setValue(const T &t, float curLocalTime);
        const T& getValue() const;

        float getLastUpdateTime() const;
        float getLastSyncTime() const;

    protected:
        T m_value;
        T m_syncValue;

        float m_lastUpdateTime;
        float m_lastSyncTime;
        float m_curLocalTime;

};

}

#include "../src/utils/SyncedAttribute.inc"

#endif // SYNCEDATTRIBUTE_H_INCLUDED
