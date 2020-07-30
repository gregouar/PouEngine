#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <map>
#include "PouEngine/core/Singleton.h"

namespace pou{

typedef std::map<std::string, std::string> ConfigSection;

class Config : public Singleton<Config>
{

    public:
        friend class Singleton<Config>;

        bool load(const std::string&);

        static void                 addValue(const std::string& s, const std::string& n, const std::string& v);
        static void                 addDefaultValue(const std::string& s, const std::string& n, const std::string& v);

        static bool                 getBool(const std::string& s, const std::string& n);
        static int                  getInt(const std::string& s, const std::string& n);
        static float                getFloat(const std::string& s, const std::string& n);
        static const std::string&   getString(const std::string& s, const std::string& n, bool useDefaultValue = false);

    protected:
        Config();
        virtual ~Config();

        bool loadSection(std::ifstream &file);
        std::string readSectionName(std::ifstream &file);

    private:
        std::map<std::string, ConfigSection> m_sections;
        std::map<std::string, ConfigSection> m_defaultSections;

        std::string m_dummy;
};

}

#endif // CONFIG_H
