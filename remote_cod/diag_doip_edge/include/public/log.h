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

#ifndef ASF_DIAG_PROXY_INCLUDE_LOG_H_
#define ASF_DIAG_PROXY_INCLUDE_LOG_H_

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <type_traits>

#ifdef USE_MIDWARE_LOG
#include "midware/log_wrapper/logging_wrapper.h"
#else
#ifdef NEUSAR_AP_R1911
#include "ara/log/logging.h"
#else
#include "ara/log/logger.h"
#endif
#endif

#ifdef LOG_FATAL
#undef LOG_FATAL
#endif
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_WARN
#undef LOG_WARN
#endif
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#ifdef LOG_VERBOSE
#undef LOG_VERBOSE
#endif

#ifdef USE_MIDWARE_LOG
#define LOG_FATAL   asf::diag::common::Logger().Fatal() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_ERROR   asf::diag::common::Logger().Error() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_INFO    asf::diag::common::Logger().Info() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_WARN    asf::diag::common::Logger().Warn() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_DEBUG   asf::diag::common::Logger().Debug() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_VERBOSE asf::diag::common::Logger().Verbose() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_HEX(x)  middleware::logwrapper::HexFormat(x)
#define LOG_RAW_BUFFER(x)  asf::diag::common::GetPrintUds(x)
#else
#define LOG_FATAL   asf::diag::common::Logger().LogFatal() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_ERROR   asf::diag::common::Logger().LogError() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_INFO    asf::diag::common::Logger().LogInfo() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_WARN    asf::diag::common::Logger().LogWarn() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_DEBUG   asf::diag::common::Logger().LogDebug() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_VERBOSE asf::diag::common::Logger().LogVerbose() << asf::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|"
#define LOG_HEX(x)  ara::log::HexFormat(x)
#define LOG_RAW_BUFFER(x)  asf::diag::common::GetPrintUds(x)
#endif

namespace asf
{
namespace diag
{
namespace common
{

#ifdef USE_MIDWARE_LOG
static inline middleware::logwrapper::LoggerWrapper& Logger()
{
    // static middleware::logwrapper::LoggerWrapper logger_instance{ara::log::CreateLogger("SFDP", "DiagDoipEdge log Context")};
    static middleware::logwrapper::LoggerWrapper logger_instance("SFDP", "DiagDoipEdge log Context", middleware::logwrapper::LogLevel::kVerbose);
    return logger_instance;
}
#else
static inline ara::log::Logger& Logger()
{
    static ara::log::Logger& logger_instance{ara::log::CreateLogger("SFDP", "DiagDoipEdge log Context")};
    return logger_instance;
}
#endif

static inline std::string GetFileName(const char* const file_path)
{
    std::string file_name_str{};
    try {
        if (file_path != nullptr) {
            const char* last_slash = strrchr(file_path, '/');
            const char* file_name = (last_slash != nullptr) ? (last_slash + 1) : file_path;
            (void)file_name_str.assign(file_name);
        }
    } catch (...) {
    }
    return file_name_str;
}

static inline std::string GetPrintUds(const std::vector<uint8_t> &uds)
{
    std::string uds_str{};
    size_t len{0U};
    uint8_t sid{0U};
    if (uds.size() > 0)
    {
        sid = uds.at(0);
    }
    switch (static_cast<uint32_t>(sid))
    {
    case 0x10:
    case 0x50:
    case 0x27:
    case 0x67:
    case 0x36:
    case 0x76:
    {
        len = 2U;
        break;
    }
    case 0x22:
    case 0x62:
    case 0x2e:
    case 0x6e:
    {
        len = 3U;
        break;
    }
    case 0x31:
    case 0x71:
    {
        len = 4U;
        break;
    }
    case 0x7f:
    {
        len = 3U;
        break;
    }
    default:
    {
        len = 1U;
        break;
    }
    }
    if (len > uds.size())
    {
        len = uds.size();
    }
    try {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < len; i++)
        {
            ss << std::setw(2) << static_cast<int>(uds[i]) << " ";
        }
        uds_str = ss.str();
    } catch (...) {
    }
    return uds_str;
}

} // common
} // diag
} // asf
#endif  // ASF_DIAG_PROXY_INCLUDE_LOG_H_
