//
//
#ifndef MIDWARE_DIAG_SCRIPT_VEHICLE_DATA_H
#define MIDWARE_DIAG_SCRIPT_VEHICLE_DATA_H

#include <string>

#include <midware/diag/script/common.h>

namespace midware::diag::script {

class VehicleData final {
public:
    VehicleData(const std::string& path);

public:
    const std::string& GetFactory() const noexcept {
        return factory_;
    }

    const std::string& GetVin() const noexcept {
        return vin_;
    }

    const std::string& GetMtoc() const noexcept {
        return mtoc_;
    }

    const std::string& GetLocation() const noexcept {
        return location_;
    }

    const std::string& GetTpms() const noexcept {
        return tpms_;
    }

    const std::string& GetPin() const noexcept {
        return pin_;
    }

    const std::string& GetEsk() const noexcept {
        return esk_;
    }

    template <typename OS>
    void Dump(rapidjson::Writer<OS>& writer) const {
        (void)writer.Key("VIN");
        (void)writer.String(vin_.c_str());
        //(void)writer.Key("TimeStamp");
        //(void)writer.String(time_stamp_.c_str());
        (void)writer.Key("MT");
        (void)writer.String(mtoc_.c_str());
        (void)writer.Key("OptionCodes");
        (void)writer.StartArray();
        for (const auto& item : las_) {
            (void)writer.String(item.c_str());
        }
        (void)writer.EndArray();
    }

private:
    std::string factory_;
    std::string vin_;
    std::string mtoc_;
    std::string location_;
    std::string tpms_;
    std::string pin_;
    std::string esk_;
    std::vector<std::string> las_;
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_VEHICLE_DATA_H
