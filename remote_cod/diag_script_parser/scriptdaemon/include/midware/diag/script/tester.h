//
//
#ifndef MIDWARE_DIAG_SCRIPT_TESTER_H
#define MIDWARE_DIAG_SCRIPT_TESTER_H

#include <string>
#include <thread>

#include <midware/diag/script/common.h>

namespace midware::diag::script {

class Tester {
public:
    Tester(const std::string& socket_path);
    virtual ~Tester();

private:
    void Thread(std::string socket_path);

private:
    std::thread thread_;
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_TESTER_H
