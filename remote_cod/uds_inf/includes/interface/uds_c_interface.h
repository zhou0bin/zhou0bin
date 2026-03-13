#ifndef UDS_INF_UDS_INTERFACE_CTYPE_H__
#define UDS_INF_UDS_INTERFACE_CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*Callback)(const int source_address, const int target_address, const int size, const unsigned char* data);

void Initialize(const char* data);

void Offer(const int address);

void StopOffer(const int address);

void RegisterCallback(const int address, Callback callback);

bool Request(const int source_address, const int target_address, const int size, const unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif // UDS_INF_UDS_INTERFACE_CTYPE_H__
