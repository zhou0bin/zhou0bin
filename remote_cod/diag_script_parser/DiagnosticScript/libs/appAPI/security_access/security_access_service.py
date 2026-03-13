from libs.appAPI.security_access.security_access_global import *
from libs.appAPI.security_access.security_access_algorithm import *
from libs.appAPI.LocalVariable import StatStep
from libs.appAPI.log_info import MSGLogger,ecuDtObj
from libs.appAPI.ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P

"""  
security access service

提供完整的安全访问服务，包括：
1、seed request
2、seed转key
3、key request
  
主要组件：    
    SecurityAccessService(对象):    安全访问服务对象，提供完整的安全访问服务。
    
示例用法：  
    # 初始化客户端并执行认证
    sa_client = SecurityAccessService(r"D:\\security\\security\\sa_config.json", program="autox")
    ecu = ECUObj(0x77c, 0x774, "FSWM")
    if sa_client.SecurityAccess(ecu, 1)[0] == SecurityAccessResult.Success:
        print("success!")
    else:
        print("fail")
"""

class SecurityAccessService:
    """  
    SecurityAccessService

	Attributes:  
        algorithm: 					安全访问算法对象。

    Methods:  
        SecurityAccess:             安全访问接口，包含请求种子与key生成部分的交互流程(用户使用)。
        _SecurityAccess:              内部接口，实现27服务。
    """ 
    def __init__(self, stat_step: StatStep, sa_config_dir: str = "", program:str = "autox", channel = None):
        self.algorithm = SecurityAccessAlgorithm(program, sa_config_dir)
        self.stat_step = stat_step

    def _SecurityAccess(self, ecu: ecuDtObj, level: int, iv: str):
        ServiceString = "27"
        ResponseString = "67"
        # result = 0x80
        subID = ("{:02X}".format(level), "{:02X}".format(level+1))
        RequestString = ServiceString + subID[0]
        TargetResponseString = ResponseString + subID[0]
        LeadingByteLength=TargetResponseString.__len__()
        IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,RequestString,TargetResponseString,LeadingByteLength,self.stat_step,self.stat_step)
        isStatStepRead:bool = False
        if isinstance(self.stat_step, StatStep):
            isStatStepRead=True
            MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        else:
            MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        
        if IsCheckSuccess:
            seed = VciResult[4:]
            if len(seed) > 0 and all(c == '0' for c in seed):
                MSGLogger.debug("FactoryMode,Don't Need Key")
                if isStatStepRead:
                    self.stat_step.Result='OK'
                    self.stat_step.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                    self.stat_step.Format='HEX'
                    self.stat_step.MeasureValue ='REQ ='+RequestString.upper() +', RESP ='+VciResult.upper()
                    self.stat_step.SetValue = TargetResponseString.upper() 
                return SecurityAccessResult.Success, VciResult
            error, key = self.algorithm.GenerateKeyFromSeed(seed, ecu.Name, level)
            if error != SecurityAccessResult.Success:
                self.stat_step.Result='NOK'
                MSGLogger.error("SecurityAccess: generate key error.")
                return error, VciResult
            else:
                RequestString = ServiceString + subID[1] + key
                TargetResponseString = ResponseString + subID[1]
                LeadingByteLength = len(TargetResponseString)
                IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,RequestString,TargetResponseString,LeadingByteLength,self.stat_step,self.stat_step)
                if isStatStepRead:
                    MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
                else:
                    MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
                if IsCheckSuccess:
                    if isStatStepRead:
                        self.stat_step.Result='OK'
                        self.stat_step.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                        self.stat_step.Format='HEX'
                        self.stat_step.MeasureValue ='REQ ='+RequestString.upper() +', RESP ='+VciResult.upper()  
                        self.stat_step.SetValue = TargetResponseString.upper()              
                    return SecurityAccessResult.Success, VciResult
                else:
                    if isStatStepRead:
                        self.stat_step.Result = 'NOK'
                        self.stat_step.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                        self.stat_step.Format = 'HEX'
                        self.stat_step.MeasureValue = 'REQ =' + RequestString.upper() + ', RESP =' + VciResult.upper()
                        self.stat_step.SetValue = TargetResponseString.upper()
                    # if VciResult.lower() == "7f2735":
                    #     result = 0x09
                    #     return result,int(VciResult[4:],16)
                    # elif VciResult[0:2].lower() == "7f":
                    #     result = 0x80
                    #     return result,int(VciResult[4:],16)
                    # else:
                    #     result = 0x80
                    #     return result,VciResult
                    return SecurityAccessResult.KeyCalculateError, VciResult
        else:
            if isStatStepRead:
                self.stat_step.Result = 'NOK'
                self.stat_step.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                self.stat_step.Format = 'HEX'
                self.stat_step.MeasureValue = 'REQ =' + RequestString.upper() + ', RESP =' + VciResult.upper()
                self.stat_step.SetValue = TargetResponseString.upper()
            # if VciResult.lower() == "7f2735":
            #     result = 0x09
            #     return result,int(VciResult[4:],16)
            # elif VciResult[0:2].lower() == "7f":
            #     return result,int(VciResult[4:],16)
            return SecurityAccessResult.InvalidSeed, VciResult
        

    def SecurityAccess(self, ecu:ecuDtObj, level: int, iv: str = ""):
        """
        安全访问接口，包含请求种子与key生成部分的交互流程
        Args:
            ecu_name(str):          ecu名称
            source_address(int):    源地址 (can tx)
            targe_address(int):     目的地址 (can rx)
            level(str):             诊断请求报文 
            iv(str):                预留，默认为空  
        Return:  
            result(SecurityAccessResult):        返回结果
            response():             结果内容
        """
        if type(level) != int:
            MSGLogger.error("SecurityAccess: level is error")
            return SecurityAccessResult.InvalidInputParameter, ""
        
        # 安全等级必须是奇数
        if level % 2 != 1:
            MSGLogger.error("SecurityAccess: level is not odd")

        return self._SecurityAccess(ecu, level, iv)  