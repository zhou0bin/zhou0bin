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

#ifndef DIAG_SCRIPT_PARSER_LOG_H__
#define DIAG_SCRIPT_PARSER_LOG_H__

#include <cstring>

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
#define LOG_FATAL   midware::diag::common::Logger().Fatal() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_ERROR   midware::diag::common::Logger().Error() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_INFO    midware::diag::common::Logger().Info() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_WARN    midware::diag::common::Logger().Warn() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_DEBUG   midware::diag::common::Logger().Debug() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_VERBOSE midware::diag::common::Logger().Verbose() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#else
#define LOG_FATAL   midware::diag::common::Logger().LogFatal() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_ERROR   midware::diag::common::Logger().LogError() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_INFO    midware::diag::common::Logger().LogInfo() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_WARN    midware::diag::common::Logger().LogWarn() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_DEBUG   midware::diag::common::Logger().LogDebug() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#define LOG_VERBOSE midware::diag::common::Logger().LogVerbose() << midware::diag::common::GetFileName(__FILE__) << ":" << __LINE__ << "|" << __func__
#endif

namespace midware
{
namespace diag
{
namespace common
{

#ifdef USE_MIDWARE_LOG
static inline middleware::logwrapper::LoggerWrapper& Logger()
{
    // static middleware::logwrapper::LoggerWrapper logger_instance{ara::log::CreateLogger("SFDS", "DiagScriptParser log Context")};
    static middleware::logwrapper::LoggerWrapper logger_instance("SFDS", "DiagScriptParser log Context", middleware::logwrapper::LogLevel::kVerbose);
    return logger_instance;
}
#else
static inline ara::log::Logger& Logger()
{
    static ara::log::Logger& logger_instance{ara::log::CreateLogger("SFDS", "DiagScriptParser log Context")};
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

} // common
} // diag
} // midware

#endif  // DIAG_SCRIPT_PARSER_LOG_H__
