//
//
#include <filesystem>

#include <midware/diag/script/common.h>
#include <midware/diag/script/log.h>

namespace midware::diag::script {

static bool initialized_{false};
static bool is_secure_startup_{false};
static rapidjson::Document config_doc_{};

const std::string kConfigPath{"/opt/DiagnosticScript/libs/util/DiagParsingConfig.json"};
const std::string kSecureStartupConfigPath{"/opt/DiagnosticScript/libs/util/SecureDiagParsingConfig.json"};
const std::string kConditionChekPath{"/opt/DiagnosticScript/conditioncheck/condition_check.json"};
const std::string kVehicleDataPath{"/opt/DiagnosticScript/vehicleData/vehicle_data.json"};
const std::string kScriptParameterPath{"/opt/DiagnosticScript/vehicleData/script_parameter.json"};
const std::string kTempCheckResultPath{"/opt/DiagnosticScript/result/tempcheckresult.json"};
const std::string kDiagResultPath{"/opt/DiagnosticScript/result/DiagResult.json"};

void GlobalInitialize()
{
#ifdef SECURE_STARTUP
    if (!initialized_)
    {
        const std::string path = GetRootPath() + kSecureStartupConfigPath;
        std::fstream file(path, std::ios::in);
        if (file.is_open())
        {
            const std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            config_doc_.Parse(buffer.c_str());
            initialized_ = true;
        }
    }
    std::fstream flag("/data/SecureFlag", std::ios::out);
    is_secure_startup_ = true;
#else
    if (std::filesystem::exists("/data/SecureFlag"))
    {
        (void)std::filesystem::remove("/data/SecureFlag");
    }
#endif
}

bool Initialized()
{
    return initialized_;
}

bool IsSecureStartup()
{
    return is_secure_startup_;
}

std::string GetRootPath()
{
    static std::string path = [] {
        const char* path_env = getenv("NEUSAR_DEPLOYMENT_PATH");
        if (path_env != nullptr) {
            return std::string(path_env);
        }
        return std::string();
    }();
    return path;
}

std::string GetConfigPath()
{
    std::string path;
    if (IsSecureStartup())
    {
        path = GetRootPath() + kSecureStartupConfigPath;
    }
    else
    {
        path = GetRootPath()+ kConfigPath;
    }
    return path;
}

std::string GetStopFlagPath()
{
    std::string stop_flag;
    if (IsSecureStartup())
    {
        stop_flag = GetConfig("StopFlag");
    }
    else
    {
        stop_flag = "/tmp/asf_ds_stop_flag.txt";
    }
    return stop_flag;
}

std::string GetCloudScriptPath()
{
    std::string script_path;
    if (IsSecureStartup())
    {
        script_path = GetConfig("Inet_Script_Path");
    }
    else
    {
        if (!ParseJson<std::string>(GetConfigPath().c_str(), "Inet_Script_Path", script_path))
        {
            LOG_ERROR << "Get Inet Script Path Faile!";
        }
    }
    return script_path;
}

std::string GetLocalScriptPath()
{
    std::string script_path;
    if (IsSecureStartup())
    {
        script_path = GetConfig("Local_Script_Path");
    }
    else
    {
        if (!ParseJson<std::string>(GetConfigPath().c_str(), "Local_Script_Path", script_path))
        {
            LOG_ERROR << "Get Local Script Path Faile!";
        }
    }
    return script_path;
}

bool FileAndDirInit()
{
    try
    {
        if (!std::filesystem::exists(GetConfig("DiagScriptPackagePath")))
        {
            LOG_ERROR << "No " << GetConfig("DiagScriptPackagePath");
            (void)std::filesystem::create_directory(GetConfig("DiagScriptPackagePath"));
        }
        if (!std::filesystem::exists(GetConfig("DLogPath")))
        {
            LOG_ERROR << "No " << GetConfig("DLogPath");
            (void)std::filesystem::create_directory(GetConfig("DLogPath"));
        }
        if (!std::filesystem::exists(GetConfig("VehDataDir")))
        {
            LOG_ERROR << "No " << GetConfig("VehDataDir");
            (void)std::filesystem::create_directory(GetConfig("VehDataDir"));
        }
        if (!std::filesystem::exists(GetConfig("ConditionCheckPath")))
        {
            LOG_ERROR << "No " << GetConfig("ConditionCheckPath");
            (void)std::filesystem::create_directory(GetConfig("ConditionCheckPath"));
        }
        if (!std::filesystem::exists(GetConfig("EngineTimeParameterDir")))
        {
            LOG_ERROR << "No " << GetConfig("EngineTimeParameterDir");
            (void)std::filesystem::create_directory(GetConfig("EngineTimeParameterDir"));
        }
        if (!std::filesystem::exists(GetConfig("VehicleConDir")))
        {
            LOG_ERROR << "No " << GetConfig("VehicleConDir");
            (void)std::filesystem::create_directory(GetConfig("VehicleConDir"));
        }
        return true;
    }
    catch (std::exception &e)
    {
        LOG_ERROR << "Excrpt " << e.what();
        return false;
    }
}

void FileMoveToData(const std::string& sourcepath)
{
    bool has_cond = false;
    bool has_veh = false;
    bool has_sp = false;
    for (const auto& ele : std::filesystem::directory_iterator(sourcepath))
    {
        if (std::filesystem::is_regular_file(ele.status()))
        {
            const auto filename = ele.path().filename().string();
            if (filename == "vehicle_data.json")
            {
                std::filesystem::copy_file(ele.path().string(), GetConfig("VehDataPath"), std::filesystem::copy_options::overwrite_existing);
                has_veh = true;
            }
            else if (filename == "script_parameter.json")
            {
                std::filesystem::copy_file(ele.path().string(), GetConfig("ScriptParam"), std::filesystem::copy_options::overwrite_existing);
                has_sp = true;
            }
            else if (filename == "condition_check.json")
            {
                std::filesystem::copy_file(ele.path().string(), GetConfig("ConditionFIle"), std::filesystem::copy_options::overwrite_existing);
                has_cond = true;
            }
        }
    }
    if (!has_cond)
    {
        LOG_ERROR << "Package No condition_check.json";
        if (std::filesystem::exists(GetConfig("ConditionFIle")))
        {
            LOG_ERROR << "Use Last condition_check.json";
        }
        else
        {
            LOG_ERROR << "Use Default condition_check.json";
            std::filesystem::copy_file(GetRootPath() + GetConfig("ConditionDefaultFIle"), GetConfig("ConditionFIle"), std::filesystem::copy_options::overwrite_existing);
        }
    }
    if (!has_veh)
    {
        LOG_ERROR << "Package No vehicle_data.json";
        LOG_ERROR << "Use Default vehicle_data.json ";
        std::filesystem::copy_file(GetRootPath() + GetConfig("VehDataDefault"), GetConfig("VehDataPath"), std::filesystem::copy_options::overwrite_existing);
    }
    if (!has_sp)
    {
        LOG_ERROR << "Package No script_parameter.json";
    }
}

void GetScriptFilePath(const std::string& py, const std::string& guid, std::string& path)
{
    if (IsSecureStartup())
    {
        path = py + "/" + guid + "/" + guid + ".py";
    }
    else
    {
        path = GetRootPath() + "/opt" + py + "/" + guid + "/" + guid + ".py";
    }
}

bool GetConditionFilePath(const std::string& py, const std::string& guid, std::string& path)
{
    if (IsSecureStartup())
    {
        if (!FileAndDirInit())
        {
            return false;
        }
        const std::string sourcepath{py + "/" + guid};
        FileMoveToData(sourcepath);
        path = GetConfig("ConditionFIle");
    }
    else
    {
        path = GetRootPath() + "/opt" + py + "/" + guid + "/condition_check.json";
    }
    return true;
}

void GetBackupConditionFilePath(std::string& path)
{
    path = GetRootPath() + kConditionChekPath;
}

std::string GetVehicleDataPath() {
    if (IsSecureStartup() && Initialized()) {
        return GetConfig("VehDataPath");
    }
    return GetRootPath() + kVehicleDataPath;
}

std::string GetDiagResultPath() {
    if (IsSecureStartup() && Initialized()) {
        return GetConfig("DiagResultLogPath");
    }
    return GetRootPath() + kDiagResultPath;
}

void GetConditionPath(std::string& vehicle_data, std::string& script_parameter, std::string& tmp_check_result, std::string& diag_result)
{
    if (IsSecureStartup() && Initialized())
    {
        vehicle_data = GetConfig("VehDataPath");
        script_parameter = GetConfig("ScriptParam");
        tmp_check_result = GetConfig("TmpCheckResult");
        diag_result = GetConfig("DiagResultLogPath");
    }
    else
    {
        vehicle_data = GetRootPath() + kVehicleDataPath;
        script_parameter = GetRootPath() + kScriptParameterPath;
        tmp_check_result = GetRootPath() + kTempCheckResultPath;
        diag_result = GetRootPath() + kDiagResultPath;
    }
}

std::string GetConfig(const char* const key)
{
    std::string _value;
    if (config_doc_[key].IsString())
    {
        _value = config_doc_[key].GetString();
    }
    return _value;
}

std::vector<uint8_t> MakeMessage(const std::string& str, const uint8_t type, const uint8_t result)
{
    const uint8_t len = static_cast<uint8_t>(str.size() + 3U);
    std::vector<uint8_t> msg{};
    msg.push_back(len);
    msg.push_back(type);
    msg.push_back(result);
    (void)msg.insert(msg.cend(), str.begin(), str.end());
    return msg;
}

void DisplaySendData(const std::vector<uint8_t>& msg)
{
    if (msg.size() < 3U)
    {
        LOG_ERROR << "msg size error! size: " << msg.size();
        return;
    }
    const uint8_t len = msg[0U];
    const uint8_t type = msg[1U];
    const uint8_t result = msg[2U];
    const std::string str(msg.begin() + 3, msg.end());
    LOG_INFO << ": " << static_cast<unsigned>(len) << " " << static_cast<unsigned>(type) << " " << static_cast<unsigned>(result) << " " << str;
}

ssize_t Write(const socket_fd sockfd, const std::vector<uint8_t>& msg)
{
    const ssize_t bytes = write(sockfd, &msg[0U], msg.size());
    if (bytes == 0)
    {
        LOG_WARN << "write error! socket closed! fd: " << sockfd;
    }
    else if (bytes < 0)
    {
        LOG_WARN << "write error! strerror: " << strerror(errno);
    }
    else
    {
        LOG_INFO << "write success! fd: " << sockfd << "bytes: " << bytes;
    }
    return bytes;
}

void Write(const socket_fd sockfd, unsigned char * buff, size_t count)
{
    const ssize_t bytes = write(sockfd, buff, count);
    if (bytes == 0)
    {
        LOG_WARN << "write error! socket closed! fd: " << sockfd;
    }
    else if (bytes < 0)
    {
        LOG_WARN << "write error! strerror: " << strerror(errno);
    }
    else
    {
        LOG_INFO << "write success! fd: " << sockfd << "bytes: " << bytes;
    }
    return;
}

ssize_t Read(const socket_fd sockfd, std::vector<uint8_t>& msg)
{
    const ssize_t bytes = read(sockfd, &msg[0U], msg.size());
    if (bytes == 0)
    {
        LOG_WARN << "read error! bytes is 0. fd: " << sockfd;
    }
    else if (bytes < 0)
    {
        LOG_WARN << "read error! strerror: " << strerror(errno);
    }
    else
    {
        LOG_INFO << "read success! fd: " << sockfd << "bytes: " << bytes;
    }
    return bytes;
}

ssize_t Read(const socket_fd sockfd, unsigned char * buff, size_t count)
{
    const ssize_t bytes = read(sockfd, buff, count);
    if (bytes == 0)
    {
        LOG_WARN << "read error! bytes is 0. fd: " << sockfd;
    }
    else if (bytes < 0)
    {
        LOG_WARN << "read error! strerror: " << strerror(errno);
    }
    else
    {
        LOG_INFO << "read success! fd: " << sockfd << "bytes: " << bytes;
    }
    return bytes;
}

void Close(const socket_fd sockfd)
{
    const int err = close(sockfd);
    if (err < 0)
    {
        LOG_WARN << "close error! strerror: " << strerror(errno);
    }
    else
    {
        LOG_INFO << "close success! fd: " << sockfd;
    }
}

} // namespace midware::diag::script
