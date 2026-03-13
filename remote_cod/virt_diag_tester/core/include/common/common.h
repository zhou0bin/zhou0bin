#ifndef VD_COMMON_H_
#define VD_COMMON_H_

#if 0
#include "Application_AsfVirtualDiagnosticTester.hpp"

#ifdef NEUSAR_AP_R2111
using CloudProxy = asf::vcpdiagnocloudinterf::proxy::App_CCU_VCCSDiagnoCloudInterf_CtrlProxy;
#else
using CloudProxy = asf::vcpdiagnocloudinterf::proxy::VCPDiagnoCloudInterfProxy;
#endif

#ifdef NEUSAR_AP_R2111
using DiagClientSkeleton = asf::dm_diagclientinterf::skeleton::App_CCU_DM_DiagClientInterf_CtrlSkeleton;
#else
using DiagClientSkeleton = asf::dm_diagclientinterf::skeleton::DM_DiagClientInterfSkeleton;
#endif

#ifdef NEUSAR_AP_R2111
using LocalDiagClientSkeleton = asf::dm_localdiagclient::skeleton::DM_LocalDiagClientSkeleton;
#else
using LocalDiagClientSkeleton = ara::com::dm_localdiagclient::skeleton::DM_LocalDiagClientSkeleton;
#endif

#endif
#endif