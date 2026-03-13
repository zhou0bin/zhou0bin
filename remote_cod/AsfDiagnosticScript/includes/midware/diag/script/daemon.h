//
//
#ifndef ASF_DIAGNOSTIC_SCRIPT_DAEMON_H
#define ASF_DIAGNOSTIC_SCRIPT_DAEMON_H

#include <map>
#include <memory>
#include <string>

#include <domain_socket/server.h>

#include <midware/diag/script/manager.h>

namespace midware::diag::script {

class Daemon: public Manager {
public:
    Daemon(Poller& poller);

    void Init() override;
    
    // Future<bool> AsyncUserAuth(const std::string& message) override;

    void UpdataState(const std::string& message);

    StateValue GetState(const std::string& key) override;

    void NotifyState();

    struct st_DiagCheckVehicleService{
        bool ignitionSwitchStatus;
        bool EPBStatus;
        bool gearPStatus;
        bool nonForbiddenArea;
        int vehicleSpeed;
        int batteryLevel;
        int engineSpeed;
    };

    st_DiagCheckVehicleService DeserializeDiagCheckVehicleService(const std::string& message);

private:
    using Cache = std::map<std::string, StateValue>;

    std::string check_vehicle_service_server_sock_{"/tmp/DiagCheckVehicleService.sock"};
    DomainSocket::Server check_vehicle_service_server_{check_vehicle_service_server_sock_};
    Cache cache_;
};

} // namespace midware::diag::script

#endif // ASF_DIAGNOSTIC_SCRIPT_DAEMON_H
