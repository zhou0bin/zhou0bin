import rsa
from typing import Optional, Tuple
from libs.appAPI.authentication.auth_base_client import *

class AuthACRClient(AuthBaseClient):
    def __init__(
            self, 
            stat_step: StatStep,
            communication_configuration:str, 
            alg_indicator:str = "01"*16, 
            additional_parameter:str = "",
            private_key_path: Optional[str] = "", 
            cryption: str = "",      
            channel = None):
        super().__init__(stat_step, channel)
        self.private_key = None
        self.server_challenge = b""                         # 存储ECU返回的挑战码
        self.comm_config = communication_configuration      # 通信配置（2905请求的#3字节）
        self.alg_indicator = alg_indicator                  # 算法标识（16字节，示例用0x01填充）
        self.hash_alg = "SHA-256"
        self.client_challenge = None
        self.additional_parameter = additional_parameter
        self.cryption = cryption
        # self.stat_step = stat_step                          # 结果文件记录工具
        # 加载RSA私钥
        if private_key_path:
            with open(private_key_path, "rb") as f:
                self.private_key = rsa.PrivateKey.load_pkcs1(f.read())

    def EncodeLength(self, field_data):
        """长度编码：2字节，MSB（高位）在前，LSB（低位）在后"""
        length = len(field_data)
        return bytes([(length >> 8) & 0xFF, length & 0xFF])

    def BuildRequestChallengeForAuthRequest(self, comm_config: str="00", alg_indicator: bytes=b"\x01"*16) -> str:
        """
        构造2905请求报文
        :param comm_config: 通信配置（#3字节，0x00-0xFF）
        :param alg_indicator: 算法标识（#4-#19字节，16字节）
        :return: 2905请求报文
        """
        # 校验参数长度
        if len(alg_indicator) != 16:
            raise ValueError("The algorithm identifier must be 16 bytes.")
        # if not (0x00 <= comm_config <= 0xFF):
        #     raise ValueError("The communication configuration must be a single byte in the range of 0x00-0xFF.")
        
        # 构造报文（#1:ARS=0x29, #2:SubFunction=0x05, #3:COCO, #4-#19:AI）
        request = UDSConfig.AUTH_SERVICE_ID                              # 0x29
        request += UDSConfig.REQUEST_CHALLENGE_FOR_AUTHENTICATION        # 子功能0x05
        request += comm_config                                           # 通信配置
        request += alg_indicator.hex()                                   # 算法表示

        # 存储当前通信配置和算法标识（用于后续2906请求）
        self.comm_config = comm_config
        if alg_indicator != bytes.fromhex(self.alg_indicator):
            self.stat_step.Result = "NOK"
            raise ValueError(f"The response algorithm identifier is not equal request algorithm identifier, the respnse ai is {alg_indicator.hex()}")
        return request

    def RequestChallengeForAuthentication(
            self, 
            ecu: ecuDtObj, 
            comm_config: str, 
            alg_indicator: str) -> Tuple[AuthResult, str]:
        """发送2905请求"""
        try:
            rcfa_request = self.BuildRequestChallengeForAuthRequest(comm_config, bytes.fromhex(alg_indicator))
        except ValueError as e:
            self.stat_step.Result = "NOK"
            MSGLogger.error(f"AuthAcrClient::__SendRequestChallengeForAuthentication: 2905 request construction failed: {e}")
            return AuthResult.RequestChallengeForAuthenticationFail, ""
        
        rcfa_target_resp = "6905"
        LeadingByteLength=rcfa_target_resp.__len__()
        IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,rcfa_request,rcfa_target_resp,LeadingByteLength,self.stat_step,self.stat_step)
        isStatStepRead:bool = False
        if isinstance(self.stat_step, StatStep):
            isStatStepRead=True
            MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        else:
            MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        if IsCheckSuccess:
            rcfa_response = bytes.fromhex(VciResult)
            if len(rcfa_response) < UDSConfig.REQUEST_CHALLENGE_FOR_AUTHENTICATION_MIN:
                self.stat_step.Result = "NOK"
                MSGLogger.error(f"AuthAcrClient::__SendRequestChallengeForAuthentication: the response length is not enough, the length is {len(rcfa_response)}")
                return AuthResult.RequestChallengeForAuthenticationFail, ""
            # 解析服务器响应
            resp_ptr = 3  # 跳过响应SID(0x69)和子功能(0x05)以及returnValue
            self.algorithm_indicator = rcfa_response[resp_ptr:19]
            resp_ptr = 19
            # 解析服务器挑战码长度
            server_challenge_len = (rcfa_response[resp_ptr] << 8) | rcfa_response[resp_ptr+1]
            resp_ptr += 2
            self.server_challenge = rcfa_response[resp_ptr:resp_ptr+server_challenge_len]
            resp_ptr += server_challenge_len
            # 附加参数长度
            needed_additional_parameter_len = (rcfa_response[resp_ptr] << 8) | rcfa_response[resp_ptr+1]
            # 附加参数
            self.needed_additional_parameter = rcfa_response[resp_ptr:resp_ptr+needed_additional_parameter_len].hex()
            return AuthResult.Success, VciResult
        else:
            if isStatStepRead:
                self.stat_step.Result = 'NOK'
                self.stat_step.Value = ' REQ=' + rcfa_request + ' ,RESP=' + VciResult
                self.stat_step.Format = 'HEX'
                self.stat_step.MeasureValue = 'REQ =' + rcfa_request.upper() + ', RESP =' + VciResult.upper()
                self.stat_step.SetValue = rcfa_target_resp.upper()
            return AuthResult.VerifyCertBidirectionalFail, ""
        
    def SignChallenge(self) -> Optional[bytes]:
        """RSA签名挑战码"""
        if not self.server_challenge or not self.private_key:
            self.stat_step.Result = 'NOK'
            MSGLogger.error(f"Signature prerequisite not met, challenge is {self.server_challenge.hex()}, private key is {self.private_key}")
            return None
        try:
            signature = rsa.sign(self.server_challenge, self.private_key, self.hash_alg)
            self.stat_step.Result = 'NOK'
            MSGLogger.info(f"AuthACRClient::SignChallenge: Challenge code signature successful, the signature is {signature}")
            return signature
        except Exception as e:
            self.stat_step.Result = 'NOK'
            MSGLogger.error(f"AuthACRClient::SignChallenge: signature fail, the exception is {str(e)}")
            return None
        
    def BuildVerifyProofOfOwnershipUnidirectional(
            self, 
            alg_indicator: bytes, 
            proof_of_ownership_client: bytes, 
            challenge: bytes|None,
            additional_parameter: bytes|None):
        """
        构造2906请求报文
        :param comm_config: 通信配置（#3字节，0x00-0xFF）
        :param alg_indicator: 算法标识（#4-#19字节，16字节）
        :return: 2906请求报文
        """
        # 校验参数长度
        if len(alg_indicator) != 16:
            self.stat_step.Result = 'NOK'
            raise ValueError("AuthACRClient::BuildVerifyProofOfOwnershipUnidirectional: the algorithm identifier must be 16 bytes.")
        if not proof_of_ownership_client:
            self.stat_step.Result = 'NOK'
            raise ValueError("AuthACRClient::BuildVerifyProofOfOwnershipUnidirectional: the proof of ownership client must be gte 0.")
        if not challenge:
            challenge_len = bytes([0x00, 0x00])
        else:
            challenge_len = self.EncodeLength(challenge)
        if not additional_parameter:
            additional_parameter_len = bytes([0x00, 0x00]) 
        else:
            additional_parameter_len = self.EncodeLength(additional_parameter)
        # 构造报文（#1:ARS=0x29, #2:SubFunction=0x05, #3:COCO, #4-#19:AI）
        request = UDSConfig.AUTH_SERVICE_ID                                     # 0x29
        request += UDSConfig.VERIFY_PROOF_OF_OWNERSHIP_UNIDIRECTIONAL           # 子功能0x06
        request += alg_indicator.hex()                                          # 算法表示
        request += self.EncodeLength(proof_of_ownership_client).hex()           # 所有权长度
        request += proof_of_ownership_client.hex()
        request += challenge_len.hex()
        if challenge:
            request += challenge.hex()
        request += additional_parameter_len.hex()
        if additional_parameter:
            request += additional_parameter.hex()
        MSGLogger.info(f"AuthACRClient::BuildVerifyProofOfOwnershipUnidirectional: build request success, request is {request}")
        return request

    def VerifyProofOfOwnershipUnidirectional(
            self, 
            ecu: ecuDtObj, 
            alg_indicator: str, 
            proof_of_ownership_client: bytes, 
            challenge: None|bytes = None, 
            additional_parameter: None|bytes = None) -> Tuple[AuthResult, str]:
        """
        """       
        try:
            """发送2906请求"""
            try:
                vpoou_request = self.BuildVerifyProofOfOwnershipUnidirectional(bytes.fromhex(alg_indicator), proof_of_ownership_client, challenge, additional_parameter)
            except ValueError as e:
                self.stat_step.Result = 'NOK'
                MSGLogger.error(f"AuthACRClient::VerifyProofOfOwnershipUnidirectional: 2906 request construction failed: {e}")
                return AuthResult.RequestChallengeForAuthenticationFail, ""
            # 发送报文并接收响应
            vpoou_target_resp = "6906"
            LeadingByteLength=vpoou_target_resp.__len__()
            IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,vpoou_request,vpoou_target_resp,LeadingByteLength,self.stat_step,self.stat_step)
            isStatStepRead:bool = False
            if isinstance(self.stat_step, StatStep):
                isStatStepRead=True
                MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            else:
                MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            if IsCheckSuccess:
                vpoou_response = bytes.fromhex(VciResult)
                if len(vpoou_response) < UDSConfig.REQUEST_VERIFY_PROOF_OF_OWNERSHIP_UNIDIRECTIONAL_MIN:
                    self.stat_step.Result = 'NOK'
                    MSGLogger.error(f"AuthAcrClient::VerifyProofOfOwnershipUnidirectional: the response length is not enough, the response is {vpoou_res[1]}.")
                    return AuthResult.VerifyProofOfOwnershipUnidirectionalFail, ""
                if isStatStepRead:
                    self.stat_step.Result='OK'
                    self.stat_step.Value = ' REQ=' + vpoou_request + ' ,RESP=' + VciResult
                    self.stat_step.Format='HEX'
                    self.stat_step.MeasureValue ='REQ ='+vpoou_request.upper() +', RESP ='+VciResult.upper()  
                    self.stat_step.SetValue = vpoou_target_resp.upper()  
                return AuthResult.Success, VciResult
            else:
                if isStatStepRead:
                    self.stat_step.Result = 'NOK'
                    self.stat_step.Value = ' REQ=' + vpoou_request + ' ,RESP=' + VciResult
                    self.stat_step.Format = 'HEX'
                    self.stat_step.MeasureValue = 'REQ =' + vpoou_request.upper() + ', RESP =' + VciResult.upper()
                    self.stat_step.SetValue = vpoou_target_resp.upper()
                MSGLogger.error(f"AuthAcrClient::VerifyProofOfOwnershipUnidirectional: send request failed,  res is {vpoou_res}")
                return AuthResult.VerifyProofOfOwnershipUnidirectionalFail, ""
        except Exception as e:
            self.stat_step.Result = 'NOK'
            MSGLogger.error(f"AuthAcrClient::VerifyProofOfOwnershipUnidirectional: catch exception, exception is {str(e)}")
            return AuthResult.VerifyProofOfOwnershipUnidirectionalFail, ""

    def ACRUnidirectional(self, ecu: ecuDtObj):
        # 步骤一 请求认证挑战码
        rcfa_res = self.RequestChallengeForAuthentication(ecu, self.comm_config, self.alg_indicator)
        if (len(rcfa_res) == 2):
            if rcfa_res[0] == AuthResult.Success:
                # 步骤二 挑战码签名
                signature = self.SignChallenge()
                if not signature:
                    self.stat_step.Result = 'NOK'
                    return AuthResult.SignChallengeFail, ""
                # 步骤三 单向验证所有权
                return self.VerifyProofOfOwnershipUnidirectional(ecu, self.alg_indicator, signature, self.client_challenge, self.additional_parameter)
            else: return rcfa_res
        else:
            self.stat_step.Result = 'NOK'
            MSGLogger.error(f"AuthACRClient::ACRUnidirectional: request challenge for authentication fail, result is {rcfa_res}")
            return AuthResult.RequestChallengeForAuthenticationFail, ""