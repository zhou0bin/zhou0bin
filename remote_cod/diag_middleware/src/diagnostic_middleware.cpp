#include "diag_middleware/diagnostic_middleware.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <utility>

namespace {

std::string Trim(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::vector<std::string> Split(const std::string& input, char delimiter) {
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> out;

    while (std::getline(ss, token, delimiter)) {
        const auto trimmed = Trim(token);
        if (!trimmed.empty()) {
            out.push_back(trimmed);
        }
    }

    return out;
}

std::string Upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

}  // namespace

namespace diag {

DiagnosticMiddleware::DiagnosticMiddleware(MiddlewareConfig config)
    : config_(std::move(config)) {}

std::string DiagnosticMiddleware::HandleMessage(const std::string& raw_message) {
    const auto msg = ParseMessage(raw_message);
    const auto action_iter = msg.find("ACTION");
    if (action_iter == msg.end()) {
        return "ERR missing ACTION";
    }

    const auto action = ToUpper(action_iter->second);

    if (action == "FLASH_ECU") {
        return HandleFlashEcu(msg);
    }
    if (action == "FLASH_ALL") {
        return HandleFlashAll(msg);
    }
    if (action == "READ_DTC") {
        return HandleReadDtc(msg);
    }
    if (action == "CLEAR_DTC") {
        return HandleClearDtc(msg);
    }

    return "ERR unknown ACTION=" + action;
}

DiagnosticMiddleware::MessageMap DiagnosticMiddleware::ParseMessage(const std::string& raw_message) {
    MessageMap out;
    for (const auto& item : Split(raw_message, ';')) {
        const auto pos = item.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        auto key = Trim(item.substr(0, pos));
        auto value = Trim(item.substr(pos + 1));
        if (key.empty() || value.empty()) {
            continue;
        }

        key = ToUpper(key);
        out[key] = value;
    }
    return out;
}

std::string DiagnosticMiddleware::ToUpper(std::string value) {
    return Upper(std::move(value));
}

bool DiagnosticMiddleware::IsEcuAllowed(const std::string& ecu) const {
    if (config_.ecu_whitelist.empty()) {
        return true;
    }

    const auto upper_ecu = ToUpper(ecu);
    for (const auto& allowed : config_.ecu_whitelist) {
        if (ToUpper(allowed) == upper_ecu) {
            return true;
        }
    }
    return false;
}

std::string DiagnosticMiddleware::HandleFlashEcu(const MessageMap& msg) const {
    if (!config_.allow_remote_flash) {
        return "ERR flash denied by config";
    }

    const auto ecu_iter = msg.find("ECU");
    const auto image_iter = msg.find("IMAGE");
    if (ecu_iter == msg.end() || image_iter == msg.end()) {
        return "ERR FLASH_ECU requires ECU and IMAGE";
    }

    if (!IsEcuAllowed(ecu_iter->second)) {
        return "ERR ECU not allowed: " + ecu_iter->second;
    }

    return "OK flash scheduled ECU=" + ecu_iter->second + " IMAGE=" + image_iter->second;
}

std::string DiagnosticMiddleware::HandleFlashAll(const MessageMap& msg) const {
    if (!config_.allow_remote_flash || !config_.allow_flash_all) {
        return "ERR flash_all denied by config";
    }

    const auto image_iter = msg.find("IMAGE");
    if (image_iter == msg.end()) {
        return "ERR FLASH_ALL requires IMAGE";
    }

    return "OK flash-all scheduled IMAGE=" + image_iter->second;
}

std::string DiagnosticMiddleware::HandleReadDtc(const MessageMap& msg) const {
    const auto ecu_iter = msg.find("ECU");
    if (ecu_iter == msg.end()) {
        return "OK read-dtc for all ECUs";
    }

    if (!IsEcuAllowed(ecu_iter->second)) {
        return "ERR ECU not allowed: " + ecu_iter->second;
    }

    return "OK read-dtc ECU=" + ecu_iter->second;
}

std::string DiagnosticMiddleware::HandleClearDtc(const MessageMap& msg) const {
    const auto ecu_iter = msg.find("ECU");
    if (ecu_iter == msg.end()) {
        return "OK clear-dtc for all ECUs";
    }

    if (!IsEcuAllowed(ecu_iter->second)) {
        return "ERR ECU not allowed: " + ecu_iter->second;
    }

    return "OK clear-dtc ECU=" + ecu_iter->second;
}

MiddlewareConfig LoadConfig(const std::string& config_path) {
    MiddlewareConfig config;

    std::ifstream ifs(config_path);
    if (!ifs.is_open()) {
        return config;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        const auto key = Upper(Trim(line.substr(0, pos)));
        const auto value = Trim(line.substr(pos + 1));

        if (key == "ALLOW_REMOTE_FLASH") {
            config.allow_remote_flash = (Upper(value) == "TRUE");
        } else if (key == "ALLOW_FLASH_ALL") {
            config.allow_flash_all = (Upper(value) == "TRUE");
        } else if (key == "ECU_WHITELIST") {
            config.ecu_whitelist = Split(value, ',');
        }
    }

    return config;
}

}  // namespace diag
