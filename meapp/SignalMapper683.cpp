#include "SignalMapper683.h"

using json = nlohmann::json;

bool SignalMapper683::SetSignal(
    Cdc_control2_683& msg,
    const std::string& signalName,
    const QJsonValue& value)
{
    static const std::unordered_map<
        std::string,
        std::function<void(Cdc_control2_683&, const QJsonValue&)>
    > mapper =
    {
        // =========================================================
        // 1. VALVE POWER / CAMERA / LIDAR CONTROL
        // =========================================================

        {"CDC_RiLiDARValvePwr",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rilidarvalvepwr =
                    (Cdc_control2_683_Cdc_rilidarvalvepwrType)v.toInt();
                m.has_cdc_rilidarvalvepwr = true;
            }},

        {"CDC_fisheye_r_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_fisheye_r_cmr_valve_pwr_req = true;
            }},

        {"CDC_FrLiDARValvePwr",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frlidarvalvepwr =
                    (Cdc_control2_683_Cdc_frlidarvalvepwrType)v.toInt();
                m.has_cdc_frlidarvalvepwr = true;
            }},

        {"CDC_fisheye_f_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_fisheye_f_cmr_valve_pwr_req = true;
            }},

        {"CDC_LeLiDARValvePwr",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_lelidarvalvepwr =
                    (Cdc_control2_683_Cdc_lelidarvalvepwrType)v.toInt();
                m.has_cdc_lelidarvalvepwr = true;
            }},

        {"CDC_fisheye_l_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_l_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_fisheye_l_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_lp_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lp_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_lp_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_b_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_b_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_b_cmr_valve_pwr_req = true;
            }},

        {"CDC_mLiDARValvePwr",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_mlidarvalvepwr =
                    (Cdc_control2_683_Cdc_mlidarvalvepwrType)v.toInt();
                m.has_cdc_mlidarvalvepwr = true;
            }},

        {"CDC_auto_lf_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lf_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_lf_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_rf_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rf_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_rf_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_l_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_l_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_l_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_r_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_r_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_r_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_rl_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rl_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_rl_cmr_valve_pwr_req = true;
            }},

        {"CDC_auto_rr_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rr_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_auto_rr_cmr_valve_pwr_req = true;
            }},

        {"CDC_fisheye_b_cmr_valve_pwr_req",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmr_valve_pwr_req =
                    (Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType)v.toInt();
                m.has_cdc_fisheye_b_cmr_valve_pwr_req = true;
            }},

        {"CDC_ReLiDARValvePwr",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_relidarvalvepwr =
                    (Cdc_control2_683_Cdc_relidarvalvepwrType)v.toInt();
                m.has_cdc_relidarvalvepwr = true;
            }},

        // =========================================================
        // 2. CHASSIS CONTROL
        // =========================================================

        {"CDC_FrChassisVvCtrl",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frchassisvvctrl =
                    (Cdc_control2_683_Cdc_frchassisvvctrlType)v.toInt();
                m.has_cdc_frchassisvvctrl = true;
            }},

        {"CDC_MidChassisVvCtrl",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_midchassisvvctrl =
                    (Cdc_control2_683_Cdc_midchassisvvctrlType)v.toInt();
                m.has_cdc_midchassisvvctrl = true;
            }},

        {"CDC_ReChassisVvCtrl",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rechassisvvctrl =
                    (Cdc_control2_683_Cdc_rechassisvvctrlType)v.toInt();
                m.has_cdc_rechassisvvctrl = true;
            }},

        // =========================================================
        // 3. SPRAY NUM + DURATION (RI / FR / LE / etc.)
        // =========================================================

        {"CDC_RiLiDARValveSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rilidarvalvespraynum = v.toInt();
                m.has_cdc_rilidarvalvespraynum = true;
            }},

        {"CDC_RiLiDARValveOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rilidarvalveopdrtn = v.toDouble();
                m.has_cdc_rilidarvalveopdrtn = true;
            }},

        {"CDC_RiLiDARValveClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rilidarvalveclsdrtn = v.toDouble();
                m.has_cdc_rilidarvalveclsdrtn = true;
            }},

        {"CDC_fisheye_r_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmrvvspraynum = v.toInt();
                m.has_cdc_fisheye_r_cmrvvspraynum = true;
            }},

        {"CDC_fisheye_r_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmrvvopdrtn = v.toDouble();
                m.has_cdc_fisheye_r_cmrvvopdrtn = true;
            }},

        {"CDC_fisheye_r_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_fisheye_r_cmrvvclsdrtn = true;
            }},

        {"CDC_FrLiDARValveSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frlidarvalvespraynum = v.toInt();
                m.has_cdc_frlidarvalvespraynum = true;
            }},

        {"CDC_FrLiDARValveOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frlidarvalveopdrtn = v.toDouble();
                m.has_cdc_frlidarvalveopdrtn = true;
            }},

        {"CDC_FrLiDARValveClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frlidarvalveclsdrtn = v.toDouble();
                m.has_cdc_frlidarvalveclsdrtn = true;
            }},

        // =========================================================
        // 4. FISHEYE FRONT / LEFT / RIGHT / BACK
        // =========================================================

        {"CDC_fisheye_f_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmrvvspraynum = v.toInt();
                m.has_cdc_fisheye_f_cmrvvspraynum = true;
            }},

        {"CDC_fisheye_f_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmrvvopdrtn = v.toDouble();
                m.has_cdc_fisheye_f_cmrvvopdrtn = true;
            }},

        {"CDC_fisheye_f_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_fisheye_f_cmrvvclsdrtn = true;
            }},

        {"CDC_LeLiDARValveSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_lelidarvalvespraynum = v.toInt();
                m.has_cdc_lelidarvalvespraynum = true;
            }},

        {"CDC_LeLiDARValveOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_lelidarvalveopdrtn = v.toDouble();
                m.has_cdc_lelidarvalveopdrtn = true;
            }},

        {"CDC_LeLiDARValveClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_lelidarvalveclsdrtn = v.toDouble();
                m.has_cdc_lelidarvalveclsdrtn = true;
            }},

        // =========================================================
        // 5. AUTO CAMERA SETS (LP / LF / RF / L / R / RL / RR / B)
        // =========================================================

        {"CDC_auto_lp_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lp_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_lp_cmrvvspraynum = true;
            }},

        {"CDC_auto_lp_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lp_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_lp_cmrvvopdrtn = true;
            }},

        {"CDC_auto_lp_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lp_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_lp_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_b_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_b_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_b_cmrvvspraynum = true;
            }},

        {"CDC_auto_b_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_b_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_b_cmrvvopdrtn = true;
            }},

        {"CDC_auto_b_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_b_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_b_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_lf_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lf_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_lf_cmrvvspraynum = true;
            }},

        {"CDC_auto_lf_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lf_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_lf_cmrvvopdrtn = true;
            }},

        {"CDC_auto_lf_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_lf_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_lf_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_rf_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rf_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_rf_cmrvvspraynum = true;
            }},

        {"CDC_auto_rf_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rf_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_rf_cmrvvopdrtn = true;
            }},

        {"CDC_auto_rf_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rf_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_rf_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_l_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_l_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_l_cmrvvspraynum = true;
            }},

        {"CDC_auto_l_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_l_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_l_cmrvvopdrtn = true;
            }},

        {"CDC_auto_l_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_l_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_l_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_r_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_r_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_r_cmrvvspraynum = true;
            }},

        {"CDC_auto_r_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_r_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_r_cmrvvopdrtn = true;
            }},

        {"CDC_auto_r_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_r_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_r_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_rl_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rl_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_rl_cmrvvspraynum = true;
            }},

        {"CDC_auto_rl_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rl_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_rl_cmrvvopdrtn = true;
            }},

        {"CDC_auto_rl_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rl_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_rl_cmrvvclsdrtn = true;
            }},

        {"CDC_auto_rr_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rr_cmrvvspraynum = v.toInt();
                m.has_cdc_auto_rr_cmrvvspraynum = true;
            }},

        {"CDC_auto_rr_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rr_cmrvvopdrtn = v.toDouble();
                m.has_cdc_auto_rr_cmrvvopdrtn = true;
            }},

        {"CDC_auto_rr_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_auto_rr_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_auto_rr_cmrvvclsdrtn = true;
            }},

        {"CDC_fisheye_b_CmrVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmrvvspraynum = v.toInt();
                m.has_cdc_fisheye_b_cmrvvspraynum = true;
            }},

        {"CDC_fisheye_b_CmrVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmrvvopdrtn = v.toDouble();
                m.has_cdc_fisheye_b_cmrvvopdrtn = true;
            }},

        {"CDC_fisheye_b_CmrVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmrvvclsdrtn = v.toDouble();
                m.has_cdc_fisheye_b_cmrvvclsdrtn = true;
            }},

        // =========================================================
        // 6. LIDAR / CHASSIS VV
        // =========================================================

        {"CDC_ReLiDARVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_relidarvvspraynum = v.toInt();
                m.has_cdc_relidarvvspraynum = true;
            }},

        {"CDC_ReLiDARVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_relidarvvopdrtn = v.toDouble();
                m.has_cdc_relidarvvopdrtn = true;
            }},

        {"CDC_ReLiDARVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_relidarvvclsdrtn = v.toDouble();
                m.has_cdc_relidarvvclsdrtn = true;
            }},

        {"CDC_FrChassisVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frchassisvvspraynum = v.toInt();
                m.has_cdc_frchassisvvspraynum = true;
            }},

        {"CDC_FrChassisVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frchassisvvopdrtn = v.toDouble();
                m.has_cdc_frchassisvvopdrtn = true;
            }},

        {"CDC_FrChassisVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_frchassisvvclsdrtn = v.toDouble();
                m.has_cdc_frchassisvvclsdrtn = true;
            }},

        {"CDC_MidChassisVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_midchassisvvspraynum = v.toInt();
                m.has_cdc_midchassisvvspraynum = true;
            }},

        {"CDC_MidChassisVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_midchassisvvopdrtn = v.toDouble();
                m.has_cdc_midchassisvvopdrtn = true;
            }},

        {"CDC_MidChassisVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_midchassisvvclsdrtn = v.toDouble();
                m.has_cdc_midchassisvvclsdrtn = true;
            }},

        {"CDC_ReChassisVvSprayNum",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rechassisvvspraynum = v.toInt();
                m.has_cdc_rechassisvvspraynum = true;
            }},

        {"CDC_ReChassisVvOpDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rechassisvvopdrtn = v.toDouble();
                m.has_cdc_rechassisvvopdrtn = true;
            }},

        {"CDC_ReChassisVvClsDrtn",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_rechassisvvclsdrtn = v.toDouble();
                m.has_cdc_rechassisvvclsdrtn = true;
            }},

        // =========================================================
        // 7. SEC OC
        // =========================================================

        {"CDC_SecOC_FreshnessCounterC2",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_secoc_freshnesscounterc2 = v.toInt();
                m.has_cdc_secoc_freshnesscounterc2 = true;
            }},

        {"CDC_SecOC_AuthCodeC2_B0_MSB",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_secoc_authcodec2_b0_msb = v.toInt();
                m.has_cdc_secoc_authcodec2_b0_msb = true;
            }},

        {"CDC_SecOC_AuthCodeC2_B1",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_secoc_authcodec2_b1 = v.toInt();
                m.has_cdc_secoc_authcodec2_b1 = true;
            }},

        {"CDC_SecOC_AuthCodeC2_B2_LSB",
            [](Cdc_control2_683& m,const QJsonValue& v){
                m.cdc_secoc_authcodec2_b2_lsb = v.toInt();
                m.has_cdc_secoc_authcodec2_b2_lsb = true;
            }},
    };

    auto it = mapper.find(signalName);
    if (it == mapper.end())
        return false;

    it->second(msg, value);
    return true;
}
