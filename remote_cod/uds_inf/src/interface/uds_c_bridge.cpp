#include "uds_c_bridge.h"
#include "uds_interface.h"
#include "log.h"
#include <cstdlib>
#include <vector>

using namespace std;

// 全局UDS实例（底层是单例，这里封装成全局，简化C接口调用）
static UdsInterface* g_uds_instance = nullptr;

// 辅助函数：C的UdsBuffer转C++的vector<uint8_t>
static vector<uint8_t> UdsBuffer2Vector(const UdsBuffer* buf) {
    if (buf == nullptr || buf->data == nullptr || buf->len == 0) {
        return {};
    }
    return vector<uint8_t>(buf->data, buf->data + buf->len);
}

// 辅助函数：C++的vector<uint8_t>转C的UdsBuffer（分配堆内存，需外部释放）
static void Vector2UdsBuffer(const vector<uint8_t>& vec, UdsBuffer* buf) {
    if (buf == nullptr) return;
    buf->len = vec.size();
    if (vec.empty()) {
        buf->data = nullptr;
        return;
    }
    // 分配堆内存，Python端调用Uds_FreeBuffer释放
    buf->data = (uint8_t*)malloc(vec.size() * sizeof(uint8_t));
    if (buf->data) {
        memcpy(buf->data, vec.data(), vec.size());
    }
}

// 1. 初始化UDS
int Uds_Init(const char* config_path) {
    if (g_uds_instance != nullptr) {
        LOG_INFO << "UDS instance already initialized";
        return 0; // 成功
    }
    if (config_path == nullptr || *config_path == '\0') {
        LOG_ERROR << "UDS config path is empty";
        return -1; // 失败
    }
    try {
        g_uds_instance = new UdsInterface(config_path);
        return 0; // 0=成功，非0=失败
    } catch (...) {
        LOG_ERROR << "UDS init failed";
        return -2;
    }
}

// 2. 连接
int Uds_Connect() {
    if (g_uds_instance == nullptr) {
        LOG_ERROR << "UDS instance not initialized";
        return 0; // 0=失败（与C++ bool对齐：true=1，false=0）
    }
    return g_uds_instance->Connect() ? 1 : 0;
}

// 2. 断开
int Uds_Disconnect() {
    if (g_uds_instance == nullptr) {
        LOG_ERROR << "UDS instance not initialized";
        return 0;
    }
    return g_uds_instance->Disconnect() ? 1 : 0;
}

// 3. 收发报文（带响应校验）
UdsRet Uds_TransmitAndReceive(uint16_t src_addr, uint16_t dst_addr, 
                              const UdsBuffer* req_buf, const UdsBuffer* target_resp,
                              UdsBuffer* out_buf) {
    if (g_uds_instance == nullptr || req_buf == nullptr || out_buf == nullptr) {
        LOG_ERROR << "UDS instance or buffer null";
        return 0;
    }
    // 初始化输出缓冲区，防止野指针
    out_buf->data = nullptr;
    out_buf->len = 0;
    try {
        vector<uint8_t> req = UdsBuffer2Vector(req_buf);
        vector<uint8_t> target = UdsBuffer2Vector(target_resp);
        auto [ret, resp] = g_uds_instance->UdsTransmitAndReceive(src_addr, dst_addr, req, target);
        // 结果写入输出缓冲区
        Vector2UdsBuffer(resp, out_buf);
        return ret;
    } catch (...) {
        LOG_ERROR << "UDS TransmitAndReceive failed";
        return 0;
    }
}

// 4. 仅发送报文
UdsRet Uds_TransmitMessage(uint16_t src_addr, uint16_t dst_addr, const UdsBuffer* req_buf) {
    if (g_uds_instance == nullptr || req_buf == nullptr) {
        LOG_ERROR << "UDS instance or req buffer null";
        return 0;
    }
    try {
        vector<uint8_t> req = UdsBuffer2Vector(req_buf);
        return g_uds_instance->UdsTransmitMessage(src_addr, dst_addr, req);
    } catch (...) {
        LOG_ERROR << "UDS TransmitMessage failed";
        return 0;
    }
}

// 5. 功能寻址发送
UdsRet Uds_FunctionalTransmitMessage(uint16_t src_addr, uint16_t dst_addr, const UdsBuffer* req_buf) {
    if (g_uds_instance == nullptr || req_buf == nullptr) {
        LOG_ERROR << "UDS instance or req buffer null";
        return 0;
    }
    try {
        vector<uint8_t> req = UdsBuffer2Vector(req_buf);
        return g_uds_instance->UdsFunctionalTransmitMessage(src_addr, dst_addr, req);
    } catch (...) {
        LOG_ERROR << "UDS FunctionalTransmitMessage failed";
        return 0;
    }
}

// 6. 启动TestPresent（is_suppress：0=不抑制响应，非0=抑制；period：周期ms）
int Uds_StartTestPresent(uint16_t src_addr, uint16_t dst_addr, int is_suppress, int period) {
    if (g_uds_instance == nullptr) {
        LOG_ERROR << "UDS instance not initialized";
        return 0;
    }
    try {
        return g_uds_instance->UdsStartTestPresent(src_addr, dst_addr, (is_suppress != 0), period) ? 1 : 0;
    } catch (...) {
        LOG_ERROR << "UDS StartTestPresent failed";
        return 0;
    }
}

// 6. 停止TestPresent
int Uds_StopTestPresent(uint16_t src_addr, uint16_t dst_addr) {
    if (g_uds_instance == nullptr) {
        LOG_ERROR << "UDS instance not initialized";
        return 0;
    }
    try {
        return g_uds_instance->UdsStopTestPresent(src_addr, dst_addr) ? 1 : 0;
    } catch (...) {
        LOG_ERROR << "UDS StopTestPresent failed";
        return 0;
    }
}

// 7. 释放缓冲区内存（必须调用，防止内存泄漏）
void Uds_FreeBuffer(UdsBuffer* buf) {
    if (buf == nullptr) return;
    if (buf->data != nullptr) {
        free(buf->data);
        buf->data = nullptr;
    }
    buf->len = 0;
}