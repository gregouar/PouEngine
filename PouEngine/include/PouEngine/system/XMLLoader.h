#ifndef XMLLOADER_H
#define XMLLOADER_H

#include "tinyxml/tinyxml.h"
#include "PouEngine/Types.h"

namespace pou
{

namespace XMLLoader
{
    bool loadFloat(float &v, TiXmlElement *element, const std::string &attName);
    bool loadInt(int &v, TiXmlElement *element, const std::string &attName);
    bool loadColor(Color &color, TiXmlElement *element);
}

/*class XMLLoader
{
    public:
        XMLLoader();
        virtual ~XMLLoader();

    protected:

    private:
};*/

}

#endif // XMLLOADER_H
