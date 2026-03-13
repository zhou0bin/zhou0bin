//
// Created by kevin on 23-2-13.
//

#pragma once

#include "midware/log_wrapper/logging_wrapper.h"

#define LOGERROR   Logger().Error() 
#define LOGINFO    Logger().Info() 
#define LOGWARN    Logger().Warn() 
#define LOGDEBUG   Logger().Debug() 

static inline middleware::logwrapper::LoggerWrapper& Logger()
{
    static middleware::logwrapper::LoggerWrapper logger_instance("VIDT", "VireDiagTester log Context", middleware::logwrapper::LogLevel::kVerbose);
    return logger_instance;
}

#if 0
#ifdef GLOG
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>
#else
#ifdef NEUSAR_AP_R2111
#include "ara/log/logger.h"
#else
#include "ara/log/logging.h"
#endif
#endif

#ifdef GLOG

#define LOGINFO  LOG(INFO)
#define LOGWARN  LOG(WARNING)
#define LOGERROR LOG(ERROR)

#else

#if !defined(LOGINFO)
#define LOGDEBUG \
        Log().LogDebug()
#endif

#if !defined(LOGINFO)
#define LOGINFO \
        Log().LogInfo()
#endif

#if !defined(LOGWARN)
#define LOGWARN \
        Log().LogWarn()
#endif

#if !defined(LOGERROR)
#define LOGERROR \
        Log().LogError()
#endif

ara::log::Logger& Log();

#endif

void InitLog();
#endif