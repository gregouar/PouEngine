#include "PouEngine/core/Config.h"

#include <iostream>
#include <fstream>
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Parser.h"

namespace pou{

Config::Config()
{
    //ctor
}

Config::~Config()
{
    //dtor
}


bool Config::load(const std::string& filePath)
{
    std::ifstream file;
    file.open(filePath.c_str(),std::ifstream::in);

    if(!file.is_open())
    {
        Logger::error("Can't open config file: "+filePath);
        return (false);
    }

    while(this->loadSection(file));

    file.close();
    return (true);
}

void Config::addValue(const std::string& s, const std::string& n, const std::string& v)
{
    Config::instance()->m_sections[s][n] = v;
}

void Config::addDefaultValue(const std::string& s, const std::string& n, const std::string& v)
{
    Config::instance()->m_defaultSections[s][n] = v;
}

bool Config::getBool(const std::string& s, const std::string& n)
{
    return Parser::parseBool(Config::getString(s,n));
}

int Config::getInt(const std::string& section, const std::string& name)
{
    /*const std::string &data = Config::getString(section,name,dflt);
    if(Parser::isInt(data))
        return Parser::parseInt(data);
    else
        return Parser::parseInt(dflt);*/
    return Parser::parseInt(Config::getString(section,name));
}

float Config::getFloat(const std::string& s, const std::string& n)
{
    return Parser::parseFloat(Config::getString(s,n));
}

const std::string &Config::getString(const std::string& s, const std::string& n, bool useDefaultValue)
{
    auto &sections = useDefaultValue ? Config::instance()->m_defaultSections : Config::instance()->m_sections;
    auto section_it = sections.find(s);

    if(section_it != sections.end())
    {
        auto option_it = section_it->second.find(n);

        if(option_it != section_it->second.end())
            return option_it->second;
    }

    if(!useDefaultValue)
        return Config::instance()->getString(s,n,true);
    return Config::instance()->m_dummy;
}

///
///Protected
///


bool Config::loadSection(std::ifstream &file)
{
    bool sectionOpened = false;
    bool clearParameter = false;
    bool readingParameter = false;
    bool readingValue = false;

    std::string sectionName;
    std::ostringstream parameterName;
    std::ostringstream value;

    while(!file.eof())
    {
        char c = file.peek();
        if(c == '#')
        {
            std::string skip;
            std::getline(file, skip);
            continue;
        }


        if(sectionOpened)
        {
            if(c == '[')
                return (true);

            c = file.get();
            //file>>c;

            if(c == ' ' || c == '\n')
            {
                if(readingParameter)
                    clearParameter = true;
                    //parameterName = std::ostringstream();
                if(readingValue)
                {
                    if(!value.str().empty())
                    {
                        this->addValue(sectionName, parameterName.str(), value.str());
                        readingValue = false;
                        readingParameter = true;
                        clearParameter = true;
                    }
                }
                continue;
            }

            if(readingParameter)
            {
                if(c == '=')
                {
                    readingValue = true;
                    readingParameter = false;
                    value = std::ostringstream();
                }
                else
                {
                    if(clearParameter)
                    {
                        clearParameter = false;
                        parameterName = std::ostringstream();
                    }
                    parameterName << c;
                }

                continue;
            }

            if(readingValue)
                value << c;
        }
        else
        {
            if(c == '[')
            {
                sectionOpened = true;
                sectionName = this->readSectionName(file);
                if(sectionName.empty())
                    return (false);

                readingParameter = true;
                clearParameter = true;
                //parameterName = std::ostringstream();
            }
        }
    }

    return (false);
}

std::string Config::readSectionName(std::ifstream &file)
{
    //std::string sectionName;
    std::ostringstream sectionName;

    char c = file.get();
    if(c != '[')
        return std::string();

    while(c != ']')
    {
        c = file.get();
        if(c == ']')
            return sectionName.str();
        sectionName << c;
    }

    return std::string();
}

}
