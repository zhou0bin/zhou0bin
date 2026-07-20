#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "singleApi/cdc_control1_680.h"
#include <nlohmann/json.hpp>
#include <qjsonvalue.h>

class SignalMapper680
{
public:
    static bool SetSignal(
        Cdc_control1_680& msg,
        const std::string& signalName,
        const QJsonValue& value);
};
