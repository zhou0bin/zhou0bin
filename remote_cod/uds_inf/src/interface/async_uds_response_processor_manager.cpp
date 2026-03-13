#include "async_uds_response_processor_manager.h"

#include "log.h"

namespace midware
{
namespace diag
{

AsyncResponseProcessorManager::AsyncResponseProcessorManager()
    : async_uds_response_processor_map_()
    , mutex_()
{
}

AsyncResponseProcessor::Ptr AsyncResponseProcessorManager::FindOrCreateAsyncResponseProcessor(const uint16_t diagnostic_address)
{
    const std::lock_guard<std::mutex> locker(mutex_);
    const uint16_t key{diagnostic_address};
    AsyncResponseProcessorMap::iterator it(async_uds_response_processor_map_.find(key));
    if (it == async_uds_response_processor_map_.end())
    {
        const std::pair<AsyncResponseProcessorMap::iterator, bool> result{
            async_uds_response_processor_map_.insert({key, std::make_shared<AsyncResponseProcessor>(key)})};
        it = result.first;
    }
    return it->second;
}


}
}
