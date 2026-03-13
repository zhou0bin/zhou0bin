from .LocalVariable import StatStep
from .log_info import handler_exception_decorator,MSGLogger,ecuDtObj
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

# 检查工厂模式
@handler_exception_decorator(length=3)
def CheckFactoryMode(EcuDtObj, TargetValue:str, StatStepRead:StatStep, StatStepCheck:StatStep):
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TargetValue == None:
        TargetValue = "00000000" 
    if type(TargetValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("CheckFactoryMode: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("CheckFactoryMode: StatStepCheck is error") 
    IsExcuteSuccessful=False
    IsCheckSuccessful=False
    VciResult=''

    #设置局部变量RequestString=“2701”
    RequestString = '2701'
    #设置局部变量TargetResponseString=“670100000000”
    TargetResponseString = '6701' + TargetValue

    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'ECU':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsExcuteSuccessful, IsCheckSuccessful, VciResult = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepRead, StatStepCheck)
    #打印StatStepExcute.Value，IsExcuteSuccessful，VciResult
    if isinstance(StatStepRead,StatStep):
        MSGLogger.debug({'StatStepRead.Value':StatStepRead.Value})
    if isinstance(StatStepCheck,StatStep):
        MSGLogger.debug({'StatStepCheck.Value':StatStepCheck.Value})
    MSGLogger.debug({'IsExcuteSuccessful':IsExcuteSuccessful,'IsCheckSuccessful':IsCheckSuccessful,'VciResult':VciResult})
    return IsExcuteSuccessful, IsCheckSuccessful, VciResult