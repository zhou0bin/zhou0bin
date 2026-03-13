//////////////////////////////////////////////////////////////////////////////
//
//    File Name:    Seed2Key.h
//    Descriptions:Interface for the GAC CCU DP security algorithm(little-endian).    
//                 Level1--27 01/02
//                 Level3--27 03/04
//                 Level11--27 11/12
//                 Level21--27 21/22
//    Author:    zhupengbo
//    CopyRight:GACRND ICV Diagnostic 2019-2030
//    Revision History
//   Version       Date            Author            Descriptions
//  --------   -----------        ----------        ------------
//        1.0        2022.07.18        zhupengbo        Initials
//        1.1        2022.10.11        zhupengbo        1.Add DCDC/PDM/SBM_EEA30/TBOX_B/DSM/DCDC/RFR
/////////////////////////////////////////////////////////////////////////////

#ifndef SEED2KEY_H
#define SEED2KEY_H

#include <string.h>
#include <stdio.h>

enum SKError
{
    NoError = 0x00,
    VendorNotFound = 0x01,
    VehicleNotFound = 0x02,//
    ECUNotFound = 0x03,//
    InvalidSeed = 0x04,
    InvalidPIN = 0x05,//
    InvalidSecurityLevel = 0x06,//
    InvalidOutParameter = 0x07,
    InvalidExtraParameter = 0x08,
    KeyCalculateError = 0x09,//
    InvalidInputParameter = 0x10,//
    StringNotMatch = 0x11,//have
    ExceptionOccurred = 0x80,
    UnkownError = 0x81,
};

//---------------------------------------------------------------------------
//The following macros are defined as ECU algorithm fast recognition rules
#define GAC_ACM			"ACM"
#define GAC_ADAS        "ADAS"
#define GAC_AMP			"AMP"
#define GAC_HUD			"HUD"
#define GAC_ACU_ICV		"ACU_ICV"
#define GAC_ACU_Harman  "ACU_Harman"
#define GAC_ACU_HSAE    "ACU_HSAE"
#define GAC_ACU_Foryou  "ACU_Foryou"
#define GAC_ACU_Desay   "ACU_Desay"
#define GAC_ACU_AVNTL   "ACU_AVNTL"
#define GAC_AVAS        "AVAS"
#define GAC_BCM_Conti_A "BCM_Conti_A"
#define GAC_BCM_Conti_B "BCM_Conti_B"
#define GAC_BCM_MM      "BCM_MM"
#define GAC_IBCM        "IBCM"
#define GAC_BSD         "BSD"
#define GAC_DMS         "DMS"
#define GAC_PEPS        "PEPS"
#define GAC_DVR         "DVR"
#define GAC_EMS_Delphi  "EMS_Delphi"
#define GAC_EMS_UAES    "EMS_UAES"
#define GAC_EPS         "EPS"
#define GAC_EPS_B       "EPS_B"
#define GAC_ESP         "ESP"
#define GAC_FAPA        "FAPA"
#define GAC_FLDCM       "FLDCM"
#define GAC_FRDCM       "FRDCM"
#define GAC_GSM_AT      "GSM_AT"
#define GAC_HVAC        "HVAC"
#define GAC_HVSM        "HVSM"
#define GAC_MSM         "MSM"
#define GAC_IAL         "IAL"
#define GAC_ICM         "ICM"
#define GAC_IFC         "IFC"
#define GAC_IHU         "IHU"
#define GAC_LCU         "LCU"
#define GAC_AFS         "AFS"
#define GAC_MLM         "MLM"
#define GAC_MRR_APTIV   "MRR_APTIV"
#define GAC_MRR_BOSCH   "MRR_BOSCH"
#define GAC_FR_APTIV    "FR_APTIV"
#define GAC_FR_BOSCH    "FR_BOSCH"
#define GAC_RPA         "RPA"
#define GAC_APA         "APA"
#define GAC_PCS         "PCS"
#define GAC_PCU         "PCU"
#define GAC_PLGM_UAES   "PLGM_UAES"
#define GAC_PLGM_MITSUI "PLGM_MITSUI"
#define GAC_RCP         "RCP"
#define GAC_SBM         "SBM"
#define GAC_SCM         "SCM"
#define GAC_SMC         "SMC"
#define GAC_SRS         "SRS"
#define GAC_TBOX        "TBOX"
#define GAC_TCU_AT      "TCU_AT"
#define GAC_TCU_WDCT    "TCU_WDCT"
#define GAC_TMM         "TMM"
#define GAC_WCM         "WCM"
#define GAC_GWM_ICV     "GWM_ICV"
#define GAC_GWM         "GWM"
#define GAC_RLCU        "RLCU"
#define GAC_FLLCU       "FLLCU"
#define GAC_FRLCU       "FRLCU"
#define GAC_ETC         "ETC"
#define GAC_PGCU        "PGCU"
#define GAC_HCP         "HCP"
#define GAC_FCP         "FCP"
#define GAC_ESCL_A      "ESCL_A"
#define GAC_ESCL_B      "ESCL_B"
#define GAC_HMS         "HMS"
#define GAC_CFPM        "CFPM"
#define GAC_DCDC_48V    "DCDC_48V"
#define GAC_DCDC_A      "DCDC_A"
#define GAC_DCDC_B      "DCDC_B"
#define GAC_DCDC        "DCDC"
#define GAC_BMS         "BMS"
#define GAC_VCU         "VCU"
#define GAC_DCU         "DCU"
#define GAC_IPS         "IPS"
#define GAC_GCU         "GCU"
#define GAC_ECV         "ECV"
#define GAC_LTWP        "LTWP"
#define GAC_MTWP        "MTWP"
#define GAC_FAN         "FAN"
#define GAC_HVH         "HVH"
#define GAC_EAV         "EAV"
#define GAC_IMR         "IMR"
#define GAC_LCM         "LCM"
#define GAC_EPB         "EPB"
#define GAC_PSDL        "PSDL"
#define GAC_PSDR        "PSDR"
#define GAC_CDC         "CDC"
#define GAC_DRM         "DRM"
#define GAC_HVSPRL      "HVSPRL"
#define GAC_HVSPRR      "HVSPRR"
#define GAC_OPC         "OPC"
#define GAC_CCU         "CCU"
#define GAC_LIDAR       "LIDAR"
#define GAC_MFS         "MFS"
#define GAC_ITS         "ITS"
#define GAC_ACID        "ACID"
#define GAC_EBB         "EBB"
#define GAC_EOP         "EOP"
#define GAC_ZCU         "ZCU"
#define GAC_VDC         "VDC"
#define GAC_PDM         "PDM"
#define GAC_SBM_EEA30   "SBM_EEA30"        
#define GAC_TBOX_B      "TBOX_B"    
#define GAC_DSM         "DSM"    
#define GAC_RFR         "RFR"
//---------------------------------------------------------------------------


#ifdef  __cplusplus
extern "C" {
#endif

    int  Seed2Key(  const char*               iECU,               /* ECU algorithm recognition*/
                    const unsigned char*      iSeedArray,         /* Array for the seed [in] */
                    unsigned char*            ioKeyArray,         /* Array for the key [in, out] */
                    const unsigned int        iSecurityLevel,     /* Security level [in] */
                    unsigned char*            iVariant            /* Name of the active variant [in] */
                );

#ifdef  __cplusplus
}
#endif

#endif /*SEED2KEY_H*/
