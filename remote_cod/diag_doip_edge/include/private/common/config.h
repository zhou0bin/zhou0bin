#ifndef ASF_DIAG_PROXY_INCLUDE_COMMON_CONFIG_H_
#define ASF_DIAG_PROXY_INCLUDE_COMMON_CONFIG_H_

#include <stdlib.h>
#include <cstring>

#define PROXY_CONFIG_PATH asf::diag::common::GetConfigPath()

namespace asf
{
namespace diag
{
namespace common
{

static inline std::string GetConfigPath()
{
    std::string path;
    const auto path_env = getenv("NEUSAR_DEPLOYMENT_PATH");
    if (path_env != nullptr) {
        path = path_env;
        if (!path.empty() && path.back() == '/') {
            path.pop_back();
        }
    }
    path += "/opt/AsfDiagnosticProxy/etc/proxy_config.json";
    return path;
}

}
}
}

#endif
