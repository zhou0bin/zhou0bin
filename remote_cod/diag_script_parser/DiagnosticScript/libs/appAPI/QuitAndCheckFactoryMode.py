

from typing import Tuple

# from .LocalVariable import StatStep

from .log_info import  handler_exception_decorator,MSGLogger,ecuDtObj,StatStep
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P


#退出和检查 ECU 工厂模式 函数定义
@handler_exception_decorator(length=5)
def QuitAndCheckFactoryMode(EcuDtObj,TargetValue:str,StatStepWrite:StatStep,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[bool,bool,bool,str]:
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if TargetValue == None or type(TargetValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if StatStepWrite == None or type(StatStepWrite) != StatStep:
        MSGLogger.error("QuitAndCheckFactoryMode:StatStepWrite is error")
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("QuitAndCheckFactoryMode:StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("QuitAndCheckFactoryMode:StatStepCheck is error")
    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)

    #定义局部变量DID=0110
    DID:str='0110'
    #设置局部变量FactoryModeValue=00
    FactoryModeValue:str='00'
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+FactoryModeValue
    TargetResponseString='62'+DID+FactoryModeValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    IsReadSuccessful:bool=False
    IsReadCheckSucessful:bool=False
    VciResultRead:str=''

    IsWriteSuccessful:bool=False
    VciResultWrite:str=''

    [IsReadSuccessful,IsReadCheckSucessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    #打印VciResultRead.Value，VciResultCheck.Value，IsReadSuccessful，IsCheckSuccessfulVciResultRead  errno VciResultRead.Value，VciResultCheck.Value 这俩写错了吧
    if isStatStepWrite:
        MSGLogger.debug({'StatStepWrite.Value': StatStepWrite.Value})
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'IsReadCheckSucessful':IsReadCheckSucessful,'VciResultRead':VciResultRead})

    if  IsReadCheckSucessful!=True:
        #设置局部变量RequestString=“2E”+DID+FactoryModeValue
        RequestString='2E'+DID+FactoryModeValue
        #设置局部变量TargetResponseString=“6E”+DID
        TargetResponseString='6E'+DID
        #打印局部变量ECU，RequestString，TargetResponseString
        MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
        
        [IsWriteSuccessful,IsWriteSuccessful,VciResultWrite]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepWrite,StatStepWrite)
        #打印VciResultWrite.Value，IsWriteSuccessful，VciResultWrite
        if isStatStepWrite:
            MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value,'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})
        else:
            MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})

        #设置局部变量RequestString=“22”+DID
        RequestString='22'+DID
        #设置局部变量TargetResponseString=“62”+DID+FactoryModeValue
        TargetResponseString='62'+DID+FactoryModeValue
        #打印局部变量ECU，RequestString，TargetResponseString
        MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
        #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepRead，StatStepCheck，IsReadSuccessful，IsCheckSuccessful，VciResultRead)
        [IsReadSuccessful,IsReadCheckSucessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
        #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
        if isStatStepCheck:
            MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if isStatStepRead:
            MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
        MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'IsReadCheckSucessful':IsReadCheckSucessful,'VciResultRead':VciResultRead})

    return [IsWriteSuccessful,IsReadSuccessful,IsReadCheckSucessful,VciResultWrite,VciResultRead]