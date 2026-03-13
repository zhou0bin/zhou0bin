from .LocalVariable import StatStep,ecuDtObj
from .log_info import MSGLogger,handler_exception_decorator
from .utils.util_helper import get_BIT_VAL

from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from .utils.util_helper import get_BIT_VAL
import math
#读取 Bit 并比对值
@handler_exception_decorator(length=3)
def ReadBitAndCheckValue(EcuDtObj, DID:str, StartBytePosition:int, StartBitPosition:int, BitLength:int, TargetValue:str, BitMask:str, StatStepRead:StatStep, StatStepCheck:StatStep):
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(DID) != str or len(DID) == 0 or DID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(StartBytePosition) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StartBytePosition == None:
        StartBytePosition = 3
    if type(StartBitPosition) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StartBitPosition == None:
        StartBitPosition = 0
    if type(BitLength) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if BitLength == None:
        BitLength = 1
    if type(TargetValue) != str or TargetValue == None or len(TargetValue) == 0:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(BitMask) != str or len(BitMask) == 0:
        BitMask = "FF"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadBitAndCheckValue: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("ReadBitAndCheckValue: StatStepCheck is error")
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“22”+DID
    RequestString = '22'+DID
    #设置局部变量TargetResponseString=“62”+DID
    TargetResponseString = '62'+DID
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'ECU':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString}) 
    #调用ExcuteServiceAndCheckResponse函数
    IsReadSuccessful, IsCheckSuccessful, VciResult = ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepRead, StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResult
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
              'VciResult': VciResult})
    #IsReadSuccessful==False?
    ResultReadValue = ""
    if IsReadSuccessful==False:
        #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ NotCorrect Response”
        IsCheckSuccessful=False
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.MeasureValue = ResultReadValue
            StatStepCheck.SetValue = TargetValue
            StatStepCheck.Format = 'HEX'
            StatStepCheck.Value='REQ='+RequestString+';RESP='+VciResult
    else:
        #VciResultRead.length<StartBytePosition+BitLength/8+1?除法表示整除不带余数
        vciarray=bytearray.fromhex(VciResult)
        if len(vciarray) >= StartBytePosition+1:
            #从VciResult的StartBytePosition的StartBitPosition开始，截取TargetValue.Length个Bit的值，存为局部变量ReadValue
            payLoagBytesArray=vciarray[:StartBytePosition+1]
            arrlen = len(payLoagBytesArray)
            # bitLength=TargetValue.__len__()
            bitLength = BitLength
            ReadValue:str=''
            for i in range(bitLength):
                sourcePos=(i+StartBitPosition)//8
                sourceOffset=(i+StartBitPosition)%8
                Pos = (arrlen-1)-sourcePos
                val=get_BIT_VAL(payLoagBytesArray[Pos],sourceOffset)
                if val:
                    ReadValue='1'+ReadValue
                else:
                    ReadValue='0'+ReadValue
            ResultReadValue=ReadValue
            #（ReadValue&BitMask）==（TargetValue&BitMask）？，&表示按位与运算

            if (int(ResultReadValue,base=2) & int(BitMask,base=16))==(int(TargetValue,base=2)&int(BitMask,base=16)):
                #IsCheckSuccessful=TrueStatStepCheck.Value=OK@ECU.Name+“ Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValue
                IsCheckSuccessful=True
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.MeasureValue = ResultReadValue
                    StatStepCheck.SetValue = TargetValue
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
            else:
                #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValue
                IsCheckSuccessful = False
                if isStatStepCheck:
                    StatStepCheck.Result = 'NOK'
                    StatStepCheck.MeasureValue = ResultReadValue
                    StatStepCheck.SetValue = TargetValue
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
        else:
            #IsCheckSuccessful=False StatStepCheck.Value=NOK@ECU.Name+“ NotCorrect Response”
            IsCheckSuccessful=False
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.MeasureValue = ResultReadValue
                StatStepCheck.SetValue = TargetValue
                StatStepCheck.Format = 'HEX'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    if isStatStepCheck:
        MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                  'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                  'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                  'StatStepCheck.SetValue': StatStepCheck.SetValue})
    return IsReadSuccessful, IsCheckSuccessful, VciResult