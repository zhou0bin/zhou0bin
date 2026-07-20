#include "SignalMapper680.h"

using json = nlohmann::json;

bool SignalMapper680::SetSignal(
    Cdc_control1_680& msg,
    const std::string& signalName,
    const QJsonValue& value)
{
    static const std::unordered_map<
        std::string,
        std::function<void(Cdc_control1_680&, const QJsonValue&)>
    > mapper =
    {
        // ================= CDC shield / power =================
        {"CDC_front_shield_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_front_shield_pwr_req =
                    (Cdc_control1_680_Cdc_front_shield_pwr_reqType)v.toInt();
            }},

        {"CDC_fisheye_r_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmr_shield_req =
                    (Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_fisheye_l_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_l_cmr_shield_req =
                    (Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_fisheye_b_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmr_shield_req =
                    (Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_fisheye_f_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmr_shield_req =
                    (Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_CleanPumpECUPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_cleanpumpecupwr =
                    (Cdc_control1_680_Cdc_cleanpumpecupwrType)v.toInt();
            }},

        {"CDC_CleanPumpSpd",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_cleanpumpspd = v.toDouble();
            }},

        // ================= LiDAR Wiper Power =================
        {"CDC_RiLiDARWprPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_rilidarwprpwr =
                    (Cdc_control1_680_Cdc_rilidarwprpwrType)v.toInt();
            }},

        {"CDC_LeLiDARWprPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_lelidarwprpwr =
                    (Cdc_control1_680_Cdc_lelidarwprpwrType)v.toInt();
            }},

        {"CDC_FrLiDARWprPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_frlidarwprpwr =
                    (Cdc_control1_680_Cdc_frlidarwprpwrType)v.toInt();
            }},

        // ================= LiDAR Wiper Move =================
        {"CDC_RiLiDARWprMove",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_rilidarwprmove =
                    (Cdc_control1_680_Cdc_rilidarwprmoveType)v.toInt();
            }},

        {"CDC_LeLiDARWprMove",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_lelidarwprmove =
                    (Cdc_control1_680_Cdc_lelidarwprmoveType)v.toInt();
            }},

        {"CDC_FrLiDARWprMove",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_frlidarwprmove =
                    (Cdc_control1_680_Cdc_frlidarwprmoveType)v.toInt();
            }},

        // ================= Valve / Power =================
        {"CDC_RiLiDARWprPwrPositive",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_rilidarvalvepwr =
                    (Cdc_control1_680_Cdc_rilidarvalvepwrType)v.toInt();
            }},

        {"CDC_LeBLiDARPwrPositive",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_lelidarvalvepwr =
                    (Cdc_control1_680_Cdc_lelidarvalvepwrType)v.toInt();
            }},

        {"CDC_FrBLiDARPwrPositive",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_frlidarvalvepwr =
                    (Cdc_control1_680_Cdc_frlidarvalvepwrType)v.toInt();
            }},

        // ================= Auto camera wiper power =================
        {"CDC_auto_lp_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_lp_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_lp_cmr_wpr_pwr_reqType)v.toInt();
            }},

        // ================= Heat req =================
        {"CDC_fisheye_l_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_l_cmr_heat_req =
                    (Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType)v.toInt();
            }},

        {"CDC_fisheye_f_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_f_cmr_heat_req =
                    (Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType)v.toInt();
            }},

        {"CDC_fisheye_r_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_r_cmr_heat_req =
                    (Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType)v.toInt();
            }},

        // ================= Shield =================
        {"CDC_top_shield_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_top_shield_pwr_req =
                    (Cdc_control1_680_Cdc_top_shield_pwr_reqType)v.toInt();
            }},

        {"CDC_mLiDARShieldMove",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_mlidarshieldmove =
                    (Cdc_control1_680_Cdc_mlidarshieldmoveType)v.toInt();
            }},

        // ================= Camera shield =================
        {"CDC_front_cmr_sreen_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_front_cmr_sreen_shield_req =
                    (Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType)v.toInt();
            }},

        {"CDC_auto_l_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_l_cmr_shield_req =
                    (Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_auto_r_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_r_cmr_shield_req =
                    (Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType)v.toInt();
            }},

        {"CDC_auto_b_cmr_shield_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_b_cmr_shield_req =
                    (Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType)v.toInt();
            }},

        // ================= Wiper req =================
        {"CDC_auto_rf_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_rf_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_rf_cmr_wpr_pwr_reqType)v.toInt();
            }},

        {"CDC_auto_lf_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_lf_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_lf_cmr_wpr_pwr_reqType)v.toInt();
            }},

        {"CDC_auto_r_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_r_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_r_cmr_wpr_pwr_reqType)v.toInt();
            }},

        {"CDC_auto_l_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_l_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_l_cmr_wpr_pwr_reqType)v.toInt();
            }},

        {"CDC_auto_b_cmr_wpr_pwr_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_b_cmr_wpr_pwr_req =
                    (Cdc_control1_680_Cdc_auto_b_cmr_wpr_pwr_reqType)v.toInt();
            }},
        // ================= LED ===============
        {"CDC_FfishEyeLedCtrl",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_ffisheyeledctrl =
                    (Cdc_control1_680_Cdc_ffisheyeledctrlType)v.toInt();
            }},

        {"CDC_FLfishEyeLedCtrl",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_flfisheyeledctrl =
                    (Cdc_control1_680_Cdc_flfisheyeledctrlType)v.toInt();
            }},

        {"CDC_FRfishEyeLedCtrl",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_frfisheyeledctrl =
                    (Cdc_control1_680_Cdc_frfisheyeledctrlType)v.toInt();
            }},

        {"CDC_RfishEyeLedCtrl",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_rfisheyeledctrl =
                    (Cdc_control1_680_Cdc_rfisheyeledctrlType)v.toInt();
            }},

        // ================= Work / PF =================
        {"CDC_WorkSts",
            [](Cdc_control1_680& m,const QJsonValue& v){
                // struct里没字段
            }},

        {"CDC_PfmSts",
            [](Cdc_control1_680& m,const QJsonValue& v){
            }},

        // ================= heat ===============
        {"CDC_fisheye_b_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_fisheye_b_cmr_heat_req =
                    (Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType)v.toInt();
            }},

        {"CDC_auto_lb_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_lb_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_rb_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_rb_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType)v.toInt();
            }},

        {"CDC_auto_lp_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_lp_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_lf_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_lf_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_rf_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_rf_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_l_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_l_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_r_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_r_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType)v.toInt();
            }},
        {"CDC_auto_b_cmr_heat_req",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_auto_b_cmr_heat_req =
                    (Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType)v.toInt();
            }},

        // ================= chassis =================
        {"CDC_ChassisCmrLEDPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_ChassisCmrLEDPwrctrl =
                    (Cdc_control1_680_Cdc_ChassisCmrLEDPwrctrlType)v.toInt();
            }},
        {"CDC_MidchsCAMLEDPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_MidchsCAMLEDPwrctrl =
                    (Cdc_control1_680_Cdc_MidchsCAMLEDPwrctrlType)v.toInt();
            }},
        {"CDC_RechsCAMLEDPwr",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_RechsCAMLEDPwrctrl =
                    (Cdc_control1_680_Cdc_RechsCAMLEDPwrctrlType)v.toInt();
            }},

        // ================= SecOC =================
        {"CDC_SecOC_FreshnessCounterC1",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_secoc_freshnesscounterc1 = v.toInt();
            }},

        {"CDC_SecOC_AuthCodeC1_B0_MSB",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_secoc_authcodec1_b0_msb = v.toInt();
            }},

        {"CDC_SecOC_AuthCodeC1_B1",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_secoc_authcodec1_b1 = v.toInt();
            }},

        {"CDC_SecOC_AuthCodeC1_B2_LSB",
            [](Cdc_control1_680& m,const QJsonValue& v){
                m.cdc_secoc_authcodec1_b2_lsb = v.toInt();
            }}
    };

    auto it = mapper.find(signalName);
    if (it == mapper.end())
        return false;

    it->second(msg, value);
    return true;
}
