#include "PouEngine/system/XMLLoader.h"

#include "PouEngine/tools/Parser.h"

#include <vector>

namespace pou
{


namespace XMLLoader
{


bool loadFloat(float &v, TiXmlElement *element, const std::string &attName)
{
    if(!element)
        return (false);

    auto att = element->Attribute(attName.c_str());
    if(att && pou::Parser::isFloat(att))
    {
        v = pou::Parser::parseFloat(att);
        return (true);
    }

    return (false);
}

bool loadInt(int &v, TiXmlElement *element, const std::string &attName)
{
    if(!element)
        return (false);

    auto att = element->Attribute(attName.c_str());
    if(att && pou::Parser::isInt(att))
    {
        v = pou::Parser::parseInt(att);
        return (true);
    }

    return (false);
}

bool loadColor(Color &color, TiXmlElement *element)
{
    if(!element)
        return (false);

    std::vector<std::string> attNames = {"r","g","b","i"};
    std::vector<std::string> attAltNames = {"red","green","blue","intensity"};

    for(int i = 0 ; i < 4 ; ++i)
    {
        auto att = element->Attribute(attNames[i].c_str());
        if(!att)
            att = element->Attribute(attAltNames[i].c_str());

        if(att && pou::Parser::isFloat(att))
            color[i] = pou::Parser::parseFloat(att);
    }

    return (true);
}

}


/*XMLLoader::XMLLoader()
{
    //ctor
}

XMLLoader::~XMLLoader()
{
    //dtor
}*/

}

