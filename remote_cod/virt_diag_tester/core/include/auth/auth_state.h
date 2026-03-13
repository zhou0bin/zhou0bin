/*****************************************************************************
* @file     auth_state.h
* @brief    vehicle proxy
* @author   kevin
* @date     2025/4/22
*****************************************************************************/
#ifndef VD_HANDLE_AUTHENTICATION_STATE_IMPL_H_
#define VD_HANDLE_AUTHENTICATION_STATE_IMPL_H_

#include <iostream>

#include "midware/diag/diag_authentication_state.h"
#include "common/log.h"

namespace asf {
namespace vdi {

class ProxyHandleAuthenticationStateImpl : public midware::diag::DiagAuthenticationState
{
public:
    ProxyHandleAuthenticationStateImpl() = default;

    virtual ~ProxyHandleAuthenticationStateImpl() = default;

    virtual void DiagAuthenticationStateChanged(uint8_t state) override
    {
        authentication_state = state;
        LOGINFO << "ProxyHandleAuthenticationStateImpl::DiagAuthenticationStateChanged: diag proxy send authentication state: " << (int)state;
    }

    uint8_t authentication_state;
};

} // namespace vdi
} // namespace asf
#endif // VD_HANDLE_AUTHENTICATION_STATE_IMPL_H_