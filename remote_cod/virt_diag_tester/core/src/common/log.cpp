#include "common/log.h"
#if 0
#if !defined(GLOG)
ara::log::Logger& Log()
{
    static ara::log::Logger& logger_instance{ara::log::CreateLogger("SFDT", "Interstellar")};
    return logger_instance;
}
#endif

void InitLog()
{
#ifdef GLOG
    google::InitGoogleLogging("VDE");
    google::SetStderrLogging(google::WARNING); 
#endif
}
#endif