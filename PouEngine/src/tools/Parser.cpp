#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"

#include <sstream>


namespace pou
{


Parser::Parser()
{
    //ctor
}

Parser::~Parser()
{
    //dtor
}

bool Parser::isBool(const std::string &data)
{
    bool value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool Parser::isInt(const std::string &data)
{
    int value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool Parser::isFloat(const std::string &data)
{
    float value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool  Parser::parseBool(const std::string& data)
{
    bool value = false;
    std::istringstream ss(data);
    ss>>value;
    if(!ss.eof())
    {
        if(data == "true" || data == "1")
            return (true);
        else
            return (false);
    }
    return value;
}

int   Parser::parseInt(const std::string& data)
{
    int value = 0;
    std::istringstream ss(data);
    ss>>value;
    return value;
}

float Parser::parseFloat(const std::string& data)
{
    float value = 0;
    std::istringstream ss(data);
    ss>>value;
    return value;
}

void Parser::parseSegment(const std::string& data, std::string &lhs, std::string &rhs)
{
    if(data.length() > 1 && data[0] == '[')
    {
        auto middleDelimiter = data.find(',', 1);
        if(middleDelimiter == std::string::npos)
            return;

        auto rightDelimiter = data.find(']', middleDelimiter+1);
        lhs = data.substr(1, middleDelimiter-1);
        rhs = data.substr(middleDelimiter+1, rightDelimiter-middleDelimiter-1);
    }
}

std::pair<float, float> Parser::parseFloatSegment(const std::string& data)
{
    if(Parser::isFloat(data))
    {
        auto v = Parser::parseFloat(data);
        return {v,v};
    }

    std::string lhs, rhs;
    Parser::parseSegment(data, lhs, rhs);

    return {Parser::parseFloat(lhs),
            Parser::parseFloat(rhs)};
}

std::pair<int, int> Parser::parseIntSegment(const std::string& data)
{
    if(Parser::isInt(data))
    {
        auto v = Parser::parseInt(data);
        return {v,v};
    }

    std::string lhs, rhs;
    Parser::parseSegment(data, lhs, rhs);

    return {Parser::parseInt(lhs),
            Parser::parseInt(rhs)};
}

std::string Parser::findFileDirectory(const std::string &filePath)
{
    std::size_t p = filePath.find_last_of("/\\");
    if(p != std::string::npos)
        return filePath.substr(0,p+1);

    Logger::error("Cannot find directory of "+filePath);
    return filePath;
}

std::string Parser::findFileExtension(const std::string &filePath)
{
    std::size_t p = filePath.find_last_of('.');

    if(p != std::string::npos)
        return filePath.substr(p+1, filePath.size());

    Logger::error("Cannot find extension of "+filePath);
    return filePath;
}

std::string Parser::removeFileExtension(const std::string &filePath)
{
    std::size_t p = filePath.find_last_of('.');

    if(p != std::string::npos)
        return filePath.substr(0, p);

    Logger::error("Cannot remove extension of "+filePath);
    return filePath;
}


}


