#ifndef SYNCEDATTRIBUTE_H_INCLUDED
#define SYNCEDATTRIBUTE_H_INCLUDED

#include "PouEngine/Types.h"

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

        void setValue(const T &t);
        void setValue(const T &t, float curLocalTime);
        const T& getValue() const;

        float getLastUpdateTime() const;
        float getSyncTime() const;

    protected:
        T m_value;

        T m_syncValue;
        //T m_lastSyncValue;

        float m_lastUpdateTime;

        float m_syncTime;
        float m_lastSyncTime;


        float m_curLocalTime;
        bool m_firstSync;

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
    protected:
        T computeWantedValue();

    protected:
        bool m_useModulo;
        T   m_minModuloValue,
            m_maxModuloValue;


        T m_lastSyncValue;
        //float m_lastSyncTime;


        //std::list< std::pair<float, T> > m_syncTimesAndValuesList;
};

}

#include "../src/utils/SyncedAttribute.inc"

#endif // SYNCEDATTRIBUTE_H_INCLUDED
