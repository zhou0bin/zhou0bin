from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

import time
# 硬件重置
@handler_exception_decorator(length=2)
def HWReset(EcuDtObj, StatStepExcute:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("HWReset: StatStepExcute is error")
    IsExcuteSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“1101”
    RequestString = '1101'
    #设置局部变量TargetResponseString=“5101”
    TargetResponseString = '5101'
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsExcuteSuccessful, IsExcuteSuccessful, VciResult = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepExcute,StatStepExcute)
    
    #打印StatStepExcute.Value，IsExcuteSuccessful，VciResult
    if isinstance(StatStepExcute,StatStep):
        MSGLogger.debug({'StatStepExcute.Value':StatStepExcute.Value, 'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})
    else:
        MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})

    #延迟1S
    time.sleep(1)

    return IsExcuteSuccessful, VciResult