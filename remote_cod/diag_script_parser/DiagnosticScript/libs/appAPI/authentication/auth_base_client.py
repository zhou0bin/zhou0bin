from libs.appAPI.authentication.auth_global import *
from typing import Optional, Tuple
from libs.appAPI.log_info import MSGLogger,ecuDtObj
from libs.appAPI.LocalVariable import StatStep
from libs.appAPI.ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P

# -------------------------- 认证客户端 --------------------------
class AuthBaseClient:
    def __init__(self, stat_step, channel=None):
        self.stat_step = stat_step

    def APCEBidirectional(self, ecu: ecuDtObj) -> Tuple[AuthResult, str]:
        return AuthResult.Success, ""
    
    def ACRUnidirectional(self, ecu: ecuDtObj) -> Tuple[AuthResult, str]:
        return AuthResult.Success, ""
    
    def DeAuthenticate(self, ecu:ecuDtObj) -> Tuple[AuthResult, str]:
        """发送2900报文，取消认证状态"""
        # 构造2900报文
        da_req = "2900"
        da_target_resp = "6900"
        LeadingByteLength = da_target_resp.__len__()
        IsExcuteSuccessful, IsCheckSuccess, VciResult = ExcuteServiceAndCheckLeadingResponse_P(ecu,da_req,da_target_resp,LeadingByteLength,self.stat_step,self.stat_step)
        isStatStepRead:bool = False
        if isinstance(self.stat_step, StatStep):
            isStatStepRead=True
            MSGLogger.debug({'self.stat_step.Value':self.stat_step.Value,'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        else:
            MSGLogger.debug({'IsReadSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
        if IsCheckSuccess:
            if isStatStepRead:
                self.stat_step.Result='OK'
                self.stat_step.Value = ' REQ=' + da_req + ' ,RESP=' + VciResult
                self.stat_step.Format='HEX'
                self.stat_step.MeasureValue ='REQ ='+da_req.upper() +', RESP ='+VciResult.upper()  
                self.stat_step.SetValue = da_target_resp.upper()              
            return AuthResult.Success, VciResult
        else:
            if isStatStepRead:
                self.stat_step.Result = 'NOK'
                self.stat_step.Value = ' REQ=' + da_req + ' ,RESP=' + VciResult
                self.stat_step.Format = 'HEX'
                self.stat_step.MeasureValue = 'REQ =' + da_req.upper() + ', RESP =' + VciResult.upper()
                self.stat_step.SetValue = da_target_resp.upper()
            MSGLogger.error(f"AuthAPCEClient::ProofOfOwnership: send request failed, result is {VciResult}")
            return AuthResult.DeAuthenticateFail, VciResult