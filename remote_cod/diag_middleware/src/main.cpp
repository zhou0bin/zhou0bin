#include "diag_middleware/diagnostic_middleware.h"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string config_path = "conf/middleware.conf";
    std::string inbox_path;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        } else if (arg == "--inbox" && i + 1 < argc) {
            inbox_path = argv[++i];
        }
    }

    auto config = diag::LoadConfig(config_path);
    diag::DiagnosticMiddleware middleware(config);

    std::cout << "diag_middleware started with config: " << config_path << '\n';

    if (!inbox_path.empty()) {
        std::ifstream ifs(inbox_path);
        if (!ifs.is_open()) {
            std::cerr << "failed to open inbox: " << inbox_path << '\n';
            return 1;
        }

        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            std::cout << "REQ  " << line << '\n';
            std::cout << "RESP " << middleware.HandleMessage(line) << '\n';
        }
        return 0;
    }

    std::cout << "Input message format: ACTION=...;ECU=...;IMAGE=..." << '\n';
    std::cout << "Example: ACTION=FLASH_ECU;ECU=BCM;IMAGE=/tmp/bcm.bin" << '\n';

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "QUIT" || line == "quit") {
            break;
        }

        if (line.empty()) {
            continue;
        }

        std::cout << middleware.HandleMessage(line) << '\n';
    }

    return 0;
}
