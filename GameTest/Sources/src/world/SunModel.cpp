#include "world/SunModel.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


SunModel::SunModel()
{
    //ctor
}

SunModel::~SunModel()
{
    //dtor
}

bool SunModel::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load sun model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


std::shared_ptr<pou::LightEntity> SunModel::generatesSun()
{
    auto sun = std::make_shared<pou::LightEntity>();
    sun->setType(pou::LightType_Directional);
    sun->enableShadowCasting();
    sun->setShadowMapExtent({1024,1024});
    //m_sunLight->setShadowBlurRadius(10);

    this->updateSun(sun.get(), 0);

    return sun;
}

glm::vec4 SunModel::updateSun(pou::LightEntity *sun, float dayTime)
{
    auto stepIt2 = m_steps.upper_bound(dayTime);
    auto stepIt1 = std::prev(stepIt2);

    float minT = stepIt1->first;

    if(stepIt2 == m_steps.end())
        stepIt2 = m_steps.begin();

    float maxT = stepIt2->first;
    if(maxT == 0)
        maxT = 1.0f;

    sun->setDirection({glm::cos(dayTime*glm::pi<float>()*2),
                       glm::sin(dayTime*glm::pi<float>()*2),-2.0});

    float r = (dayTime - minT) / (maxT - minT);
    auto interpDirectColor = glm::mix(stepIt1->second.directColor, stepIt2->second.directColor, r);
    auto interpAmbientColor = glm::mix(stepIt1->second.ambientColor, stepIt2->second.ambientColor, r);
    auto interpShadowBlur = glm::mix(stepIt1->second.shadowBlur, stepIt2->second.shadowBlur, r);

    sun->setDiffuseColor(glm::vec3(interpDirectColor));
    sun->setIntensity(interpDirectColor[3]);
    sun->setShadowBlurRadius(interpShadowBlur);

    return interpAmbientColor;
}


///
///protected
///

bool SunModel::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    float defaultShadowBlur = 0.0f;

    auto att = hdl->ToElement()->Attribute("shadowBlur");
    if(att && pou::Parser::isFloat(att))
        defaultShadowBlur = pou::Parser::parseFloat(att);

    element = hdl->FirstChildElement("step").Element();
    while(element)
    {
        float t = 0;
        if(!pou::XMLLoader::loadFloat(t, element, "t")
        && !pou::XMLLoader::loadFloat(t, element, "time"))
            continue;

        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec4 directColor(1.0f);
        pou::XMLLoader::loadColor(directColor, element->FirstChildElement("direct"));

        glm::vec4 ambientColor(directColor);
        pou::XMLLoader::loadColor(ambientColor, element->FirstChildElement("ambient"));

        float shadowBlur = defaultShadowBlur;
        pou::XMLLoader::loadFloat(shadowBlur, element, "shadowBlur");

        SunModel_Step step;
        step.directColor    = directColor;
        step.ambientColor   = ambientColor;
        step.shadowBlur     = shadowBlur;
        m_steps.insert({t, step});

        element = element->NextSiblingElement("step");
    }

    return loaded;
}


