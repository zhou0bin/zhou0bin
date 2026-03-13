from libs.appAPI.log_info import MSGLogger,ecuDtObj,handler_exception_decorator
from libs.appAPI.authentication.auth_client import *
from libs.appAPI.authentication.auth_base_client import *
from .LocalVariable import StatStep

@handler_exception_decorator(2)
def Authenticate(EcuDtObj:ecuDtObj, StatStepRead:StatStep, config_dir:str = ""):
    if type(EcuDtObj) != ecuDtObj:
        MSGLogger.error("Authenticate: EcuDtObj is error")
        return False, ""
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("SecurityAccess:StatStepRead is error")
        return False, ""
    if type(config_dir) != str:
        MSGLogger.info("SecurityAccess: config dir is error")
        return False, ""
    
    # 初始化客户端并执行认证
    auth_client = AuthClient(StatStepRead, config_dir, program="autox")
    result = auth_client.Authenticate(EcuDtObj)
    if len(result) == 2 and result[0] == AuthResult.Success:
        return True, result[1]
    else:
        return False, result[1]
    
@handler_exception_decorator(2)
def DeAuthenticate(EcuDtObj:ecuDtObj, StatStepRead:StatStep):
    if type(EcuDtObj) != ecuDtObj:
        MSGLogger.error("Authenticate: EcuDtObj is error")
        return False, ""
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("SecurityAccess:StatStepRead is error")
        return False, ""
    
    # 初始化客户端并执行认证
    auth_client = AuthBaseClient(StatStepRead)
    result = auth_client.DeAuthenticate(EcuDtObj)
    if len(result) == 2 and result[0] == AuthResult.Success:
        return True, result[1]
    else:
        return False, result[1]