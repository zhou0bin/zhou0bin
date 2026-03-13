// --------------------------------------------------------------------------
// |                _     _              _____         _____                |
// |               |  \  | |            / ____|  /\   |  __ \               |
// |               | | \ | |  __       | (___   /  \  | |__) |              |
// |               | |\ \| | /__\|   |  \___ \ / /\ \ |  _  /               |
// |               | | \ \ ||    |   |   ___) / /__\ \| | \ \               |
// |               |_|  \_\| \__/ \_/|/|_____/________\_|  \_\              |
// |                                                                        |
// --------------------------------------------------------------------------
// COPYRIGHT
// --------------------------------------------------------------------------
//
// This software is copyright protected and proprietary to Neusoft Reach.
// Neusoft Reach grants to you only those rights as set out in the license 
// conditions.
// All other rights remain with Neusoft Reach.
// --------------------------------------------------------------------------

#include "dcm/connection/entity_identification.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>

#include "dcm/connection/do_ip_constants.h"

namespace asf
{
namespace diag
{
namespace dcm
{
namespace connection
{

bool EntityIdentification::GetEntityIdentification(const int givenSocket, const struct in_addr local_addr, asf::diag::uds_transport::ByteVector& eid)
{
    bool success = false;
    struct ifaddrs *currentInterface;
    struct ifaddrs *firstInterface;

    try {
        if (getifaddrs(&firstInterface) == 0)
        {
            for (currentInterface = firstInterface; currentInterface != nullptr; currentInterface = currentInterface->ifa_next)
            {
                if (currentInterface->ifa_addr == nullptr)
                {
                    continue;
                }
                if (currentInterface->ifa_addr->sa_family == AF_INET)
                {
                    if ((currentInterface->ifa_flags & IFF_LOOPBACK) || (strcmp("lo", currentInterface->ifa_name) == 0))
                    {
                        continue;
                    }
                    struct sockaddr_in* const currentSocketAddress = reinterpret_cast<struct sockaddr_in*>(currentInterface->ifa_addr);
                    if ((local_addr.s_addr == currentSocketAddress->sin_addr.s_addr))
                    {
                        struct ifreq macBuffer;
                        auto ifa_name_len = strlen(currentInterface->ifa_name);
                        (void)strncpy(macBuffer.ifr_name, currentInterface->ifa_name, sizeof(macBuffer.ifr_name) - 1); // NOLINT
                        macBuffer.ifr_name[ifa_name_len + 1] = '\0';
                        #if !defined(__QNX__) && !defined(__QNXNTO__)
                        if (ioctl(givenSocket, SIOCGIFHWADDR, &macBuffer) == 0)
                        {
                            eid.clear();
                            constexpr int kMacLength = 6;
                            for (int i = 0; i < kMacLength; i++)
                            {
                                eid.push_back(macBuffer.ifr_hwaddr.sa_data[i]);
                            }
                            success = true;
                            break;
                        } // else continue
                        #endif
                    }
                }
            }
            freeifaddrs(firstInterface);
        }
    } catch (...) {
        success = false;
    }

    if (!success) {
        // We did not find an interface with givenIp, or other errors occured.
        //   -> Return some kind of error
        // default value should be returned
        FillBufferWithInvalidValue(eid);
    }

    return success;
}

void EntityIdentification::FillBufferWithInvalidValue(asf::diag::uds_transport::ByteVector& eid)
{
    // Init with default values as of table 40 of ISO:13400-2:2012
    eid.clear();
    eid.reserve(doip::kDefaultEntityIdentification.size());
    (void)std::copy(doip::kDefaultEntityIdentification.begin(), doip::kDefaultEntityIdentification.end(), std::back_inserter(eid));
}

}  // namespace connection
}  // namespace dcm
}  // namespace diag
}  // namespace asf
