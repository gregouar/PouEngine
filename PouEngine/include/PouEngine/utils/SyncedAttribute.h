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
        SyncedAttribute(const T &t, uint32_t curTime);
        virtual ~SyncedAttribute();

        virtual void syncFrom(const SyncedAttribute<T> &t);
        virtual bool update(const Time &elapsed_time, uint32_t curTime);

        void setValue(const T &t, bool forceUpdate = false);
        void setValue(const T &t, uint32_t curLocalTime, bool forceUpdate = false);
        const T& getValue(bool useRewind = false) const;

        uint32_t getLastUpdateTime(bool useRewind = true) const;
        uint32_t getSyncTime() const;

        bool rewind(uint32_t time);

        void setSyncPrecision(T precision);
        void setSyncDelay(uint32_t time);
        void setMaxRewindAmount(size_t maxRewind);

    protected:
        T m_value;
        //T m_syncValue;
        std::map<uint32_t, T> m_syncValues;

        bool m_firstSync;
        uint32_t m_curLocalTime;
        uint32_t m_lastUpdateTime;

        uint32_t m_syncTime;
        uint32_t m_lastSyncTime;

        size_t m_maxRewindAmount;
        std::map<uint32_t, T> m_rewindValues;

        uint32_t   m_syncDelay;
        T       m_syncPrecision;
        //T m_wantedNewSyncValue;
        ///Timer m_syncTimer;


        uint32_t m_timeBeforeRewind;
        T m_valueBeforeRewind;
};


template<typename T>
class LinSyncedAttribute : public SyncedAttribute<T>
{
    public:
        LinSyncedAttribute();
        LinSyncedAttribute(const T &t, uint32_t curTime);

       // virtual void syncFrom(const LinSyncedAttribute<T> &t);

        virtual bool update(const Time &elapsed_time, uint32_t curTime);

        void setModuloRange(const T& min, const T& max);
        void setInterpolationDelay(uint32_t delay);

    protected:
        T computeWantedValue(T &value);

    protected:
        bool m_useModulo;
        T   m_minModuloValue,
            m_maxModuloValue;

        //T m_lastSyncValue;
        //float m_lastSyncTime;

        uint32_t m_interpolationDelay;


        //std::list< std::pair<float, T> > m_syncTimesAndValuesList;
};

}

#include "../src/utils/SyncedAttribute.inc"

#endif // SYNCEDATTRIBUTE_H_INCLUDED
