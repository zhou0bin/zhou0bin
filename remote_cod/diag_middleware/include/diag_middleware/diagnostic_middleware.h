#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace diag {

struct MiddlewareConfig {
    bool allow_remote_flash{true};
    bool allow_flash_all{true};
    std::vector<std::string> ecu_whitelist;
};

class DiagnosticMiddleware {
public:
    explicit DiagnosticMiddleware(MiddlewareConfig config);

    std::string HandleMessage(const std::string& raw_message);

private:
    using MessageMap = std::unordered_map<std::string, std::string>;

    MiddlewareConfig config_;

    static MessageMap ParseMessage(const std::string& raw_message);
    static std::string ToUpper(std::string value);
    bool IsEcuAllowed(const std::string& ecu) const;

    std::string HandleFlashEcu(const MessageMap& msg) const;
    std::string HandleFlashAll(const MessageMap& msg) const;
    std::string HandleReadDtc(const MessageMap& msg) const;
    std::string HandleClearDtc(const MessageMap& msg) const;
};

MiddlewareConfig LoadConfig(const std::string& config_path);

}  // namespace diag
