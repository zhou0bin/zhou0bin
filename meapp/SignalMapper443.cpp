#include "SignalMapper443.h"

using json = nlohmann::json;

bool SignalMapper443::SetSignal(
    Cdc_15_443& msg,
    const std::string& signalName,
    const QJsonValue& value)
{
    static const std::unordered_map<
        std::string,
        std::function<void(Cdc_15_443&, const QJsonValue&)>
    > mapper =
    {
        // ================= RI CAM =================
        {"Ri_Cam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_cam_wiper_ctl =
                    (Cdc_15_443_Ri_cam_wiper_ctlType)v.toInt();
            }},

        {"Ri_Cam_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_cam_wiper_serv =
                    (Cdc_15_443_Ri_cam_wiper_servType)v.toInt();
            }},

        {"Ri_Cam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_cam_wiper_number_ctl = v.toInt(); }},

        {"Ri_Cam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_cam_wiper_frequency_ctl = v.toInt(); }},

        {"Ri_Cam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_cam_wiper_travel_ctl = v.toInt(); }},

        {"Ri_Cam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_cam_wiper_location_ctl =
                    (Cdc_15_443_Ri_cam_wiper_location_ctlType)v.toInt();
            }},

        // ================= RE CAM =================
        {"Re_Cam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.re_cam_wiper_ctl =
                    (Cdc_15_443_Re_cam_wiper_ctlType)v.toInt();
            }},

        {"Re_Cam_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.re_cam_wiper_serv =
                    (Cdc_15_443_Re_cam_wiper_servType)v.toInt();
            }},

        {"Re_Cam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.re_cam_wiper_number_ctl = v.toInt(); }},

        {"Re_Cam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.re_cam_wiper_frequency_ctl = v.toInt(); }},

        {"Re_Cam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.re_cam_wiper_travel_ctl = v.toInt(); }},

        {"Re_Cam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.re_cam_wiper_location_ctl =
                    (Cdc_15_443_Re_cam_wiper_location_ctlType)v.toInt();
            }},

        // ================= LE CAM =================
        {"Le_Cam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_cam_wiper_ctl =
                    (Cdc_15_443_Le_cam_wiper_ctlType)v.toInt();
            }},

        {"Le_Cam_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_cam_wiper_serv =
                    (Cdc_15_443_Le_cam_wiper_servType)v.toInt();
            }},

        {"Le_Cam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_cam_wiper_number_ctl = v.toInt(); }},

        {"Le_Cam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_cam_wiper_frequency_ctl = v.toInt(); }},

        {"Le_Cam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_cam_wiper_travel_ctl = v.toInt(); }},

        {"Le_Cam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_cam_wiper_location_ctl =
                    (Cdc_15_443_Le_cam_wiper_location_ctlType)v.toInt();
            }},

        // ================= FR CAM =================
        {"FR_Cam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_cam_wiper_ctl =
                    (Cdc_15_443_Fr_cam_wiper_ctlType)v.toInt();
            }},

        {"FR_Cam_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_cam_wiper_serv =
                    (Cdc_15_443_Fr_cam_wiper_servType)v.toInt();
            }},

        {"FR_Cam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_cam_wiper_number_ctl = v.toInt(); }},

        {"FR_Cam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_cam_wiper_frequency_ctl = v.toInt(); }},

        {"FR_Cam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_cam_wiper_travel_ctl = v.toInt(); }},

        {"FR_Cam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_cam_wiper_location_ctl =
                    (Cdc_15_443_Fr_cam_wiper_location_ctlType)v.toInt();
            }},

        // ================= FL CAM =================
        {"FL_Cam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fl_cam_wiper_ctl =
                    (Cdc_15_443_Fl_cam_wiper_ctlType)v.toInt();
            }},

        {"FL_Cam_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fl_cam_wiper_serv =
                    (Cdc_15_443_Fl_cam_wiper_servType)v.toInt();
            }},

        {"FL_Cam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fl_cam_wiper_number_ctl = v.toInt(); }},

        {"FL_Cam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fl_cam_wiper_frequency_ctl = v.toInt(); }},

        {"FL_Cam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fl_cam_wiper_travel_ctl = v.toInt(); }},

        {"FL_Cam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fl_cam_wiper_location_ctl =
                    (Cdc_15_443_Fl_cam_wiper_location_ctlType)v.toInt();
            }},

        // ================= LF PCAM =================
        {"LF_pCam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.lf_pcam_wiper_ctl =
                    (Cdc_15_443_Lf_pcam_wiper_ctlType)v.toInt();
            }},

        {"LF_pCam_Wiper_Serv1",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.lf_pcam_wiper_serv1 =
                    (Cdc_15_443_Lf_pcam_wiper_serv1Type)v.toInt();
            }},

        {"LF_pCam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lf_pcam_wiper_number_ctl = v.toInt(); }},

        {"LF_pCam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lf_pcam_wiper_frequency_ctl = v.toInt(); }},

        {"LF_pCam_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lf_pcam_wiper_travel_ctl = v.toInt(); }},

        {"LF_pCam_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.lf_pcam_wiper_location_ctl =
                    (Cdc_15_443_Lf_pcam_wiper_location_ctlType)v.toInt();
            }},

        // ================= LRB PCAM =================
        {"LRB_pCam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.lrb_pcam_wiper_ctl =
                    (Cdc_15_443_Lrb_pcam_wiper_ctlType)v.toInt();
            }},

//        {"LRB_pCam_Wiper_Serv1",
//            [](Cdc_15_443& m,const QJsonValue& v){
//                m.lf_pcam_wiper_serv1 =
//                    (Cdc_15_443_Lf_pcam_wiper_serv1Type)v.toInt();
//            }},

        {"LRB_pCam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lrb_pcam_wiper_number_ctl = v.toInt(); }},

        {"LRB_pCam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lrb_pcam_wiper_frequency_ctl = v.toInt(); }},

//        {"LF_pCam_Wiper_Travel_Ctl",
//            [](Cdc_15_443& m,const QJsonValue& v){ m.lf_pcam_wiper_travel_ctl = v.toInt(); }},

//        {"LF_pCam_Wiper_Location_Ctl",
//            [](Cdc_15_443& m,const QJsonValue& v){
//                m.lf_pcam_wiper_location_ctl =
//                    (Cdc_15_443_Lf_pcam_wiper_location_ctlType)v.toInt();
//            }},

        // ================= RRB PCAM =================
        {"RRB_pCam_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.rrb_pcam_wiper_ctl =
                    (Cdc_15_443_Rrb_pcam_wiper_ctlType)v.toInt();
            }},

        //        {"LRB_pCam_Wiper_Serv1",
        //            [](Cdc_15_443& m,const QJsonValue& v){
        //                m.lf_pcam_wiper_serv1 =
        //                    (Cdc_15_443_Lf_pcam_wiper_serv1Type)v.toInt();
        //            }},

        {"RRB_pCam_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.rrb_pcam_wiper_number_ctl = v.toInt(); }},

        {"RRB_pCam_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.rrb_pcam_wiper_frequency_ctl = v.toInt(); }},

        //        {"LF_pCam_Wiper_Travel_Ctl",
        //            [](Cdc_15_443& m,const QJsonValue& v){ m.lf_pcam_wiper_travel_ctl = v.toInt(); }},

        //        {"LF_pCam_Wiper_Location_Ctl",
        //            [](Cdc_15_443& m,const QJsonValue& v){
        //                m.lf_pcam_wiper_location_ctl =
        //                    (Cdc_15_443_Lf_pcam_wiper_location_ctlType)v.toInt();
        //            }},
        // ================= RI BLID =================
        {"Ri_bLid_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_blid_wiper_ctl =
                    (Cdc_15_443_Ri_blid_wiper_ctlType)v.toInt();
            }},

        {"Ri_bLid_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_blid_wiper_serv =
                    (Cdc_15_443_Ri_blid_wiper_servType)v.toInt();
            }},

        {"Ri_bLid_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_blid_wiper_number_ctl = v.toInt(); }},

        {"Ri_bLid_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_blid_wiper_frequency_ctl = v.toInt(); }},

        {"Ri_bLid_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_blid_wiper_travel_ctl = v.toInt(); }},

        {"Ri_bLid_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.ri_blid_wiper_location_ctl =
                    (Cdc_15_443_Ri_blid_wiper_location_ctlType)v.toInt();
            }},

        // ================= RE BLID =================
        {"Re_bLid_RAC_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.re_blid_rac_wiper_ctl =
                    (Cdc_15_443_Re_blid_rac_wiper_ctlType)v.toInt();
            }},

//        {"Ri_bLid_Wiper_Serv",
//            [](Cdc_15_443& m,const QJsonValue& v){
//                m.ri_blid_wiper_serv =
//                    (Cdc_15_443_Ri_blid_wiper_servType)v.toInt();
//            }},

        {"Re_bLid_RAC_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.re_blid_rac_wiper_number_ctl = v.toInt(); }},

        {"Re_bLid_RAC_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.re_blid_rac_wiper_frequency_ctl = v.toInt(); }},

//        {"Ri_bLid_Wiper_Travel_Ctl",
//            [](Cdc_15_443& m,const QJsonValue& v){ m.ri_blid_wiper_travel_ctl = v.toInt(); }},

//        {"Ri_bLid_Wiper_Location_Ctl",
//            [](Cdc_15_443& m,const QJsonValue& v){
//                m.ri_blid_wiper_location_ctl =
//                    (Cdc_15_443_Ri_blid_wiper_location_ctlType)v.toInt();
//            }},

        // ================= LE BLID =================
        {"Le_bLid_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_blid_wiper_ctl =
                    (Cdc_15_443_Le_blid_wiper_ctlType)v.toInt();
            }},

        {"Le_bLid_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_blid_wiper_serv =
                    (Cdc_15_443_Le_blid_wiper_servType)v.toInt();
            }},

        {"Le_bLid_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_blid_wiper_number_ctl = v.toInt(); }},

        {"Le_bLid_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_blid_wiper_frequency_ctl = v.toInt(); }},

        {"Le_bLid_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.le_blid_wiper_travel_ctl = v.toInt(); }},

        {"Le_bLid_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.le_blid_wiper_location_ctl =
                    (Cdc_15_443_Le_blid_wiper_location_ctlType)v.toInt();
            }},

        // ================= FR BLID =================
        {"Fr_bLid_Wiper_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_blid_wiper_ctl =
                    (Cdc_15_443_Fr_blid_wiper_ctlType)v.toInt();
            }},

        {"Fr_bLid_Wiper_Serv",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_blid_wiper_serv =
                    (Cdc_15_443_Fr_blid_wiper_servType)v.toInt();
            }},

        {"Fr_bLid_Wiper_Number_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_blid_wiper_number_ctl = v.toInt(); }},

        {"Fr_bLid_Wiper_Frequency_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_blid_wiper_frequency_ctl = v.toInt(); }},

        {"Fr_bLid_Wiper_Travel_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){ m.fr_blid_wiper_travel_ctl = v.toInt(); }},

        {"Fr_bLid_Wiper_Location_Ctl",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.fr_blid_wiper_location_ctl =
                    (Cdc_15_443_Fr_blid_wiper_location_ctlType)v.toInt();
            }},

        // ================= LM / RAC =================
        {"LM_Control_Move",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.lm_control_move =
                    (Cdc_15_443_Lm_control_moveType)v.toInt();
            }},

        {"LM_Move_Target_Position",
            [](Cdc_15_443& m,const QJsonValue& v){ m.lm_move_target_position = v.toInt(); }},

        {"RAC_Control_Move",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.rac_control_move =
                    (Cdc_15_443_Rac_control_moveType)v.toInt();
            }},

        {"RAC_5_Control_Move",
            [](Cdc_15_443& m,const QJsonValue& v){
                m.rac_5_control_move =
                    (Cdc_15_443_Rac_5_control_moveType)v.toInt();
            }},

        {"RAC_4_MoveTarget_Position",
            [](Cdc_15_443& m,const QJsonValue& v){ m.rac_4_movetarget_position = v.toInt(); }},

        {"RAC_5_MoveTarget_Position",
            [](Cdc_15_443& m,const QJsonValue& v){ m.rac_5_movetarget_position = v.toInt(); }},

        // ================= SEC OC =================
        {"CDC_SecOC_FreshnessCounter",
            [](Cdc_15_443& m,const QJsonValue& v){ m.cdc_secoc_freshnesscounter = v.toInt(); }},

        {"CDC_SecOC_AuthCode_B0_MSB",
            [](Cdc_15_443& m,const QJsonValue& v){ m.cdc_secoc_authcode_b0_msb = v.toInt(); }},

        {"CDC_SecOC_AuthCode_B1",
            [](Cdc_15_443& m,const QJsonValue& v){ m.cdc_secoc_authcode_b1 = v.toInt(); }},

        {"CDC_SecOC_AuthCode_B2_LSB",
            [](Cdc_15_443& m,const QJsonValue& v){ m.cdc_secoc_authcode_b2_lsb = v.toInt(); }}
    };

    auto it = mapper.find(signalName);
    if (it == mapper.end())
        return false;

    it->second(msg, value);
    return true;
}
