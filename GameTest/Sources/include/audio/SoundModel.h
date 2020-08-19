#ifndef SOUNDMODEL_H
#define SOUNDMODEL_H

#include "tinyxml/tinyxml.h"
#include "PouEngine/Types.h"

#include <string>

class SoundModel
{
    public:
        SoundModel();
        virtual ~SoundModel();

        bool loadFromXML(TiXmlElement *element);

        pou::HashedString getName();
        const std::string &getPath();
        bool isEvent();

    protected:

    private:
        pou::HashedString m_name;
        std::string m_path;
        bool m_isEvent;
};

#endif // SOUNDMODEL_H
