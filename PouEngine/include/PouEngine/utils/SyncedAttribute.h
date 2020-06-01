#ifndef SYNCEDATTRIBUTE_H_INCLUDED
#define SYNCEDATTRIBUTE_H_INCLUDED

#include "PouEngine/Types.h"
#include "PouEngine/utils/Timer.h"

#include <list>

namespace pou
{

template<typename T>
class SyncedAttribute
{
    public:
        SyncedAttribute();
        SyncedAttribute(const T &t, float curTime);
        virtual ~SyncedAttribute();

        virtual void syncFrom(const SyncedAttribute<T> &t);
        virtual bool update(const Time &elapsed_time, float curTime);

        void setValue(const T &t, bool forceUpdate = false);
        void setValue(const T &t, float curLocalTime, bool forceUpdate = false);
        const T& getValue(bool useRewind = false) const;

        float getLastUpdateTime(bool useRewind = true) const;
        float getSyncTime() const;

        bool rewind(float time);

        void setSyncPrecision(T precision);
        void setSyncDelay(float time);

    protected:
        T m_value;
        //T m_syncValue;
        std::map<float, T> m_syncValues;

        bool m_firstSync;
        float m_curLocalTime;
        float m_lastUpdateTime;

        float m_syncTime;
        float m_lastSyncTime;

        size_t m_maxRewindAmount;
        std::map<float, T> m_rewindValues;

        float   m_syncDelay;
        T       m_syncPrecision;
        //T m_wantedNewSyncValue;
        Timer m_syncTimer;


        float m_timeBeforeRewind;
        T m_valueBeforeRewind;
};


template<typename T>
class LinSyncedAttribute : public SyncedAttribute<T>
{
    public:
        LinSyncedAttribute();
        LinSyncedAttribute(const T &t, float curTime);

        virtual void syncFrom(const LinSyncedAttribute<T> &t);

        virtual bool update(const Time &elapsed_time, float curTime);

        void setModuloRange(const T& min, const T& max);
        void setInterpolationDelay(float delay);

    protected:
        T computeWantedValue(T &value);

    protected:
        bool m_useModulo;
        T   m_minModuloValue,
            m_maxModuloValue;

        //T m_lastSyncValue;
        //float m_lastSyncTime;

        float m_interpolationDelay;


        //std::list< std::pair<float, T> > m_syncTimesAndValuesList;
};

}

#include "../src/utils/SyncedAttribute.inc"

#endif // SYNCEDATTRIBUTE_H_INCLUDED