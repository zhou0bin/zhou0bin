from .log_info import handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from .LocalVariable import StatStep,ecuDtObj

#进入默认模式
@handler_exception_decorator(length=2)
def DefaultSession(EcuDtObj, StatStepExcute:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("DefaultSession: StatStepExcute is error")
    IsExcuteSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“1001”
    RequestString = '1001'
    #设置局部变量TargetResponseString=“5001”
    TargetResponseString = '5001'
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    [IsExcuteSuccessful, IsExcuteSuccessful, VciResult] = ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString,LeadingByteLength, StatStepExcute,StatStepExcute)
    #打印StatStepExcute.Value，IsExcuteSuccessful，VciResult
    if isinstance(StatStepExcute,StatStep):
        MSGLogger.debug({'StatStepExcute.Value':StatStepExcute.Value, 'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})
    else:
        MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful, 'VciResult':VciResult})

    return [IsExcuteSuccessful, VciResult]