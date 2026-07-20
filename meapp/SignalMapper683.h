#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "singleApi/cdc_control2_683.h"
#include <nlohmann/json.hpp>
#include <qjsonvalue.h>

class SignalMapper683
{
public:
    static bool SetSignal(
        Cdc_control2_683& msg,
        const std::string& signalName,
        const QJsonValue& value);
};

