import subprocess
from libs.appAPI.authentication.auth_base_client import *

# -------------------------- 认证客户端 --------------------------
class AuthAPCEClient(AuthBaseClient):
    def __init__(self, stat_step, communication_configuration, client_cert_path, vehicle_ca_path, root_cert_path, private_key_path, client_challenge_length, tool_path, channel = None):
        """初始化：加载证书/私钥，生成客户端挑战值"""
        super().__init__(stat_step, channel)
        # 设备端证书&根证书
        self.client_cert = client_cert_path                                         # 诊断仪设备证书
        self.vehicle_ca_cert = vehicle_ca_path                                      # 诊断仪中间ca证书
        self.root_cert = root_cert_path                                             # 根证书
        self.private_key = private_key_path                                         # 私钥
        self.tool = tool_path                                                       # 工具
        self.client_challenge = self.GenerateChallenge(client_challenge_length)     # 客户端随机挑战值
        self.server_challenge = None                                                # 服务器挑战值
        self.server_cert = None                                                     # 服务器证书
        self.comm_config = communication_configuration                              # 通信配置字符串

    def GenerateChallenge(self, client_challenge_len: int=UDSConfig.CHALLENGE_LEN) -> str:
        # 生成挑战数
        challenge_cmd = f"{self.tool} challenge -l {client_challenge_len}"
        # 执行命令并捕获输出
        result = subprocess.run(
            challenge_cmd,
            shell=True,                 # Windows环境必须指定，执行CMD命令
            stdout=subprocess.PIPE,     # 捕获标准输出（核心：获取命令返回内容）
            stderr=subprocess.PIPE,     # 捕获错误输出（可选，排查命令执行失败原因）
            encoding="gbk"              # Windows CMD默认编码gbk，解决中文乱码
        )
        # 判断命令是否执行成功
        if result.returncode == 0:
            challenge = result.stdout.split('\n')[0]
            if challenge == None:
                MSGLogger.error(f"AuthAPCEClient::GenerateChallenge: generate challenge return null.")
                raise ValueError("Generate chanllenge failed")                    
            return challenge
        else:
            MSGLogger.error(f"AuthAPCEClient::GenerateChallenge: generate challenge excute cmd error, error is {result.stderr}")
            raise ValueError("Generate chanllenge failed")

    def EncodeLength(self, field_data):
        """长度编码：2字节，MSB（高位）在前，LSB（低位）在后"""
        length = len(field_data)
        return bytes([(length >> 8) & 0xFF, length & 0xFF])

    def BuildVerifyCertBidirectionalRequest(self, comm_config: str, client_cert_path: str|None, client_challenge: str|None):
        """构造2902报文（29子功能0x02）"""
        # 字段顺序：SID → SubFunction → communicationConfiguration → 证书长度 → 证书 → 挑战值长度 → 挑战值
        # 双向认证请求报文
        request = UDSConfig.AUTH_SERVICE_ID                           # 0x29
        request += UDSConfig.VERIFY_CERT_BIDIRECTIONAL                # 子功能0x02
        request += comm_config                                        # 通信配置
        verify_cert_bidirectional_cmd = f"{self.tool} assemble -c {client_challenge} -p {client_cert_path}"
        # print(verify_cert_bidirectional_cmd)
        # 执行命令并捕获输出
        result = subprocess.run(
            verify_cert_bidirectional_cmd,
            shell=True,  # Windows环境必须指定，执行CMD命令
            stdout=subprocess.PIPE,  # 捕获标准输出（核心：获取命令返回内容）
            stderr=subprocess.PIPE,  # 捕获错误输出（可选，排查命令执行失败原因）
            encoding="gbk"  # Windows CMD默认编码gbk，解决中文乱码
        )
        # 判断命令是否执行成功
        if result.returncode == 0:
            vcb_req = result.stdout.split('\n')[0]
            request += vcb_req
            return request
        else:
            MSGLogger.error(f"AuthAPCEClient::BuildVerifyCertBidirectionalRequest: generate challenge excute cmd error, error is {result.stderr}")
            raise Exception("Generate chanllenge failed")

    def VerifyCertServerAndProofOfOwnershipServer(
            self, 
            cert_server: bytes|None, 
            proof_of_ownership_server: bytes|None) -> bool:
            # 调用autox的exe
            verify_cert_cmd = f"{self.tool} validate -e {self.ecu_data} -t {self.client_challenge} -r {self.root_cert} -v {self.vehicle_ca_cert}"
            # 执行命令并捕获输出
            result = subprocess.run(
                verify_cert_cmd,
                shell=True,              # Windows环境必须指定，执行CMD命令
                stdout=subprocess.PIPE,  # 捕获标准输出（核心：获取命令返回内容）
                stderr=subprocess.PIPE,  # 捕获错误输出（可选，排查命令执行失败原因）
                encoding="gbk"           # Windows CMD默认编码gbk，解决中文乱码
            )
            # 判断命令是否执行成功
            if result.returncode == 0:
                # 获取输出内容，按行分割
                output_lines = result.stdout.splitlines()
                for _, line_content in enumerate(output_lines, start=1):
                    if line_content:  # 过滤空行（避免命令输出末尾的空行干扰）
                        if line_content.lower() == "valid":
                            return True
                        elif line_content.lower() == "invalid":
                            self.stat_step.Result = "NOK"
                            MSGLogger.info(f"AuthAPCEClient::VerifyCertServerAndProofOfOwnershipServer: verify cert invalid.")
                            return False
                        else:
                            pass
                            # self.stat_step.Result = "NOK"
                            # MSGLogger.info(f"AuthAPCEClient::VerifyCertServerAndProofOfOwnershipServer: verify cert failed, return value is {line_content}")
                            # return False
                self.stat_step.Result = "NOK"
                MSGLogger.info(f"AuthAPCEClient::VerifyCertServerAndProofOfOwnershipServer: verify cert failed, return value is null.")
                return False
            else:
                self.stat_step.Result = "NOK"
                MSGLogger.error(f"AuthAPCEClient::VerifyCertServerAndProofOfOwnershipServer: verify cert error, error is {result.stderr} ")
                return False

    def VerifyCertBidirectional(
            self, 
            ecu: ecuDtObj, 
            comm_config: str = "00",
            certificate_client: str|None = None,
            challenge_client: str|None = None ) -> Tuple[AuthResult, str]:
        try:
            """发送2902报文，启动双向证书验证"""
            # 构造2902报文
            vcb_req = self.BuildVerifyCertBidirectionalRequest(
                comm_config,
                certificate_client,
                challenge_client
            )
            vcb_target_resp = "6902"
            LeadingByteLength=vcb_target_resp.__len__()
            IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,vcb_req,vcb_target_resp,LeadingByteLength,self.stat_step,self.stat_step)
            isStatStepRead:bool = False
            if isinstance(self.stat_step, StatStep):
                isStatStepRead=True
                MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            else:
                MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            if IsExcuteSuccessful:
                vcb_response = bytes.fromhex(VciResult)
                if len(vcb_response) < UDSConfig.VERIFY_CERT_BIDIRECTIONAL_MIN:
                    self.stat_step.Result = "NOK"
                    MSGLogger.error(f"AuthAcrClient::VerifyCertBidirectional: the response length is not enough, the length is {len(vcb_response)}")
                    return AuthResult.VerifyCertBidirectionalFail, ""
                resp_ptr = 3  # 跳过响应SID(0x69)和子功能(0x02)以及returnValue
                self.ecu_data = vcb_response[resp_ptr:].hex()
                # 解析服务器挑战值
                server_challenge_len = (vcb_response[resp_ptr] << 8) | vcb_response[resp_ptr+1]
                resp_ptr += 2
                self.server_challenge = vcb_response[resp_ptr:resp_ptr+server_challenge_len].hex()
                resp_ptr += server_challenge_len
                # 解析服务器证书
                server_cert_len = (vcb_response[resp_ptr] << 8) | vcb_response[resp_ptr+1]
                resp_ptr += 2
                self.server_cert = vcb_response[resp_ptr:resp_ptr+server_cert_len]
                resp_ptr += server_cert_len
                server_proof_of_ownership_len = (vcb_response[resp_ptr] << 8) | vcb_response[resp_ptr+1]
                resp_ptr += 2
                self.server_proof_of_ownership = vcb_response[resp_ptr:resp_ptr+server_proof_of_ownership_len]
                # 记录日志
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server challenge length is {server_challenge_len}")
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server challenge is {self.server_challenge}")
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server cert length is {server_cert_len}")
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server cert  is {self.server_cert.hex()}")
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server proof of ownership length is {server_proof_of_ownership_len}")
                MSGLogger.info(f"AuthAPCEClient:VerifyCertBidirectional: server proof of ownership is {self.server_proof_of_ownership.hex()}")
                MSGLogger.info("AuthAPCEClient:VerifyCertBidirectional: VerifyCertBidirectional success!")
                return AuthResult.Success, ""
            else:
                if isStatStepRead:
                    self.stat_step.Result = 'NOK'
                    self.stat_step.Value = ' REQ=' + vcb_req + ' ,RESP=' + VciResult
                    self.stat_step.Format = 'HEX'
                    # self.stat_step.MeasureValue = 'REQ =' + vcb_req.upper() + ', RESP =' + VciResult.upper()
                    self.stat_step.SetValue = vcb_target_resp.upper()
                return AuthResult.VerifyCertBidirectionalFail, ""
        except Exception as e:
            self.stat_step.Result = "NOK"
            MSGLogger.error(f"AuthAPCEClient::VerifyCertBidirectional: catch except, exception is {str(e)}")
            return AuthResult.VerifyCertBidirectionalFail, ""
        
    def Sign(
            self, 
            challenge_client: str|None,
            challenge_server: str|None,
            ephemeral_public_key_client: bytes|None = None
            ) -> str:
        # 生成所有权证明（签名服务器挑战值）
        if not challenge_client:
            raise ValueError(f"AuthAPCEClient::Sign: challenge client must exist.")
        if not challenge_server:
            raise ValueError(f"AuthAPCEClient::Sign: challenge server must exist.")
        sign_proof_cmd = f"{self.tool} tester-po -c {challenge_client} -e {challenge_server} -k {self.private_key}"
        # 执行命令并捕获输出
        result = subprocess.run(
            sign_proof_cmd,
            shell=True,  # Windows环境必须指定，执行CMD命令
            stdout=subprocess.PIPE,  # 捕获标准输出（核心：获取命令返回内容）
            stderr=subprocess.PIPE,  # 捕获错误输出（可选，排查命令执行失败原因）
            encoding="utf-8"  # Windows CMD默认编码gbk，解决中文乱码
        ) 
        # 判断命令是否执行成功
        if result.returncode == 0:
            sign_proof = result.stdout.split('\n')[0]
            if sign_proof == None:
                self.stat_step.Result = "NOK"
                MSGLogger.error(f"AuthAPCEClient::SignProofOfOwnership: sign proof of ownership return null.")
                raise ValueError("Sign Proof Of Ownership failed.")                    
            return sign_proof
        else:
            self.stat_step.Result = "NOK"
            MSGLogger.error(f"Sign proof of ownership error, error is {result.stderr}")
            raise ValueError("Sign Proof Of Ownership failed.")
    
    def BuildProofOfOwnershipRequest(
            self, 
            proof_of_ownership_client, 
            ephemeral_public_key_client: bytes|None) -> str:
        """构造proofOfOwnership请求帧（子功能0x03）"""
        # 字段顺序：SID → SubFunction → 所有权证明长度 → 所有权证明（无临时公钥字段）
        request = UDSConfig.AUTH_SERVICE_ID                                                 # 0x29
        request += UDSConfig.PROOF_OF_OWNERSHIP                                             # 子功能0x03
        # request += self.EncodeLength(bytes.fromhex(proof_of_ownership_client)).hex()        # 所有权证明长度（2字节）
        request += proof_of_ownership_client                                                # 所有权证明（签名数据） 
        if not ephemeral_public_key_client:
            request += "0000"                                                               # 临时公钥长度
        else:
            request += self.EncodeLength(ephemeral_public_key_client).hex()
            request += ephemeral_public_key_client.hex()
        return request

    def ProofOfOwnership(
            self, 
            ecu: ecuDtObj, 
            proof_of_ownership_client: str,
            ephemeral_public_key_client: bytes|None = None) -> Tuple[AuthResult, str]:
        """发送2903报文，完成所有权证明"""
        try:
            # 构造29 03报文
            poo_req = self.BuildProofOfOwnershipRequest(proof_of_ownership_client, ephemeral_public_key_client)
            # 发送报文并接收响应
            poo_target_resp = "6903"
            LeadingByteLength = poo_target_resp.__len__()
            IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,poo_req,poo_target_resp,LeadingByteLength,self.stat_step,self.stat_step)
            isStatStepRead:bool = False
            if isinstance(self.stat_step, StatStep):
                isStatStepRead=True
                MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            else:
                MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
            if IsCheckSuccess:
                if isStatStepRead:
                    self.stat_step.Result='OK'
                    self.stat_step.Value = ' REQ=' + poo_req + ' ,RESP=' + VciResult
                    self.stat_step.Format='HEX'
                    self.stat_step.MeasureValue ='REQ ='+poo_req.upper() +', RESP ='+VciResult.upper()  
                    self.stat_step.SetValue = poo_target_resp.upper()
                return AuthResult.Success, VciResult
            else:
                if isStatStepRead:
                    self.stat_step.Result = 'NOK'
                    self.stat_step.Value = ' REQ=' + poo_req + ' ,RESP=' + VciResult
                    self.stat_step.Format = 'HEX'
                    self.stat_step.MeasureValue = 'REQ =' + poo_req.upper() + ', RESP =' + VciResult.upper()
                    self.stat_step.SetValue = poo_target_resp.upper()
                # MSGLogger.error(f"AuthAPCEClient::ProofOfOwnership: send request failed, res is {poo_res}")
                return AuthResult.ProofOfOwnershipFail, ""
        except Exception as e:
            self.stat_step.Result = "NOK"
            MSGLogger.error(f"AuthAPCEClient::ProofOfOwnership: catch exception, exception is {str(e)}")
            return AuthResult.ProofOfOwnershipFail, ""
        
    def APCEBidirectional(self, ecu: ecuDtObj):
        """执行完整双向认证流程（证书验证+所有权证明）"""
        try:
            # 步骤1： 双向证书请求
            vcb_res = self.VerifyCertBidirectional(ecu, self.comm_config, self.client_cert, self.client_challenge)
            if vcb_res[0] != AuthResult.Success: 
                return vcb_res
            # 步骤2： 验证服务端所有权证明
            verify_res = self.VerifyCertServerAndProofOfOwnershipServer(self.server_cert, self.server_proof_of_ownership)
            if verify_res == False:
                return AuthResult.VerifyServerCertAndProofOfOwnershipFail, ""
            # 步骤3： 生成客户端所有权证明
            proof_of_ownership_client = self.Sign(self.client_challenge, self.server_challenge)
            # 步骤4： 发送客户端所有权证明
            return self.ProofOfOwnership(ecu, proof_of_ownership_client)
        except Exception as e:
            self.stat_step.Result = "NOK"
            return AuthResult.VerifyCertBidirectionalFail, ""