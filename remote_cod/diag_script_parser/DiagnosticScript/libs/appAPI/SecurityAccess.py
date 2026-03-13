import os 
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from libs.appAPI.log_info import MSGLogger,ecuDtObj,handler_exception_decorator
from libs.appAPI.security_access.security_access_service import *
from enum import Enum
from .LocalVariable import StatStep

NRCCODE = 0xFF # Negative code defined by myself

@handler_exception_decorator(2)
def SecurityAccess(EcuDtObj:ecuDtObj,ECUSection:str,Level:int,iVariant:str, StatStepRead:StatStep, config_dir:str = ""):
    try:
        if type(EcuDtObj) != ecuDtObj:
            MSGLogger.error("SecurityAccess: EcuDtObj is error")
            return False, ""
        # if type(ECUSection) != str:
        #     MSGLogger.error("SecurityAccess: ECUSection is error")
        #     return ERROR.InvalidInputParameter.value,NRCCODE
        if type(Level) != int:
            MSGLogger.error("SecurityAccess: Level is error")
            return False, ""
        if type(iVariant) != str:
            MSGLogger.info("SecurityAccess: iVariant is error")
            return False, ""
        if StatStepRead == None or type(StatStepRead) != StatStep:
            MSGLogger.error("SecurityAccess:StatStepRead is error")
            return False, ""
        if type(config_dir) != str:
            MSGLogger.info("SecurityAccess: config dir is error")
            return False, ""
        
        sa_client = SecurityAccessService(StatStepRead, config_dir, program="autox")
        result, response = sa_client.SecurityAccess(EcuDtObj, Level, iVariant)
        if result == SecurityAccessResult.Success:
            return True, response
        else:
            return False, response
    except:
        return False, ""