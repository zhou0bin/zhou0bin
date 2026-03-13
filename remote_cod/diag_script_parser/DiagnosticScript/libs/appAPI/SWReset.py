import time

from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#软件重启
@handler_exception_decorator(length=2)
def SWReset(EcuDtObj, StatStepExcute:StatStep):
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        MSGLogger.error("SWReset InputParam errors:ECUDtObj is error")
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("SWReset InputParam errors:StatStepExcute is error")
    isStatStepExist=False
    if isinstance(StatStepExcute,StatStep):
        isStatStepExist = True
    IsExcuteSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“1103”
    RequestString = '1103'
    #设置局部变量TargetResponseString=“5103”
    TargetResponseString = '5103'
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'ECU':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsExcuteSuccessful, IsExcuteSuccessful, VciResult = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepExcute, StatStepExcute)
    #打印StatStepExcute.Value，IsExcuteSuccessful，VciResult
    if isStatStepExist:
        MSGLogger.debug({'StatvStepExcute.Value':StatStepExcute.Value, 'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})
    else:
        MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})

    #延迟1S
    time.sleep(1)
    return IsExcuteSuccessful, VciResult
