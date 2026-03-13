// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#ifndef DIAG_SCRIPT_PARSER_PARSER_JSON_H__
#define DIAG_SCRIPT_PARSER_PARSER_JSON_H__

#include <iostream>
#include <fstream>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

namespace midware
{
namespace diag
{
namespace script
{

enum JsonDataType
{
    kNull = 0x0,
    kFalse = 0x1,
    kTrue = 0x2,
    kObject = 0x3,
    kArray = 0x4,
    kString = 0x5,
    kNumber = 0x6
};

template<typename T>
bool GetJsonData(rapidjson::Document& doc, const char* const key, T& t)
{
    bool flag = false;
    try {
        if (doc[key].IsInt())
        {
            t = doc[key].GetInt();
            flag = true;
        }
    } catch (...) {
    }
    return flag;
}

template<>
inline bool GetJsonData<float>(rapidjson::Document& doc, const char* const key, float& t)
{
    bool flag = false;
    try {
        if (doc[key].IsFloat())
        {
            t = doc[key].GetFloat();
            flag = true;
        }
    } catch (...) {
    }
    return flag;
}

template<> 
inline bool GetJsonData<bool>(rapidjson::Document& doc, const char* const key, bool& t)
{
    bool flag = false;
    try {
        if (doc[key].IsBool())
        {
            t = doc[key].GetBool();
            flag = true;
        }
    } catch (...) {
    }
    return flag;
}

template<>
inline bool GetJsonData<std::string>(rapidjson::Document& doc, const char* const key, std::string& t)
{
    bool flag = false;
    try {
        if (doc[key].IsString())
        {
            t = doc[key].GetString();
            flag = true;
        }
    } catch (...) {
    }
    return flag;
}

template<typename T>
bool ParseJson(const char* const path, const char* const key, T& t)
{
    if ((access(path, F_OK) == -1) || (!static_cast<bool>(key)))
    {
        return false;
    }
    std::fstream file;
    file.open(path, std::ios::in);
    if (!file.is_open())
    {
        return false;
    }
    const std::string bufferstring((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    rapidjson::Document doc;
    if (doc.Parse(bufferstring.c_str()).HasParseError())
    {
        return false;
    }
    if (!doc.HasMember(key))
    {
        return false;
    }
    if (GetJsonData<T>(doc, key, t))
    {
        return true;
    }
    return false;
}

} // script
} // diag
} // midware

#endif  // DIAG_SCRIPT_PARSER_PARSER_JSON_H__
