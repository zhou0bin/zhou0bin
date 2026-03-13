//
#include <string>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <midware/diag/script/log.h>
#include <midware/diag/script/vehicle_data.h>

namespace midware::diag::script {

VehicleData::VehicleData(const std::string& path) {
    LOG_INFO << "Loading vehicle data from " << path << "...";
    std::ifstream ifs(path);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (!doc.IsObject()) {
        LOG_ERROR << "The vehicle data file is not a object.";
        throw std::runtime_error("Invalid vehicle data file: not a object.");
    }

    auto vin_it = doc.FindMember("VIN");
    if (vin_it == doc.MemberEnd()) {
        LOG_ERROR << "No vin in vehicle data file.";
        throw std::runtime_error("Invalid vehicle data file: no vin.");
    }
    if (!vin_it->value.IsString()) {
        LOG_ERROR << "Invalid vin type in vehicle data file.";
        throw std::runtime_error("Invalid vehicle data file: invalid vin type.");
    }
    vin_ = vin_it->value.GetString();
    LOG_INFO << "VIN: " << vin_;

    LOG_INFO << "Loading MTOC...";
    auto mtoc_it = doc.FindMember("MTOC");
    if (mtoc_it != doc.MemberEnd()) {
        if (mtoc_it->value.IsString()) {
            mtoc_ = mtoc_it->value.GetString();
            LOG_INFO << "MTOC: " << mtoc_;
        } else {
            LOG_ERROR << "Invalid mtoc type in vehicle data file.";
            throw std::runtime_error("Invalid vehicle data file: invalid mtoc type.");
        }
    } else {
        LOG_WARN << "No MTOC in vehicle data file.";
    }

    LOG_INFO << "Loading LOCATION...";
    auto location_it = doc.FindMember("LOCATION");
    if (location_it != doc.MemberEnd()) {
        if (location_it->value.IsString()) {
            location_ = location_it->value.GetString();
            LOG_INFO << "LOCATION: " << location_;
        } else {
            LOG_ERROR << "Invalid location type in vehicle data file.";
            throw std::runtime_error("Invalid vehicle data file: invalid location type.");
        }
    } else {
        LOG_WARN << "No LOCATION in vehicle data file.";
    }

    LOG_INFO << "Loading TPMS...";
    auto tpms_it = doc.FindMember("TPMS");
    if (tpms_it != doc.MemberEnd()) {
        if (tpms_it->value.IsString()) {
            tpms_ = tpms_it->value.GetString();
            LOG_INFO << "TPMS: " << tpms_;
        } else {
            LOG_ERROR << "Invalid tpms type in vehicle data file.";
            throw std::runtime_error("Invalid vehicle data file: invalid tpms type.");
        }
    } else {
        LOG_WARN << "No TPMS in vehicle data file.";
    }

    LOG_INFO << "Loading PIN...";
    auto pin_it = doc.FindMember("PIN");
    if (pin_it != doc.MemberEnd()) {
        if (pin_it->value.IsString()) {
            pin_ = pin_it->value.GetString();
            LOG_INFO << "PIN: " << pin_;
        } else {
            LOG_ERROR << "Invalid pin type in vehicle data file.";
            throw std::runtime_error("Invalid vehicle data file: invalid pin type.");
        }
    } else {
        LOG_WARN << "No PIN in vehicle data file.";
    }

    LOG_INFO << "Loading ESK...";
    auto esk_it = doc.FindMember("ESK");
    if (esk_it != doc.MemberEnd()) {
        if (esk_it->value.IsString()) {
            esk_ = esk_it->value.GetString();
            LOG_INFO << "ESK: " << esk_;
        } else {
            LOG_ERROR << "Invalid esk type in vehicle data file.";
            throw std::runtime_error("Invalid vehicle data file: invalid esk type.");
        }
    } else {
        LOG_WARN << "No ESK in vehicle data file.";
    }
}

} // namespace midware::diag::script
