//
//
#include <rapidjson/istreamwrapper.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/condition.h>

namespace midware::diag::script {

ConditionSet::ConditionSet(Context& context, const std::filesystem::path& path, const std::string& description)
    : context_(context)
    , description_(description) {
    LOG_INFO << "Loading condition from " << path.string() << "...";

    if (!std::filesystem::exists(path)) {
        LOG_INFO << "No condition_check.json.";
        return;
    }

    std::ifstream ifs(path);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (!doc.IsObject()) {
        LOG_ERROR << "The task condition json file is not a object.";
        throw std::invalid_argument("The task condition json file is not a object.");
    }

    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        LOG_INFO << "Loading condition key: " << key;
        if (key == "diagTaskUserAuth") {
            // user_auth_ = LoadUserAuth(it->value);
            // diagTaskUserAuth在虚拟诊断仪处理
            LOG_INFO << "diagTaskUserAuth need not process.";
            continue;
        }
        if (std::holds_alternative<std::monostate>(context_.GetState(key))) {
            LOG_WARN << "Invalid condition key.";
            continue;
        }
        if (!it->value.IsObject()) {
            LOG_WARN << "Invalid condition obj.";
            continue;
        }
        try {
            conditions_map_[key] = LoadCondition(it->value);
        } catch (const std::exception& e) {
            LOG_WARN << "Failed to load condition for " << key << ": " << e.what();
            continue;
        } catch (...) {
            LOG_WARN << "Failed to load condition for " << key;
            continue;
        }
    }

    state_connection_ = context_.OnStateChanged.connect([this](const std::string& key, const Context::StateValue& value) {
        try {
            LOG_INFO << "Condition state changed: " << key << " " << value.index();
            if (UpdateValue(key, value) && !pending_ready_.empty() && Check()) {
                LOG_INFO << "Condition check pass, notifying...";
                for (auto& promise: pending_ready_) {
                    promise.Set();
                }
                pending_ready_.clear();
            }
        } catch (...) {
        }
    });
}

bool ConditionSet::Check() {
    not_pass_num_ = 0;
    for (auto& it: conditions_map_) {
        LOG_INFO << "Checking condition " << it.first << "...";
        bool ret = it.second.Check();
         if (!ret) {
            not_pass_num_ += 1;
            LOG_ERROR << "Condition " << it.first << " not pass.";
        } else {
            LOG_INFO << "Condition " << it.first << " pass.";
        }
    }
    return (not_pass_num_ == 0);
}

bool ConditionSet::UpdateValue(const std::string& key) {
    auto it = conditions_map_.find(key);
    if (it != conditions_map_.end()) {
        it->second.current_value = context_.GetState(key);
        return true;
    }
    return false;
}

bool ConditionSet::UpdateValue(const std::string& key, Context::StateValue value) {
    auto it = conditions_map_.find(key);
    if (it != conditions_map_.end()) {
        it->second.current_value = value;
        return true;
    }
    return false;
}

void ConditionSet::UpdateAllValues() {
    LOG_INFO << "Update all condition values...";
    for (auto& it: conditions_map_) {
        LOG_INFO << "Update condition " << it.first << " value...";
        auto value = context_.GetState(it.first);
        if (!std::holds_alternative<std::monostate>(value)) {
            it.second.current_value = value;
        }
    }    
}

Future<void> ConditionSet::AsyncWait() {
    LOG_INFO << "Async wait for condition...";
    Promise<void> promise;
    auto future = promise.GetFuture();
    if (Check()) {
        promise.Set();
    } else {
        pending_ready_.emplace_back(std::move(promise));
    }
    return future;
}

// bool ConditionSet::LoadUserAuth(const rapidjson::Value& obj) {
//     if (!obj.IsObject()) {
//         return false;
//     }
//     auto enable_it = obj.FindMember("enable");
//     if (enable_it != obj.MemberEnd() && enable_it->value.IsBool()) {
//         return enable_it->value.GetBool();
//     }
//     return false;
// }

Condition ConditionSet::LoadCondition(const rapidjson::Value& obj) {
    Condition condition;
    
    LOG_INFO << "Loading condition...";
    auto value_it = obj.FindMember("value");
    if (value_it == obj.MemberEnd()) {
        LOG_ERROR << "No condition value.";
        throw std::runtime_error("No condition value.");
    }

    // auto enable_it = obj.FindMember("enable");
    // if (enable_it != obj.MemberEnd()) {
    //     if (enable_it->value.IsBool() && !enable_it->value.GetBool()) {
    //         LOG_ERROR << "Disabled condition.";
    //         throw std::runtime_error("Disabled condition.");
    //     }
    // }

    auto op_it = obj.FindMember("op");
    if (op_it != obj.MemberEnd() && op_it->value.IsString()) {
        std::string op = op_it->value.GetString();
        LOG_INFO << "Condition op: " << op;
        if (op == "==") {
            condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a == b; }; // TODO: what about 'double'?
        } else if (op == "<") {
            condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a < b; };
        } else if (op == ">") {
            condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a > b; };
        } else if (op == "<=") {
            condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a <= b; };
        } else if (op == ">=") {
            condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a >= b; };
        } else {
            LOG_ERROR << "Invalid condition op: " << op;
            throw std::runtime_error("Invalid condition op.");
        }
    } else {
        LOG_INFO << "Condition op: == (default)";
        condition.op = [](const Context::StateValue& a, const Context::StateValue& b) { return a == b; };
    }

    if (value_it->value.IsBool()) {
        LOG_INFO << "Condition target value type: bool " << value_it->value.GetBool();
        condition.target_value = value_it->value.GetBool();
    } else if (value_it->value.IsInt()) {
        LOG_INFO << "Condition target value type: int " << value_it->value.GetInt();
        condition.target_value = value_it->value.GetInt();
    } else if (value_it->value.IsDouble()) {
        LOG_INFO << "Condition target value type: double " << value_it->value.GetDouble();
        condition.target_value = value_it->value.GetDouble();
    } else {
        LOG_ERROR << "Invalid condition value type.";
        throw std::runtime_error("Invalid condition target value type.");
    }

    return condition;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
ConditionMG::ConditionMG()
    : pack_name_("")
    , VehMode_list_({"NORMAL","MANUFACTURE","TRANSPORT","SHOW","POSTCRASH","MAINTENANCE","INVALID"})
    , UsageMode_list_({"STANDBY","COMFORT","DRV","INVALID"})
{
}

ConditionDecorator::ConditionDecorator(ConditonAbstract* const cd)
    : DecoratorBase(cd)
{
    not_pass_num_ = 0;
    GetConditionPath(veh_config_data_path_, script_parameter_path_, temporary_result_path_, result_json_);
    location_field_ = "";
    release_number_field_ = "1611900407";
    session_id_field_ = "123";
    vin_field_ = "";
    mtoc_field_ = "";
}

ConditionDecorator::~ConditionDecorator()
{
    not_pass_num_ = 0;
    std::vector<std::string>().swap(option_code_);
}

bool ConditionDecorator::ConditionJudge(const std::string& path)
{
    ClearLastFile();
    MY_COND condition;
    bool result_flag = true;
    result_flag = condition_manager_->ConditionJudgment(path, condition);
    PrintResultJson(condition);
    return result_flag;
}

void ConditionDecorator::SetPackName(const std::string& pn)
{
    condition_manager_->SetPackName(pn);
    pack_name_ = pn;
}

void ConditionDecorator::SetLocationField(const char* const location)
{
    location_field_ = location;
}

void ConditionDecorator::SetReleaseNumber(const char* const release)
{
    release_number_field_ = release;
}

void ConditionDecorator::SetSessionID(const char* const sessiondi)
{
    session_id_field_ = sessiondi;
}

void ConditionDecorator::PrintResultJson(MY_COND& cond)
{
    GetVehicleConfigData();
    rapidjson::StringBuffer strBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuffer);
    (void)writer.SetFormatOptions(rapidjson::kFormatDefault);
    CreateStart(writer);
    PrintVehicleConfig(writer);
    PrintResult(writer, cond);
    CreateEnd(writer);
    std::fstream file;
    file.open(temporary_result_path_, std::ios::out);
    if (file.is_open())
    {
        file << strBuffer.GetString();
    }
    file.close();
}

void ConditionDecorator::GetVehicleConfigData()
{
    std::fstream file(veh_config_data_path_);
    if (!file.is_open())
    {
        return;
    }
    rapidjson::Document doc;
    const std::string datastring((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (doc.Parse(datastring.c_str()).HasParseError())
    {
        return;
    }
    if (doc.HasMember("VIN"))
    {
        if (doc["VIN"].IsString())
        {
            vin_field_ = doc["VIN"].GetString();
        }
    }
    char tstr[50] = "";
    time_t timep = time(nullptr);
    struct tm *p = gmtime(&timep);
    p->tm_hour += 8;
    std::string format = "%Y-%m-%dT%H:%M:%S";
    (void)strftime(tstr, sizeof(tstr), format.c_str(), p);
    time_stamp_ = tstr;
    if (doc.HasMember("MTOC"))
    {
        if (doc["MTOC"].IsString())
        {
            mtoc_field_ = doc["MTOC"].GetString();
        }
    }
    if (doc["LAS"].IsArray())
    {
        for (const auto& item : doc["LAS"].GetArray())
        {
            if (item.IsString())
            {
                option_code_.push_back(item.GetString());
            }
        }
    }
}

void ConditionDecorator::PrintVehicleConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    (void)writer.Key("VIN");
    (void)writer.String(vin_field_.c_str());
    (void)writer.Key("TimeStamp");
    (void)writer.String(time_stamp_.c_str());
    (void)writer.Key("MT");
    (void)writer.String(mtoc_field_.c_str());
    (void)writer.Key("OptionCodes");
    (void)writer.StartArray();
    for (const auto& item : option_code_)
    {
        (void)writer.String(item.c_str());
    }
    std::vector<std::string>().swap(option_code_);
    (void)writer.EndArray();
}

void ConditionDecorator::PrintResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, MY_COND& cond)
{
    (void)writer.Key("Result");
    CreateStart(writer);
    PrintCloudInfo(writer);
    PrintConditonInfo(writer, cond);
    PrintBlocks(writer);
    CreateEnd(writer);
}

void ConditionDecorator::PrintCloudInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    (void)writer.Key("Location");
    std::fstream sp(script_parameter_path_);
    if (sp.is_open())
    {
        const std::string params((std::istreambuf_iterator<char>(sp)), std::istreambuf_iterator<char>());
        rapidjson::Document doc;
        if (!doc.Parse(params.c_str()).HasParseError())
        {
            if (doc.HasMember("LOCATION"))
            {
                if (doc["LOCATION"].IsString())
                {
                    SetLocationField(doc["LOCATION"].GetString());
                }
            }
        }
    }
    (void)writer.String(location_field_.c_str());
    SetLocationField("");
    (void)writer.Key("TotalResult");
    if (not_pass_num_ == 0)
    {
        (void)writer.String("OK");
    }
    else
    {
        (void)writer.String("NOK");
    }
    (void)writer.Key("ReleaseNumber");
    (void)writer.String(release_number_field_.c_str());
    (void)writer.Key("SessionID");
    (void)writer.String(session_id_field_.c_str());
    (void)writer.Key("PackageName");
    (void)writer.String(pack_name_.c_str());
}

void ConditionDecorator::PrintConditonInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, MY_COND& cond) const
{
    (void)writer.Key("ConditionChecks");
    CreateStart(writer);
    (void)writer.Key("TotalResult");
    if (not_pass_num_ == 0)
    {
        (void)writer.String("OK");
    }
    else
    {
        (void)writer.String("NOK");
    }
    (void)writer.Key("Conditions");
    CreateStart(writer);
    for (auto item : cond)
    {
        (void)writer.Key(item.condname.c_str());
        CreateStart(writer);
        (void)writer.Key("Enable");
        (void)writer.String(item.enable.c_str());
        (void)writer.Key("SetValue");
        if (item.setvaluetype == rapidjson::kArrayType)
        {
            (void)writer.StartArray();
            for (const auto& it : item.setvalue)
            {
                (void)writer.String(it.c_str());
            }
            (void)writer.EndArray();
        }
        else
        {
            (void)writer.String(item.setvalue[0U].c_str());
        }
        (void)writer.Key("MeasureValue");
        (void)writer.String(item.measurevalue.c_str());
        CreateEnd(writer);
    }
    CreateEnd(writer);
    CreateEnd(writer);
}

void ConditionDecorator::PrintBlocks(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    (void)writer.Key("StatBlocks");
    (void)writer.StartArray();
    (void)writer.EndArray();
}

void ConditionDecorator::CreateStart(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    (void)writer.StartObject();
}

void ConditionDecorator::CreateEnd(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    (void)writer.EndObject();
}

bool ConditionDecorator::ResultFileNeedUpdate() const
{
    if (access(result_json_.c_str(), F_OK) == -1)
    {
        return true;
    }
    struct stat statbuf;
    (void)stat(result_json_.c_str(), &statbuf);
    if (statbuf.st_size == 0)
    {
        return true;
    }
    return false;
 }

void ConditionDecorator::ClearLastFile() const
{
    if (access(temporary_result_path_.c_str(), F_OK) == 0)
    {
        (void)remove(temporary_result_path_.c_str());
    }
    if (access(result_json_.c_str(), F_OK) == 0)
    {
        (void)remove(result_json_.c_str());
    }
}

bool ConditionMG::ConditionJudgment(const std::string& path, MY_COND& condition)
{
    ConditionsDataSet chresult;
    if (!GetCondition(path, condition, chresult))
    {
        LOG_ERROR << "Get Conditions Fail, There Seems To Be An Error In The Condition's File.";
        return false;
    }
    if ((!chresult.usgmode_check) && (!chresult.vehmode_check) && (!chresult.soc_check) && (!chresult.speed_check))
    {
        LOG_INFO << "Not Need Check Anyone, All Pass. ";
        return true;
    }
    DoGetChcekData(chresult);
    for (auto& item : condition)
    {
        if (item.enable == "True")
        {
            ResultParas(item, chresult);
        }
    }
    if ((chresult.usgmode_check) || (chresult.vehmode_check) || (chresult.soc_check) || (chresult.speed_check))
    {
        return false;
    }
    LOG_INFO << "All Pass. ";
    return true;
}

bool ConditionMG::GetCondition(const std::string& path, MY_COND& condition, ConditionsDataSet& result)
{
    std::string tpath = path;
    if (access(tpath.c_str(), F_OK) == -1)
    {
        GetBackupConditionFilePath(tpath);
        if (access(tpath.c_str(), F_OK) == -1)
        {
            LOG_ERROR << "No Usable File Found.";
            return false;
        }
        LOG_ERROR << "File Missing, Use Alternate File.";
    }
    DoConditonGet(tpath.c_str(), condition, result);
    if (condition.size() == 0U)
    {
        return false;
    }
    return true;
}

void ConditionMG::DoConditonGet(const std::string& path, MY_COND& condition, ConditionsDataSet& result)
{
    std::fstream file;
    file.open(path.c_str(), std::ios::in);
    if (!file.is_open())
    {
        return;
    }
    const std::string filebuffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    rapidjson::Document doc;
    if (doc.Parse(filebuffer.c_str()).HasParseError())
    {
        return;
    }
    ReadConditionData(doc, condition, result);
}

void ConditionMG::ReadConditionData(const rapidjson::Document& doc, MY_COND& vehcond, ConditionsDataSet& result)
{
    try
    {
        for (const auto& elem : doc.GetObject())
        {
            CondStr cond;
            cond.condname = elem.name.GetString();
            if (elem.value.GetType() == rapidjson::kObjectType)
            {
                for (const auto& data : elem.value.GetObject())
                {
                    if (data.name.GetString() == std::string("Enable"))
                    {
                        cond.enable = data.value.GetString();
                        if (cond.enable == "True")
                        {
                            if (cond.condname == "UsageModeCheck")
                            {
                                result.usgmode_check = true;
                            }
                            if (cond.condname == "VehicleModeCheck")
                            {
                                result.vehmode_check = true;
                            }
                            if (cond.condname == "VehicleSpeedCheck")
                            {
                                result.speed_check = true;
                            }
                            if (cond.condname == "SocCheck")
                            {
                                result.soc_check = true;
                            }
                        }
                    }
                    if (data.name.GetString() == std::string("SetValue"))
                    {
                        if (data.value.GetType() == rapidjson::kArrayType)
                        {
                            for (const auto& varr : data.value.GetArray())
                            {
                                if (cond.condname == "VehicleModeCheck")
                                {
                                    if (VehMode_list_.find(varr.GetString()) == VehMode_list_.end())
                                    {
                                        LOG_WARN << "[condition_check.json] VehicleModeCheck Exists Invalid Field" << varr.GetString();
                                    }
                                }
                                if (cond.condname == "UsageModeCheck")
                                {
                                    if (UsageMode_list_.find(varr.GetString()) == UsageMode_list_.end())
                                    {
                                        LOG_WARN << "[condition_check.json] UsageModeCheck Exists Invalid Field" << varr.GetString();
                                    }
                                }
                                (void)cond.setvalue.emplace_back(varr.GetString());
                            }
                            cond.setvaluetype = rapidjson::kArrayType;
                        }
                        else if (data.value.GetType() == rapidjson::kStringType)
                        {
                            if (cond.condname == "VehicleSpeedCheck")
                            {
                                if ((std::stod(data.value.GetString()) < 0) || (std::stod(data.value.GetString()) > 5))
                                {
                                    (void)cond.setvalue.emplace_back("5.0");
                                    LOG_WARN << "VehicleSpeedCheck Using Default 5.0km/h";
                                }
                                else
                                {
                                    (void)cond.setvalue.emplace_back(data.value.GetString());
                                }
                            }
                            else
                            {
                                (void)cond.setvalue.emplace_back(data.value.GetString());
                            }
                            cond.setvaluetype = rapidjson::kStringType;
                        }
                        else if (data.value.GetType() == rapidjson::kNumberType)
                        {
                            if (cond.condname == "VehicleSpeedCheck")
                            {
                                if ((data.value.GetDouble() < 0) || (data.value.GetDouble() > 5.0))
                                {
                                    (void)cond.setvalue.emplace_back("5.0");
                                    LOG_WARN << "VehicleSpeedCheck Using Default 5.0km/h";
                                }
                                else
                                {
                                    (void)cond.setvalue.emplace_back(std::to_string(data.value.GetDouble()));
                                }
                            }
                            else
                            {
                               (void) cond.setvalue.emplace_back(std::to_string(data.value.GetDouble()));
                            }
                            cond.setvaluetype = rapidjson::kStringType;
                        }
                        else
                        {
                            if (cond.condname == "VehicleSpeedCheck")
                            {
                                (void)cond.setvalue.emplace_back("5.0");
                                cond.setvaluetype = rapidjson::kStringType;
                            }
                            if (cond.condname == "SocCheck")
                            {
                                (void)cond.setvalue.emplace_back("-1.0");
                                cond.setvaluetype = rapidjson::kStringType;
                            }
                        }
                    }
                }
            }
            (void)vehcond.emplace_back(cond);
        }
        return;
    }
    catch (...)
    {
        return;
    }
}

void ConditionMG::ResultParas(CondStr& condtion, ConditionsDataSet& result)
{
    if (condtion.setvaluetype == rapidjson::kStringType)
    {
        if (condtion.condname == "VehicleSpeedCheck")
        {
            try
            {
                condtion.measurevalue = result.speed_;
                LOG_INFO << "condname: " << condtion.condname << "value: " << result.speed_;
                if (condtion.setvalue[0U].size() == 0U)
                {
                    LOG_ERROR << "condname: " << condtion.condname << "Not Pass Because Of SetValue is Error";
                    return;
                }
                else
                {
                    const double set = std::stod(condtion.setvalue[0U].c_str());
                    const double test = std::stod(result.speed_.c_str());
                    if (test > set)
                    {
                        LOG_ERROR << "condname: " << condtion.condname << "Not Pass Current Is: " << result.speed_ << "Need < " << set;
                        return;
                    }
                }
                result.speed_check = false;
            }
            catch (...)
            {
                LOG_ERROR << "condname: " << condtion.condname << "Speed Result Comparison Failed, Please Check The Conditions File";
                return;
            }
        }
        if (condtion.condname == "SocCheck")
        {
            try
            {
                condtion.measurevalue = result.soc_;
                LOG_INFO << "condname: " << condtion.condname << "value: " << result.soc_;
                if (condtion.setvalue[0U].size() == 0U)
                {
                    LOG_ERROR << "condname: " << condtion.condname << "Not Pass Because Of SetValue is Error";
                    return;
                }
                else
                {
                    const double set = std::stod(condtion.setvalue[0U].c_str());
                    const double test = std::stod(result.soc_.c_str());
                    if (test < set)
                    {
                        LOG_ERROR << "condname: " << condtion.condname << "Not Pass Current Is: " << result.soc_ << "Need > " << set;
                        return;
                    }
                }
                result.soc_check = false;
            }
            catch (...)
            {
                LOG_ERROR << "condname: " << condtion.condname << "Battery Result Comparison Failed, Please Check The Conditions File";
                return;
            }
        }
    }
    else if (condtion.setvaluetype == rapidjson::kArrayType)
    {
        if (condtion.condname == "VehicleModeCheck")
        {
            condtion.measurevalue = result.vehmode_;
            LOG_INFO << "condname: " << condtion.condname << "value: " << result.vehmode_;
            if (find(condtion.setvalue.begin(), condtion.setvalue.end(), result.vehmode_) == condtion.setvalue.end())
            {
                LOG_ERROR << "condname: " << condtion.condname << "Not Pass Current Is" << result.vehmode_;
                return;
            }
            result.vehmode_check = false;
        }
        if (condtion.condname == "UsageModeCheck")
        {
            condtion.measurevalue = result.usgmode_;
            LOG_INFO << "condname: " << condtion.condname << "value: " << result.usgmode_;
            if (find(condtion.setvalue.begin(), condtion.setvalue.end(), result.usgmode_) == condtion.setvalue.end())
            {
                LOG_ERROR << "condname: " << condtion.condname << "Not Pass Current Is" << result.usgmode_;
                return;
            }
            result.usgmode_check = false;
        }
    }
    else
    {
    }
    return;
}

void ConditionMG::DoGetChcekData(ConditionsDataSet& result)
{
    ConditionGetter::GetInstance().ConditionsData(result);
}
#endif

} // namespace midware::diag::script
