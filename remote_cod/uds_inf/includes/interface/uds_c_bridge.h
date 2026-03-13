#ifndef UDS_C_BRIDGE_H
#define UDS_C_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

// 定义返回码类型（与底层UdsResult对齐，int类型）
typedef int UdsRet;

// 定义字节数组结构体（传递std::vector<uint8_t>，兼容C/ctypes）
typedef struct {
    uint8_t* data;  // 数据首地址
    size_t len;     // 数据长度
} UdsBuffer;

// 1. 初始化UDS接口（创建UdsInterfacePy实例）
int Uds_Init(const char* config_path);

// 2. 连接/断开
int Uds_Connect();
int Uds_Disconnect();

// 3. 收发UDS报文（带响应校验），输出结果到out_buf，返回码为UdsRet
UdsRet Uds_TransmitAndReceive(uint16_t src_addr, uint16_t dst_addr, 
                              const UdsBuffer* req_buf, const UdsBuffer* target_resp,
                              UdsBuffer* out_buf);

// 4. 仅发送UDS报文（普通）
UdsRet Uds_TransmitMessage(uint16_t src_addr, uint16_t dst_addr, const UdsBuffer* req_buf);

// 5. 功能寻址发送UDS报文
UdsRet Uds_FunctionalTransmitMessage(uint16_t src_addr, uint16_t dst_addr, const UdsBuffer* req_buf);

// 6. 启动/停止TestPresent
int Uds_StartTestPresent(uint16_t src_addr, uint16_t dst_addr, int is_suppress, int period);
int Uds_StopTestPresent(uint16_t src_addr, uint16_t dst_addr);

// 7. 释放UdsBuffer的内存（防止泄漏，由Python调用）
void Uds_FreeBuffer(UdsBuffer* buf);

#ifdef __cplusplus
}
#endif

#endif // UDS_C_BRIDGE_H
