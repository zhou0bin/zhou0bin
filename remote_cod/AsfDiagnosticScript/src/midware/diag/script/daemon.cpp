//
//
#include <midware/diag/script/daemon.h>

namespace midware::diag::script {

Daemon::Daemon(Poller& poller)
    : Manager(poller) 
{
	cache_["EPBStatus"] = true;
	cache_["batteryLevel"] = 40,
	// cache_["diagTaskUserAuth"] = false,
	cache_["engineSpeed"] = 5000,
	cache_["gearPStatus"] = true,
    cache_["ignitionSwitchStatus"] = true,
    cache_["nonForbiddenArea"] = true,
    cache_["vehicleSpeed"] = 5;
	// cache_["EPBStatus"] = false;
	// cache_["batteryLevel"] = 0,
	// // cache_["diagTaskUserAuth"] = false,
	// cache_["engineSpeed"] = 0,
	// cache_["gearPStatus"] = false,
    // cache_["ignitionSwitchStatus"] = false,
    // cache_["nonForbiddenArea"] = false,
    // cache_["vehicleSpeed"] = 0;

    check_vehicle_service_server_.Start();
    DomainSocket::Server::MessageCallback cvs_message_callback = [this](std::string_view msg){
        LOG_INFO << "receive check vehicle service message.";
        this->UpdataState(std::string(msg));
    };
    check_vehicle_service_server_.SetMessageCallback(cvs_message_callback);

    std::thread(&Daemon::NotifyState, this).detach();
}

void Daemon::Init() {
    Manager::Init();
}

// Future<bool> Daemon::AsyncUserAuth(const std::string& message) {
//     return Promise<bool>(true).GetFuture();
// }

void Daemon::UpdataState(const std::string& message) {
    st_DiagCheckVehicleService data = DeserializeDiagCheckVehicleService(message);
    LOG_INFO << "UpdataState ignitionSwitchStatus: " << data.ignitionSwitchStatus;
    LOG_INFO << "UpdataState EPBStatus: " << data.EPBStatus;
    LOG_INFO << "UpdataState gearPStatus: " << data.gearPStatus;
    LOG_INFO << "UpdataState nonForbiddenArea: " << data.nonForbiddenArea;
    LOG_INFO << "UpdataState vehicleSpeed: " << data.vehicleSpeed;
    LOG_INFO << "UpdataState batteryLevel: " << data.batteryLevel;
    LOG_INFO << "UpdataState engineSpeed: " << data.engineSpeed;

    cache_["ignitionSwitchStatus"] = data.ignitionSwitchStatus;
    cache_["EPBStatus"] = data.EPBStatus;
    cache_["gearPStatus"] = data.gearPStatus;
    cache_["nonForbiddenArea"] = data.nonForbiddenArea;
    cache_["vehicleSpeed"] = data.vehicleSpeed;
    cache_["batteryLevel"] = data.batteryLevel;
    cache_["engineSpeed"] = data.engineSpeed;
}

Daemon::st_DiagCheckVehicleService Daemon::DeserializeDiagCheckVehicleService(const std::string& message){
    st_DiagCheckVehicleService data;
    std::vector<uint8_t> recv_data(message.begin(), message.end());
    data.ignitionSwitchStatus = recv_data[0];
    data.EPBStatus = recv_data[1];
    data.gearPStatus = recv_data[2];
    data.nonForbiddenArea = recv_data[3];
    data.vehicleSpeed = recv_data[4];
    data.batteryLevel = recv_data[5];
    //电机转数用uint8_t存储不够，所以用uint16_t存储
    data.engineSpeed = recv_data[6] << 8 | recv_data[7];
    return data;
}

Daemon::StateValue Daemon::GetState(const std::string& key) {
    LOG_INFO << "Getting state for key: " << key;
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }
    return StateValue();
}

void Daemon::NotifyState() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        for(auto data: cache_){
            LOG_INFO << "Sending state for key: " << data.first;
            OnStateChanged(data.first, data.second);
        }
    }
}

} // namespace midware::diag::script
