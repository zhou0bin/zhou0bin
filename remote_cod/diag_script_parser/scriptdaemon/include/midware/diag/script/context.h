//
//
#ifndef MIDWARE_DIAG_SCRIPT_CONTEXT_H
#define MIDWARE_DIAG_SCRIPT_CONTEXT_H

#include <string>
#include <vector>
#include <variant>
#include <filesystem>
#include <functional>

#include <midware/diag/script/common.h>
#include <midware/diag/script/loop.h>
#include <midware/diag/script/async.h>
#include <midware/diag/script/sigslot.h>
#include <midware/diag/script/vehicle_data.h>

namespace midware::diag::script {

class Context {
public:
    using UserAuthCallback = std::function<void (bool)>;
    using StateValue = std::variant<std::monostate, bool, int, double>;

    enum class ForkState {
        kPending = 0,
        kCommitted,
        kCancelled
    };

    sigslot::signal_st<const std::string&, const StateValue&> OnStateChanged;

public:
    Context() = default;
    virtual ~Context() = default;

public:
    virtual Poller& GetPoller() = 0;
    virtual const VehicleData& GetVehicleData() const = 0;
    virtual const std::string& GetPythonCommand() const = 0;
    virtual const std::vector<std::filesystem::path>& GetScriptPaths() const = 0;
    virtual void SendMessage(ScriptOpc opc, std::vector<uint8_t> payload) = 0;
    virtual Future<int> AsyncFork(int priority) = 0;
    virtual Future<int> AsyncWaitPid(int pid) = 0;
    // virtual Future<bool> AsyncUserAuth(const std::string& message) = 0;
    virtual StateValue GetState(const std::string& key) = 0;
    virtual void HandleTaskStopped(const std::filesystem::path& task_path) = 0;

    void SendMessage(ScriptOpc opc, const std::string& payload) {
        SendMessage(opc, std::vector<uint8_t>(payload.begin(), payload.end()));
    }
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_CONTEXT_H