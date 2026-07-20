#pragma once

#include "protocol_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>  // C99 支持 bool 类型

#define Cdc15443_CHANNLE     0
#define Cdc15443_INDEX       10

typedef enum _Cdc_15_443_Ri_cam_wiper_ctlType {
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Ri_cam_wiper_ctlType;

typedef enum _Cdc_15_443_Ri_cam_wiper_servType {
    Cdc_15_443_Ri_cam_wiper_servType_RI_CAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Ri_cam_wiper_servType_RI_CAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Ri_cam_wiper_servType_RI_CAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Ri_cam_wiper_servType;

//typedef enum _Cdc_15_443_Ri_cam_wiper_number_ctlType {
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Ri_cam_wiper_number_ctlType_RI_CAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Ri_cam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Ri_cam_wiper_frequency_ctlType {
//    Cdc_15_443_Ri_cam_wiper_frequency_ctlType_RI_CAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Ri_cam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Ri_cam_wiper_location_ctlType {
    Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Ri_cam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Re_cam_wiper_ctlType {
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Re_cam_wiper_ctlType;

typedef enum _Cdc_15_443_Re_cam_wiper_servType {
    Cdc_15_443_Re_cam_wiper_servType_RE_CAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Re_cam_wiper_servType_RE_CAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Re_cam_wiper_servType_RE_CAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Re_cam_wiper_servType;

//typedef enum _Cdc_15_443_Re_cam_wiper_number_ctlType {
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Re_cam_wiper_number_ctlType_RE_CAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Re_cam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Re_cam_wiper_frequency_ctlType {
//    Cdc_15_443_Re_cam_wiper_frequency_ctlType_RE_CAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Re_cam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Re_cam_wiper_location_ctlType {
    Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Re_cam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Le_cam_wiper_ctlType {
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Le_cam_wiper_ctlType;

typedef enum _Cdc_15_443_Le_cam_wiper_servType {
    Cdc_15_443_Le_cam_wiper_servType_LE_CAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Le_cam_wiper_servType_LE_CAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Le_cam_wiper_servType_LE_CAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Le_cam_wiper_servType;

//typedef enum _Cdc_15_443_Le_cam_wiper_number_ctlType {
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Le_cam_wiper_number_ctlType_LE_CAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Le_cam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Le_cam_wiper_frequency_ctlType {
//    Cdc_15_443_Le_cam_wiper_frequency_ctlType_LE_CAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Le_cam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Le_cam_wiper_location_ctlType {
    Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Le_cam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Fr_cam_wiper_ctlType {
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Fr_cam_wiper_ctlType;

typedef enum _Cdc_15_443_Fr_cam_wiper_servType {
    Cdc_15_443_Fr_cam_wiper_servType_FR_CAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Fr_cam_wiper_servType_FR_CAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Fr_cam_wiper_servType_FR_CAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Fr_cam_wiper_servType;

//typedef enum _Cdc_15_443_Fr_cam_wiper_number_ctlType {
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Fr_cam_wiper_number_ctlType_FR_CAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Fr_cam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Fr_cam_wiper_frequency_ctlType {
//    Cdc_15_443_Fr_cam_wiper_frequency_ctlType_FR_CAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Fr_cam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Fr_cam_wiper_location_ctlType {
    Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Fr_cam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Fl_cam_wiper_ctlType {
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Fl_cam_wiper_ctlType;

typedef enum _Cdc_15_443_Fl_cam_wiper_servType {
    Cdc_15_443_Fl_cam_wiper_servType_FL_CAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Fl_cam_wiper_servType_FL_CAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Fl_cam_wiper_servType_FL_CAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Fl_cam_wiper_servType;

//typedef enum _Cdc_15_443_Fl_cam_wiper_number_ctlType {
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Fl_cam_wiper_number_ctlType_FL_CAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Fl_cam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Fl_cam_wiper_frequency_ctlType {
//    Cdc_15_443_Fl_cam_wiper_frequency_ctlType_FL_CAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Fl_cam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Fl_cam_wiper_location_ctlType {
    Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Fl_cam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Lf_pcam_wiper_ctlType {
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Lf_pcam_wiper_ctlType;

typedef enum _Cdc_15_443_Lf_pcam_wiper_serv1Type {
    Cdc_15_443_Lf_pcam_wiper_serv1Type_LF_PCAM_WIPER_SERV1_INVALID_E0 = 0,
    Cdc_15_443_Lf_pcam_wiper_serv1Type_LF_PCAM_WIPER_SERV1_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Lf_pcam_wiper_serv1Type_LF_PCAM_WIPER_SERV1_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Lf_pcam_wiper_serv1Type;

//typedef enum _Cdc_15_443_Lf_pcam_wiper_number_ctlType {
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Lf_pcam_wiper_number_ctlType_LF_PCAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Lf_pcam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Lf_pcam_wiper_frequency_ctlType {
//    Cdc_15_443_Lf_pcam_wiper_frequency_ctlType_LF_PCAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Lf_pcam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Lf_pcam_wiper_location_ctlType {
    Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Lf_pcam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Ri_blid_wiper_ctlType {
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Ri_blid_wiper_ctlType;

typedef enum _Cdc_15_443_Ri_blid_wiper_servType {
    Cdc_15_443_Ri_blid_wiper_servType_RI_BLID_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Ri_blid_wiper_servType_RI_BLID_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Ri_blid_wiper_servType_RI_BLID_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Ri_blid_wiper_servType;

//typedef enum _Cdc_15_443_Ri_blid_wiper_number_ctlType {
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Ri_blid_wiper_number_ctlType_RI_BLID_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Ri_blid_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Ri_blid_wiper_frequency_ctlType {
//    Cdc_15_443_Ri_blid_wiper_frequency_ctlType_RI_BLID_WIPER_FREQUENCY_CTL_INVALID_E0 = 0,
//} Cdc_15_443_Ri_blid_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Ri_blid_wiper_location_ctlType {
    Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Ri_blid_wiper_location_ctlType;

typedef enum _Cdc_15_443_Le_blid_wiper_ctlType {
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Le_blid_wiper_ctlType;

typedef enum _Cdc_15_443_Le_blid_wiper_servType {
    Cdc_15_443_Le_blid_wiper_servType_LE_BLID_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Le_blid_wiper_servType_LE_BLID_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Le_blid_wiper_servType_LE_BLID_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Le_blid_wiper_servType;

//typedef enum _Cdc_15_443_Le_blid_wiper_number_ctlType {
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Le_blid_wiper_number_ctlType_LE_BLID_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Le_blid_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Le_blid_wiper_frequency_ctlType {
//    Cdc_15_443_Le_blid_wiper_frequency_ctlType_LE_BLID_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Le_blid_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Le_blid_wiper_location_ctlType {
    Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Le_blid_wiper_location_ctlType;

typedef enum _Cdc_15_443_Fr_blid_wiper_ctlType {
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Fr_blid_wiper_ctlType;

typedef enum _Cdc_15_443_Fr_blid_wiper_servType {
    Cdc_15_443_Fr_blid_wiper_servType_FR_BLID_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Fr_blid_wiper_servType_FR_BLID_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Fr_blid_wiper_servType_FR_BLID_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Fr_blid_wiper_servType;

//typedef enum _Cdc_15_443_Fr_blid_wiper_number_ctlType {
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Fr_blid_wiper_number_ctlType_FR_BLID_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Fr_blid_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Fr_blid_wiper_frequency_ctlType {
//    Cdc_15_443_Fr_blid_wiper_frequency_ctlType_FR_BLID_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Fr_blid_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Fr_blid_wiper_location_ctlType {
    Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Fr_blid_wiper_location_ctlType;

typedef enum _Cdc_15_443_Lm_control_moveType {
    Cdc_15_443_Lm_control_moveType_LM_CONTROL_MOVE__E0 = 0,
    Cdc_15_443_Lm_control_moveType_LM_CONTROL_MOVE__E1 = 1
} Cdc_15_443_Lm_control_moveType;

typedef enum _Cdc_15_443_Rrb_pcam_wiper_ctlType {
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Rrb_pcam_wiper_ctlType;

typedef enum _Cdc_15_443_Rrb_pcam_wiper_servType {
    Cdc_15_443_Rrb_pcam_wiper_servType_RRB_PCAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Rrb_pcam_wiper_servType_RRB_PCAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Rrb_pcam_wiper_servType_RRB_PCAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Rrb_pcam_wiper_servType;

//typedef enum _Cdc_15_443_Rrb_pcam_wiper_number_ctlType {
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Rrb_pcam_wiper_number_ctlType_RRB_PCAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Rrb_pcam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Rrb_pcam_wiper_frequency_ctlType {
//    Cdc_15_443_Rrb_pcam_wiper_frequency_ctlType_RRB_PCAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Rrb_pcam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Rrb_pcam_wiper_location_ctlType {
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Rrb_pcam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Lrb_pcam_wiper_ctlType {
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Lrb_pcam_wiper_ctlType;

typedef enum _Cdc_15_443_Lrb_pcam_wiper_servType {
    Cdc_15_443_Lrb_pcam_wiper_servType_LRB_PCAM_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Lrb_pcam_wiper_servType_LRB_PCAM_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Lrb_pcam_wiper_servType_LRB_PCAM_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Lrb_pcam_wiper_servType;

//typedef enum _Cdc_15_443_Lrb_pcam_wiper_number_ctlType {
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Lrb_pcam_wiper_number_ctlType_LRB_PCAM_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Lrb_pcam_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Lrb_pcam_wiper_frequency_ctlType {
//    Cdc_15_443_Lrb_pcam_wiper_frequency_ctlType_LRB_PCAM_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Lrb_pcam_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Lrb_pcam_wiper_location_ctlType {
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Lrb_pcam_wiper_location_ctlType;

typedef enum _Cdc_15_443_Re_blid_rac_wiper_ctlType {
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_INVALID_E0 = 0,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1 = 1,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2 = 2,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_EMEGENCY_STOP_E3 = 3,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_FAULT_E4 = 4,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_MOVE_FAULT_E5 = 5,
    Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_NO_PROTECT_WORK_E6 = 6
} Cdc_15_443_Re_blid_rac_wiper_ctlType;

typedef enum _Cdc_15_443_Re_blid_rac_wiper_servType {
    Cdc_15_443_Re_blid_rac_wiper_servType_RE_BLID_RAC_WIPER_SERV_INVALID_E0 = 0,
    Cdc_15_443_Re_blid_rac_wiper_servType_RE_BLID_RAC_WIPER_SERV_REPAIR_POSITION_E1 = 1,
    Cdc_15_443_Re_blid_rac_wiper_servType_RE_BLID_RAC_WIPER_SERV_COMPLETE_BLADE_REPLACEMENT_E2 = 2
} Cdc_15_443_Re_blid_rac_wiper_servType;

//typedef enum _Cdc_15_443_Re_blid_rac_wiper_number_ctlType {
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_INVALID_E0 = 0,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_ONCE_E1 = 1,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_TWICE_E2 = 2,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_THREE_E3 = 3,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_FOUR_E4 = 4,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_FIVE_E5 = 5,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_SIX_E6 = 6,
//    Cdc_15_443_Re_blid_rac_wiper_number_ctlType_RE_BLID_RAC_WIPER_NUMBER_CTL_CONTINUOUS_SCRUB_E7 = 7
//} Cdc_15_443_Re_blid_rac_wiper_number_ctlType;

//typedef enum _Cdc_15_443_Re_blid_rac_wiper_frequency_ctlType {
//    Cdc_15_443_Re_blid_rac_wiper_frequency_ctlType_RE_BLID_RAC_WIPER_FREQUENCY_CTL_INVALID_E0 = 0
//} Cdc_15_443_Re_blid_rac_wiper_frequency_ctlType;

typedef enum _Cdc_15_443_Re_blid_rac_wiper_location_ctlType {
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_INVALID_E0 = 0,
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_TRAVEL_0_PERCENT_E1 = 1,
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_TRAVEL_1_PERCENT_E2 = 2,
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_TRAVEL_50_PERCENT_E51 = 51,
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_TRAVEL_100_PERCENT_E101 = 101
} Cdc_15_443_Re_blid_rac_wiper_location_ctlType;

typedef enum _Cdc_15_443_Rac_control_moveType {
    Cdc_15_443_Rac_control_moveType_RAC_CONTROL_MOVE__E0 = 0,
    Cdc_15_443_Rac_control_moveType_RAC_CONTROL_MOVE__E1 = 1
} Cdc_15_443_Rac_control_moveType;

typedef enum _Cdc_15_443_Rac_5_control_moveType {
    Cdc_15_443_Rac_5_control_moveType_RAC_5_CONTROL_MOVE__E0 = 0,
    Cdc_15_443_Rac_5_control_moveType_RAC_5_CONTROL_MOVE__E1 = 1
} Cdc_15_443_Rac_5_control_moveType;

//typedef enum _Cdc_15_443_Rac_4_movetarget_positionType {
//    Cdc_15_443_Rac_4_movetarget_positionType_RAC_4_MOVETARGET_POSITION_0_E0 = 0,
//    Cdc_15_443_Rac_4_movetarget_positionType_RAC_4_MOVETARGET_POSITION_1_E1 = 1,
//    Cdc_15_443_Rac_4_movetarget_positionType_RAC_4_MOVETARGET_POSITION_50_E50 = 50,
//    Cdc_15_443_Rac_4_movetarget_positionType_RAC_4_MOVETARGET_POSITION_100_E100 = 100
//} Cdc_15_443_Rac_4_movetarget_positionType;

//typedef enum _Cdc_15_443_Rac_5_movetarget_positionType {
//    Cdc_15_443_Rac_5_movetarget_positionType_RAC_5_MOVETARGET_POSITION_0_E0 = 0,
//    Cdc_15_443_Rac_5_movetarget_positionType_RAC_5_MOVETARGET_POSITION_1_E1 = 1,
//    Cdc_15_443_Rac_5_movetarget_positionType_RAC_5_MOVETARGET_POSITION_50_E50 = 50,
//    Cdc_15_443_Rac_5_movetarget_positionType_RAC_5_MOVETARGET_POSITION_100_E100 = 100
//} Cdc_15_443_Rac_5_movetarget_positionType;

typedef struct _Cdc_15_443 {
    bool has_ri_cam_wiper_ctl;
    Cdc_15_443_Ri_cam_wiper_ctlType ri_cam_wiper_ctl;
    bool has_ri_cam_wiper_serv;
    Cdc_15_443_Ri_cam_wiper_servType ri_cam_wiper_serv;
    bool has_ri_cam_wiper_number_ctl;
    int32_t ri_cam_wiper_number_ctl;
    bool has_ri_cam_wiper_frequency_ctl;
    int32_t ri_cam_wiper_frequency_ctl;
    bool has_ri_cam_wiper_travel_ctl;
    int32_t ri_cam_wiper_travel_ctl;
    bool has_ri_cam_wiper_location_ctl;
    Cdc_15_443_Ri_cam_wiper_location_ctlType ri_cam_wiper_location_ctl;
    bool has_re_cam_wiper_ctl;
    Cdc_15_443_Re_cam_wiper_ctlType re_cam_wiper_ctl;
    bool has_re_cam_wiper_serv;
    Cdc_15_443_Re_cam_wiper_servType re_cam_wiper_serv;
    bool has_re_cam_wiper_number_ctl;
    int32_t re_cam_wiper_number_ctl;
    bool has_re_cam_wiper_frequency_ctl;
    int32_t re_cam_wiper_frequency_ctl;
    bool has_re_cam_wiper_travel_ctl;
    int32_t re_cam_wiper_travel_ctl;
    bool has_re_cam_wiper_location_ctl;
    Cdc_15_443_Re_cam_wiper_location_ctlType re_cam_wiper_location_ctl;
    bool has_le_cam_wiper_ctl;
    Cdc_15_443_Le_cam_wiper_ctlType le_cam_wiper_ctl;
    bool has_le_cam_wiper_serv;
    Cdc_15_443_Le_cam_wiper_servType le_cam_wiper_serv;
    bool has_le_cam_wiper_number_ctl;
    int32_t le_cam_wiper_number_ctl;
    bool has_le_cam_wiper_frequency_ctl;
    int32_t le_cam_wiper_frequency_ctl;
    bool has_le_cam_wiper_travel_ctl;
    int32_t le_cam_wiper_travel_ctl;
    bool has_le_cam_wiper_location_ctl;
    Cdc_15_443_Le_cam_wiper_location_ctlType le_cam_wiper_location_ctl;
    bool has_fr_cam_wiper_ctl;
    Cdc_15_443_Fr_cam_wiper_ctlType fr_cam_wiper_ctl;
    bool has_fr_cam_wiper_serv;
    Cdc_15_443_Fr_cam_wiper_servType fr_cam_wiper_serv;
    bool has_fr_cam_wiper_number_ctl;
    int32_t fr_cam_wiper_number_ctl;
    bool has_fr_cam_wiper_frequency_ctl;
    int32_t fr_cam_wiper_frequency_ctl;
    bool has_fr_cam_wiper_travel_ctl;
    int32_t fr_cam_wiper_travel_ctl;
    bool has_fr_cam_wiper_location_ctl;
    Cdc_15_443_Fr_cam_wiper_location_ctlType fr_cam_wiper_location_ctl;
    bool has_fl_cam_wiper_ctl;
    Cdc_15_443_Fl_cam_wiper_ctlType fl_cam_wiper_ctl;
    bool has_fl_cam_wiper_serv;
    Cdc_15_443_Fl_cam_wiper_servType fl_cam_wiper_serv;
    bool has_fl_cam_wiper_number_ctl;
    int32_t fl_cam_wiper_number_ctl;
    bool has_fl_cam_wiper_frequency_ctl;
    int32_t fl_cam_wiper_frequency_ctl;
    bool has_fl_cam_wiper_travel_ctl;
    int32_t fl_cam_wiper_travel_ctl;
    bool has_fl_cam_wiper_location_ctl;
    Cdc_15_443_Fl_cam_wiper_location_ctlType fl_cam_wiper_location_ctl;
    bool has_lf_pcam_wiper_ctl;
    Cdc_15_443_Lf_pcam_wiper_ctlType lf_pcam_wiper_ctl;
    bool has_lf_pcam_wiper_serv1;
    Cdc_15_443_Lf_pcam_wiper_serv1Type lf_pcam_wiper_serv1;
    bool has_lf_pcam_wiper_number_ctl;
    int32_t lf_pcam_wiper_number_ctl;
    bool has_lf_pcam_wiper_frequency_ctl;
    int32_t lf_pcam_wiper_frequency_ctl;
    bool has_lf_pcam_wiper_travel_ctl;
    int32_t lf_pcam_wiper_travel_ctl;
    bool has_lf_pcam_wiper_location_ctl;
    Cdc_15_443_Lf_pcam_wiper_location_ctlType lf_pcam_wiper_location_ctl;
    bool has_ri_blid_wiper_ctl;
    Cdc_15_443_Ri_blid_wiper_ctlType ri_blid_wiper_ctl;
    bool has_ri_blid_wiper_serv;
    Cdc_15_443_Ri_blid_wiper_servType ri_blid_wiper_serv;
    bool has_ri_blid_wiper_number_ctl;
    int32_t ri_blid_wiper_number_ctl;
    bool has_ri_blid_wiper_frequency_ctl;
    int32_t ri_blid_wiper_frequency_ctl;
    bool has_ri_blid_wiper_travel_ctl;
    int32_t ri_blid_wiper_travel_ctl;
    bool has_ri_blid_wiper_location_ctl;
    Cdc_15_443_Ri_blid_wiper_location_ctlType ri_blid_wiper_location_ctl;
    bool has_le_blid_wiper_ctl;
    Cdc_15_443_Le_blid_wiper_ctlType le_blid_wiper_ctl;
    bool has_le_blid_wiper_serv;
    Cdc_15_443_Le_blid_wiper_servType le_blid_wiper_serv;
    bool has_le_blid_wiper_number_ctl;
    int32_t le_blid_wiper_number_ctl;
    bool has_le_blid_wiper_frequency_ctl;
    int32_t le_blid_wiper_frequency_ctl;
    bool has_le_blid_wiper_travel_ctl;
    int32_t le_blid_wiper_travel_ctl;
    bool has_le_blid_wiper_location_ctl;
    Cdc_15_443_Le_blid_wiper_location_ctlType le_blid_wiper_location_ctl;
    bool has_fr_blid_wiper_ctl;
    Cdc_15_443_Fr_blid_wiper_ctlType fr_blid_wiper_ctl;
    bool has_fr_blid_wiper_serv;
    Cdc_15_443_Fr_blid_wiper_servType fr_blid_wiper_serv;
    bool has_fr_blid_wiper_number_ctl;
    int32_t fr_blid_wiper_number_ctl;
    bool has_fr_blid_wiper_frequency_ctl;
    int32_t fr_blid_wiper_frequency_ctl;
    bool has_fr_blid_wiper_travel_ctl;
    int32_t fr_blid_wiper_travel_ctl;
    bool has_fr_blid_wiper_location_ctl;
    Cdc_15_443_Fr_blid_wiper_location_ctlType fr_blid_wiper_location_ctl;
    bool has_lm_control_move;
    Cdc_15_443_Lm_control_moveType lm_control_move;
    bool has_lm_move_target_position;
    int32_t lm_move_target_position;
    bool has_rrb_pcam_wiper_ctl;
    Cdc_15_443_Rrb_pcam_wiper_ctlType rrb_pcam_wiper_ctl;
    bool has_rrb_pcam_wiper_serv;
    Cdc_15_443_Rrb_pcam_wiper_servType rrb_pcam_wiper_serv;
    bool has_rrb_pcam_wiper_number_ctl;
    int32_t rrb_pcam_wiper_number_ctl;
    bool has_rrb_pcam_wiper_frequency_ctl;
    int32_t rrb_pcam_wiper_frequency_ctl;
    bool has_rrb_pcam_wiper_travel_ctl;
    int32_t rrb_pcam_wiper_travel_ctl;
    bool has_rrb_pcam_wiper_location_ctl;
    Cdc_15_443_Rrb_pcam_wiper_location_ctlType rrb_pcam_wiper_location_ctl;
    bool has_lrb_pcam_wiper_ctl;
    Cdc_15_443_Lrb_pcam_wiper_ctlType lrb_pcam_wiper_ctl;
    bool has_lrb_pcam_wiper_serv;
    Cdc_15_443_Lrb_pcam_wiper_servType lrb_pcam_wiper_serv;
    bool has_lrb_pcam_wiper_number_ctl;
    int32_t lrb_pcam_wiper_number_ctl;
    bool has_lrb_pcam_wiper_frequency_ctl;
    int32_t lrb_pcam_wiper_frequency_ctl;
    bool has_lrb_pcam_wiper_travel_ctl;
    int32_t lrb_pcam_wiper_travel_ctl;
    bool has_lrb_pcam_wiper_location_ctl;
    Cdc_15_443_Lrb_pcam_wiper_location_ctlType lrb_pcam_wiper_location_ctl;
    bool has_re_blid_rac_wiper_ctl;
    Cdc_15_443_Re_blid_rac_wiper_ctlType re_blid_rac_wiper_ctl;
    bool has_re_blid_rac_wiper_serv;
    Cdc_15_443_Re_blid_rac_wiper_servType re_blid_rac_wiper_serv;
    bool has_re_blid_rac_wiper_number_ctl;
    int32_t re_blid_rac_wiper_number_ctl;
    bool has_re_blid_rac_wiper_frequency_ctl;
    int32_t re_blid_rac_wiper_frequency_ctl;
    bool has_re_blid_rac_wiper_travel_ctl;
    int32_t re_blid_rac_wiper_travel_ctl;
    bool has_re_blid_rac_wiper_location_ctl;
    Cdc_15_443_Re_blid_rac_wiper_location_ctlType re_blid_rac_wiper_location_ctl;
    bool has_rac_control_move;
    Cdc_15_443_Rac_control_moveType rac_control_move;
    bool has_rac_5_control_move;
    Cdc_15_443_Rac_5_control_moveType rac_5_control_move;
    bool has_rac_4_movetarget_position;
    int rac_4_movetarget_position;
    bool has_rac_5_movetarget_position;
    int rac_5_movetarget_position;
    bool has_cdc_secoc_freshnesscounter;
    int32_t cdc_secoc_freshnesscounter;
    bool has_cdc_secoc_authcode_b0_msb;
    int32_t cdc_secoc_authcode_b0_msb;
    bool has_cdc_secoc_authcode_b1;
    int32_t cdc_secoc_authcode_b1;
    bool has_cdc_secoc_authcode_b2_lsb;
    int32_t cdc_secoc_authcode_b2_lsb;
} Cdc_15_443;

//extern Cdc_15_443 Cdc15443_g;

extern char *Data443;
void Cdc15443_Reset(Cdc_15_443 *chassis);
void Cdc15443_UpdateData(Cdc_15_443 *chassis);
//void Cdc15443_init();
//extern Cdc15443_chassis_copy(ChassisDetailMsg *chassis_sr, ChassisDetailMsg *chassis_de);

#ifdef __cplusplus
}
#endif
