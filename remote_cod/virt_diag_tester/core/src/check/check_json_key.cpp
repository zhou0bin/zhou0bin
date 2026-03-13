/*****************************************************************************
* @file     condition_check.cpp
* @brief    Condition Check
* @date     2025/4/24
*****************************************************************************/

#include "check/check_json_key.h"

namespace asf {
namespace vdi {

bool CheckJsonKey::CheckJsKey(std::string jsonStr)
{
    bool flag = true;
    Json::Reader reader;
    Json::Value root;
    int msgType = 0;
    if (!reader.parse(jsonStr, root))
    {
        flag = false;
        goto result;
    }

    if (!CheckKey(root, Json::intValue, "messageType", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(root, Json::stringValue, "protocolVersion", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(root, Json::uintValue, "timestamp", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(root, Json::stringValue, "taskId", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(root, Json::objectValue, "customParameter", true))
    {
        flag = false;
        goto result;
    }

    msgType = root["messageType"].asInt();
    LOGINFO << "CheckJsonKey::CheckJsKey: msgType " << msgType;

    switch (msgType)
    {
    case 2:
        flag = CheckSignAndPublickeyJson(root);
        break;
    case 100:
        flag = CheckDiagReqJson(root);

        break;
    case 103:
        flag = CheckDiagStopJson(root);
        break;
    case 106:
        flag = CheckDiagInteractRespJson(root);

        break;
    case 200:
        flag = CheckDiagExecParamModifyReqJson(root);

        break;
    case 202:
        flag = CheckDiagEngineTimeParamModifyReqJson(root);

        break;
    case 301:
        flag = CheckScriptSelfstartRspJson(root);

        break;
    case 400:
        flag = CheckDeleteDiagFileReqJson(root);

        break;
    case 402:
        flag = CheckAddOrUpdateFileReqJson(root);

        break;
    case 500:
        flag = CheckFileInfoUploadReqJson(root);

        break;
    case 600:
        flag = CheckPassThroughJson(root);

    default:
        break;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckSignAndPublickeyJson(Json::Value root)
{
    bool flag = true;

    Json::Value customParamValue;
    Json::Reader customParamReader;

    customParamValue = root["customParameter"];
    // if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    // {
    //     LOGERROR << "CheckJsonKey::CheckSignAndPublickeyJson: customParameter is not a valid json string.";
    //     flag = false;
    //     goto result;
    // }
    if (!CheckKey(customParamValue, Json::booleanValue, "pkiSign", false))
    {
        flag = false;
        goto result;
    }
    else
    {
        bool pkisign =  customParamValue["pkiSign"].asBool();
        if(pkisign)
        {
            if (!CheckKey(customParamValue, Json::stringValue, "pkiSignValue", true))
            {
                flag = false;
                goto result;
            }
            if (!CheckKey(customParamValue, Json::stringValue, "pkiSigner", true))
            {
                flag = false;
                goto result;
            }
        }
    }
    if (!CheckKey(customParamValue, Json::stringValue, "publicKey", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "publicKeySha256", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDiagReqJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;

    if (!CheckKey(root, Json::stringValue, "signBase64", false))
    {
        flag = false;
        goto result;
    }

    // if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    // {
    //     flag = false;
    //     goto result;
    // }
    customParamValue = root["customParameter"];

    for (size_t i = 0; i < diagReqCustomParamVec.size(); i++)
    {
        std::string key = diagReqCustomParamVec[i].key;
        Json::ValueType key_type = diagReqCustomParamVec[i].key_type;
        bool required = diagReqCustomParamVec[i].required_field;
        if (!CheckKey(customParamValue, key_type, key, required))
        {
            flag = false;
            goto result;
        }
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDiagStopJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;

    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "packageName", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDiagInteractRespJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))

    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "sequence", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "packageName", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::objectValue, "interactResponseParameter", true))
    {
        flag = false;
        goto result;
    }

    else
    {
        Json::Value interactRspParamValue;
        interactRspParamValue = customParamValue["interactResponseParameter"];
        if (!interactRspParamValue /* condition */)
        // if (!interactRspParamReader.parse(customParamValue["interactResponseParameter"].asString(), interactRspParamValue))
        {
            flag = false;
            goto result;
        }
        if (!CheckKey(interactRspParamValue, Json::stringValue, "interactType", true))
        {
            flag = false;
            goto result;
        }
        std::string interact_type = interactRspParamValue["interactType"].asString();
        if (strcmp(interact_type.c_str(), "1") == 0)
        {
            if (!CheckKey(interactRspParamValue, Json::intValue, "result", true))
            {
                flag = false;
                goto result;
            }
        }
        if (strcmp(interact_type.c_str(), "2") == 0)
        {
            if (!CheckKey(interactRspParamValue, Json::intValue, "result", true))
            {
                flag = false;
                goto result;
            }
            if (!CheckKey(interactRspParamValue, Json::arrayValue, "inputValues", true))
            {
                flag = false;
                goto result;
            }
        }
        if (strcmp(interact_type.c_str(), "3") == 0)
        {
            if (!CheckKey(interactRspParamValue, Json::intValue, "result", true))
            {
                flag = false;
                goto result;
            }
            if (!CheckKey(interactRspParamValue, Json::intValue, "selectedIndex", true))
            {
                flag = false;
                goto result;
            }
        }
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDiagExecParamModifyReqJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::objectValue, "scriptRunCondition", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDiagEngineTimeParamModifyReqJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::objectValue, "diagEngine", true))
    {
        flag = false;
        goto result;
    }
    else
    {
        Json::Value diagEngineValue;
        // Json::Reader diagEngineReader;
        // if (!diagEngineReader.parse(customParamValue["diagEngine"].asString(), diagEngineValue))
        diagEngineValue = customParamValue["diagEngine"];
        if (!diagEngineValue /* condition */)
        {
            flag = false;
            goto result;
        }
        if (!CheckKey(diagEngineValue, Json::intValue, "p2Client", true))
        {
            flag = false;
            goto result;
        }
        if (!CheckKey(diagEngineValue, Json::intValue, "p2StarClient", true))
        {
            flag = false;
            goto result;
        }
    }

result:
    return flag;
}

bool CheckJsonKey::CheckScriptSelfstartRspJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))

    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "resultUrlSsl", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "resultUrl", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::booleanValue, "stateUpload", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "uploadPeriod", false))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::booleanValue, "runAllowed", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "runForbiddenReason", false))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::objectValue, "scriptParameter", false))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckAddOrUpdateFileReqJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
        customParamValue = root["customParameter"];
    if (!customParamValue /* condition */)
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "fileType", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "packageUrl", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "packageUrlSsl", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "packageMd5", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckDeleteDiagFileReqJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))

    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::intValue, "fileType", true))
    {
        flag = false;
        goto result;
    }
    if (!CheckKey(customParamValue, Json::stringValue, "packageName", false))
    {
        flag = false;
        goto result;
    }

    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckFileInfoUploadReqJson(Json::Value root)
{
    bool flag = true;

    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))
    {
        flag = false;
        goto result;
    }

    if (!CheckKey(customParamValue, Json::intValue, "fileType", true))
    {
        flag = false;
        goto result;
    }

    if (!CheckKey(customParamValue, Json::stringValue, "packageName", false))
    {
        flag = false;
        goto result;
    }

    if (!CheckKey(customParamValue, Json::intValue, "taskTimeout", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckPassThroughJson(Json::Value root)
{
    bool flag = true;
    Json::Value customParamValue;
    Json::Reader customParamReader;
    if (!CheckKey(root, Json::stringValue, "signBase64", true))
    {
        flag = false;
        goto result;
    }

    if (!customParamReader.parse(root["customParameter"].asString(), customParamValue))

    {
        flag = false;
        goto result;
    }

    if (!CheckKey(customParamValue, Json::objectValue, "scriptParameter", true))
    {
        flag = false;
        goto result;
    }

result:
    return flag;
}

bool CheckJsonKey::CheckKey(Json::Value jsonValue, Json::ValueType type, std::string key, bool required_field)
{
    bool flag = true;

    if ( !jsonValue.isObject())//in Json::Value::demand(begin, end): requires objectValue or nullValue
    {
        LOGERROR << "CheckJsonKey::CheckKey: jsonValue is not object";
        flag = false;
        goto result;
    }

    if (!jsonValue.isMember(key) && required_field)
    {
        flag = false;

        LOGERROR << "CheckJsonKey::CheckKey: do not has key " << key.c_str();
        goto result;
    }

    else if (!jsonValue.isMember(key) && !required_field)
    {
        LOGINFO << "CheckJsonKey::CheckKey: jsonvalue dose not contain " << key.c_str() << " which is not required_field";

        goto result;
    }
    else
    {
        if (!required_field && jsonValue[key].isNull())
        {
            LOGINFO << "CheckJsonKey::CheckKey: " << key.c_str() << " is null, which is not required_field";
            goto result;
        }
        if (jsonValue[key].type() != type)
        {
            flag = false;
            LOGERROR << "json key " << key.c_str() << " ,Type is " << jsonValue[key].type() << " Type is" << type;
            goto result;
        }
    }
result:

    return flag;
}

} // namespace vdi
} // namespace asf