#ifndef SYNCEDATTRIBUTE_H_INCLUDED
#define SYNCEDATTRIBUTE_H_INCLUDED

#include "PouEngine/Types.h"
#include "PouEngine/system/Timer.h"

#include <list>
#include <map>

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

        bool setValue(const T &t, bool forceUpdate = false);
        bool setValue(const T &t, uint32_t curLocalTime, bool forceUpdate = false);
        //const T& getValue(bool useRewind = false) const;
        const T& getValue() const;

        //uint32_t getLastUpdateTime(bool useRewind = true) const;
        uint32_t getLastUpdateTime() const;
        uint32_t getSyncTime() const;

        ///bool rewind(uint32_t time);

        void setReconciliationPrecision(T precision);
        void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1); //-1 means it takes the server value
        void setMaxRewindAmount(size_t maxRewind);

    protected:
        T m_value;
        std::map<uint32_t, T> m_syncValues;

        bool m_firstSync;
        uint32_t m_curLocalTime;
        uint32_t m_lastUpdateTime;

        uint32_t m_syncTime;
        uint32_t m_lastSyncTime;

        size_t m_maxRewindAmount;
        std::map<uint32_t, T> m_rewindValues;

        uint32_t    m_reconciliationDelay_client,
                    m_reconciliationDelay_server;
        T       m_reconciliationPrecision;
        Timer   m_desyncTimer;


        /**uint32_t m_timeBeforeRewind;
        T m_valueBeforeRewind;**/
};


template<typename T>
class LinSyncedAttribute : public SyncedAttribute<T>
{
    public:
        LinSyncedAttribute();
        LinSyncedAttribute(const T &t, uint32_t curTime);

        virtual bool update(const Time &elapsed_time, uint32_t curTime);

        void setModuloRange(const T& min, const T& max);

    protected:
        T computeWantedValue(T &value);

    protected:
        bool m_useModulo;
        T   m_minModuloValue,
            m_maxModuloValue;
};

}

#include "../src/net/SyncedAttribute.inc"

#endif // SYNCEDATTRIBUTE_H_INCLUDED
