#pragma once

// cdc_control1_680.h
#ifndef CDC_CONTROL1_680_H
#define CDC_CONTROL1_680_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>  // C99 支持 bool 类型

#define Cdccontrol1680_CHANNLE     0
#define Cdccontrol1680_INDEX       11

typedef enum _Cdc_control1_680_Cdc_front_shield_pwr_reqType {
    Cdc_control1_680_Cdc_front_shield_pwr_reqType_CDC_FRONT_SHIELD_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_front_shield_pwr_reqType_CDC_FRONT_SHIELD_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_front_shield_pwr_reqType_CDC_FRONT_SHIELD_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_front_shield_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType {
    Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType {
    Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType {
    Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_cleanpumpecupwrType {
    Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_cleanpumpecupwrType;

typedef enum _Cdc_control1_680_Cdc_rilidarwprpwrType {
    Cdc_control1_680_Cdc_rilidarwprpwrType_CDC_RILIDARWPRPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_rilidarwprpwrType_CDC_RILIDARWPRPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rilidarwprpwrType_CDC_RILIDARWPRPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rilidarwprpwrType;

typedef enum _Cdc_control1_680_Cdc_lelidarwprpwrType {
    Cdc_control1_680_Cdc_lelidarwprpwrType_CDC_LELIDARWPRPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_lelidarwprpwrType_CDC_LELIDARWPRPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_lelidarwprpwrType_CDC_LELIDARWPRPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_lelidarwprpwrType;

typedef enum _Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType_CDC_AUTO_LP_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType_CDC_AUTO_LP_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType_CDC_AUTO_LP_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_frlidarwprpwrType {
    Cdc_control1_680_Cdc_frlidarwprpwrType_CDC_FRLIDARWPRPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_frlidarwprpwrType_CDC_FRLIDARWPRPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_frlidarwprpwrType_CDC_FRLIDARWPRPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_frlidarwprpwrType;

typedef enum _Cdc_control1_680_Cdc_rilidarwprmoveType {
    Cdc_control1_680_Cdc_rilidarwprmoveType_CDC_RILIDARWPRMOVE_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_rilidarwprmoveType_CDC_RILIDARWPRMOVE_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rilidarwprmoveType_CDC_RILIDARWPRMOVE_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rilidarwprmoveType;

typedef enum _Cdc_control1_680_Cdc_lelidarwprmoveType {
    Cdc_control1_680_Cdc_lelidarwprmoveType_CDC_LELIDARWPRMOVE_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_lelidarwprmoveType_CDC_LELIDARWPRMOVE_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_lelidarwprmoveType_CDC_LELIDARWPRMOVE_OPEN_E2 = 2
} Cdc_control1_680_Cdc_lelidarwprmoveType;

typedef enum _Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType_CDC_AUTO_LP_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType_CDC_AUTO_LP_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType_CDC_AUTO_LP_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_frlidarwprmoveType {
    Cdc_control1_680_Cdc_frlidarwprmoveType_CDC_FRLIDARWPRMOVE_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_frlidarwprmoveType_CDC_FRLIDARWPRMOVE_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_frlidarwprmoveType_CDC_FRLIDARWPRMOVE_OPEN_E2 = 2
} Cdc_control1_680_Cdc_frlidarwprmoveType;

typedef enum _Cdc_control1_680_Cdc_rilidarvalvepwrType {
    Cdc_control1_680_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rilidarvalvepwrType;

typedef enum _Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_frlidarvalvepwrType {
    Cdc_control1_680_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_frlidarvalvepwrType;

typedef enum _Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_lelidarvalvepwrType {
    Cdc_control1_680_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_lelidarvalvepwrType;

typedef enum _Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType {
    Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType {
    Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType {
    Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_top_shield_pwr_reqType {
    Cdc_control1_680_Cdc_top_shield_pwr_reqType_CDC_TOP_SHIELD_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_top_shield_pwr_reqType_CDC_TOP_SHIELD_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_top_shield_pwr_reqType_CDC_TOP_SHIELD_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_top_shield_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_mlidarshieldmoveType {
    Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_mlidarshieldmoveType;

typedef enum _Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType {
    Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType {
    Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType {
    Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType {
    Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType_CDC_AUTO_RF_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType_CDC_AUTO_RF_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType_CDC_AUTO_RF_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType_CDC_AUTO_LF_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType_CDC_AUTO_LF_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType_CDC_AUTO_LF_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType_CDC_AUTO_R_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType_CDC_AUTO_R_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType_CDC_AUTO_R_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType_CDC_AUTO_L_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType_CDC_AUTO_L_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType_CDC_AUTO_L_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType_CDC_AUTO_B_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType_CDC_AUTO_B_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType_CDC_AUTO_B_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType_CDC_AUTO_RF_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType_CDC_AUTO_RF_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType_CDC_AUTO_RF_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType_CDC_AUTO_LF_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType_CDC_AUTO_LF_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType_CDC_AUTO_LF_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType_CDC_AUTO_R_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType_CDC_AUTO_R_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType_CDC_AUTO_R_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType_CDC_AUTO_L_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType_CDC_AUTO_L_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType_CDC_AUTO_L_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType_CDC_AUTO_B_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType_CDC_AUTO_B_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType_CDC_AUTO_B_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_mlidarvalvepwrType {
    Cdc_control1_680_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_mlidarvalvepwrType;

typedef enum _Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_rear_shield_pwr_reqType {
    Cdc_control1_680_Cdc_rear_shield_pwr_reqType_CDC_REAR_SHIELD_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_rear_shield_pwr_reqType_CDC_REAR_SHIELD_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rear_shield_pwr_reqType_CDC_REAR_SHIELD_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rear_shield_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType {
    Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_OPEN_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_CLOSE_E2 = 2
} Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType;

typedef enum _Cdc_control1_680_Cdc_relidarwprpwrType {
    Cdc_control1_680_Cdc_relidarwprpwrType_CDC_RELIDARWPRPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_relidarwprpwrType_CDC_RELIDARWPRPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_relidarwprpwrType_CDC_RELIDARWPRPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_relidarwprpwrType;

typedef enum _Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType_CDC_AUTO_LB_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType_CDC_AUTO_LB_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType_CDC_AUTO_LB_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType {
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType_CDC_AUTO_RB_CMR_WPR_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType_CDC_AUTO_RB_CMR_WPR_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType_CDC_AUTO_RB_CMR_WPR_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_relidarwprmoveType {
    Cdc_control1_680_Cdc_relidarwprmoveType_CDC_RELIDARWPRMOVE_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_relidarwprmoveType_CDC_RELIDARWPRMOVE_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_relidarwprmoveType_CDC_RELIDARWPRMOVE_OPEN_E2 = 2
} Cdc_control1_680_Cdc_relidarwprmoveType;

typedef enum _Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType_CDC_AUTO_LB_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType_CDC_AUTO_LB_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType_CDC_AUTO_LB_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType {
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType_CDC_AUTO_RB_CMR_WPR_REQ_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType_CDC_AUTO_RB_CMR_WPR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType_CDC_AUTO_RB_CMR_WPR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType {
    Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType;

typedef enum _Cdc_control1_680_Cdc_relidarvalvepwrType {
    Cdc_control1_680_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OPEN_E2 = 2
} Cdc_control1_680_Cdc_relidarvalvepwrType;

typedef enum _Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType {
    Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType {
    Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_INVALID_E0 = 0,
    Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_OPEN_E2 = 2
} Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType;

typedef enum _Cdc_control1_680_Cdc_rechassisctrlType {
    Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_NOT_MOVE_E0 = 0,
    Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1 = 1
} Cdc_control1_680_Cdc_rechassisctrlType;

typedef enum _Cdc_control1_680_Cdc_midchassisctrlType {
    Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_NOT_MOVE_E0 = 0,
    Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1 = 1
} Cdc_control1_680_Cdc_midchassisctrlType;

typedef enum _Cdc_control1_680_Cdc_frchassisctrlType {
    Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_NOT_MOVE_E0 = 0,
    Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1 = 1
} Cdc_control1_680_Cdc_frchassisctrlType;

typedef enum _Cdc_control1_680_Cdc_ffisheyeledctrlType {
    Cdc_control1_680_Cdc_ffisheyeledctrlType_CDC_FFISHEYELEDCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_ffisheyeledctrlType_CDC_FFISHEYELEDCTRL_OFF_E1 = 1,
    Cdc_control1_680_Cdc_ffisheyeledctrlType_CDC_FFISHEYELEDCTRL_ON_E2 = 2
} Cdc_control1_680_Cdc_ffisheyeledctrlType;

typedef enum _Cdc_control1_680_Cdc_flfisheyeledctrlType {
    Cdc_control1_680_Cdc_flfisheyeledctrlType_CDC_FLFISHEYELEDCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_flfisheyeledctrlType_CDC_FLFISHEYELEDCTRL_OFF_E1 = 1,
    Cdc_control1_680_Cdc_flfisheyeledctrlType_CDC_FLFISHEYELEDCTRL_ON_E2 = 2
} Cdc_control1_680_Cdc_flfisheyeledctrlType;

typedef enum _Cdc_control1_680_Cdc_frfisheyeledctrlType {
    Cdc_control1_680_Cdc_frfisheyeledctrlType_CDC_FRFISHEYELEDCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_frfisheyeledctrlType_CDC_FRFISHEYELEDCTRL_OFF_E1 = 1,
    Cdc_control1_680_Cdc_frfisheyeledctrlType_CDC_FRFISHEYELEDCTRL_ON_E2 = 2
} Cdc_control1_680_Cdc_frfisheyeledctrlType;

typedef enum _Cdc_control1_680_Cdc_rfisheyeledctrlType {
    Cdc_control1_680_Cdc_rfisheyeledctrlType_CDC_RFISHEYELEDCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_rfisheyeledctrlType_CDC_RFISHEYELEDCTRL_OFF_E1 = 1,
    Cdc_control1_680_Cdc_rfisheyeledctrlType_CDC_RFISHEYELEDCTRL_ON_E2 = 2
} Cdc_control1_680_Cdc_rfisheyeledctrlType;

typedef enum _Cdc_control1_680_Cdc_frchassisvvctrlType {
    Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_frchassisvvctrlType;

typedef enum _Cdc_control1_680_Cdc_midchassisvvctrlType {
    Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_midchassisvvctrlType;

typedef enum _Cdc_control1_680_Cdc_rechassisvvctrlType {
    Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rechassisvvctrlType;

typedef enum _Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType {
    Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType_CDC_FRCHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType_CDC_FRCHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType_CDC_FRCHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType;

typedef enum _Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType {
    Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType_CDC_MICHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType_CDC_MICHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType_CDC_MICHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType;

typedef enum _Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType {
    Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType;

typedef enum _Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType {
    Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType;

typedef enum _Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType {
    Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType;

typedef enum _Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType {
    Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0 = 0,
    Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_CLOSE_E1 = 1,
    Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_OPEN_E2 = 2
} Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType;

typedef struct _Cdc_control1_680 {
    bool has_cdc_rechassistgtctrl;
    int cdc_rechassistgtctrl;
    bool has_cdc_rechassisctrl;
    Cdc_control1_680_Cdc_rechassisctrlType cdc_rechassisctrl;
    bool has_cdc_midchassistgtctrl;
     int cdc_midchassistgtctrl;
    bool has_cdc_midchassisctrl;
    Cdc_control1_680_Cdc_midchassisctrlType cdc_midchassisctrl;
    bool has_cdc_frchassistgtctrl;
     int cdc_frchassistgtctrl;
    bool has_cdc_frchassisctrl;
    Cdc_control1_680_Cdc_frchassisctrlType cdc_frchassisctrl;
    bool has_cdc_front_shield_pwr_req;
    Cdc_control1_680_Cdc_front_shield_pwr_reqType cdc_front_shield_pwr_req;
    bool has_cdc_fisheye_r_cmr_shield_req;
    Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType cdc_fisheye_r_cmr_shield_req;
    bool has_cdc_fisheye_l_cmr_shield_req;
    Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType cdc_fisheye_l_cmr_shield_req;
    bool has_cdc_fisheye_f_cmr_shield_req;
    Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType cdc_fisheye_f_cmr_shield_req;
    bool has_cdc_cleanpumpecupwr;
    Cdc_control1_680_Cdc_cleanpumpecupwrType cdc_cleanpumpecupwr;
    bool has_cdc_cleanpumpspd;
    double cdc_cleanpumpspd;
    bool has_cdc_rilidarwprpwr;
    Cdc_control1_680_Cdc_rilidarwprpwrType cdc_rilidarwprpwr;
    bool has_cdc_lelidarwprpwr;
    Cdc_control1_680_Cdc_lelidarwprpwrType cdc_lelidarwprpwr;
    bool has_cdc_auto_lp_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType cdc_auto_lp_cmr_wpr_pwr_req;
    bool has_cdc_frlidarwprpwr;
    Cdc_control1_680_Cdc_frlidarwprpwrType cdc_frlidarwprpwr;
    bool has_cdc_rilidarwprmove;
    Cdc_control1_680_Cdc_rilidarwprmoveType cdc_rilidarwprmove;
    bool has_cdc_lelidarwprmove;
    Cdc_control1_680_Cdc_lelidarwprmoveType cdc_lelidarwprmove;
    bool has_cdc_auto_lp_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType cdc_auto_lp_cmr_wpr_req;
    bool has_cdc_frlidarwprmove;
    Cdc_control1_680_Cdc_frlidarwprmoveType cdc_frlidarwprmove;
    bool has_cdc_rilidarvalvepwr;
    Cdc_control1_680_Cdc_rilidarvalvepwrType cdc_rilidarvalvepwr;
    bool has_cdc_fisheye_r_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType cdc_fisheye_r_cmr_valve_pwr_req;
    bool has_cdc_frlidarvalvepwr;
    Cdc_control1_680_Cdc_frlidarvalvepwrType cdc_frlidarvalvepwr;
    bool has_cdc_fisheye_f_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType cdc_fisheye_f_cmr_valve_pwr_req;
    bool has_cdc_lelidarvalvepwr;
    Cdc_control1_680_Cdc_lelidarvalvepwrType cdc_lelidarvalvepwr;
    bool has_cdc_fisheye_l_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType cdc_fisheye_l_cmr_valve_pwr_req;
    bool has_cdc_auto_lp_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType cdc_auto_lp_cmr_valve_pwr_req;
    bool has_cdc_fisheye_l_cmr_heat_req;
    Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType cdc_fisheye_l_cmr_heat_req;
    bool has_cdc_fisheye_f_cmr_heat_req;
    Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType cdc_fisheye_f_cmr_heat_req;
    bool has_cdc_fisheye_r_cmr_heat_req;
    Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType cdc_fisheye_r_cmr_heat_req;
    bool has_cdc_auto_lp_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType cdc_auto_lp_cmr_heat_req;
    bool has_cdc_top_shield_pwr_req;
    Cdc_control1_680_Cdc_top_shield_pwr_reqType cdc_top_shield_pwr_req;
    bool has_cdc_mlidarshieldmove;
    Cdc_control1_680_Cdc_mlidarshieldmoveType cdc_mlidarshieldmove;
    bool has_cdc_front_cmr_sreen_shield_req;
    Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType cdc_front_cmr_sreen_shield_req;
    bool has_cdc_auto_l_cmr_shield_req;
    Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType cdc_auto_l_cmr_shield_req;
    bool has_cdc_auto_r_cmr_shield_req;
    Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType cdc_auto_r_cmr_shield_req;
    bool has_cdc_auto_b_cmr_shield_req;
    Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType cdc_auto_b_cmr_shield_req;
    bool has_cdc_auto_rf_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType cdc_auto_rf_cmr_wpr_pwr_req;
    bool has_cdc_auto_lf_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType cdc_auto_lf_cmr_wpr_pwr_req;
    bool has_cdc_auto_r_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType cdc_auto_r_cmr_wpr_pwr_req;
    bool has_cdc_auto_l_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType cdc_auto_l_cmr_wpr_pwr_req;
    bool has_cdc_auto_b_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType cdc_auto_b_cmr_wpr_pwr_req;
    bool has_cdc_auto_rf_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType cdc_auto_rf_cmr_wpr_req;
    bool has_cdc_auto_lf_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType cdc_auto_lf_cmr_wpr_req;
    bool has_cdc_auto_r_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType cdc_auto_r_cmr_wpr_req;
    bool has_cdc_auto_l_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType cdc_auto_l_cmr_wpr_req;
    bool has_cdc_auto_b_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType cdc_auto_b_cmr_wpr_req;
    bool has_cdc_auto_b_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType cdc_auto_b_cmr_valve_pwr_req;
    bool has_cdc_mlidarvalvepwr;
    Cdc_control1_680_Cdc_mlidarvalvepwrType cdc_mlidarvalvepwr;
    bool has_cdc_auto_lf_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType cdc_auto_lf_cmr_valve_pwr_req;
    bool has_cdc_auto_rf_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType cdc_auto_rf_cmr_valve_pwr_req;
    bool has_cdc_auto_l_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType cdc_auto_l_cmr_valve_pwr_req;
    bool has_cdc_auto_r_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType cdc_auto_r_cmr_valve_pwr_req;
    bool has_cdc_auto_b_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType cdc_auto_b_cmr_heat_req;
    bool has_cdc_auto_r_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType cdc_auto_r_cmr_heat_req;
    bool has_cdc_auto_l_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType cdc_auto_l_cmr_heat_req;
    bool has_cdc_auto_rf_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType cdc_auto_rf_cmr_heat_req;
    bool has_cdc_auto_lf_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType cdc_auto_lf_cmr_heat_req;
    bool has_cdc_rear_shield_pwr_req;
    Cdc_control1_680_Cdc_rear_shield_pwr_reqType cdc_rear_shield_pwr_req;
    bool has_cdc_fisheye_b_cmr_shield_req;
    Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType cdc_fisheye_b_cmr_shield_req;
    bool has_cdc_relidarwprpwr;
    Cdc_control1_680_Cdc_relidarwprpwrType cdc_relidarwprpwr;
    bool has_cdc_auto_lb_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType cdc_auto_lb_cmr_wpr_pwr_req;
    bool has_cdc_auto_rb_cmr_wpr_pwr_req;
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType cdc_auto_rb_cmr_wpr_pwr_req;
    bool has_cdc_relidarwprmove;
    Cdc_control1_680_Cdc_relidarwprmoveType cdc_relidarwprmove;
    bool has_cdc_auto_lb_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType cdc_auto_lb_cmr_wpr_req;
    bool has_cdc_auto_rb_cmr_wpr_req;
    Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType cdc_auto_rb_cmr_wpr_req;
    bool has_cdc_auto_rl_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType cdc_auto_rl_cmr_valve_pwr_req;
    bool has_cdc_auto_rr_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType cdc_auto_rr_cmr_valve_pwr_req;
    bool has_cdc_fisheye_b_cmr_valve_pwr_req;
    Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType cdc_fisheye_b_cmr_valve_pwr_req;
    bool has_cdc_relidarvalvepwr;
    Cdc_control1_680_Cdc_relidarvalvepwrType cdc_relidarvalvepwr;
    bool has_cdc_auto_rb_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType cdc_auto_rb_cmr_heat_req;
    bool has_cdc_auto_lb_cmr_heat_req;
    Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType cdc_auto_lb_cmr_heat_req;
    bool has_cdc_fisheye_b_cmr_heat_req;
    Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType cdc_fisheye_b_cmr_heat_req;
    bool has_cdc_secoc_freshnesscounterc1;
     int cdc_secoc_freshnesscounterc1;
    bool has_cdc_secoc_authcodec1_b0_msb;
     int cdc_secoc_authcodec1_b0_msb;
    bool has_cdc_secoc_authcodec1_b1;
     int cdc_secoc_authcodec1_b1;
    bool has_cdc_secoc_authcodec1_b2_lsb;
     int cdc_secoc_authcodec1_b2_lsb;
    bool has_cdc_ffisheyeledctrl;
    Cdc_control1_680_Cdc_ffisheyeledctrlType cdc_ffisheyeledctrl;
    bool has_cdc_flfisheyeledctrl;
    Cdc_control1_680_Cdc_flfisheyeledctrlType cdc_flfisheyeledctrl;
    bool has_cdc_frfisheyeledctrl;
    Cdc_control1_680_Cdc_frfisheyeledctrlType cdc_frfisheyeledctrl;
    bool has_cdc_rfisheyeledctrl;
    Cdc_control1_680_Cdc_rfisheyeledctrlType cdc_rfisheyeledctrl;
    bool has_cdc_frchassisvvctrl;
    Cdc_control1_680_Cdc_frchassisvvctrlType cdc_frchassisvvctrl;
    bool has_cdc_midchassisvvctrl;
    Cdc_control1_680_Cdc_midchassisvvctrlType cdc_midchassisvvctrl;
    bool has_cdc_rechassisvvctrl;
    Cdc_control1_680_Cdc_rechassisvvctrlType cdc_rechassisvvctrl;
    bool has_cdc_frchassiscmrheatpwrctrl;
    Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType cdc_frchassiscmrheatpwrctrl;
    bool has_cdc_michassiscmrheatpwrctrl;
    Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType cdc_michassiscmrheatpwrctrl;
    bool has_cdc_rechassiscmrheatpwrctrl;
    Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType cdc_rechassiscmrheatpwrctrl;
    bool has_cdc_ChassisCmrLEDPwrctrl;
    Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType cdc_ChassisCmrLEDPwrctrl;
    bool has_cdc_MidchsCAMLEDPwrctrl;
    Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType cdc_MidchsCAMLEDPwrctrl;
    bool has_cdc_RechsCAMLEDPwrctrl;
    Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType cdc_RechsCAMLEDPwrctrl;
} Cdc_control1_680;

typedef struct _Work_control1_680{
    Cdc_control1_680 *control1_680;
    int time;
    int delay;
    int times;
}Work_control1_680;

//extern control_msg_st Cdccontrol1680_g;
extern char *Data;

//void Cdccontrol1680_init();
void Cdccontrol1680_UpdateData(Cdc_control1_680 *cdc_control1_680);
void Cdccontrol1680_Reset(Cdc_control1_680 *cdc_control1_680);
//eCdccontrol1680_chassis_copyxtern void (ChassisDetailMsg *chassis_sr, ChassisDetailMsg *chassis_de);
// your definitions...

#ifdef __cplusplus
}
#endif

#endif // CDC_CONTROL1_680_H
