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

#ifndef MIDWARE_DIAG_CONDITION_H
#define MIDWARE_DIAG_CONDITION_H

#include <string>
#include <vector>
#include <filesystem>
#include <functional>

#include <rapidjson/rapidjson.h>

#include <midware/diag/script/common.h>
#include <midware/diag/script/async.h>
#include <midware/diag/script/sigslot.h>
#include <midware/diag/script/context.h>

namespace midware::diag::script {

struct Condition {
    //bool enabled;
    Context::StateValue target_value;
    Context::StateValue current_value;
    std::function<bool (const Context::StateValue&, const Context::StateValue&)> op;

    bool Check() const noexcept {
        // if (enabled) {
            assert(op);
            return op(current_value, target_value);
        // }
        // return true;
    }
};

class ConditionSet {
public:
    using ConditionMap = std::map<std::string, Condition>;
    using CheckCallback = std::function<void (bool)>;

public:
    ConditionSet(Context& context, const std::filesystem::path& path, const std::string& description = std::string());

public:
    const ConditionMap& GetAll() const noexcept {
        return conditions_map_;
    }

    // bool IsUserAuth() const noexcept {
    //     return user_auth_;
    // }

    bool Check();
    bool UpdateValue(const std::string& key);
    bool UpdateValue(const std::string& key, Context::StateValue value);
    void UpdateAllValues();
    Future<void> AsyncWait();

    template <typename OS>
    void Dump(rapidjson::Writer<OS>& writer) const {
        (void)writer.Key("TotalResult");
        (void)writer.String(not_pass_num_ == 0 ? "OK" : "NOK");
        (void)writer.Key("Conditions");
        (void)writer.StartObject();
        for (auto& it: conditions_map_) {
            (void)writer.Key(it.first.c_str());
            (void)writer.StartObject();
            // (void)writer.Key("Enable");
            // (void)writer.Bool(it.second.enabled);
            (void)writer.Key("SetValue");
            WriteStateValue(writer, it.second.target_value);
            (void)writer.Key("MeasureValue");
            WriteStateValue(writer, it.second.current_value);
            (void)writer.EndObject();
        }
        (void)writer.EndObject();
    }

    template <typename OS>
    static void WriteStateValue(rapidjson::Writer<OS>& writer, const Context::StateValue& value) {
        if (std::holds_alternative<bool>(value)) {
            writer.Bool(std::get<bool>(value));
        } else if (std::holds_alternative<int>(value)) {
            writer.Int(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            writer.Double(std::get<double>(value));
        } else {
            writer.String("<UNSPEC>");
        }
    }

private:
    // bool LoadUserAuth(const rapidjson::Value& obj);
    Condition LoadCondition(const rapidjson::Value& obj);

private:
    Context& context_;
    std::string description_;
    ConditionMap conditions_map_;
    // bool user_auth_;
    sigslot::scoped_connection state_connection_;
    size_t not_pass_num_;
    std::vector<Promise<void>> pending_ready_;
};

///////////////////////////////////////////////////////////////////////////////
#if 0
struct CondStr
{
    std::string condname;
    std::string enable;
    std::string measurevalue;
    std::vector<std::string> setvalue;
    int setvaluetype = 0;
};
using MY_COND = std::vector<CondStr>;

class ConditonAbstract
{
public:
    ConditonAbstract() = default;
    virtual ~ConditonAbstract() = default;
    virtual bool ConditionJudgment(const std::string& path, MY_COND& mycond) = 0;
    virtual void SetPackName(const std::string& pn) = 0;

protected:
    ConditonAbstract(const ConditonAbstract& ) = default;
    ConditonAbstract(ConditonAbstract&& ) = delete;
    ConditonAbstract& operator=(const ConditonAbstract& ) = default;
    ConditonAbstract& operator=(ConditonAbstract&& ) = delete;
};

class DecoratorBase : public ConditonAbstract
{
public:
    DecoratorBase(ConditonAbstract* const cd)
        : condition_manager_(cd)
        , work_status_(false)
    {
    }
    virtual ~DecoratorBase() = default;
    virtual void RunCheck() {};
    bool ConditionJudgment(const std::string& , MY_COND& ) override { return true;};
    void SetPackName(const std::string& ) override {};
    virtual void ResultFileUptatd() = 0;

protected:
    std::unique_ptr<ConditonAbstract> condition_manager_;
    bool work_status_;

private:
    DecoratorBase(DecoratorBase& db) = delete;
    DecoratorBase(DecoratorBase&& db) = delete;
    DecoratorBase& operator = (const DecoratorBase& db) = delete;
    DecoratorBase& operator = (DecoratorBase&& db) = delete;
};

class ConditionDecorator : public DecoratorBase
{
public:
    ConditionDecorator(ConditonAbstract* const cd);
    virtual ~ConditionDecorator() noexcept override;
    virtual bool ConditionJudge(const std::string& path);
    void SetPackName(const std::string& pn) override;
    void SetLocationField(const char* const location);
    void SetReleaseNumber(const char* const release);
    void SetSessionID(const char* const sessiondi);
    void ResultFileUptatd() override;
    void ClearLastFile() const;

private:
    ConditionDecorator(ConditionDecorator& cd) = delete;
    ConditionDecorator(ConditionDecorator&& cd) = delete;
    ConditionDecorator& operator = (const ConditionDecorator& cd) = delete;
    ConditionDecorator& operator = (ConditionDecorator&& cd) = delete;

private:
    void PrintResultJson(MY_COND& cond);
    void GetVehicleConfigData();
    void PrintVehicleConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    static void CreateStart(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    static void CreateEnd(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void PrintResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, MY_COND& cond);
    void PrintCloudInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    void PrintConditonInfo(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, MY_COND& cond) const;
    static void PrintBlocks(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    bool ResultFileNeedUpdate() const;

private:
    std::string veh_config_data_path_;
    std::string script_parameter_path_;
    std::string temporary_result_path_;
    std::string result_json_;
    std::string location_field_;
    std::string release_number_field_;
    std::string session_id_field_;
    std::string vin_field_;
    std::string mtoc_field_;
    std::vector<std::string> option_code_;
    std::string time_stamp_;
    std::string pack_name_;
    int not_pass_num_;
};

class ConditionMG : public ConditonAbstract
{
public:
    explicit ConditionMG();
    virtual ~ConditionMG() = default;

private:
    ConditionMG(ConditionMG& cm) = delete;
    ConditionMG(ConditionMG&& cm) = delete;
    ConditionMG& operator = (const ConditionMG& cm) = delete;
    ConditionMG& operator = (ConditionMG&& cm) = delete;

public:
    bool ConditionJudgment(const std::string& path, MY_COND& condition) override;
    inline void SetPackName(const std::string& pn) override
    {
        pack_name_ = pn;
    }

private:
    bool GetCondition(const std::string& path, MY_COND& condition, ConditionsDataSet& result);
    void DoConditonGet(const std::string& path, MY_COND& condition, ConditionsDataSet& result);
    void ReadConditionData(const rapidjson::Document& doc, MY_COND& vehcond, ConditionsDataSet& result);
    void ResultParas(CondStr& condtion, ConditionsDataSet& result);

    void DoGetChcekData(ConditionsDataSet& result);

private:
    std::string pack_name_;
    std::set<std::string> VehMode_list_;
    std::set<std::string> UsageMode_list_;

};

#endif

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_CONDITION_H
