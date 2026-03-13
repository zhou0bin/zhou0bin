
from .LocalVariable import StatStep,ecuDtObj

from .log_info import  handler_exception_decorator ,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P

#进入拓展模式
@handler_exception_decorator(length=2)
def StartCom(EcuDtObj,StatStepExcute:StatStep):
    if ecuDtObj != type(EcuDtObj) or EcuDtObj == None:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("StartCom: StatStepExcute is error")
    isStatStepExist=False
    if isinstance(StatStepExcute,StatStep):
        isStatStepExist = True
    IsExcuteSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“1003”
    RequestString='1003'
    #设置局部变量TargetResponseString=“5003”
    TargetResponseString='5003'
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    [IsExcuteSuccessful,IsExcuteSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepExcute,StatStepExcute)
    # if(not IsExcuteSuccessful):
    #     [IsExcuteSuccessful,IsExcuteSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepExcute,StatStepExcute)
    #     MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
    #     if(not IsExcuteSuccessful):
    #         [IsExcuteSuccessful,IsExcuteSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepExcute,StatStepExcute)
    #         MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
    if isStatStepExist:
        MSGLogger.debug({'StatStepExcute.Value':StatStepExcute.Value,'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult})
    return [IsExcuteSuccessful,VciResult]
