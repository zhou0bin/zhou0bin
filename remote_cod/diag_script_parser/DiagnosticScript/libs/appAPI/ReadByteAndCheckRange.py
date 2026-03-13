from typing import Tuple

from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
#读取 Byte 并比对范围
@handler_exception_decorator(length=3)
def ReadByteAndCheckRange(EcuDtObj,DID:str,StartBytePosition:int,ByteLength:int,MaxValue:float,MinValue:float,coefficient_a:float,coefficient_b:float,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[bool,bool,str]:
    if ecuDtObj != type(EcuDtObj) or EcuDtObj == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if str != type(DID) or DID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StartBytePosition == None:
        StartBytePosition = 3
    if type(StartBytePosition) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if ByteLength == None:
        ByteLength = 1
    if type(ByteLength) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MaxValue == None or type(MaxValue) != float:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MinValue == None or type(MinValue) != float:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if coefficient_a == None or type(coefficient_a) != float:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if coefficient_b == None or type(coefficient_b) != float:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadByteAndCheckRange: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("ReadByteAndCheckRange: StatStepCheck is error")

    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    if not isinstance(StartBytePosition,int):
        StartBytePosition = 3
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID 
    #设置局部变量TargetResponseString=“62”+DID
    TargetResponseString='62'+DID
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepRead，StatStepCheck，IsReadSuccessful，IsCheckSuccessful，VciResult)
    IsReadSuccessful:bool
    IsCheckSuccessful:bool
    VciResult:str
    #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepRead，StatStepCheck，IsReadSuccessful，IsCheckSuccessful，VciResult)
    [IsReadSuccessful,IsCheckSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepRead, StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResult
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
              'VciResult': VciResult})

    if IsReadSuccessful==False:
        #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ Not Correct Response”
        IsCheckSuccessful=False
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
            StatStepCheck.Minimum = MinValue
            StatStepCheck.MeasureValue = VciResult.upper()
            StatStepCheck.Maximum = MaxValue
            StatStepCheck.Format = 'DOUBLE'

    else:
        vciarray:bytearray= bytearray.fromhex(VciResult)
        #从VciResultRead的StartBytePosition开始，截取TargetValue.Length个Byte的值 ? 这里确认是Length 小于么 小于不是没法截取么
        if not len(vciarray)<StartBytePosition+ByteLength:
            payLoad=vciarray[StartBytePosition:StartBytePosition+ByteLength]
            ReadValue=int(payLoad.hex(),base=16)
            # for item in payLoad:
            #     ReadValue+=int(item,16)
            #ReadValue=ReadValue*Coeficent_a+Coeficent_b
            ReadValue=ReadValue*coefficient_a+coefficient_b
            if ReadValue>=MinValue and ReadValue<=MaxValue:
                #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“Response Not in Range”+MinValue+“<=”+ReadValue+”<=”+MaxValue
                IsCheckSuccessful=True
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                    StatStepCheck.Minimum = MinValue
                    StatStepCheck.MeasureValue = ReadValue
                    StatStepCheck.Maximum = MaxValue
                    StatStepCheck.Format = 'DOUBLE'
            else:
                #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“Response Not in Range”+MinValue+“<=”+ReadValue+”<=”+MaxValue
                IsCheckSuccessful=False
                if isStatStepCheck:
                    StatStepCheck.Result = 'NOK'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                    StatStepCheck.Minimum = MinValue
                    StatStepCheck.MeasureValue = ReadValue
                    StatStepCheck.Maximum = MaxValue
                    StatStepCheck.Format = 'DOUBLE'
        else:
            #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ Not Match Response,Response Too Short”
            IsCheckSuccessful=False
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                StatStepCheck.Minimum = MinValue
                StatStepCheck.MeasureValue = VciResult
                StatStepCheck.Maximum = MaxValue
                StatStepCheck.Format = 'DOUBLE'
    if isStatStepCheck:
        StatStepCheck.SetValue = ""
        MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                  'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Minimum': StatStepCheck.Minimum,
                  'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue, 'StatStepCheck.Maximum': StatStepCheck.Maximum,
                  'StatStepCheck.Format': StatStepCheck.Format})
    return [IsReadSuccessful,IsCheckSuccessful,VciResult]