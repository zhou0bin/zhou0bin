#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#include "message_manager.h"
#include "protocol_data.h"
//#include "chassis_detail_msg.pb.h"
#include <stdbool.h>  // C99 支持 bool 类型

#define Cdccontrol2683_CHANNLE     0
#define Cdccontrol2683_INDEX       16

typedef enum _Cdc_control2_683_Cdc_rilidarvalvepwrType {
    Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OFF_E1 = 1,
    Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_ON_E2 = 2,
    Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_rilidarvalvepwrType;

typedef enum _Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_frlidarvalvepwrType {
    Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OFF_E1 = 1,
    Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_ON_E2 = 2,
    Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_frlidarvalvepwrType;

typedef enum _Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_lelidarvalvepwrType {
    Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OFF_E1 = 1,
    Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_ON_E2 = 2,
    Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_lelidarvalvepwrType;

typedef enum _Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_mlidarvalvepwrType {
    Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OFF_E1 = 1,
    Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_ON_E2 = 2,
    Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_mlidarvalvepwrType;

typedef enum _Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType {
    Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OFF_E1 = 1,
    Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_ON_E2 = 2,
    Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType;

typedef enum _Cdc_control2_683_Cdc_relidarvalvepwrType {
    Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OFF_E1 = 1,
    Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_ON_E2 = 2,
    Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_relidarvalvepwrType;

typedef enum _Cdc_control2_683_Cdc_frchassisvvctrlType {
    Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OFF_E1 = 1,
    Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_ON_E2 = 2,
    Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_frchassisvvctrlType;

typedef enum _Cdc_control2_683_Cdc_midchassisvvctrlType {
    Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OFF_E1 = 1,
    Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_ON_E2 = 2,
    Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_midchassisvvctrlType;

typedef enum _Cdc_control2_683_Cdc_rechassisvvctrlType {
    Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OFF_E1 = 1,
    Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_ON_E2 = 2,
    Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_INTERMIT_SPRAY_E3 = 3
} Cdc_control2_683_Cdc_rechassisvvctrlType;

typedef struct _Cdc_control2_683 {
    bool has_cdc_rilidarvalvepwr;
    Cdc_control2_683_Cdc_rilidarvalvepwrType cdc_rilidarvalvepwr;
    bool has_cdc_fisheye_r_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType cdc_fisheye_r_cmr_valve_pwr_req;
    bool has_cdc_frlidarvalvepwr;
    Cdc_control2_683_Cdc_frlidarvalvepwrType cdc_frlidarvalvepwr;
    bool has_cdc_fisheye_f_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType cdc_fisheye_f_cmr_valve_pwr_req;
    bool has_cdc_lelidarvalvepwr;
    Cdc_control2_683_Cdc_lelidarvalvepwrType cdc_lelidarvalvepwr;
    bool has_cdc_fisheye_l_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType cdc_fisheye_l_cmr_valve_pwr_req;
    bool has_cdc_auto_lp_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType cdc_auto_lp_cmr_valve_pwr_req;
    bool has_cdc_auto_b_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType cdc_auto_b_cmr_valve_pwr_req;
    bool has_cdc_mlidarvalvepwr;
    Cdc_control2_683_Cdc_mlidarvalvepwrType cdc_mlidarvalvepwr;
    bool has_cdc_auto_lf_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType cdc_auto_lf_cmr_valve_pwr_req;
    bool has_cdc_auto_rf_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType cdc_auto_rf_cmr_valve_pwr_req;
    bool has_cdc_auto_l_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType cdc_auto_l_cmr_valve_pwr_req;
    bool has_cdc_auto_r_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType cdc_auto_r_cmr_valve_pwr_req;
    bool has_cdc_auto_rl_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType cdc_auto_rl_cmr_valve_pwr_req;
    bool has_cdc_auto_rr_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType cdc_auto_rr_cmr_valve_pwr_req;
    bool has_cdc_fisheye_b_cmr_valve_pwr_req;
    Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType cdc_fisheye_b_cmr_valve_pwr_req;
    bool has_cdc_relidarvalvepwr;
    Cdc_control2_683_Cdc_relidarvalvepwrType cdc_relidarvalvepwr;
    bool has_cdc_frchassisvvctrl;
    Cdc_control2_683_Cdc_frchassisvvctrlType cdc_frchassisvvctrl;
    bool has_cdc_midchassisvvctrl;
    Cdc_control2_683_Cdc_midchassisvvctrlType cdc_midchassisvvctrl;
    bool has_cdc_rechassisvvctrl;
    Cdc_control2_683_Cdc_rechassisvvctrlType cdc_rechassisvvctrl;
    bool has_cdc_rilidarvalvespraynum;
    int32_t cdc_rilidarvalvespraynum;
    bool has_cdc_rilidarvalveopdrtn;
    double cdc_rilidarvalveopdrtn;
    bool has_cdc_rilidarvalveclsdrtn;
    double cdc_rilidarvalveclsdrtn;
    bool has_cdc_fisheye_r_cmrvvspraynum;
    int32_t cdc_fisheye_r_cmrvvspraynum;
    bool has_cdc_fisheye_r_cmrvvopdrtn;
    double cdc_fisheye_r_cmrvvopdrtn;
    bool has_cdc_fisheye_r_cmrvvclsdrtn;
    double cdc_fisheye_r_cmrvvclsdrtn;
    bool has_cdc_frlidarvalvespraynum;
    int32_t cdc_frlidarvalvespraynum;
    bool has_cdc_frlidarvalveopdrtn;
    double cdc_frlidarvalveopdrtn;
    bool has_cdc_frlidarvalveclsdrtn;
    double cdc_frlidarvalveclsdrtn;
    bool has_cdc_fisheye_f_cmrvvspraynum;
    int32_t cdc_fisheye_f_cmrvvspraynum;
    bool has_cdc_fisheye_f_cmrvvopdrtn;
    double cdc_fisheye_f_cmrvvopdrtn;
    bool has_cdc_fisheye_f_cmrvvclsdrtn;
    double cdc_fisheye_f_cmrvvclsdrtn;
    bool has_cdc_lelidarvalvespraynum;
    int32_t cdc_lelidarvalvespraynum;
    bool has_cdc_lelidarvalveopdrtn;
    double cdc_lelidarvalveopdrtn;
    bool has_cdc_lelidarvalveclsdrtn;
    double cdc_lelidarvalveclsdrtn;
    bool has_cdc_fisheye_l_cmrvvspraynum;
    int32_t cdc_fisheye_l_cmrvvspraynum;
    bool has_cdc_fisheye_l_cmrvvopdrtn;
    double cdc_fisheye_l_cmrvvopdrtn;
    bool has_cdc_fisheye_l_cmrvvclsdrtn;
    double cdc_fisheye_l_cmrvvclsdrtn;
    bool has_cdc_auto_lp_cmrvvspraynum;
    int32_t cdc_auto_lp_cmrvvspraynum;
    bool has_cdc_auto_lp_cmrvvopdrtn;
    double cdc_auto_lp_cmrvvopdrtn;
    bool has_cdc_auto_lp_cmrvvclsdrtn;
    double cdc_auto_lp_cmrvvclsdrtn;
    bool has_cdc_auto_b_cmrvvspraynum;
    int32_t cdc_auto_b_cmrvvspraynum;
    bool has_cdc_auto_b_cmrvvopdrtn;
    double cdc_auto_b_cmrvvopdrtn;
    bool has_cdc_auto_b_cmrvvclsdrtn;
    double cdc_auto_b_cmrvvclsdrtn;
    bool has_cdc_mlidarvvspraynum;
    int32_t cdc_mlidarvvspraynum;
    bool has_cdc_mlidarvvopdrtn;
    double cdc_mlidarvvopdrtn;
    bool has_cdc_mlidarvvclsdrtn;
    double cdc_mlidarvvclsdrtn;
    bool has_cdc_auto_lf_cmrvvspraynum;
    int32_t cdc_auto_lf_cmrvvspraynum;
    bool has_cdc_auto_lf_cmrvvopdrtn;
    double cdc_auto_lf_cmrvvopdrtn;
    bool has_cdc_auto_lf_cmrvvclsdrtn;
    double cdc_auto_lf_cmrvvclsdrtn;
    bool has_cdc_auto_rf_cmrvvspraynum;
    int32_t cdc_auto_rf_cmrvvspraynum;
    bool has_cdc_auto_rf_cmrvvopdrtn;
    double cdc_auto_rf_cmrvvopdrtn;
    bool has_cdc_auto_rf_cmrvvclsdrtn;
    double cdc_auto_rf_cmrvvclsdrtn;
    bool has_cdc_auto_l_cmrvvspraynum;
    int32_t cdc_auto_l_cmrvvspraynum;
    bool has_cdc_auto_l_cmrvvopdrtn;
    double cdc_auto_l_cmrvvopdrtn;
    bool has_cdc_auto_l_cmrvvclsdrtn;
    double cdc_auto_l_cmrvvclsdrtn;
    bool has_cdc_auto_r_cmrvvspraynum;
    int32_t cdc_auto_r_cmrvvspraynum;
    bool has_cdc_auto_r_cmrvvopdrtn;
    double cdc_auto_r_cmrvvopdrtn;
    bool has_cdc_auto_r_cmrvvclsdrtn;
    double cdc_auto_r_cmrvvclsdrtn;
    bool has_cdc_auto_rl_cmrvvspraynum;
    int32_t cdc_auto_rl_cmrvvspraynum;
    bool has_cdc_auto_rl_cmrvvopdrtn;
    double cdc_auto_rl_cmrvvopdrtn;
    bool has_cdc_auto_rl_cmrvvclsdrtn;
    double cdc_auto_rl_cmrvvclsdrtn;
    bool has_cdc_auto_rr_cmrvvspraynum;
    int32_t cdc_auto_rr_cmrvvspraynum;
    bool has_cdc_auto_rr_cmrvvopdrtn;
    double cdc_auto_rr_cmrvvopdrtn;
    bool has_cdc_auto_rr_cmrvvclsdrtn;
    double cdc_auto_rr_cmrvvclsdrtn;
    bool has_cdc_fisheye_b_cmrvvspraynum;
    int32_t cdc_fisheye_b_cmrvvspraynum;
    bool has_cdc_fisheye_b_cmrvvopdrtn;
    double cdc_fisheye_b_cmrvvopdrtn;
    bool has_cdc_fisheye_b_cmrvvclsdrtn;
    double cdc_fisheye_b_cmrvvclsdrtn;
    bool has_cdc_relidarvvspraynum;
    int32_t cdc_relidarvvspraynum;
    bool has_cdc_relidarvvopdrtn;
    double cdc_relidarvvopdrtn;
    bool has_cdc_relidarvvclsdrtn;
    double cdc_relidarvvclsdrtn;
    bool has_cdc_frchassisvvspraynum;
    int32_t cdc_frchassisvvspraynum;
    bool has_cdc_frchassisvvopdrtn;
    double cdc_frchassisvvopdrtn;
    bool has_cdc_frchassisvvclsdrtn;
    double cdc_frchassisvvclsdrtn;
    bool has_cdc_midchassisvvspraynum;
    int32_t cdc_midchassisvvspraynum;
    bool has_cdc_midchassisvvopdrtn;
    double cdc_midchassisvvopdrtn;
    bool has_cdc_midchassisvvclsdrtn;
    double cdc_midchassisvvclsdrtn;
    bool has_cdc_rechassisvvspraynum;
    int32_t cdc_rechassisvvspraynum;
    bool has_cdc_rechassisvvopdrtn;
    double cdc_rechassisvvopdrtn;
    bool has_cdc_rechassisvvclsdrtn;
    double cdc_rechassisvvclsdrtn;
    bool has_cdc_secoc_freshnesscounterc2;
    int32_t cdc_secoc_freshnesscounterc2;
    bool has_cdc_secoc_authcodec2_b0_msb;
    int32_t cdc_secoc_authcodec2_b0_msb;
    bool has_cdc_secoc_authcodec2_b1;
    int32_t cdc_secoc_authcodec2_b1;
    bool has_cdc_secoc_authcodec2_b2_lsb;
    int32_t cdc_secoc_authcodec2_b2_lsb;
} Cdc_control2_683;

//extern control_msg_st Cdccontrol2683_g;

//void Cdccontrol2683_init();
//extern Cdccontrol2683_chassis_copy(ChassisDetailMsg *chassis_sr, ChassisDetailMsg *chassis_de);
extern char *Data683;

void Cdccontrol1683_UpdateData(Cdc_control2_683 *cdc_control2_683);
void Cdccontrol1683_Reset(Cdc_control2_683 *cdc_control2_683);

#ifdef __cplusplus
}
#endif
