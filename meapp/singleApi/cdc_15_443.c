#include "cdc_15_443.h"
#include "byte.h"

//Cdc_15_443 Cdc15443_g;

//static uint8_t msg_rawdata[64];

static char msg_rawdata[64];
char *Data443 = msg_rawdata;


void Cdc15443_set_p_ri_cam_wiper_ctl(Cdc_15_443_Ri_cam_wiper_ctlType ri_cam_wiper_ctl) {
  int x = ri_cam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 0);
}


void Cdc15443_set_p_ri_cam_wiper_serv(Cdc_15_443_Ri_cam_wiper_servType ri_cam_wiper_serv) {
  int x = ri_cam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 0);
}


void Cdc15443_set_p_ri_cam_wiper_number_ctl(int32_t ri_cam_wiper_number_ctl) {
  int x = ri_cam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 0);
}


void Cdc15443_set_p_ri_cam_wiper_frequency_ctl(int32_t ri_cam_wiper_frequency_ctl) {
  int x = ri_cam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 1);
}


void Cdc15443_set_p_ri_cam_wiper_travel_ctl(int ri_cam_wiper_travel_ctl) {
  ri_cam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, ri_cam_wiper_travel_ctl);
  int x = ri_cam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 2);
}


void Cdc15443_set_p_ri_cam_wiper_location_ctl(Cdc_15_443_Ri_cam_wiper_location_ctlType ri_cam_wiper_location_ctl) {
  int x = ri_cam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 3);
}


void Cdc15443_set_p_re_cam_wiper_ctl(Cdc_15_443_Re_cam_wiper_ctlType re_cam_wiper_ctl) {
  int x = re_cam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 4);
}


void Cdc15443_set_p_re_cam_wiper_serv(Cdc_15_443_Re_cam_wiper_servType re_cam_wiper_serv) {
  int x = re_cam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 4);
}


void Cdc15443_set_p_re_cam_wiper_number_ctl(int32_t re_cam_wiper_number_ctl) {
  int x = re_cam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 4);
}


void Cdc15443_set_p_re_cam_wiper_frequency_ctl(int32_t re_cam_wiper_frequency_ctl) {
  int x = re_cam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 5);
}


void Cdc15443_set_p_re_cam_wiper_travel_ctl(int re_cam_wiper_travel_ctl) {
  re_cam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, re_cam_wiper_travel_ctl);
  int x = re_cam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 6);
}


void Cdc15443_set_p_re_cam_wiper_location_ctl(Cdc_15_443_Re_cam_wiper_location_ctlType re_cam_wiper_location_ctl) {
  int x = re_cam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 7);
}


void Cdc15443_set_p_le_cam_wiper_ctl(Cdc_15_443_Le_cam_wiper_ctlType le_cam_wiper_ctl) {
  int x = le_cam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 8);
}


void Cdc15443_set_p_le_cam_wiper_serv(Cdc_15_443_Le_cam_wiper_servType le_cam_wiper_serv) {
  int x = le_cam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 8);
}


void Cdc15443_set_p_le_cam_wiper_number_ctl(int32_t le_cam_wiper_number_ctl) {
  int x = le_cam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 8);
}


void Cdc15443_set_p_le_cam_wiper_frequency_ctl(int32_t le_cam_wiper_frequency_ctl) {
  int x = le_cam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 9);
}


void Cdc15443_set_p_le_cam_wiper_travel_ctl(int le_cam_wiper_travel_ctl) {
  le_cam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, le_cam_wiper_travel_ctl);
  int x = le_cam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 10);
}


void Cdc15443_set_p_le_cam_wiper_location_ctl(Cdc_15_443_Le_cam_wiper_location_ctlType le_cam_wiper_location_ctl) {
  int x = le_cam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 11);
}


void Cdc15443_set_p_fr_cam_wiper_ctl(Cdc_15_443_Fr_cam_wiper_ctlType fr_cam_wiper_ctl) {
  int x = fr_cam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 12);
}


void Cdc15443_set_p_fr_cam_wiper_serv(Cdc_15_443_Fr_cam_wiper_servType fr_cam_wiper_serv) {
  int x = fr_cam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 12);
}


void Cdc15443_set_p_fr_cam_wiper_number_ctl(int32_t fr_cam_wiper_number_ctl) {
  int x = fr_cam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 12);
}


void Cdc15443_set_p_fr_cam_wiper_frequency_ctl(int32_t fr_cam_wiper_frequency_ctl) {
  int x = fr_cam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 13);
}


void Cdc15443_set_p_fr_cam_wiper_travel_ctl(int fr_cam_wiper_travel_ctl) {
  fr_cam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, fr_cam_wiper_travel_ctl);
  int x = fr_cam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 14);
}


void Cdc15443_set_p_fr_cam_wiper_location_ctl(Cdc_15_443_Fr_cam_wiper_location_ctlType fr_cam_wiper_location_ctl) {
  int x = fr_cam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 15);
}


void Cdc15443_set_p_fl_cam_wiper_ctl(Cdc_15_443_Fl_cam_wiper_ctlType fl_cam_wiper_ctl) {
  int x = fl_cam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 16);
}


void Cdc15443_set_p_fl_cam_wiper_serv(Cdc_15_443_Fl_cam_wiper_servType fl_cam_wiper_serv) {
  int x = fl_cam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 16);
}


void Cdc15443_set_p_fl_cam_wiper_number_ctl(int32_t fl_cam_wiper_number_ctl) {
  int x = fl_cam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 16);
}


void Cdc15443_set_p_fl_cam_wiper_frequency_ctl(int32_t fl_cam_wiper_frequency_ctl) {
  int x = fl_cam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 17);
}


void Cdc15443_set_p_fl_cam_wiper_travel_ctl(int fl_cam_wiper_travel_ctl) {
  fl_cam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, fl_cam_wiper_travel_ctl);
  int x = fl_cam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 18);
}


void Cdc15443_set_p_fl_cam_wiper_location_ctl(Cdc_15_443_Fl_cam_wiper_location_ctlType fl_cam_wiper_location_ctl) {
  int x = fl_cam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 19);
}


void Cdc15443_set_p_lf_pcam_wiper_ctl(Cdc_15_443_Lf_pcam_wiper_ctlType lf_pcam_wiper_ctl) {
  int x = lf_pcam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 20);
}


void Cdc15443_set_p_lf_pcam_wiper_serv1(Cdc_15_443_Lf_pcam_wiper_serv1Type lf_pcam_wiper_serv1) {
  int x = lf_pcam_wiper_serv1;

  Byte_set_value(x, 3, 2, Data443 + 20);
}


void Cdc15443_set_p_lf_pcam_wiper_number_ctl(int32_t lf_pcam_wiper_number_ctl) {
  int x = lf_pcam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 20);
}


void Cdc15443_set_p_lf_pcam_wiper_frequency_ctl(int32_t lf_pcam_wiper_frequency_ctl) {
  int x = lf_pcam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 21);
}


void Cdc15443_set_p_lf_pcam_wiper_travel_ctl(int lf_pcam_wiper_travel_ctl) {
  lf_pcam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, lf_pcam_wiper_travel_ctl);
  int x = lf_pcam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 22);
}


void Cdc15443_set_p_lf_pcam_wiper_location_ctl(Cdc_15_443_Lf_pcam_wiper_location_ctlType lf_pcam_wiper_location_ctl) {
  int x = lf_pcam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 23);
}


void Cdc15443_set_p_ri_blid_wiper_ctl(Cdc_15_443_Ri_blid_wiper_ctlType ri_blid_wiper_ctl) {
  int x = ri_blid_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 24);
}


void Cdc15443_set_p_ri_blid_wiper_serv(Cdc_15_443_Ri_blid_wiper_servType ri_blid_wiper_serv) {
  int x = ri_blid_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 24);
}


void Cdc15443_set_p_ri_blid_wiper_number_ctl(int32_t ri_blid_wiper_number_ctl) {
  int x = ri_blid_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 24);
}


void Cdc15443_set_p_ri_blid_wiper_frequency_ctl(int32_t ri_blid_wiper_frequency_ctl) {
  int x = ri_blid_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 25);
}


void Cdc15443_set_p_ri_blid_wiper_travel_ctl(int ri_blid_wiper_travel_ctl) {
  ri_blid_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, ri_blid_wiper_travel_ctl);
  int x = ri_blid_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 26);
}


void Cdc15443_set_p_ri_blid_wiper_location_ctl(Cdc_15_443_Ri_blid_wiper_location_ctlType ri_blid_wiper_location_ctl) {
  int x = ri_blid_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 27);
}


void Cdc15443_set_p_le_blid_wiper_ctl(Cdc_15_443_Le_blid_wiper_ctlType le_blid_wiper_ctl) {
  int x = le_blid_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 28);
}


void Cdc15443_set_p_le_blid_wiper_serv(Cdc_15_443_Le_blid_wiper_servType le_blid_wiper_serv) {
  int x = le_blid_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 28);
}


void Cdc15443_set_p_le_blid_wiper_number_ctl(int32_t le_blid_wiper_number_ctl) {
  int x = le_blid_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 28);
}


void Cdc15443_set_p_le_blid_wiper_frequency_ctl(int32_t le_blid_wiper_frequency_ctl) {
  int x = le_blid_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 29);
}


void Cdc15443_set_p_le_blid_wiper_travel_ctl(int le_blid_wiper_travel_ctl) {
  le_blid_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, le_blid_wiper_travel_ctl);
  int x = le_blid_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 30);
}


void Cdc15443_set_p_le_blid_wiper_location_ctl(Cdc_15_443_Le_blid_wiper_location_ctlType le_blid_wiper_location_ctl) {
  int x = le_blid_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 31);
}


void Cdc15443_set_p_fr_blid_wiper_ctl(Cdc_15_443_Fr_blid_wiper_ctlType fr_blid_wiper_ctl) {
  int x = fr_blid_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 32);
}


void Cdc15443_set_p_fr_blid_wiper_serv(Cdc_15_443_Fr_blid_wiper_servType fr_blid_wiper_serv) {
  int x = fr_blid_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 32);
}


void Cdc15443_set_p_fr_blid_wiper_number_ctl(int32_t fr_blid_wiper_number_ctl) {
  int x = fr_blid_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 32);
}


void Cdc15443_set_p_fr_blid_wiper_frequency_ctl(int32_t fr_blid_wiper_frequency_ctl) {
  int x = fr_blid_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 33);
}


void Cdc15443_set_p_fr_blid_wiper_travel_ctl(int fr_blid_wiper_travel_ctl) {
  fr_blid_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, fr_blid_wiper_travel_ctl);
  int x = fr_blid_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 34);
}


void Cdc15443_set_p_fr_blid_wiper_location_ctl(Cdc_15_443_Fr_blid_wiper_location_ctlType fr_blid_wiper_location_ctl) {
  int x = fr_blid_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 35);
}


void Cdc15443_set_p_lm_control_move(Cdc_15_443_Lm_control_moveType lm_control_move) {
  int x = lm_control_move;

  Byte_set_value(x, 0, 1, Data443 + 36);
}


void Cdc15443_set_p_lm_move_target_position(int lm_move_target_position) {
  lm_move_target_position = ProtocolData_BoundedValue(0, 100, lm_move_target_position);
  int x = lm_move_target_position;

  Byte_set_value(x, 0, 8, Data443 + 37);
}


void Cdc15443_set_p_rrb_pcam_wiper_ctl(Cdc_15_443_Rrb_pcam_wiper_ctlType rrb_pcam_wiper_ctl) {
  int x = rrb_pcam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 38);
}


void Cdc15443_set_p_rrb_pcam_wiper_serv(Cdc_15_443_Rrb_pcam_wiper_servType rrb_pcam_wiper_serv) {
  int x = rrb_pcam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 38);
}


void Cdc15443_set_p_rrb_pcam_wiper_number_ctl(int32_t rrb_pcam_wiper_number_ctl) {
  int x = rrb_pcam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 38);
}


void Cdc15443_set_p_rrb_pcam_wiper_frequency_ctl(int32_t rrb_pcam_wiper_frequency_ctl) {
  int x = rrb_pcam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 39);
}


void Cdc15443_set_p_rrb_pcam_wiper_travel_ctl(int rrb_pcam_wiper_travel_ctl) {
  rrb_pcam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, rrb_pcam_wiper_travel_ctl);
  int x = rrb_pcam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 40);
}


void Cdc15443_set_p_rrb_pcam_wiper_location_ctl(Cdc_15_443_Rrb_pcam_wiper_location_ctlType rrb_pcam_wiper_location_ctl) {
  int x = rrb_pcam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 41);
}


void Cdc15443_set_p_lrb_pcam_wiper_ctl(Cdc_15_443_Lrb_pcam_wiper_ctlType lrb_pcam_wiper_ctl) {
  int x = lrb_pcam_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 42);
}


void Cdc15443_set_p_lrb_pcam_wiper_serv(Cdc_15_443_Lrb_pcam_wiper_servType lrb_pcam_wiper_serv) {
  int x = lrb_pcam_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 42);
}


void Cdc15443_set_p_lrb_pcam_wiper_number_ctl(int32_t lrb_pcam_wiper_number_ctl) {
  int x = lrb_pcam_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 42);
}


void Cdc15443_set_p_lrb_pcam_wiper_frequency_ctl(int32_t lrb_pcam_wiper_frequency_ctl) {
  int x = lrb_pcam_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 43);
}


void Cdc15443_set_p_lrb_pcam_wiper_travel_ctl(int lrb_pcam_wiper_travel_ctl) {
  lrb_pcam_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, lrb_pcam_wiper_travel_ctl);
  int x = lrb_pcam_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 44);
}


void Cdc15443_set_p_lrb_pcam_wiper_location_ctl(Cdc_15_443_Lrb_pcam_wiper_location_ctlType lrb_pcam_wiper_location_ctl) {
  int x = lrb_pcam_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 45);
}


void Cdc15443_set_p_re_blid_rac_wiper_ctl(Cdc_15_443_Re_blid_rac_wiper_ctlType re_blid_rac_wiper_ctl) {
  int x = re_blid_rac_wiper_ctl;

  Byte_set_value(x, 0, 3, Data443 + 46);
}


void Cdc15443_set_p_re_blid_rac_wiper_serv(Cdc_15_443_Re_blid_rac_wiper_servType re_blid_rac_wiper_serv) {
  int x = re_blid_rac_wiper_serv;

  Byte_set_value(x, 3, 2, Data443 + 46);
}


void Cdc15443_set_p_re_blid_rac_wiper_number_ctl(int32_t re_blid_rac_wiper_number_ctl) {
  int x = re_blid_rac_wiper_number_ctl;

  Byte_set_value(x, 5, 3, Data443 + 46);
}


void Cdc15443_set_p_re_blid_rac_wiper_frequency_ctl(int32_t re_blid_rac_wiper_frequency_ctl) {
  int x = re_blid_rac_wiper_frequency_ctl;

  Byte_set_value(x, 0, 8, Data443 + 47);
}


void Cdc15443_set_p_re_blid_rac_wiper_travel_ctl(int re_blid_rac_wiper_travel_ctl) {
  re_blid_rac_wiper_travel_ctl = ProtocolData_BoundedValue(0, 100, re_blid_rac_wiper_travel_ctl);
  int x = re_blid_rac_wiper_travel_ctl;

  Byte_set_value(x, 0, 8, Data443 + 48);
}


void Cdc15443_set_p_re_blid_rac_wiper_location_ctl(Cdc_15_443_Re_blid_rac_wiper_location_ctlType re_blid_rac_wiper_location_ctl) {
  int x = re_blid_rac_wiper_location_ctl;

  Byte_set_value(x, 0, 8, Data443 + 49);
}


void Cdc15443_set_p_rac_control_move(Cdc_15_443_Rac_control_moveType rac_control_move) {
  int x = rac_control_move;

  Byte_set_value(x, 0, 1, Data443 + 50);
}


void Cdc15443_set_p_rac_5_control_move(Cdc_15_443_Rac_5_control_moveType rac_5_control_move) {
  int x = rac_5_control_move;

  Byte_set_value(x, 1, 1, Data443 + 50);
}


void Cdc15443_set_p_rac_4_movetarget_position(int rac_4_movetarget_position) {
  int x = rac_4_movetarget_position;

  Byte_set_value(x, 0, 8, Data443 + 51);
}


void Cdc15443_set_p_rac_5_movetarget_position(int rac_5_movetarget_position) {
  int x = rac_5_movetarget_position;

  Byte_set_value(x, 0, 8, Data443 + 52);
}


void Cdc15443_set_p_cdc_secoc_freshnesscounter(int cdc_secoc_freshnesscounter) {
  cdc_secoc_freshnesscounter = ProtocolData_BoundedValue(0, 255, cdc_secoc_freshnesscounter);
  int x = cdc_secoc_freshnesscounter;

  Byte_set_value(x, 0, 8, Data443 + 60);
}


void Cdc15443_set_p_cdc_secoc_authcode_b0_msb(int cdc_secoc_authcode_b0_msb) {
  cdc_secoc_authcode_b0_msb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcode_b0_msb);
  int x = cdc_secoc_authcode_b0_msb;

  Byte_set_value(x, 0, 8, Data443 + 61);
}


void Cdc15443_set_p_cdc_secoc_authcode_b1(int cdc_secoc_authcode_b1) {
  cdc_secoc_authcode_b1 = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcode_b1);
  int x = cdc_secoc_authcode_b1;

  Byte_set_value(x, 0, 8, Data443 + 62);
}


void Cdc15443_set_p_cdc_secoc_authcode_b2_lsb(int cdc_secoc_authcode_b2_lsb) {
  cdc_secoc_authcode_b2_lsb = ProtocolData_BoundedValue(0, 255, cdc_secoc_authcode_b2_lsb);
  int x = cdc_secoc_authcode_b2_lsb;

  Byte_set_value(x, 0, 8, Data443 + 63);
}


void Cdc15443_UpdateData(Cdc_15_443 *chassis) {
  Cdc15443_set_p_ri_cam_wiper_ctl(chassis->ri_cam_wiper_ctl);
  Cdc15443_set_p_ri_cam_wiper_serv(chassis->ri_cam_wiper_serv);
  Cdc15443_set_p_ri_cam_wiper_number_ctl(chassis->ri_cam_wiper_number_ctl);
  Cdc15443_set_p_ri_cam_wiper_frequency_ctl(chassis->ri_cam_wiper_frequency_ctl);
  Cdc15443_set_p_ri_cam_wiper_travel_ctl(chassis->ri_cam_wiper_travel_ctl);
  Cdc15443_set_p_ri_cam_wiper_location_ctl(chassis->ri_cam_wiper_location_ctl);
  Cdc15443_set_p_re_cam_wiper_ctl(chassis->re_cam_wiper_ctl);
  Cdc15443_set_p_re_cam_wiper_serv(chassis->re_cam_wiper_serv);
  Cdc15443_set_p_re_cam_wiper_number_ctl(chassis->re_cam_wiper_number_ctl);
  Cdc15443_set_p_re_cam_wiper_frequency_ctl(chassis->re_cam_wiper_frequency_ctl);
  Cdc15443_set_p_re_cam_wiper_travel_ctl(chassis->re_cam_wiper_travel_ctl);
  Cdc15443_set_p_re_cam_wiper_location_ctl(chassis->re_cam_wiper_location_ctl);
  Cdc15443_set_p_le_cam_wiper_ctl(chassis->le_cam_wiper_ctl);
  Cdc15443_set_p_le_cam_wiper_serv(chassis->le_cam_wiper_serv);
  Cdc15443_set_p_le_cam_wiper_number_ctl(chassis->le_cam_wiper_number_ctl);
  Cdc15443_set_p_le_cam_wiper_frequency_ctl(chassis->le_cam_wiper_frequency_ctl);
  Cdc15443_set_p_le_cam_wiper_travel_ctl(chassis->le_cam_wiper_travel_ctl);
  Cdc15443_set_p_le_cam_wiper_location_ctl(chassis->le_cam_wiper_location_ctl);
  Cdc15443_set_p_fr_cam_wiper_ctl(chassis->fr_cam_wiper_ctl);
  Cdc15443_set_p_fr_cam_wiper_serv(chassis->fr_cam_wiper_serv);
  Cdc15443_set_p_fr_cam_wiper_number_ctl(chassis->fr_cam_wiper_number_ctl);
  Cdc15443_set_p_fr_cam_wiper_frequency_ctl(chassis->fr_cam_wiper_frequency_ctl);
  Cdc15443_set_p_fr_cam_wiper_travel_ctl(chassis->fr_cam_wiper_travel_ctl);
  Cdc15443_set_p_fr_cam_wiper_location_ctl(chassis->fr_cam_wiper_location_ctl);
  Cdc15443_set_p_fl_cam_wiper_ctl(chassis->fl_cam_wiper_ctl);
  Cdc15443_set_p_fl_cam_wiper_serv(chassis->fl_cam_wiper_serv);
  Cdc15443_set_p_fl_cam_wiper_number_ctl(chassis->fl_cam_wiper_number_ctl);
  Cdc15443_set_p_fl_cam_wiper_frequency_ctl(chassis->fl_cam_wiper_frequency_ctl);
  Cdc15443_set_p_fl_cam_wiper_travel_ctl(chassis->fl_cam_wiper_travel_ctl);
  Cdc15443_set_p_fl_cam_wiper_location_ctl(chassis->fl_cam_wiper_location_ctl);
  Cdc15443_set_p_lf_pcam_wiper_ctl(chassis->lf_pcam_wiper_ctl);
  Cdc15443_set_p_lf_pcam_wiper_serv1(chassis->lf_pcam_wiper_serv1);
  Cdc15443_set_p_lf_pcam_wiper_number_ctl(chassis->lf_pcam_wiper_number_ctl);
  Cdc15443_set_p_lf_pcam_wiper_frequency_ctl(chassis->lf_pcam_wiper_frequency_ctl);
  Cdc15443_set_p_lf_pcam_wiper_travel_ctl(chassis->lf_pcam_wiper_travel_ctl);
  Cdc15443_set_p_lf_pcam_wiper_location_ctl(chassis->lf_pcam_wiper_location_ctl);
  Cdc15443_set_p_ri_blid_wiper_ctl(chassis->ri_blid_wiper_ctl);
  Cdc15443_set_p_ri_blid_wiper_serv(chassis->ri_blid_wiper_serv);
  Cdc15443_set_p_ri_blid_wiper_number_ctl(chassis->ri_blid_wiper_number_ctl);
  Cdc15443_set_p_ri_blid_wiper_frequency_ctl(chassis->ri_blid_wiper_frequency_ctl);
  Cdc15443_set_p_ri_blid_wiper_travel_ctl(chassis->ri_blid_wiper_travel_ctl);
  Cdc15443_set_p_ri_blid_wiper_location_ctl(chassis->ri_blid_wiper_location_ctl);
  Cdc15443_set_p_le_blid_wiper_ctl(chassis->le_blid_wiper_ctl);
  Cdc15443_set_p_le_blid_wiper_serv(chassis->le_blid_wiper_serv);
  Cdc15443_set_p_le_blid_wiper_number_ctl(chassis->le_blid_wiper_number_ctl);
  Cdc15443_set_p_le_blid_wiper_frequency_ctl(chassis->le_blid_wiper_frequency_ctl);
  Cdc15443_set_p_le_blid_wiper_travel_ctl(chassis->le_blid_wiper_travel_ctl);
  Cdc15443_set_p_le_blid_wiper_location_ctl(chassis->le_blid_wiper_location_ctl);
  Cdc15443_set_p_fr_blid_wiper_ctl(chassis->fr_blid_wiper_ctl);
  Cdc15443_set_p_fr_blid_wiper_serv(chassis->fr_blid_wiper_serv);
  Cdc15443_set_p_fr_blid_wiper_number_ctl(chassis->fr_blid_wiper_number_ctl);
  Cdc15443_set_p_fr_blid_wiper_frequency_ctl(chassis->fr_blid_wiper_frequency_ctl);
  Cdc15443_set_p_fr_blid_wiper_travel_ctl(chassis->fr_blid_wiper_travel_ctl);
  Cdc15443_set_p_fr_blid_wiper_location_ctl(chassis->fr_blid_wiper_location_ctl);
  Cdc15443_set_p_lm_control_move(chassis->lm_control_move);
  Cdc15443_set_p_lm_move_target_position(chassis->lm_move_target_position);
  Cdc15443_set_p_rrb_pcam_wiper_ctl(chassis->rrb_pcam_wiper_ctl);
  Cdc15443_set_p_rrb_pcam_wiper_serv(chassis->rrb_pcam_wiper_serv);
  Cdc15443_set_p_rrb_pcam_wiper_number_ctl(chassis->rrb_pcam_wiper_number_ctl);
  Cdc15443_set_p_rrb_pcam_wiper_frequency_ctl(chassis->rrb_pcam_wiper_frequency_ctl);
  Cdc15443_set_p_rrb_pcam_wiper_travel_ctl(chassis->rrb_pcam_wiper_travel_ctl);
  Cdc15443_set_p_rrb_pcam_wiper_location_ctl(chassis->rrb_pcam_wiper_location_ctl);
  Cdc15443_set_p_lrb_pcam_wiper_ctl(chassis->lrb_pcam_wiper_ctl);
  Cdc15443_set_p_lrb_pcam_wiper_serv(chassis->lrb_pcam_wiper_serv);
  Cdc15443_set_p_lrb_pcam_wiper_number_ctl(chassis->lrb_pcam_wiper_number_ctl);
  Cdc15443_set_p_lrb_pcam_wiper_frequency_ctl(chassis->lrb_pcam_wiper_frequency_ctl);
  Cdc15443_set_p_lrb_pcam_wiper_travel_ctl(chassis->lrb_pcam_wiper_travel_ctl);
  Cdc15443_set_p_lrb_pcam_wiper_location_ctl(chassis->lrb_pcam_wiper_location_ctl);
  Cdc15443_set_p_re_blid_rac_wiper_ctl(chassis->re_blid_rac_wiper_ctl);
  Cdc15443_set_p_re_blid_rac_wiper_serv(chassis->re_blid_rac_wiper_serv);
  Cdc15443_set_p_re_blid_rac_wiper_number_ctl(chassis->re_blid_rac_wiper_number_ctl);
  Cdc15443_set_p_re_blid_rac_wiper_frequency_ctl(chassis->re_blid_rac_wiper_frequency_ctl);
  Cdc15443_set_p_re_blid_rac_wiper_travel_ctl(chassis->re_blid_rac_wiper_travel_ctl);
  Cdc15443_set_p_re_blid_rac_wiper_location_ctl(chassis->re_blid_rac_wiper_location_ctl);
  Cdc15443_set_p_rac_control_move(chassis->rac_control_move);
  Cdc15443_set_p_rac_5_control_move(chassis->rac_5_control_move);
  Cdc15443_set_p_rac_4_movetarget_position(chassis->rac_4_movetarget_position);
  Cdc15443_set_p_rac_5_movetarget_position(chassis->rac_5_movetarget_position);
  Cdc15443_set_p_cdc_secoc_freshnesscounter(chassis->cdc_secoc_freshnesscounter);
  Cdc15443_set_p_cdc_secoc_authcode_b0_msb(chassis->cdc_secoc_authcode_b0_msb);
  Cdc15443_set_p_cdc_secoc_authcode_b1(chassis->cdc_secoc_authcode_b1);
  Cdc15443_set_p_cdc_secoc_authcode_b2_lsb(chassis->cdc_secoc_authcode_b2_lsb);
}

void Cdc15443_Reset(Cdc_15_443 *chassis) {
  chassis->ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_INVALID_E0;
  chassis->ri_cam_wiper_serv = Cdc_15_443_Ri_cam_wiper_servType_RI_CAM_WIPER_SERV_INVALID_E0;
  chassis->ri_cam_wiper_number_ctl = 0;
  chassis->ri_cam_wiper_frequency_ctl = 0;
  chassis->ri_cam_wiper_travel_ctl = 0.0;
  chassis->ri_cam_wiper_location_ctl = Cdc_15_443_Ri_cam_wiper_location_ctlType_RI_CAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_INVALID_E0;
  chassis->re_cam_wiper_serv = Cdc_15_443_Re_cam_wiper_servType_RE_CAM_WIPER_SERV_INVALID_E0;
  chassis->re_cam_wiper_number_ctl = 0;
  chassis->re_cam_wiper_frequency_ctl = 0;
  chassis->re_cam_wiper_travel_ctl = 0.0;
  chassis->re_cam_wiper_location_ctl = Cdc_15_443_Re_cam_wiper_location_ctlType_RE_CAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_INVALID_E0;
  chassis->le_cam_wiper_serv = Cdc_15_443_Le_cam_wiper_servType_LE_CAM_WIPER_SERV_INVALID_E0;
  chassis->le_cam_wiper_number_ctl = 0;
  chassis->le_cam_wiper_frequency_ctl = 0;
  chassis->le_cam_wiper_travel_ctl = 0.0;
  chassis->le_cam_wiper_location_ctl = Cdc_15_443_Le_cam_wiper_location_ctlType_LE_CAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_INVALID_E0;
  chassis->fr_cam_wiper_serv = Cdc_15_443_Fr_cam_wiper_servType_FR_CAM_WIPER_SERV_INVALID_E0;
  chassis->fr_cam_wiper_number_ctl = 0;
  chassis->fr_cam_wiper_frequency_ctl = 0;
  chassis->fr_cam_wiper_travel_ctl = 0.0;
  chassis->fr_cam_wiper_location_ctl = Cdc_15_443_Fr_cam_wiper_location_ctlType_FR_CAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_INVALID_E0;
  chassis->fl_cam_wiper_serv = Cdc_15_443_Fl_cam_wiper_servType_FL_CAM_WIPER_SERV_INVALID_E0;
  chassis->fl_cam_wiper_number_ctl = 0;
  chassis->fl_cam_wiper_frequency_ctl = 0;
  chassis->fl_cam_wiper_travel_ctl = 0.0;
  chassis->fl_cam_wiper_location_ctl = Cdc_15_443_Fl_cam_wiper_location_ctlType_FL_CAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_INVALID_E0;
  chassis->lf_pcam_wiper_serv1 = Cdc_15_443_Lf_pcam_wiper_serv1Type_LF_PCAM_WIPER_SERV1_INVALID_E0;
  chassis->lf_pcam_wiper_number_ctl = 0;
  chassis->lf_pcam_wiper_frequency_ctl = 0;
  chassis->lf_pcam_wiper_travel_ctl = 0.0;
  chassis->lf_pcam_wiper_location_ctl = Cdc_15_443_Lf_pcam_wiper_location_ctlType_LF_PCAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_INVALID_E0;
  chassis->ri_blid_wiper_serv = Cdc_15_443_Ri_blid_wiper_servType_RI_BLID_WIPER_SERV_INVALID_E0;
  chassis->ri_blid_wiper_number_ctl = 0;
  chassis->ri_blid_wiper_frequency_ctl = 0;
  chassis->ri_blid_wiper_travel_ctl = 0.0;
  chassis->ri_blid_wiper_location_ctl = Cdc_15_443_Ri_blid_wiper_location_ctlType_RI_BLID_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_INVALID_E0;
  chassis->le_blid_wiper_serv = Cdc_15_443_Le_blid_wiper_servType_LE_BLID_WIPER_SERV_INVALID_E0;
  chassis->le_blid_wiper_number_ctl = 0;
  chassis->le_blid_wiper_frequency_ctl = 0;
  chassis->le_blid_wiper_travel_ctl = 0.0;
  chassis->le_blid_wiper_location_ctl = Cdc_15_443_Le_blid_wiper_location_ctlType_LE_BLID_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_INVALID_E0;
  chassis->fr_blid_wiper_serv = Cdc_15_443_Fr_blid_wiper_servType_FR_BLID_WIPER_SERV_INVALID_E0;
  chassis->fr_blid_wiper_number_ctl = 0;
  chassis->fr_blid_wiper_frequency_ctl = 0;
  chassis->fr_blid_wiper_travel_ctl = 0.0;
  chassis->fr_blid_wiper_location_ctl = Cdc_15_443_Fr_blid_wiper_location_ctlType_FR_BLID_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->lm_control_move = Cdc_15_443_Lm_control_moveType_LM_CONTROL_MOVE__E0;
  chassis->lm_move_target_position = 0.0;
  chassis->rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_INVALID_E0;
  chassis->rrb_pcam_wiper_serv = Cdc_15_443_Rrb_pcam_wiper_servType_RRB_PCAM_WIPER_SERV_INVALID_E0;
  chassis->rrb_pcam_wiper_number_ctl = 0;
  chassis->rrb_pcam_wiper_frequency_ctl = 0;
  chassis->rrb_pcam_wiper_travel_ctl = 0.0;
  chassis->rrb_pcam_wiper_location_ctl = Cdc_15_443_Rrb_pcam_wiper_location_ctlType_RRB_PCAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_INVALID_E0;
  chassis->lrb_pcam_wiper_serv = Cdc_15_443_Lrb_pcam_wiper_servType_LRB_PCAM_WIPER_SERV_INVALID_E0;
  chassis->lrb_pcam_wiper_number_ctl = 0;
  chassis->lrb_pcam_wiper_frequency_ctl = 0;
  chassis->lrb_pcam_wiper_travel_ctl = 0.0;
  chassis->lrb_pcam_wiper_location_ctl = Cdc_15_443_Lrb_pcam_wiper_location_ctlType_LRB_PCAM_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_INVALID_E0;
  chassis->re_blid_rac_wiper_serv = Cdc_15_443_Re_blid_rac_wiper_servType_RE_BLID_RAC_WIPER_SERV_INVALID_E0;
  chassis->re_blid_rac_wiper_number_ctl = 0;
  chassis->re_blid_rac_wiper_frequency_ctl = 0;
  chassis->re_blid_rac_wiper_travel_ctl = 0.0;
  chassis->re_blid_rac_wiper_location_ctl = Cdc_15_443_Re_blid_rac_wiper_location_ctlType_RE_BLID_RAC_WIPER_LOCATION_CTL_INVALID_E0;
  chassis->rac_control_move = Cdc_15_443_Rac_control_moveType_RAC_CONTROL_MOVE__E0;
  chassis->rac_5_control_move = Cdc_15_443_Rac_5_control_moveType_RAC_5_CONTROL_MOVE__E0;
  chassis->rac_4_movetarget_position = 0;
  chassis->rac_5_movetarget_position = 0;
  chassis->cdc_secoc_freshnesscounter = 0.0;
  chassis->cdc_secoc_authcode_b0_msb = 0.0;
  chassis->cdc_secoc_authcode_b1 = 0.0;
  chassis->cdc_secoc_authcode_b2_lsb = 0.0;
}

//void Cdc15443_init(void) {
//  Cdc15443_g.can_index = 0;
//  Cdc15443_g.msg_id = 0x443;
//  Cdc15443_g.msg_length = 64;
//  Cdc15443_g.msg_period = 100;
//  Cdc15443_g.is_canfd = 1;
//  Data443 = msg_rawdata;
//  memset(Data443, 0x00, Cdc15443_g.msg_length);
//  Cdc15443_g.reset = Cdc15443_Reset;
//  Cdc15443_g.UpdateData = Cdc15443_UpdateData;
//}
