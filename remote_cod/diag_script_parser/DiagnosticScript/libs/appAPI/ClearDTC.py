from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#清除 DTC 函数定义
@handler_exception_decorator(length=2)
def ClearDTC(EcuDtObj,StatStepExcute:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("ClearDTC: StatStepExcute is error")
    IsExcuteSuccessful=False
    VciResult=''

    #设置局部变量RequestString=“14FFFFFF”
    RequestString='14FFFFFF'
    #设置局部变量TargetResponseString=“54”
    TargetResponseString='54'
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'ECU':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse
    #打印StatStepExcute.Value，IsExcuteSuccessful，VciResult
    [IsExcuteSuccessful,IsExcuteSuccessful,VciResult]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepExcute,StatStepExcute)
    if isinstance(StatStepExcute,StatStep):
        MSGLogger.debug({'StatStepExcute.Value':StatStepExcute.Value,'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
    else:
        MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult})

    return [IsExcuteSuccessful,VciResult]


     