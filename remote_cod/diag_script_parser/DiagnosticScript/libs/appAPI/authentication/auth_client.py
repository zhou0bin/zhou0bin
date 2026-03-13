from libs.appAPI.authentication.auth_global import *
from libs.appAPI.log_info import MSGLogger,ecuDtObj
from libs.appAPI.LocalVariable import StatStep
import json, os

# -------------------------- 认证客户端 --------------------------
class AuthClient:
    def __init__(self, stat_step: StatStep, auth_config: str = "", program: str = "autox", channel = None):
        self.method = ""
        self.stat_step = stat_step
        self.ParseAuthConfig(stat_step, auth_config, program, channel)
    
    def ParseAuthConfig(self, stat_step, auth_config, program, channel):
        if auth_config == "":
            auth_config = DEFAULT_AUTH_CONFIG_PATH
        auth_dir = os.path.dirname(auth_config)
        try:
            with open(auth_config, "r", encoding="utf-8") as f:
                config_list = json.load(f)
        except Exception as e:
            from libs.appAPI.authentication.auth_base_client import AuthBaseClient
            self.auth_client = AuthBaseClient(stat_step, channel)
            return
        for config in config_list:
            if config["program"] == program:
                communication_configuration = config["communication_configuration"]
                security_concept = config["security_concept"]
                concept = security_concept["concept"]
                direction = security_concept["direction"]
                self.method = concept + "_" + direction
                if concept == "apce":
                    sc_config = security_concept["config"]
                    if "client_certificate_path" in sc_config and sc_config["client_certificate_path"] != "":
                        client_certificate_path = sc_config["client_certificate_path"]
                    else:
                        client_certificate_path = os.path.join(auth_dir, "tester.pem")
                    if "root_ca_path" in sc_config and sc_config["root_ca_path"] != "":
                        root_ca_path = sc_config["root_ca_path"]
                    else:
                        root_ca_path = os.path.join(auth_dir, "root_ca.pem")
                    if "vehicle_ca_path" in sc_config and sc_config["vehicle_ca_path"] != "":
                        vehicle_ca_path = sc_config["vehicle_ca_path"]
                    else:
                        vehicle_ca_path = os.path.join(auth_dir, "vehicle_ca.pem")
                    if "key_path" in sc_config and sc_config["key_path"] != "":
                        key_path = sc_config["key_path"]
                    else:
                        key_path = os.path.join(auth_dir, "Tester-Diagnostic-001.key")
                    if "client_challenge_length" in sc_config:
                        client_challenge_length = sc_config["client_challenge_length"]
                    else:
                        client_challenge_length = UDSConfig.CHALLENGE_LEN
                    if "uds_tool_path" in config and config["uds_tool_path"] != "":
                        uds_tool_path = config["uds_tool_path"]
                    else:
                        uds_tool_path = os.path.join(os.path.join(os.path.join(os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))))), "resources"), "uds29service"), "bin"), "UDS29ServiceTool.exe")
                    try:
                        from libs.appAPI.authentication.auth_apce_client import AuthAPCEClient
                        self.auth_client = AuthAPCEClient(
                            stat_step,
                            communication_configuration, 
                            client_certificate_path, 
                            vehicle_ca_path,
                            root_ca_path, 
                            key_path, 
                            client_challenge_length,
                            uds_tool_path,
                            channel)
                        MSGLogger.info(f"AuthClient-AuthAPCEClient init success.")
                        return
                    except ModuleNotFoundError:
                        raise ValueError("module auth_apce_client is not exist.")
                elif concept == "acr":
                    sc_config = security_concept["config"]
                    if "key_path" in sc_config and sc_config["key_path"] != "":
                        key_path = sc_config["key_path"]
                    else:
                        key_path = os.path.join(auth_dir, "private.pem")
                    if "cryption_algorithm" in sc_config and sc_config["cryption_algorithm"] != "":
                        cryption = sc_config["cryption_algorithm"]
                    else:
                        raise ValueError("acr cryption algorithm must hava value.")
                    if "additional_parameter" in sc_config and sc_config["additional_parameter"] != "":
                        additional_parameter = sc_config["additional_parameter"]
                    else:
                        additional_parameter = ""
                    if "algorithm_indicator" in sc_config and sc_config["algorithm_indicator"] != "":
                        algorithm_indicator = sc_config["algorithm_indicator"]
                    else:
                        algorithm_indicator = "01"*16
                    try:
                        from libs.appAPI.authentication.auth_acr_client import AuthACRClient
                        self.auth_client = AuthACRClient(
                            stat_step,
                            communication_configuration, 
                            algorithm_indicator, 
                            additional_parameter,
                            key_path,
                            cryption,
                            channel)
                        MSGLogger.info(f"AuthClient-AuthAPCEClient init success.")
                        return
                    except ModuleNotFoundError:
                        raise ValueError("module auth_acr_client is not exist.")                    
                else:
                    stat_step.Result = "NOK"
                    raise ValueError(f"security concept is not suppose, concept is {concept}")
        stat_step.Result = "NOK"
        raise ValueError(f"program is not find in config, program is {program}")

    def Authenticate(self, ecu: ecuDtObj):
        if self.method == "apce_bidirectional":
            return self.auth_client.APCEBidirectional(ecu)
        elif self.method == "acr_unidirectional":
            return self.auth_client.ACRUnidirectional(ecu)
        else:
            self.stat_step.Result = "NOK"
            MSGLogger.error(f"AuthClient::Authentication: method is not support, method is {self.method}")
            return AuthResult.AuthNoSupport, ""
