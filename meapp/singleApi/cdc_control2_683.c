#include "cdc_control2_683.h"
#include "byte.h"

//control_msg_st Cdccontrol2683_g;
//static uint8_t msg_rawData683[64];

static char msg_rawData683[64];
char *Data683 = msg_rawData683;


void Cdccontrol2683_set_p_cdc_rilidarvalvepwr(Cdc_control2_683_Cdc_rilidarvalvepwrType cdc_rilidarvalvepwr) {
  int x = cdc_rilidarvalvepwr;

  Byte_set_value(x, 0, 2, Data683 + 2);
}


void Cdccontrol2683_set_p_cdc_fisheye_r_cmr_valve_pwr_req(Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType cdc_fisheye_r_cmr_valve_pwr_req) {
  int x = cdc_fisheye_r_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2, Data683 + 2);
}


void Cdccontrol2683_set_p_cdc_frlidarvalvepwr(Cdc_control2_683_Cdc_frlidarvalvepwrType cdc_frlidarvalvepwr) {
  int x = cdc_frlidarvalvepwr;

  Byte_set_value(x, 4, 2, Data683 + 2);
}


void Cdccontrol2683_set_p_cdc_fisheye_f_cmr_valve_pwr_req(Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType cdc_fisheye_f_cmr_valve_pwr_req) {
  int x = cdc_fisheye_f_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2, Data683 + 2);
}


void Cdccontrol2683_set_p_cdc_lelidarvalvepwr(Cdc_control2_683_Cdc_lelidarvalvepwrType cdc_lelidarvalvepwr) {
  int x = cdc_lelidarvalvepwr;

  Byte_set_value(x, 0, 2, Data683 + 3);
}


void Cdccontrol2683_set_p_cdc_fisheye_l_cmr_valve_pwr_req(Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType cdc_fisheye_l_cmr_valve_pwr_req) {
  int x = cdc_fisheye_l_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2, Data683 + 3);
}


void Cdccontrol2683_set_p_cdc_auto_lp_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType cdc_auto_lp_cmr_valve_pwr_req) {
  int x = cdc_auto_lp_cmr_valve_pwr_req;

  Byte_set_value(x, 4, 2, Data683 + 3);
}


void Cdccontrol2683_set_p_cdc_auto_b_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType cdc_auto_b_cmr_valve_pwr_req) {
  int x = cdc_auto_b_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2, Data683 + 3);
}


void Cdccontrol2683_set_p_cdc_mlidarvalvepwr(Cdc_control2_683_Cdc_mlidarvalvepwrType cdc_mlidarvalvepwr) {
  int x = cdc_mlidarvalvepwr;

  Byte_set_value(x, 0, 2, Data683 + 4);
}


void Cdccontrol2683_set_p_cdc_auto_lf_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType cdc_auto_lf_cmr_valve_pwr_req) {
  int x = cdc_auto_lf_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2, Data683 + 4);
}


void Cdccontrol2683_set_p_cdc_auto_rf_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType cdc_auto_rf_cmr_valve_pwr_req) {
  int x = cdc_auto_rf_cmr_valve_pwr_req;

  Byte_set_value(x, 4, 2, Data683 + 4);
}


void Cdccontrol2683_set_p_cdc_auto_l_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType cdc_auto_l_cmr_valve_pwr_req) {
  int x = cdc_auto_l_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2, Data683 + 4);
}


void Cdccontrol2683_set_p_cdc_auto_r_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType cdc_auto_r_cmr_valve_pwr_req) {
  int x = cdc_auto_r_cmr_valve_pwr_req;

  Byte_set_value(x, 0, 2, Data683 + 5);
}


void Cdccontrol2683_set_p_cdc_auto_rl_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType cdc_auto_rl_cmr_valve_pwr_req) {
  int x = cdc_auto_rl_cmr_valve_pwr_req;

  Byte_set_value(x, 2, 2, Data683 + 5);
}


void Cdccontrol2683_set_p_cdc_auto_rr_cmr_valve_pwr_req(Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType cdc_auto_rr_cmr_valve_pwr_req) {
  int x = cdc_auto_rr_cmr_valve_pwr_req;

  Byte_set_value(x, 4, 2, Data683 + 5);
}


void Cdccontrol2683_set_p_cdc_fisheye_b_cmr_valve_pwr_req(Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType cdc_fisheye_b_cmr_valve_pwr_req) {
  int x = cdc_fisheye_b_cmr_valve_pwr_req;

  Byte_set_value(x, 6, 2, Data683 + 5);
}


void Cdccontrol2683_set_p_cdc_relidarvalvepwr(Cdc_control2_683_Cdc_relidarvalvepwrType cdc_relidarvalvepwr) {
  int x = cdc_relidarvalvepwr;

  Byte_set_value(x, 0, 2, Data683 + 6);
}


void Cdccontrol2683_set_p_cdc_frchassisvvctrl(Cdc_control2_683_Cdc_frchassisvvctrlType cdc_frchassisvvctrl) {
  int x = cdc_frchassisvvctrl;

  Byte_set_value(x, 2, 2, Data683 + 6);
}


void Cdccontrol2683_set_p_cdc_midchassisvvctrl(Cdc_control2_683_Cdc_midchassisvvctrlType cdc_midchassisvvctrl) {
  int x = cdc_midchassisvvctrl;

  Byte_set_value(x, 4, 2, Data683 + 6);
}


void Cdccontrol2683_set_p_cdc_rechassisvvctrl(Cdc_control2_683_Cdc_rechassisvvctrlType cdc_rechassisvvctrl) {
  int x = cdc_rechassisvvctrl;

  Byte_set_value(x, 6, 2, Data683 + 6);
}


void Cdccontrol2683_set_p_cdc_rilidarvalvespraynum(int cdc_rilidarvalvespraynum) {
  cdc_rilidarvalvespraynum = ProtocolData_BoundedValue(0, 15, cdc_rilidarvalvespraynum);
  int x = cdc_rilidarvalvespraynum;

  Byte_set_value(x, 0, 4, Data683 + 7);
}


void Cdccontrol2683_set_p_cdc_rilidarvalveopdrtn(double cdc_rilidarvalveopdrtn) {
  cdc_rilidarvalveopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_rilidarvalveopdrtn);
  int x = cdc_rilidarvalveopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 7);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 8);
}


void Cdccontrol2683_set_p_cdc_rilidarvalveclsdrtn(double cdc_rilidarvalveclsdrtn) {
  cdc_rilidarvalveclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_rilidarvalveclsdrtn);
  int x = cdc_rilidarvalveclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 8);
}


void Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvspraynum(int cdc_fisheye_r_cmrvvspraynum) {
  cdc_fisheye_r_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_fisheye_r_cmrvvspraynum);
  int x = cdc_fisheye_r_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 9);
}


void Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvopdrtn(double cdc_fisheye_r_cmrvvopdrtn) {
  cdc_fisheye_r_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_fisheye_r_cmrvvopdrtn);
  int x = cdc_fisheye_r_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 9);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 10);
}


void Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvclsdrtn(double cdc_fisheye_r_cmrvvclsdrtn) {
  cdc_fisheye_r_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_fisheye_r_cmrvvclsdrtn);
  int x = cdc_fisheye_r_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 10);
}


void Cdccontrol2683_set_p_cdc_frlidarvalvespraynum(int cdc_frlidarvalvespraynum) {
  cdc_frlidarvalvespraynum = ProtocolData_BoundedValue(0, 15, cdc_frlidarvalvespraynum);
  int x = cdc_frlidarvalvespraynum;

  Byte_set_value(x, 0, 4, Data683 + 11);
}


void Cdccontrol2683_set_p_cdc_frlidarvalveopdrtn(double cdc_frlidarvalveopdrtn) {
  cdc_frlidarvalveopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_frlidarvalveopdrtn);
  int x = cdc_frlidarvalveopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 11);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 12);
}


void Cdccontrol2683_set_p_cdc_frlidarvalveclsdrtn(double cdc_frlidarvalveclsdrtn) {
  cdc_frlidarvalveclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_frlidarvalveclsdrtn);
  int x = cdc_frlidarvalveclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 12);
}


void Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvspraynum(int cdc_fisheye_f_cmrvvspraynum) {
  cdc_fisheye_f_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_fisheye_f_cmrvvspraynum);
  int x = cdc_fisheye_f_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 13);
}


void Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvopdrtn(double cdc_fisheye_f_cmrvvopdrtn) {
  cdc_fisheye_f_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_fisheye_f_cmrvvopdrtn);
  int x = cdc_fisheye_f_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 13);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 14);
}


void Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvclsdrtn(double cdc_fisheye_f_cmrvvclsdrtn) {
  cdc_fisheye_f_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_fisheye_f_cmrvvclsdrtn);
  int x = cdc_fisheye_f_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 14);
}


void Cdccontrol2683_set_p_cdc_lelidarvalvespraynum(int cdc_lelidarvalvespraynum) {
  cdc_lelidarvalvespraynum = ProtocolData_BoundedValue(0, 15, cdc_lelidarvalvespraynum);
  int x = cdc_lelidarvalvespraynum;

  Byte_set_value(x, 0, 4, Data683 + 15);
}


void Cdccontrol2683_set_p_cdc_lelidarvalveopdrtn(double cdc_lelidarvalveopdrtn) {
  cdc_lelidarvalveopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_lelidarvalveopdrtn);
  int x = cdc_lelidarvalveopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 15);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 16);
}


void Cdccontrol2683_set_p_cdc_lelidarvalveclsdrtn(double cdc_lelidarvalveclsdrtn) {
  cdc_lelidarvalveclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_lelidarvalveclsdrtn);
  int x = cdc_lelidarvalveclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 16);
}


void Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvspraynum(int cdc_fisheye_l_cmrvvspraynum) {
  cdc_fisheye_l_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_fisheye_l_cmrvvspraynum);
  int x = cdc_fisheye_l_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 17);
}


void Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvopdrtn(double cdc_fisheye_l_cmrvvopdrtn) {
  cdc_fisheye_l_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_fisheye_l_cmrvvopdrtn);
  int x = cdc_fisheye_l_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 17);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 18);
}


void Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvclsdrtn(double cdc_fisheye_l_cmrvvclsdrtn) {
  cdc_fisheye_l_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_fisheye_l_cmrvvclsdrtn);
  int x = cdc_fisheye_l_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 18);
}


void Cdccontrol2683_set_p_cdc_auto_lp_cmrvvspraynum(int cdc_auto_lp_cmrvvspraynum) {
  cdc_auto_lp_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_lp_cmrvvspraynum);
  int x = cdc_auto_lp_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 19);
}


void Cdccontrol2683_set_p_cdc_auto_lp_cmrvvopdrtn(double cdc_auto_lp_cmrvvopdrtn) {
  cdc_auto_lp_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_lp_cmrvvopdrtn);
  int x = cdc_auto_lp_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 19);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 20);
}


void Cdccontrol2683_set_p_cdc_auto_lp_cmrvvclsdrtn(double cdc_auto_lp_cmrvvclsdrtn) {
  cdc_auto_lp_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_lp_cmrvvclsdrtn);
  int x = cdc_auto_lp_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 20);
}


void Cdccontrol2683_set_p_cdc_auto_b_cmrvvspraynum(int cdc_auto_b_cmrvvspraynum) {
  cdc_auto_b_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_b_cmrvvspraynum);
  int x = cdc_auto_b_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 21);
}


void Cdccontrol2683_set_p_cdc_auto_b_cmrvvopdrtn(double cdc_auto_b_cmrvvopdrtn) {
  cdc_auto_b_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_b_cmrvvopdrtn);
  int x = cdc_auto_b_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 21);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 22);
}


void Cdccontrol2683_set_p_cdc_auto_b_cmrvvclsdrtn(double cdc_auto_b_cmrvvclsdrtn) {
  cdc_auto_b_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_b_cmrvvclsdrtn);
  int x = cdc_auto_b_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 22);
}


void Cdccontrol2683_set_p_cdc_mlidarvvspraynum(int cdc_mlidarvvspraynum) {
  cdc_mlidarvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_mlidarvvspraynum);
  int x = cdc_mlidarvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 23);
}


void Cdccontrol2683_set_p_cdc_mlidarvvopdrtn(double cdc_mlidarvvopdrtn) {
  cdc_mlidarvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_mlidarvvopdrtn);
  int x = cdc_mlidarvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 23);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 24);
}


void Cdccontrol2683_set_p_cdc_mlidarvvclsdrtn(double cdc_mlidarvvclsdrtn) {
  cdc_mlidarvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_mlidarvvclsdrtn);
  int x = cdc_mlidarvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 24);
}


void Cdccontrol2683_set_p_cdc_auto_lf_cmrvvspraynum(int cdc_auto_lf_cmrvvspraynum) {
  cdc_auto_lf_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_lf_cmrvvspraynum);
  int x = cdc_auto_lf_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 25);
}


void Cdccontrol2683_set_p_cdc_auto_lf_cmrvvopdrtn(double cdc_auto_lf_cmrvvopdrtn) {
  cdc_auto_lf_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_lf_cmrvvopdrtn);
  int x = cdc_auto_lf_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 25);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 26);
}


void Cdccontrol2683_set_p_cdc_auto_lf_cmrvvclsdrtn(double cdc_auto_lf_cmrvvclsdrtn) {
  cdc_auto_lf_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_lf_cmrvvclsdrtn);
  int x = cdc_auto_lf_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 26);
}


void Cdccontrol2683_set_p_cdc_auto_rf_cmrvvspraynum(int cdc_auto_rf_cmrvvspraynum) {
  cdc_auto_rf_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_rf_cmrvvspraynum);
  int x = cdc_auto_rf_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 27);
}


void Cdccontrol2683_set_p_cdc_auto_rf_cmrvvopdrtn(double cdc_auto_rf_cmrvvopdrtn) {
  cdc_auto_rf_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_rf_cmrvvopdrtn);
  int x = cdc_auto_rf_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 27);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 28);
}


void Cdccontrol2683_set_p_cdc_auto_rf_cmrvvclsdrtn(double cdc_auto_rf_cmrvvclsdrtn) {
  cdc_auto_rf_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_rf_cmrvvclsdrtn);
  int x = cdc_auto_rf_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 28);
}


void Cdccontrol2683_set_p_cdc_auto_l_cmrvvspraynum(int cdc_auto_l_cmrvvspraynum) {
  cdc_auto_l_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_l_cmrvvspraynum);
  int x = cdc_auto_l_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 29);
}


void Cdccontrol2683_set_p_cdc_auto_l_cmrvvopdrtn(double cdc_auto_l_cmrvvopdrtn) {
  cdc_auto_l_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_l_cmrvvopdrtn);
  int x = cdc_auto_l_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 29);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 30);
}


void Cdccontrol2683_set_p_cdc_auto_l_cmrvvclsdrtn(double cdc_auto_l_cmrvvclsdrtn) {
  cdc_auto_l_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_l_cmrvvclsdrtn);
  int x = cdc_auto_l_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 30);
}


void Cdccontrol2683_set_p_cdc_auto_r_cmrvvspraynum(int cdc_auto_r_cmrvvspraynum) {
  cdc_auto_r_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_r_cmrvvspraynum);
  int x = cdc_auto_r_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 31);
}


void Cdccontrol2683_set_p_cdc_auto_r_cmrvvopdrtn(double cdc_auto_r_cmrvvopdrtn) {
  cdc_auto_r_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_r_cmrvvopdrtn);
  int x = cdc_auto_r_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 31);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 32);
}


void Cdccontrol2683_set_p_cdc_auto_r_cmrvvclsdrtn(double cdc_auto_r_cmrvvclsdrtn) {
  cdc_auto_r_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_r_cmrvvclsdrtn);
  int x = cdc_auto_r_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 32);
}


void Cdccontrol2683_set_p_cdc_auto_rl_cmrvvspraynum(int cdc_auto_rl_cmrvvspraynum) {
  cdc_auto_rl_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_rl_cmrvvspraynum);
  int x = cdc_auto_rl_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 33);
}


void Cdccontrol2683_set_p_cdc_auto_rl_cmrvvopdrtn(double cdc_auto_rl_cmrvvopdrtn) {
  cdc_auto_rl_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_rl_cmrvvopdrtn);
  int x = cdc_auto_rl_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 33);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 34);
}


void Cdccontrol2683_set_p_cdc_auto_rl_cmrvvclsdrtn(double cdc_auto_rl_cmrvvclsdrtn) {
  cdc_auto_rl_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_rl_cmrvvclsdrtn);
  int x = cdc_auto_rl_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 34);
}


void Cdccontrol2683_set_p_cdc_auto_rr_cmrvvspraynum(int cdc_auto_rr_cmrvvspraynum) {
  cdc_auto_rr_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_auto_rr_cmrvvspraynum);
  int x = cdc_auto_rr_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 35);
}


void Cdccontrol2683_set_p_cdc_auto_rr_cmrvvopdrtn(double cdc_auto_rr_cmrvvopdrtn) {
  cdc_auto_rr_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_auto_rr_cmrvvopdrtn);
  int x = cdc_auto_rr_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 35);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 36);
}


void Cdccontrol2683_set_p_cdc_auto_rr_cmrvvclsdrtn(double cdc_auto_rr_cmrvvclsdrtn) {
  cdc_auto_rr_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_auto_rr_cmrvvclsdrtn);
  int x = cdc_auto_rr_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 36);
}


void Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvspraynum(int cdc_fisheye_b_cmrvvspraynum) {
  cdc_fisheye_b_cmrvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_fisheye_b_cmrvvspraynum);
  int x = cdc_fisheye_b_cmrvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 37);
}


void Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvopdrtn(double cdc_fisheye_b_cmrvvopdrtn) {
  cdc_fisheye_b_cmrvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_fisheye_b_cmrvvopdrtn);
  int x = cdc_fisheye_b_cmrvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 37);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 38);
}


void Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvclsdrtn(double cdc_fisheye_b_cmrvvclsdrtn) {
  cdc_fisheye_b_cmrvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_fisheye_b_cmrvvclsdrtn);
  int x = cdc_fisheye_b_cmrvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 38);
}


void Cdccontrol2683_set_p_cdc_relidarvvspraynum(int cdc_relidarvvspraynum) {
  cdc_relidarvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_relidarvvspraynum);
  int x = cdc_relidarvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 39);
}


void Cdccontrol2683_set_p_cdc_relidarvvopdrtn(double cdc_relidarvvopdrtn) {
  cdc_relidarvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_relidarvvopdrtn);
  int x = cdc_relidarvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 39);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 40);
}


void Cdccontrol2683_set_p_cdc_relidarvvclsdrtn(double cdc_relidarvvclsdrtn) {
  cdc_relidarvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_relidarvvclsdrtn);
  int x = cdc_relidarvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 40);
}


void Cdccontrol2683_set_p_cdc_frchassisvvspraynum(int cdc_frchassisvvspraynum) {
  cdc_frchassisvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_frchassisvvspraynum);
  int x = cdc_frchassisvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 41);
}


void Cdccontrol2683_set_p_cdc_frchassisvvopdrtn(double cdc_frchassisvvopdrtn) {
  cdc_frchassisvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_frchassisvvopdrtn);
  int x = cdc_frchassisvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 41);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 42);
}


void Cdccontrol2683_set_p_cdc_frchassisvvclsdrtn(double cdc_frchassisvvclsdrtn) {
  cdc_frchassisvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_frchassisvvclsdrtn);
  int x = cdc_frchassisvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 42);
}


void Cdccontrol2683_set_p_cdc_midchassisvvspraynum(int cdc_midchassisvvspraynum) {
  cdc_midchassisvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_midchassisvvspraynum);
  int x = cdc_midchassisvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 43);
}


void Cdccontrol2683_set_p_cdc_midchassisvvopdrtn(double cdc_midchassisvvopdrtn) {
  cdc_midchassisvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_midchassisvvopdrtn);
  int x = cdc_midchassisvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 43);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 44);
}


void Cdccontrol2683_set_p_cdc_midchassisvvclsdrtn(double cdc_midchassisvvclsdrtn) {
  cdc_midchassisvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_midchassisvvclsdrtn);
  int x = cdc_midchassisvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 44);
}


void Cdccontrol2683_set_p_cdc_rechassisvvspraynum(int cdc_rechassisvvspraynum) {
  cdc_rechassisvvspraynum = ProtocolData_BoundedValue(0, 15, cdc_rechassisvvspraynum);
  int x = cdc_rechassisvvspraynum;

  Byte_set_value(x, 0, 4, Data683 + 45);
}


void Cdccontrol2683_set_p_cdc_rechassisvvopdrtn(double cdc_rechassisvvopdrtn) {
  cdc_rechassisvvopdrtn = ProtocolData_BoundedValue(0.0, 200.0, cdc_rechassisvvopdrtn);
  int x = cdc_rechassisvvopdrtn / 5.000000;
  uint8_t t = 0;

  t = x & 0xF;
  Byte_set_value(t, 4, 4, Data683 + 45);
  x >>= 4;

  t = x & 0x3;
  Byte_set_value(t, 0, 2, Data683 + 46);
}


void Cdccontrol2683_set_p_cdc_rechassisvvclsdrtn(double cdc_rechassisvvclsdrtn) {
  cdc_rechassisvvclsdrtn = ProtocolData_BoundedValue(0.0, 2000.0, cdc_rechassisvvclsdrtn);
  int x = cdc_rechassisvvclsdrtn / 50.000000;

  Byte_set_value(x, 2, 6, Data683 + 46);
}


void Cdccontrol2683_set_p_cdc_secoc_freshnesscounterc2(int cdc_secoc_freshnesscounterc2) {
  cdc_secoc_freshnesscounterc2 = ProtocolData_BoundedValue(0, 255, cdc_secoc_freshnesscounterc2);
  int x = cdc_secoc_freshnesscounterc2;

  Byte_set_value(x, 0, 8, Data683 + 60);
}


void Cdccontrol2683_set_p_cdc_secoc_authcodec2_b0_msb(int cdc_secoc_authcodec2_b0_msb) {
  cdc_secoc_authcodec2_b0_msb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec2_b0_msb);
  int x = cdc_secoc_authcodec2_b0_msb;

  Byte_set_value(x, 0, 8, Data683 + 61);
}


void Cdccontrol2683_set_p_cdc_secoc_authcodec2_b1(int cdc_secoc_authcodec2_b1) {
  cdc_secoc_authcodec2_b1 = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec2_b1);
  int x = cdc_secoc_authcodec2_b1;

  Byte_set_value(x, 0, 8, Data683 + 62);
}


void Cdccontrol2683_set_p_cdc_secoc_authcodec2_b2_lsb(int cdc_secoc_authcodec2_b2_lsb) {
  cdc_secoc_authcodec2_b2_lsb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcodec2_b2_lsb);
  int x = cdc_secoc_authcodec2_b2_lsb;

  Byte_set_value(x, 0, 8, Data683 + 63);
}


void Cdccontrol1683_UpdateData(Cdc_control2_683 *cdc_control2_683) {
  Cdccontrol2683_set_p_cdc_rilidarvalvepwr(cdc_control2_683->cdc_rilidarvalvepwr);
  Cdccontrol2683_set_p_cdc_fisheye_r_cmr_valve_pwr_req(cdc_control2_683->cdc_fisheye_r_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_frlidarvalvepwr(cdc_control2_683->cdc_frlidarvalvepwr);
  Cdccontrol2683_set_p_cdc_fisheye_f_cmr_valve_pwr_req(cdc_control2_683->cdc_fisheye_f_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_lelidarvalvepwr(cdc_control2_683->cdc_lelidarvalvepwr);
  Cdccontrol2683_set_p_cdc_fisheye_l_cmr_valve_pwr_req(cdc_control2_683->cdc_fisheye_l_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_lp_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_lp_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_b_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_b_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_mlidarvalvepwr(cdc_control2_683->cdc_mlidarvalvepwr);
  Cdccontrol2683_set_p_cdc_auto_lf_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_lf_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_rf_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_rf_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_l_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_l_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_r_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_r_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_rl_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_rl_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_auto_rr_cmr_valve_pwr_req(cdc_control2_683->cdc_auto_rr_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_fisheye_b_cmr_valve_pwr_req(cdc_control2_683->cdc_fisheye_b_cmr_valve_pwr_req);
  Cdccontrol2683_set_p_cdc_relidarvalvepwr(cdc_control2_683->cdc_relidarvalvepwr);
  Cdccontrol2683_set_p_cdc_frchassisvvctrl(cdc_control2_683->cdc_frchassisvvctrl);
  Cdccontrol2683_set_p_cdc_midchassisvvctrl(cdc_control2_683->cdc_midchassisvvctrl);
  Cdccontrol2683_set_p_cdc_rechassisvvctrl(cdc_control2_683->cdc_rechassisvvctrl);
  Cdccontrol2683_set_p_cdc_rilidarvalvespraynum(cdc_control2_683->cdc_rilidarvalvespraynum);
  Cdccontrol2683_set_p_cdc_rilidarvalveopdrtn(cdc_control2_683->cdc_rilidarvalveopdrtn);
  Cdccontrol2683_set_p_cdc_rilidarvalveclsdrtn(cdc_control2_683->cdc_rilidarvalveclsdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvspraynum(cdc_control2_683->cdc_fisheye_r_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvopdrtn(cdc_control2_683->cdc_fisheye_r_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_r_cmrvvclsdrtn(cdc_control2_683->cdc_fisheye_r_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_frlidarvalvespraynum(cdc_control2_683->cdc_frlidarvalvespraynum);
  Cdccontrol2683_set_p_cdc_frlidarvalveopdrtn(cdc_control2_683->cdc_frlidarvalveopdrtn);
  Cdccontrol2683_set_p_cdc_frlidarvalveclsdrtn(cdc_control2_683->cdc_frlidarvalveclsdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvspraynum(cdc_control2_683->cdc_fisheye_f_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvopdrtn(cdc_control2_683->cdc_fisheye_f_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_f_cmrvvclsdrtn(cdc_control2_683->cdc_fisheye_f_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_lelidarvalvespraynum(cdc_control2_683->cdc_lelidarvalvespraynum);
  Cdccontrol2683_set_p_cdc_lelidarvalveopdrtn(cdc_control2_683->cdc_lelidarvalveopdrtn);
  Cdccontrol2683_set_p_cdc_lelidarvalveclsdrtn(cdc_control2_683->cdc_lelidarvalveclsdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvspraynum(cdc_control2_683->cdc_fisheye_l_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvopdrtn(cdc_control2_683->cdc_fisheye_l_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_l_cmrvvclsdrtn(cdc_control2_683->cdc_fisheye_l_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_lp_cmrvvspraynum(cdc_control2_683->cdc_auto_lp_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_lp_cmrvvopdrtn(cdc_control2_683->cdc_auto_lp_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_lp_cmrvvclsdrtn(cdc_control2_683->cdc_auto_lp_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_b_cmrvvspraynum(cdc_control2_683->cdc_auto_b_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_b_cmrvvopdrtn(cdc_control2_683->cdc_auto_b_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_b_cmrvvclsdrtn(cdc_control2_683->cdc_auto_b_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_mlidarvvspraynum(cdc_control2_683->cdc_mlidarvvspraynum);
  Cdccontrol2683_set_p_cdc_mlidarvvopdrtn(cdc_control2_683->cdc_mlidarvvopdrtn);
  Cdccontrol2683_set_p_cdc_mlidarvvclsdrtn(cdc_control2_683->cdc_mlidarvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_lf_cmrvvspraynum(cdc_control2_683->cdc_auto_lf_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_lf_cmrvvopdrtn(cdc_control2_683->cdc_auto_lf_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_lf_cmrvvclsdrtn(cdc_control2_683->cdc_auto_lf_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_rf_cmrvvspraynum(cdc_control2_683->cdc_auto_rf_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_rf_cmrvvopdrtn(cdc_control2_683->cdc_auto_rf_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_rf_cmrvvclsdrtn(cdc_control2_683->cdc_auto_rf_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_l_cmrvvspraynum(cdc_control2_683->cdc_auto_l_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_l_cmrvvopdrtn(cdc_control2_683->cdc_auto_l_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_l_cmrvvclsdrtn(cdc_control2_683->cdc_auto_l_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_r_cmrvvspraynum(cdc_control2_683->cdc_auto_r_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_r_cmrvvopdrtn(cdc_control2_683->cdc_auto_r_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_r_cmrvvclsdrtn(cdc_control2_683->cdc_auto_r_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_rl_cmrvvspraynum(cdc_control2_683->cdc_auto_rl_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_rl_cmrvvopdrtn(cdc_control2_683->cdc_auto_rl_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_rl_cmrvvclsdrtn(cdc_control2_683->cdc_auto_rl_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_auto_rr_cmrvvspraynum(cdc_control2_683->cdc_auto_rr_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_auto_rr_cmrvvopdrtn(cdc_control2_683->cdc_auto_rr_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_auto_rr_cmrvvclsdrtn(cdc_control2_683->cdc_auto_rr_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvspraynum(cdc_control2_683->cdc_fisheye_b_cmrvvspraynum);
  Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvopdrtn(cdc_control2_683->cdc_fisheye_b_cmrvvopdrtn);
  Cdccontrol2683_set_p_cdc_fisheye_b_cmrvvclsdrtn(cdc_control2_683->cdc_fisheye_b_cmrvvclsdrtn);
  Cdccontrol2683_set_p_cdc_relidarvvspraynum(cdc_control2_683->cdc_relidarvvspraynum);
  Cdccontrol2683_set_p_cdc_relidarvvopdrtn(cdc_control2_683->cdc_relidarvvopdrtn);
  Cdccontrol2683_set_p_cdc_relidarvvclsdrtn(cdc_control2_683->cdc_relidarvvclsdrtn);
  Cdccontrol2683_set_p_cdc_frchassisvvspraynum(cdc_control2_683->cdc_frchassisvvspraynum);
  Cdccontrol2683_set_p_cdc_frchassisvvopdrtn(cdc_control2_683->cdc_frchassisvvopdrtn);
  Cdccontrol2683_set_p_cdc_frchassisvvclsdrtn(cdc_control2_683->cdc_frchassisvvclsdrtn);
  Cdccontrol2683_set_p_cdc_midchassisvvspraynum(cdc_control2_683->cdc_midchassisvvspraynum);
  Cdccontrol2683_set_p_cdc_midchassisvvopdrtn(cdc_control2_683->cdc_midchassisvvopdrtn);
  Cdccontrol2683_set_p_cdc_midchassisvvclsdrtn(cdc_control2_683->cdc_midchassisvvclsdrtn);
  Cdccontrol2683_set_p_cdc_rechassisvvspraynum(cdc_control2_683->cdc_rechassisvvspraynum);
  Cdccontrol2683_set_p_cdc_rechassisvvopdrtn(cdc_control2_683->cdc_rechassisvvopdrtn);
  Cdccontrol2683_set_p_cdc_rechassisvvclsdrtn(cdc_control2_683->cdc_rechassisvvclsdrtn);
  Cdccontrol2683_set_p_cdc_secoc_freshnesscounterc2(cdc_control2_683->cdc_secoc_freshnesscounterc2);
  Cdccontrol2683_set_p_cdc_secoc_authcodec2_b0_msb(cdc_control2_683->cdc_secoc_authcodec2_b0_msb);
  Cdccontrol2683_set_p_cdc_secoc_authcodec2_b1(cdc_control2_683->cdc_secoc_authcodec2_b1);
  Cdccontrol2683_set_p_cdc_secoc_authcodec2_b2_lsb(cdc_control2_683->cdc_secoc_authcodec2_b2_lsb);
}

void Cdccontrol1683_Reset(Cdc_control2_683 *cdc_control2_683) {
  cdc_control2_683->cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_NO_REQUEST_E0;
  cdc_control2_683->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_NO_REQUEST_E0;
  cdc_control2_683->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_NO_REQUEST_E0;
  cdc_control2_683->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
  cdc_control2_683->cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_NO_REQUEST_E0;
  cdc_control2_683->cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_NO_REQUEST_E0;
  cdc_control2_683->cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_NO_REQUEST_E0;
  cdc_control2_683->cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_NO_REQUEST_E0;
  cdc_control2_683->cdc_rilidarvalvespraynum = 0.0;
  cdc_control2_683->cdc_rilidarvalveopdrtn = 0.0;
  cdc_control2_683->cdc_rilidarvalveclsdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_r_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_fisheye_r_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_r_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_frlidarvalvespraynum = 0.0;
  cdc_control2_683->cdc_frlidarvalveopdrtn = 0.0;
  cdc_control2_683->cdc_frlidarvalveclsdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_f_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_fisheye_f_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_f_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_lelidarvalvespraynum = 0.0;
  cdc_control2_683->cdc_lelidarvalveopdrtn = 0.0;
  cdc_control2_683->cdc_lelidarvalveclsdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_l_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_fisheye_l_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_l_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_lp_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_lp_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_lp_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_b_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_b_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_b_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_mlidarvvspraynum = 0.0;
  cdc_control2_683->cdc_mlidarvvopdrtn = 0.0;
  cdc_control2_683->cdc_mlidarvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_lf_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_lf_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_lf_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_rf_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_rf_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_rf_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_l_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_l_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_l_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_r_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_r_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_r_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_rl_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_rl_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_rl_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_auto_rr_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_auto_rr_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_auto_rr_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_b_cmrvvspraynum = 0.0;
  cdc_control2_683->cdc_fisheye_b_cmrvvopdrtn = 0.0;
  cdc_control2_683->cdc_fisheye_b_cmrvvclsdrtn = 0.0;
  cdc_control2_683->cdc_relidarvvspraynum = 0.0;
  cdc_control2_683->cdc_relidarvvopdrtn = 0.0;
  cdc_control2_683->cdc_relidarvvclsdrtn = 0.0;
  cdc_control2_683->cdc_frchassisvvspraynum = 0.0;
  cdc_control2_683->cdc_frchassisvvopdrtn = 0.0;
  cdc_control2_683->cdc_frchassisvvclsdrtn = 0.0;
  cdc_control2_683->cdc_midchassisvvspraynum = 0.0;
  cdc_control2_683->cdc_midchassisvvopdrtn = 0.0;
  cdc_control2_683->cdc_midchassisvvclsdrtn = 0.0;
  cdc_control2_683->cdc_rechassisvvspraynum = 0.0;
  cdc_control2_683->cdc_rechassisvvopdrtn = 0.0;
  cdc_control2_683->cdc_rechassisvvclsdrtn = 0.0;
  cdc_control2_683->cdc_secoc_freshnesscounterc2 = 0.0;
  cdc_control2_683->cdc_secoc_authcodec2_b0_msb = 0.0;
  cdc_control2_683->cdc_secoc_authcodec2_b1 = 0.0;
  cdc_control2_683->cdc_secoc_authcodec2_b2_lsb = 0.0;
}

//void Cdccontrol2683_init(void) {
//  Cdccontrol2683_g.can_index = 0;
//  Cdccontrol2683_g.msg_id = 0x683;
//  Cdccontrol2683_g.msg_length = 64;
//  Cdccontrol2683_g.msg_period = 0;
//  Cdccontrol2683_g.is_canfd = 1;
//  Data683 = msg_rawData683;
//  memset(Data683, 0x00, Cdccontrol2683_g.msg_length);
//  Cdccontrol2683_g.reset = Cdccontrol2683_Reset;
//  Cdccontrol2683_g.UpdateData683 = Cdccontrol2683_UpdateData683;
//}
