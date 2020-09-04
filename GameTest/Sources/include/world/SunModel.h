#ifndef SUNMODEL_H
#define SUNMODEL_H

#include "PouEngine/system/XMLLoader.h"
#include "PouEngine/scene/LightEntity.h"

struct SunModel_Step
{
    glm::vec4   directColor;
    float       shadowBlur;
    glm::vec4   ambientColor;
};

class SunModel
{
    public:
        SunModel();
        virtual ~SunModel();

        bool loadFromFile(const std::string &filePath);

        std::shared_ptr<pou::LightEntity> generatesSun();
        glm::vec4 updateSun(pou::LightEntity *sun, float dayTime); //return ambient

    protected:
        bool loadFromXML(TiXmlHandle *hdl);

    private:
        std::map<float, SunModel_Step> m_steps;
};

#endif // SUNMODEL_H
