#ifndef UDS_INF_ASYNC_UDS_RESPONSE_MANAGER_H__
#define UDS_INF_ASYNC_UDS_RESPONSE_MANAGER_H__

#include "async_uds_response_processor.h"

namespace midware
{
namespace diag
{

class AsyncResponseProcessorManager
{
public:
    using AsyncResponseProcessorMap = std::map<uint16_t, AsyncResponseProcessor::Ptr>;

public:
    static AsyncResponseProcessorManager& GetInstance()
    {
        static AsyncResponseProcessorManager instance;
        return instance;
    }

    AsyncResponseProcessor::Ptr FindOrCreateAsyncResponseProcessor(const uint16_t diagnostic_address);

private:
    explicit AsyncResponseProcessorManager();
    virtual ~AsyncResponseProcessorManager() = default;

private:
    AsyncResponseProcessorMap async_uds_response_processor_map_;

    std::mutex mutex_;

};

}
}

#endif // UDS_INF_ASYNC_UDS_RESPONSE_MANAGER_H__
