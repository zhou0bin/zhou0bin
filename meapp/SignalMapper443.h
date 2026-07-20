#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "singleApi/cdc_15_443.h"
#include <nlohmann/json.hpp>
#include <qjsonvalue.h>

class SignalMapper443
{
public:
    static bool SetSignal(
        Cdc_15_443& msg,
        const std::string& signalName,
        const QJsonValue& value);
};
