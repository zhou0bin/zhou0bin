/*****************************************************************************
* @file     check_json_key.h
* @brief    check Json key
* @author   kevin
* @date     2025/4/24
*****************************************************************************/
#ifndef VD_CHECK_JSON_KEY_H_
#define VD_CHECK_JSON_KEY_H_

#include <iostream>
#include <string>

#include "common/json.h"
#include "common/log.h"

namespace asf {
namespace vdi {

struct KeySt
{
    std::string key;
    Json::ValueType key_type;
    bool required_field;
};

class CheckJsonKey
{
private:
    std::vector<KeySt> diagReqCustomParamVec;

public:
    CheckJsonKey()
    {
        KeySt key1;
        key1.key = "publicKeySha256";
        key1.key_type = Json::stringValue;
        key1.required_field = true;
        KeySt key2;
        key2.key = "tempOrFixed";
        key2.key_type = Json::intValue;
        key2.required_field = true;

        KeySt key3;
        key3.key = "packageName";
        key3.key_type = Json::stringValue;
        key3.required_field = true;

        KeySt key10;
        key10.key = "key0";
        key10.key_type = Json::stringValue;
        key10.required_field = true;

        KeySt key11;
        key11.key = "stateUpload";
        key11.key_type = Json::booleanValue;
        key11.required_field = true;

        KeySt key13;
        key13.key = "interactScript";
        key13.key_type = Json::booleanValue;
        key13.required_field = true;

        KeySt key14;
        key14.key = "resultUrl";
        key14.key_type = Json::stringValue;
        key14.required_field = true;

        KeySt key15;
        key15.key = "resultUrlSsl";
        key15.key_type = Json::intValue;
        key15.required_field = true;

        KeySt key18;
        key18.key = "taskTimeout";
        key18.key_type = Json::intValue;
        key18.required_field = true;

        diagReqCustomParamVec.push_back(key1);
        diagReqCustomParamVec.push_back(key2);
        diagReqCustomParamVec.push_back(key3);
        diagReqCustomParamVec.push_back(key10);
        diagReqCustomParamVec.push_back(key11);
        diagReqCustomParamVec.push_back(key13);
        diagReqCustomParamVec.push_back(key14);
        diagReqCustomParamVec.push_back(key15);
        diagReqCustomParamVec.push_back(key18);
        KeySt key4;
        key4.key = "packageUrl";
        key4.key_type = Json::stringValue;
        key4.required_field = false;

        KeySt key6;
        key6.key = "packageUrlSsl";
        key6.key_type = Json::intValue;
        key6.required_field = false;

        KeySt key7;
        key7.key = "packageSize";
        key7.key_type = Json::intValue;
        key7.required_field = false;

        KeySt key8;
        key8.key = "packageSha256";
        key8.key_type = Json::stringValue;
        key8.required_field = false;

        KeySt key9;
        key9.key = "encryptFileSha256";
        key9.key_type = Json::stringValue;
        key9.required_field = false;

        KeySt key12;
        key12.key = "uploadPeriod";
        key12.key_type = Json::intValue;
        key12.required_field = false;

        KeySt key16;
        key16.key = "vehicleData";
        key16.key_type = Json::stringValue;
        key16.required_field = false;

        KeySt key17;
        key17.key = "scriptParameter";
        key17.key_type = Json::objectValue;
        key17.required_field = false;

        diagReqCustomParamVec.push_back(key4);
        // diagReqCustomParamVec.push_back(key5);
        diagReqCustomParamVec.push_back(key6);
        diagReqCustomParamVec.push_back(key7);
        diagReqCustomParamVec.push_back(key8);
        diagReqCustomParamVec.push_back(key9);
        diagReqCustomParamVec.push_back(key12);
        diagReqCustomParamVec.push_back(key16);
        diagReqCustomParamVec.push_back(key17);
    }
    
    ~CheckJsonKey() = default;
    
    bool CheckJsKey(std::string);

private:
    bool CheckSignAndPublickeyJson(Json::Value);

    bool CheckDiagReqJson(Json::Value);
    
    bool CheckDiagStopJson(Json::Value);
    
    bool CheckDiagInteractRespJson(Json::Value);
    
    bool CheckDiagExecParamModifyReqJson(Json::Value);
    
    bool CheckDiagEngineTimeParamModifyReqJson(Json::Value);
    
    bool CheckScriptSelfstartRspJson(Json::Value);

    bool CheckDeleteDiagFileReqJson(Json::Value);

    bool CheckAddOrUpdateFileReqJson(Json::Value);

    bool CheckFileInfoUploadReqJson(Json::Value);

    bool CheckPassThroughJson(Json::Value);

    bool CheckKey(Json::Value value, Json::ValueType type, std::string key, bool required_field);
};

} // namespace vdi
} // namespace asf

#endif // VD_CHECK_JSON_KEY_H_