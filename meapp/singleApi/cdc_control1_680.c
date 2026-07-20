#include "cdc_control1_680.h"
#include "byte.h"
#include "protocol_data.h"

//control_msg_st Cdccontrol1680_g;

static char msg_rawdata[48];
char *Data = msg_rawdata;


void Cdccontrol1680_set_p_cdc_rechassistgtctrl(int cdc_rechassistgtctrl) {
  cdc_rechassistgtctrl = ProtocolData_BoundedValue(0, 100, cdc_rechassistgtctrl);
  int x = cdc_rechassistgtctrl;

  Byte_set_value(x, 0, 8,  Data + 24);
}


void Cdccontrol1680_set_p_cdc_rechassisctrl(Cdc_control1_680_Cdc_rechassisctrlType cdc_rechassisctrl) {
  int x = cdc_rechassisctrl;

  Byte_set_value(x, 6, 1,  Data + 21);
}


void Cdccontrol1680_set_p_cdc_midchassistgtctrl(int cdc_midchassistgtctrl) {
  cdc_midchassistgtctrl = ProtocolData_BoundedValue(0, 100, cdc_midchassistgtctrl);
  int x = cdc_midchassistgtctrl;

  Byte_set_value(x, 0, 8,  Data + 23);
}


void Cdccontrol1680_set_p_cdc_midchassisctrl(Cdc_control1_680_Cdc_midchassisctrlType cdc_midchassisctrl) {
  int x = cdc_midchassisctrl;

  Byte_set_value(x, 5, 1,  Data + 21);
}


void Cdccontrol1680_set_p_cdc_frchassistgtctrl(int cdc_frchassistgtctrl) {
  cdc_frchassistgtctrl = ProtocolData_BoundedValue(0, 100, cdc_frchassistgtctrl);
  int x = cdc_frchassistgtctrl;

  Byte_set_value(x, 0, 8,  Data + 22);
}


void Cdccontrol1680_set_p_cdc_frchassisctrl(Cdc_control1_680_Cdc_frchassisctrlType cdc_frchassisctrl) {
  int x = cdc_frchassisctrl;

  Byte_set_value(x, 4, 1,  Data + 21);
}


void Cdccontrol1680_set_p_cdc_front_shield_pwr_req(Cdc_control1_680_Cdc_front_shield_pwr_reqType cdc_front_shield_pwr_req) {
  int x = cdc_front_shield_pwr_req;

  Byte_set_value(x, 0, 2,  Data + 0);
}


void Cdccontrol1680_set_p_cdc_fisheye_r_cmr_shield_req(Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType cdc_fisheye_r_cmr_shield_req) {
  int x = cdc_fisheye_r_cmr_shield_req;

  Byte_set_value(x, 2, 2,  Data + 0);
}


void Cdccontrol1680_set_p_cdc_fisheye_l_cmr_shield_req(Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType cdc_fisheye_l_cmr_shield_req) {
  int x = cdc_fisheye_l_cmr_shield_req;

  Byte_set_value(x, 4, 2,  Data + 0);
}


void Cdccontrol1680_set_p_cdc_fisheye_f_cmr_shield_req(Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType cdc_fisheye_f_cmr_shield_req) {
  int x = cdc_fisheye_f_cmr_shield_req;

  Byte_set_value(x, 6, 2,  Data + 0);
}


void Cdccontrol1680_set_p_cdc_cleanpumpecupwr(Cdc_control1_680_Cdc_cleanpumpecupwrType cdc_cleanpumpecupwr) {
  int x = cdc_cleanpumpecupwr;

  Byte_set_value(x, 0, 2,  Data + 1);
}


void Cdccontrol1680_set_p_cdc_cleanpumpspd(double cdc_cleanpumpspd) {
  cdc_cleanpumpspd = ProtocolData_BoundedValue(0.0, 25.5, cdc_cleanpumpspd);
  int x = cdc_cleanpumpspd / 0.100000;

  Byte_set_value(x, 0, 8,  Data + 2);
}


void Cdccontrol1680_set_p_cdc_rilidarwprpwr(Cdc_control1_680_Cdc_rilidarwprpwrType cdc_rilidarwprpwr) {
  int x = cdc_rilidarwprpwr;

  Byte_set_value(x, 0, 2,  Data + 3);
}


void Cdccontrol1680_set_p_cdc_lelidarwprpwr(Cdc_control1_680_Cdc_lelidarwprpwrType cdc_lelidarwprpwr) {
  int x = cdc_lelidarwprpwr;

  Byte_set_value(x, 2, 2,  Data + 3);
}


void Cdccontrol1680_set_p_cdc_auto_lp_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType cdc_auto_lp_cmr_wpr_pwr_req) {
  int x = cdc_auto_lp_cmr_wpr_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 3);
}


void Cdccontrol1680_set_p_cdc_frlidarwprpwr(Cdc_control1_680_Cdc_frlidarwprpwrType cdc_frlidarwprpwr) {
  int x = cdc_frlidarwprpwr;

  Byte_set_value(x, 6, 2,  Data + 3);
}


void Cdccontrol1680_set_p_cdc_rilidarwprmove(Cdc_control1_680_Cdc_rilidarwprmoveType cdc_rilidarwprmove) {
  int x = cdc_rilidarwprmove;

  Byte_set_value(x, 0, 2,  Data + 17);
}


void Cdccontrol1680_set_p_cdc_lelidarwprmove(Cdc_control1_680_Cdc_lelidarwprmoveType cdc_lelidarwprmove) {
  int x = cdc_lelidarwprmove;

  Byte_set_value(x, 2, 2,  Data + 17);
}


void Cdccontrol1680_set_p_cdc_auto_lp_cmr_wpr_req(Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType cdc_auto_lp_cmr_wpr_req) {
  int x = cdc_auto_lp_cmr_wpr_req;

  Byte_set_value(x, 4, 2,  Data + 17);
}


void Cdccontrol1680_set_p_cdc_frlidarwprmove(Cdc_control1_680_Cdc_frlidarwprmoveType cdc_frlidarwprmove) {
  int x = cdc_frlidarwprmove;

  Byte_set_value(x, 6, 2,  Data + 17);
}


void Cdccontrol1680_set_p_cdc_rilidarvalvepwr(Cdc_control1_680_Cdc_rilidarvalvepwrType cdc_rilidarvalvepwr) {
  int x = cdc_rilidarvalvepwr;

  Byte_set_value(x, 4, 2,  Data + 4);
}


void Cdccontrol1680_set_p_cdc_fisheye_r_cmr_valve_pwr_req(Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType cdc_fisheye_r_cmr_valve_pwr_req) {
  int x = cdc_fisheye_r_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 4);
}


void Cdccontrol1680_set_p_cdc_frlidarvalvepwr(Cdc_control1_680_Cdc_frlidarvalvepwrType cdc_frlidarvalvepwr) {
  int x = cdc_frlidarvalvepwr;

  Byte_set_value(x, 0, 2,  Data + 5);
}


void Cdccontrol1680_set_p_cdc_fisheye_f_cmr_valve_pwr_req(Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType cdc_fisheye_f_cmr_valve_pwr_req) {
  int x = cdc_fisheye_f_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 5);
}


void Cdccontrol1680_set_p_cdc_lelidarvalvepwr(Cdc_control1_680_Cdc_lelidarvalvepwrType cdc_lelidarvalvepwr) {
  int x = cdc_lelidarvalvepwr;

  Byte_set_value(x, 4, 2,  Data + 5);
}


void Cdccontrol1680_set_p_cdc_fisheye_l_cmr_valve_pwr_req(Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType cdc_fisheye_l_cmr_valve_pwr_req) {
  int x = cdc_fisheye_l_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 5);
}


void Cdccontrol1680_set_p_cdc_auto_lp_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType cdc_auto_lp_cmr_valve_pwr_req) {
  int x = cdc_auto_lp_cmr_valve_pwr_req;

  Byte_set_value(x, 0, 2,  Data + 6);
}


void Cdccontrol1680_set_p_cdc_fisheye_l_cmr_heat_req(Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType cdc_fisheye_l_cmr_heat_req) {
  int x = cdc_fisheye_l_cmr_heat_req;

  Byte_set_value(x, 2, 2,  Data + 6);
}


void Cdccontrol1680_set_p_cdc_fisheye_f_cmr_heat_req(Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType cdc_fisheye_f_cmr_heat_req) {
  int x = cdc_fisheye_f_cmr_heat_req;

  Byte_set_value(x, 4, 2,  Data + 6);
}


void Cdccontrol1680_set_p_cdc_fisheye_r_cmr_heat_req(Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType cdc_fisheye_r_cmr_heat_req) {
  int x = cdc_fisheye_r_cmr_heat_req;

  Byte_set_value(x, 6, 2,  Data + 6);
}


void Cdccontrol1680_set_p_cdc_auto_lp_cmr_heat_req(Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType cdc_auto_lp_cmr_heat_req) {
  int x = cdc_auto_lp_cmr_heat_req;

  Byte_set_value(x, 0, 2,  Data + 7);
}


void Cdccontrol1680_set_p_cdc_top_shield_pwr_req(Cdc_control1_680_Cdc_top_shield_pwr_reqType cdc_top_shield_pwr_req) {
  int x = cdc_top_shield_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 7);
}


void Cdccontrol1680_set_p_cdc_mlidarshieldmove(Cdc_control1_680_Cdc_mlidarshieldmoveType cdc_mlidarshieldmove) {
  int x = cdc_mlidarshieldmove;

  Byte_set_value(x, 4, 2,  Data + 7);
}


void Cdccontrol1680_set_p_cdc_front_cmr_sreen_shield_req(Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType cdc_front_cmr_sreen_shield_req) {
  int x = cdc_front_cmr_sreen_shield_req;

  Byte_set_value(x, 6, 2,  Data + 7);
}


void Cdccontrol1680_set_p_cdc_auto_l_cmr_shield_req(Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType cdc_auto_l_cmr_shield_req) {
  int x = cdc_auto_l_cmr_shield_req;

  Byte_set_value(x, 0, 2,  Data + 8);
}


void Cdccontrol1680_set_p_cdc_auto_r_cmr_shield_req(Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType cdc_auto_r_cmr_shield_req) {
  int x = cdc_auto_r_cmr_shield_req;

  Byte_set_value(x, 2, 2,  Data + 8);
}


void Cdccontrol1680_set_p_cdc_auto_b_cmr_shield_req(Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType cdc_auto_b_cmr_shield_req) {
  int x = cdc_auto_b_cmr_shield_req;

  Byte_set_value(x, 4, 2,  Data + 8);
}


void Cdccontrol1680_set_p_cdc_auto_rf_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType cdc_auto_rf_cmr_wpr_pwr_req) {
  int x = cdc_auto_rf_cmr_wpr_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 8);
}


void Cdccontrol1680_set_p_cdc_auto_lf_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType cdc_auto_lf_cmr_wpr_pwr_req) {
  int x = cdc_auto_lf_cmr_wpr_pwr_req;

  Byte_set_value(x, 0, 2,  Data + 9);
}


void Cdccontrol1680_set_p_cdc_auto_r_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType cdc_auto_r_cmr_wpr_pwr_req) {
  int x = cdc_auto_r_cmr_wpr_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 9);
}


void Cdccontrol1680_set_p_cdc_auto_l_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType cdc_auto_l_cmr_wpr_pwr_req) {
  int x = cdc_auto_l_cmr_wpr_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 9);
}


void Cdccontrol1680_set_p_cdc_auto_b_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType cdc_auto_b_cmr_wpr_pwr_req) {
  int x = cdc_auto_b_cmr_wpr_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 9);
}


void Cdccontrol1680_set_p_cdc_auto_rf_cmr_wpr_req(Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType cdc_auto_rf_cmr_wpr_req) {
  int x = cdc_auto_rf_cmr_wpr_req;

  Byte_set_value(x, 0, 2,  Data + 18);
}


void Cdccontrol1680_set_p_cdc_auto_lf_cmr_wpr_req(Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType cdc_auto_lf_cmr_wpr_req) {
  int x = cdc_auto_lf_cmr_wpr_req;

  Byte_set_value(x, 2, 2,  Data + 18);
}


void Cdccontrol1680_set_p_cdc_auto_r_cmr_wpr_req(Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType cdc_auto_r_cmr_wpr_req) {
  int x = cdc_auto_r_cmr_wpr_req;

  Byte_set_value(x, 4, 2,  Data + 18);
}


void Cdccontrol1680_set_p_cdc_auto_l_cmr_wpr_req(Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType cdc_auto_l_cmr_wpr_req) {
  int x = cdc_auto_l_cmr_wpr_req;

  Byte_set_value(x, 6, 2,  Data + 18);
}


void Cdccontrol1680_set_p_cdc_auto_b_cmr_wpr_req(Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType cdc_auto_b_cmr_wpr_req) {
  int x = cdc_auto_b_cmr_wpr_req;

  Byte_set_value(x, 0, 2,  Data + 19);
}


void Cdccontrol1680_set_p_cdc_auto_b_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType cdc_auto_b_cmr_valve_pwr_req) {
  int x = cdc_auto_b_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 10);
}


void Cdccontrol1680_set_p_cdc_mlidarvalvepwr(Cdc_control1_680_Cdc_mlidarvalvepwrType cdc_mlidarvalvepwr) {
  int x = cdc_mlidarvalvepwr;

  Byte_set_value(x, 0, 2,  Data + 11);
}


void Cdccontrol1680_set_p_cdc_auto_lf_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType cdc_auto_lf_cmr_valve_pwr_req) {
  int x = cdc_auto_lf_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 11);
}


void Cdccontrol1680_set_p_cdc_auto_rf_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType cdc_auto_rf_cmr_valve_pwr_req) {
  int x = cdc_auto_rf_cmr_valve_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 11);
}


void Cdccontrol1680_set_p_cdc_auto_l_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType cdc_auto_l_cmr_valve_pwr_req) {
  int x = cdc_auto_l_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 11);
}


void Cdccontrol1680_set_p_cdc_auto_r_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType cdc_auto_r_cmr_valve_pwr_req) {
  int x = cdc_auto_r_cmr_valve_pwr_req;

  Byte_set_value(x, 0, 2,  Data + 12);
}


void Cdccontrol1680_set_p_cdc_auto_b_cmr_heat_req(Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType cdc_auto_b_cmr_heat_req) {
  int x = cdc_auto_b_cmr_heat_req;

  Byte_set_value(x, 2, 2,  Data + 12);
}


void Cdccontrol1680_set_p_cdc_auto_r_cmr_heat_req(Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType cdc_auto_r_cmr_heat_req) {
  int x = cdc_auto_r_cmr_heat_req;

  Byte_set_value(x, 4, 2,  Data + 12);
}


void Cdccontrol1680_set_p_cdc_auto_l_cmr_heat_req(Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType cdc_auto_l_cmr_heat_req) {
  int x = cdc_auto_l_cmr_heat_req;

  Byte_set_value(x, 6, 2,  Data + 12);
}


void Cdccontrol1680_set_p_cdc_auto_rf_cmr_heat_req(Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType cdc_auto_rf_cmr_heat_req) {
  int x = cdc_auto_rf_cmr_heat_req;

  Byte_set_value(x, 0, 2,  Data + 13);
}


void Cdccontrol1680_set_p_cdc_auto_lf_cmr_heat_req(Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType cdc_auto_lf_cmr_heat_req) {
  int x = cdc_auto_lf_cmr_heat_req;

  Byte_set_value(x, 2, 2,  Data + 13);
}


void Cdccontrol1680_set_p_cdc_rear_shield_pwr_req(Cdc_control1_680_Cdc_rear_shield_pwr_reqType cdc_rear_shield_pwr_req) {
  int x = cdc_rear_shield_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 13);
}


void Cdccontrol1680_set_p_cdc_fisheye_b_cmr_shield_req(Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType cdc_fisheye_b_cmr_shield_req) {
  int x = cdc_fisheye_b_cmr_shield_req;

  Byte_set_value(x, 6, 2,  Data + 13);
}


void Cdccontrol1680_set_p_cdc_relidarwprpwr(Cdc_control1_680_Cdc_relidarwprpwrType cdc_relidarwprpwr) {
  int x = cdc_relidarwprpwr;

  Byte_set_value(x, 0, 2,  Data + 14);
}


void Cdccontrol1680_set_p_cdc_auto_lb_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType cdc_auto_lb_cmr_wpr_pwr_req) {
  int x = cdc_auto_lb_cmr_wpr_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 14);
}


void Cdccontrol1680_set_p_cdc_auto_rb_cmr_wpr_pwr_req(Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType cdc_auto_rb_cmr_wpr_pwr_req) {
  int x = cdc_auto_rb_cmr_wpr_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 14);
}


void Cdccontrol1680_set_p_cdc_relidarwprmove(Cdc_control1_680_Cdc_relidarwprmoveType cdc_relidarwprmove) {
  int x = cdc_relidarwprmove;

  Byte_set_value(x, 2, 2,  Data + 19);
}


void Cdccontrol1680_set_p_cdc_auto_lb_cmr_wpr_req(Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType cdc_auto_lb_cmr_wpr_req) {
  int x = cdc_auto_lb_cmr_wpr_req;

  Byte_set_value(x, 4, 2,  Data + 19);
}


void Cdccontrol1680_set_p_cdc_auto_rb_cmr_wpr_req(Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType cdc_auto_rb_cmr_wpr_req) {
  int x = cdc_auto_rb_cmr_wpr_req;

  Byte_set_value(x, 6, 2,  Data + 19);
}


void Cdccontrol1680_set_p_cdc_auto_rl_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType cdc_auto_rl_cmr_valve_pwr_req) {
  int x = cdc_auto_rl_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2,  Data + 15);
}


void Cdccontrol1680_set_p_cdc_auto_rr_cmr_valve_pwr_req(Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType cdc_auto_rr_cmr_valve_pwr_req) {
  int x = cdc_auto_rr_cmr_valve_pwr_req;

  Byte_set_value(x, 4, 2,  Data + 15);
}


void Cdccontrol1680_set_p_cdc_fisheye_b_cmr_valve_pwr_req(Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType cdc_fisheye_b_cmr_valve_pwr_req) {
  int x = cdc_fisheye_b_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2,  Data + 15);
}


void Cdccontrol1680_set_p_cdc_relidarvalvepwr(Cdc_control1_680_Cdc_relidarvalvepwrType cdc_relidarvalvepwr) {
  int x = cdc_relidarvalvepwr;

  Byte_set_value(x, 0, 2,  Data + 16);
}


void Cdccontrol1680_set_p_cdc_auto_rb_cmr_heat_req(Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType cdc_auto_rb_cmr_heat_req) {
  int x = cdc_auto_rb_cmr_heat_req;

  Byte_set_value(x, 2, 2,  Data + 16);
}


void Cdccontrol1680_set_p_cdc_auto_lb_cmr_heat_req(Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType cdc_auto_lb_cmr_heat_req) {
  int x = cdc_auto_lb_cmr_heat_req;

  Byte_set_value(x, 4, 2,  Data + 16);
}


void Cdccontrol1680_set_p_cdc_fisheye_b_cmr_heat_req(Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType cdc_fisheye_b_cmr_heat_req) {
  int x = cdc_fisheye_b_cmr_heat_req;

  Byte_set_value(x, 6, 2,  Data + 16);
}


void Cdccontrol1680_set_p_cdc_secoc_freshnesscounterc1(int cdc_secoc_freshnesscounterc1) {
  cdc_secoc_freshnesscounterc1 = ProtocolData_BoundedValue(0, 255, cdc_secoc_freshnesscounterc1);
  int x = cdc_secoc_freshnesscounterc1;

  Byte_set_value(x, 0, 8,  Data + 28);
}


void Cdccontrol1680_set_p_cdc_secoc_authcodec1_b0_msb(int cdc_secoc_authcodec1_b0_msb) {
  cdc_secoc_authcodec1_b0_msb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec1_b0_msb);
  int x = cdc_secoc_authcodec1_b0_msb;

  Byte_set_value(x, 0, 8,  Data + 29);
}


void Cdccontrol1680_set_p_cdc_secoc_authcodec1_b1(int cdc_secoc_authcodec1_b1) {
  cdc_secoc_authcodec1_b1 = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec1_b1);
  int x = cdc_secoc_authcodec1_b1;

  Byte_set_value(x, 0, 8,  Data + 30);
}


void Cdccontrol1680_set_p_cdc_secoc_authcodec1_b2_lsb(int cdc_secoc_authcodec1_b2_lsb) {
  cdc_secoc_authcodec1_b2_lsb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec1_b2_lsb);
  int x = cdc_secoc_authcodec1_b2_lsb;

  Byte_set_value(x, 0, 8,  Data + 31);
}


void Cdccontrol1680_set_p_cdc_ffisheyeledctrl(Cdc_control1_680_Cdc_ffisheyeledctrlType cdc_ffisheyeledctrl) {
  int x = cdc_ffisheyeledctrl;

  Byte_set_value(x, 2, 2,  Data + 1);
}


void Cdccontrol1680_set_p_cdc_flfisheyeledctrl(Cdc_control1_680_Cdc_flfisheyeledctrlType cdc_flfisheyeledctrl) {
  int x = cdc_flfisheyeledctrl;

  Byte_set_value(x, 4, 2,  Data + 1);
}


void Cdccontrol1680_set_p_cdc_frfisheyeledctrl(Cdc_control1_680_Cdc_frfisheyeledctrlType cdc_frfisheyeledctrl) {
  int x = cdc_frfisheyeledctrl;

  Byte_set_value(x, 6, 2,  Data + 1);
}


void Cdccontrol1680_set_p_cdc_rfisheyeledctrl(Cdc_control1_680_Cdc_rfisheyeledctrlType cdc_rfisheyeledctrl) {
  int x = cdc_rfisheyeledctrl;

  Byte_set_value(x, 6, 2,  Data + 14);
}


void Cdccontrol1680_set_p_cdc_frchassisvvctrl(Cdc_control1_680_Cdc_frchassisvvctrlType cdc_frchassisvvctrl) {
  int x = cdc_frchassisvvctrl;

  Byte_set_value(x, 0, 2,  Data + 20);
}


void Cdccontrol1680_set_p_cdc_midchassisvvctrl(Cdc_control1_680_Cdc_midchassisvvctrlType cdc_midchassisvvctrl) {
  int x = cdc_midchassisvvctrl;

  Byte_set_value(x, 2, 2,  Data + 20);
}


void Cdccontrol1680_set_p_cdc_rechassisvvctrl(Cdc_control1_680_Cdc_rechassisvvctrlType cdc_rechassisvvctrl) {
  int x = cdc_rechassisvvctrl;

  Byte_set_value(x, 4, 2,  Data + 20);
}


void Cdccontrol1680_set_p_cdc_frchassiscmrheatpwrctrl(Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType cdc_frchassiscmrheatpwrctrl) {
  int x = cdc_frchassiscmrheatpwrctrl;

  Byte_set_value(x, 6, 2,  Data + 20);
}


void Cdccontrol1680_set_p_cdc_michassiscmrheatpwrctrl(Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType cdc_michassiscmrheatpwrctrl) {
  int x = cdc_michassiscmrheatpwrctrl;

  Byte_set_value(x, 0, 2,  Data + 21);
}


void Cdccontrol1680_set_p_cdc_rechassiscmrheatpwrctrl(Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType cdc_rechassiscmrheatpwrctrl) {
  int x = cdc_rechassiscmrheatpwrctrl;

  Byte_set_value(x, 2, 2,  Data + 21);
}

void Cdccontrol1680_set_p_cdc_ChassisCmrLEDPwrctrl(Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType cdc_ChassisCmrLEDPwrctrl) {
  int x = cdc_ChassisCmrLEDPwrctrl;

  Byte_set_value(x, 2, 2,  Data + 30);
}

void Cdccontrol1680_set_p_cdc_MidchsCAMLEDPwrctrl(Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType cdc_MidchsCAMLEDPwrctrl) {
  int x = cdc_MidchsCAMLEDPwrctrl;

  Byte_set_value(x, 4, 2,  Data + 37);
}

void Cdccontrol1680_set_p_cdc_RechsCAMLEDPwrctrl(Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType cdc_RechsCAMLEDPwrctrl) {
  int x = cdc_RechsCAMLEDPwrctrl;

  Byte_set_value(x, 2, 2,  Data + 37);
}



void Cdccontrol1680_UpdateData(Cdc_control1_680 *cdc_control1_680) {
    Cdccontrol1680_set_p_cdc_rechassistgtctrl( cdc_control1_680->cdc_rechassistgtctrl);
    Cdccontrol1680_set_p_cdc_rechassisctrl( cdc_control1_680->cdc_rechassisctrl);
    Cdccontrol1680_set_p_cdc_midchassistgtctrl( cdc_control1_680->cdc_midchassistgtctrl);
    Cdccontrol1680_set_p_cdc_midchassisctrl( cdc_control1_680->cdc_midchassisctrl);
    Cdccontrol1680_set_p_cdc_frchassistgtctrl( cdc_control1_680->cdc_frchassistgtctrl);
    Cdccontrol1680_set_p_cdc_frchassisctrl( cdc_control1_680->cdc_frchassisctrl);
    Cdccontrol1680_set_p_cdc_front_shield_pwr_req( cdc_control1_680->cdc_front_shield_pwr_req);
    Cdccontrol1680_set_p_cdc_fisheye_r_cmr_shield_req( cdc_control1_680->cdc_fisheye_r_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_fisheye_l_cmr_shield_req( cdc_control1_680->cdc_fisheye_l_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_fisheye_f_cmr_shield_req( cdc_control1_680->cdc_fisheye_f_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_cleanpumpecupwr( cdc_control1_680->cdc_cleanpumpecupwr);
    Cdccontrol1680_set_p_cdc_cleanpumpspd( cdc_control1_680->cdc_cleanpumpspd);
    Cdccontrol1680_set_p_cdc_rilidarwprpwr( cdc_control1_680->cdc_rilidarwprpwr);
    Cdccontrol1680_set_p_cdc_lelidarwprpwr( cdc_control1_680->cdc_lelidarwprpwr);
    Cdccontrol1680_set_p_cdc_auto_lp_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_lp_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_frlidarwprpwr( cdc_control1_680->cdc_frlidarwprpwr);
    Cdccontrol1680_set_p_cdc_rilidarwprmove( cdc_control1_680->cdc_rilidarwprmove);
    Cdccontrol1680_set_p_cdc_lelidarwprmove( cdc_control1_680->cdc_lelidarwprmove);
    Cdccontrol1680_set_p_cdc_auto_lp_cmr_wpr_req( cdc_control1_680->cdc_auto_lp_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_frlidarwprmove( cdc_control1_680->cdc_frlidarwprmove);
    Cdccontrol1680_set_p_cdc_rilidarvalvepwr( cdc_control1_680->cdc_rilidarvalvepwr);
    Cdccontrol1680_set_p_cdc_fisheye_r_cmr_valve_pwr_req( cdc_control1_680->cdc_fisheye_r_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_frlidarvalvepwr( cdc_control1_680->cdc_frlidarvalvepwr);
    Cdccontrol1680_set_p_cdc_fisheye_f_cmr_valve_pwr_req( cdc_control1_680->cdc_fisheye_f_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_lelidarvalvepwr( cdc_control1_680->cdc_lelidarvalvepwr);
    Cdccontrol1680_set_p_cdc_fisheye_l_cmr_valve_pwr_req( cdc_control1_680->cdc_fisheye_l_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_lp_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_lp_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_fisheye_l_cmr_heat_req( cdc_control1_680->cdc_fisheye_l_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_fisheye_f_cmr_heat_req( cdc_control1_680->cdc_fisheye_f_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_fisheye_r_cmr_heat_req( cdc_control1_680->cdc_fisheye_r_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_lp_cmr_heat_req( cdc_control1_680->cdc_auto_lp_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_top_shield_pwr_req( cdc_control1_680->cdc_top_shield_pwr_req);
    Cdccontrol1680_set_p_cdc_mlidarshieldmove( cdc_control1_680->cdc_mlidarshieldmove);
    Cdccontrol1680_set_p_cdc_front_cmr_sreen_shield_req( cdc_control1_680->cdc_front_cmr_sreen_shield_req);
    Cdccontrol1680_set_p_cdc_auto_l_cmr_shield_req( cdc_control1_680->cdc_auto_l_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_auto_r_cmr_shield_req( cdc_control1_680->cdc_auto_r_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_auto_b_cmr_shield_req( cdc_control1_680->cdc_auto_b_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_auto_rf_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_rf_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_lf_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_lf_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_r_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_r_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_l_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_l_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_b_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_b_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_rf_cmr_wpr_req( cdc_control1_680->cdc_auto_rf_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_lf_cmr_wpr_req( cdc_control1_680->cdc_auto_lf_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_r_cmr_wpr_req( cdc_control1_680->cdc_auto_r_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_l_cmr_wpr_req( cdc_control1_680->cdc_auto_l_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_b_cmr_wpr_req( cdc_control1_680->cdc_auto_b_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_b_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_b_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_mlidarvalvepwr( cdc_control1_680->cdc_mlidarvalvepwr);
    Cdccontrol1680_set_p_cdc_auto_lf_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_lf_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_rf_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_rf_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_l_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_l_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_r_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_r_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_b_cmr_heat_req( cdc_control1_680->cdc_auto_b_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_r_cmr_heat_req( cdc_control1_680->cdc_auto_r_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_l_cmr_heat_req( cdc_control1_680->cdc_auto_l_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_rf_cmr_heat_req( cdc_control1_680->cdc_auto_rf_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_lf_cmr_heat_req( cdc_control1_680->cdc_auto_lf_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_rear_shield_pwr_req( cdc_control1_680->cdc_rear_shield_pwr_req);
    Cdccontrol1680_set_p_cdc_fisheye_b_cmr_shield_req( cdc_control1_680->cdc_fisheye_b_cmr_shield_req);
    Cdccontrol1680_set_p_cdc_relidarwprpwr( cdc_control1_680->cdc_relidarwprpwr);
    Cdccontrol1680_set_p_cdc_auto_lb_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_lb_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_rb_cmr_wpr_pwr_req( cdc_control1_680->cdc_auto_rb_cmr_wpr_pwr_req);
    Cdccontrol1680_set_p_cdc_relidarwprmove( cdc_control1_680->cdc_relidarwprmove);
    Cdccontrol1680_set_p_cdc_auto_lb_cmr_wpr_req( cdc_control1_680->cdc_auto_lb_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_rb_cmr_wpr_req( cdc_control1_680->cdc_auto_rb_cmr_wpr_req);
    Cdccontrol1680_set_p_cdc_auto_rl_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_rl_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_auto_rr_cmr_valve_pwr_req( cdc_control1_680->cdc_auto_rr_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_fisheye_b_cmr_valve_pwr_req( cdc_control1_680->cdc_fisheye_b_cmr_valve_pwr_req);
    Cdccontrol1680_set_p_cdc_relidarvalvepwr( cdc_control1_680->cdc_relidarvalvepwr);
    Cdccontrol1680_set_p_cdc_auto_rb_cmr_heat_req( cdc_control1_680->cdc_auto_rb_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_auto_lb_cmr_heat_req( cdc_control1_680->cdc_auto_lb_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_fisheye_b_cmr_heat_req( cdc_control1_680->cdc_fisheye_b_cmr_heat_req);
    Cdccontrol1680_set_p_cdc_secoc_freshnesscounterc1( cdc_control1_680->cdc_secoc_freshnesscounterc1);
    Cdccontrol1680_set_p_cdc_secoc_authcodec1_b0_msb( cdc_control1_680->cdc_secoc_authcodec1_b0_msb);
    Cdccontrol1680_set_p_cdc_secoc_authcodec1_b1( cdc_control1_680->cdc_secoc_authcodec1_b1);
    Cdccontrol1680_set_p_cdc_secoc_authcodec1_b2_lsb( cdc_control1_680->cdc_secoc_authcodec1_b2_lsb);
    Cdccontrol1680_set_p_cdc_ffisheyeledctrl( cdc_control1_680->cdc_ffisheyeledctrl);
    Cdccontrol1680_set_p_cdc_flfisheyeledctrl( cdc_control1_680->cdc_flfisheyeledctrl);
    Cdccontrol1680_set_p_cdc_frfisheyeledctrl( cdc_control1_680->cdc_frfisheyeledctrl);
    Cdccontrol1680_set_p_cdc_rfisheyeledctrl( cdc_control1_680->cdc_rfisheyeledctrl);
    Cdccontrol1680_set_p_cdc_frchassisvvctrl( cdc_control1_680->cdc_frchassisvvctrl);
    Cdccontrol1680_set_p_cdc_midchassisvvctrl( cdc_control1_680->cdc_midchassisvvctrl);
    Cdccontrol1680_set_p_cdc_rechassisvvctrl( cdc_control1_680->cdc_rechassisvvctrl);
    Cdccontrol1680_set_p_cdc_frchassiscmrheatpwrctrl( cdc_control1_680->cdc_frchassiscmrheatpwrctrl);
    Cdccontrol1680_set_p_cdc_michassiscmrheatpwrctrl( cdc_control1_680->cdc_michassiscmrheatpwrctrl);
    Cdccontrol1680_set_p_cdc_rechassiscmrheatpwrctrl( cdc_control1_680->cdc_rechassiscmrheatpwrctrl);
    Cdccontrol1680_set_p_cdc_ChassisCmrLEDPwrctrl( cdc_control1_680->cdc_ChassisCmrLEDPwrctrl);
    Cdccontrol1680_set_p_cdc_MidchsCAMLEDPwrctrl( cdc_control1_680->cdc_MidchsCAMLEDPwrctrl);
    Cdccontrol1680_set_p_cdc_RechsCAMLEDPwrctrl( cdc_control1_680->cdc_RechsCAMLEDPwrctrl);
}

void Cdccontrol1680_Reset(Cdc_control1_680 *cdc_control1_680) {
     cdc_control1_680->cdc_rechassistgtctrl = 0.0;
     cdc_control1_680->cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_NOT_MOVE_E0;
     cdc_control1_680->cdc_midchassistgtctrl = 0.0;
     cdc_control1_680->cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_NOT_MOVE_E0;
     cdc_control1_680->cdc_frchassistgtctrl = 0.0;
     cdc_control1_680->cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_NOT_MOVE_E0;
     cdc_control1_680->cdc_front_shield_pwr_req = Cdc_control1_680_Cdc_front_shield_pwr_reqType_CDC_FRONT_SHIELD_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_r_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_l_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_f_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_INVALID_E0;
     cdc_control1_680->cdc_cleanpumpspd = 0.0;
     cdc_control1_680->cdc_rilidarwprpwr = Cdc_control1_680_Cdc_rilidarwprpwrType_CDC_RILIDARWPRPWR_INVALID_E0;
     cdc_control1_680->cdc_lelidarwprpwr = Cdc_control1_680_Cdc_lelidarwprpwrType_CDC_LELIDARWPRPWR_INVALID_E0;
     cdc_control1_680->cdc_auto_lp_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType_CDC_AUTO_LP_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_frlidarwprpwr = Cdc_control1_680_Cdc_frlidarwprpwrType_CDC_FRLIDARWPRPWR_INVALID_E0;
     cdc_control1_680->cdc_rilidarwprmove = Cdc_control1_680_Cdc_rilidarwprmoveType_CDC_RILIDARWPRMOVE_NO_REQUEST_E0;
     cdc_control1_680->cdc_lelidarwprmove = Cdc_control1_680_Cdc_lelidarwprmoveType_CDC_LELIDARWPRMOVE_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_lp_cmr_wpr_req = Cdc_control1_680_Cdc_auto_lp_cmr_wpr_reqType_CDC_AUTO_LP_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_frlidarwprmove = Cdc_control1_680_Cdc_frlidarwprmoveType_CDC_FRLIDARWPRMOVE_NO_REQUEST_E0;
     cdc_control1_680->cdc_rilidarvalvepwr = Cdc_control1_680_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_INVALID_E0;
     cdc_control1_680->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control1_680_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_frlidarvalvepwr = Cdc_control1_680_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_INVALID_E0;
     cdc_control1_680->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control1_680_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_lelidarvalvepwr = Cdc_control1_680_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_INVALID_E0;
     cdc_control1_680->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control1_680_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_l_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_f_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_r_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_lp_cmr_heat_req = Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_top_shield_pwr_req = Cdc_control1_680_Cdc_top_shield_pwr_reqType_CDC_TOP_SHIELD_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_INVALID_E0;
     cdc_control1_680->cdc_front_cmr_sreen_shield_req = Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_l_cmr_shield_req = Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_r_cmr_shield_req = Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_b_cmr_shield_req = Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rf_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType_CDC_AUTO_RF_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_lf_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType_CDC_AUTO_LF_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_r_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType_CDC_AUTO_R_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_l_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType_CDC_AUTO_L_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_b_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType_CDC_AUTO_B_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rf_cmr_wpr_req = Cdc_control1_680_Cdc_auto_rf_cmr_wpr_reqType_CDC_AUTO_RF_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_lf_cmr_wpr_req = Cdc_control1_680_Cdc_auto_lf_cmr_wpr_reqType_CDC_AUTO_LF_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_r_cmr_wpr_req = Cdc_control1_680_Cdc_auto_r_cmr_wpr_reqType_CDC_AUTO_R_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_l_cmr_wpr_req = Cdc_control1_680_Cdc_auto_l_cmr_wpr_reqType_CDC_AUTO_L_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_b_cmr_wpr_req = Cdc_control1_680_Cdc_auto_b_cmr_wpr_reqType_CDC_AUTO_B_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_b_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_mlidarvalvepwr = Cdc_control1_680_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_INVALID_E0;
     cdc_control1_680->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_l_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_r_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_b_cmr_heat_req = Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_r_cmr_heat_req = Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_l_cmr_heat_req = Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rf_cmr_heat_req = Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_lf_cmr_heat_req = Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_rear_shield_pwr_req = Cdc_control1_680_Cdc_rear_shield_pwr_reqType_CDC_REAR_SHIELD_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_b_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_INVALID_E0;
     cdc_control1_680->cdc_relidarwprpwr = Cdc_control1_680_Cdc_relidarwprpwrType_CDC_RELIDARWPRPWR_INVALID_E0;
     cdc_control1_680->cdc_auto_lb_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_lb_cmr_wpr_pwr_reqType_CDC_AUTO_LB_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rb_cmr_wpr_pwr_req = Cdc_control1_680_Cdc_auto_rb_cmr_wpr_pwr_reqType_CDC_AUTO_RB_CMR_WPR_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_relidarwprmove = Cdc_control1_680_Cdc_relidarwprmoveType_CDC_RELIDARWPRMOVE_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_lb_cmr_wpr_req = Cdc_control1_680_Cdc_auto_lb_cmr_wpr_reqType_CDC_AUTO_LB_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_rb_cmr_wpr_req = Cdc_control1_680_Cdc_auto_rb_cmr_wpr_reqType_CDC_AUTO_RB_CMR_WPR_REQ_NO_REQUEST_E0;
     cdc_control1_680->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control1_680_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control1_680_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_INVALID_E0;
     cdc_control1_680->cdc_relidarvalvepwr = Cdc_control1_680_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_INVALID_E0;
     cdc_control1_680->cdc_auto_rb_cmr_heat_req = Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_auto_lb_cmr_heat_req = Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_fisheye_b_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_INVALID_E0;
     cdc_control1_680->cdc_secoc_freshnesscounterc1 = 0.0;
     cdc_control1_680->cdc_secoc_authcodec1_b0_msb = 0.0;
     cdc_control1_680->cdc_secoc_authcodec1_b1 = 0.0;
     cdc_control1_680->cdc_secoc_authcodec1_b2_lsb = 0.0;
     cdc_control1_680->cdc_ffisheyeledctrl = Cdc_control1_680_Cdc_ffisheyeledctrlType_CDC_FFISHEYELEDCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_flfisheyeledctrl = Cdc_control1_680_Cdc_flfisheyeledctrlType_CDC_FLFISHEYELEDCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_frfisheyeledctrl = Cdc_control1_680_Cdc_frfisheyeledctrlType_CDC_FRFISHEYELEDCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_rfisheyeledctrl = Cdc_control1_680_Cdc_rfisheyeledctrlType_CDC_RFISHEYELEDCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_frchassisvvctrl = Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_midchassisvvctrl = Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_rechassisvvctrl = Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_frchassiscmrheatpwrctrl = Cdc_control1_680_Cdc_frchassiscmrheatpwrctrlType_CDC_FRCHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_michassiscmrheatpwrctrl = Cdc_control1_680_Cdc_michassiscmrheatpwrctrlType_CDC_MICHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_rechassiscmrheatpwrctrl = Cdc_control1_680_Cdc_rechassiscmrheatpwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_ChassisCmrLEDPwrctrl = Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_MidchsCAMLEDPwrctrl = Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
     cdc_control1_680->cdc_RechsCAMLEDPwrctrl = Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType_CDC_RECHASSISCMRHEATPWRCTRL_NO_REQUEST_E0;
}

//void Cdccontrol1680_init(void) {
//  Cdccontrol1680_g.can_index = 0;
//  Cdccontrol1680_g.msg_id = 0x680;
//  Cdccontrol1680_g.msg_length = 24;
//  Cdccontrol1680_g.msg_period = 0;//200;
//  Cdccontrol1680_g.is_canfd = 1;
//  Cdccontrol1680_g.Data = msg_rawdata;
//  memset(Cdccontrol1680_g.Data, 0x00, Cdccontrol1680_g.msg_length);
//  Cdccontrol1680_g.reset = Cdccontrol1680_Reset;
//  Cdccontrol1680_g.UpdateData = Cdccontrol1680_UpdateData;
//}
