from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P

#读取 Byte 并比对值
@handler_exception_decorator(length=3)
def ReadByteAndCheckValue(EcuDtObj, DID:str, StartBytePosition:int, ByteLength:int, TargetValue:str, BitMask:str, StatStepRead:StatStep, StatStepCheck:StatStep):
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
    if type(TargetValue) != str or TargetValue == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(BitMask) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if BitMask == None:
        BitMask = "FF"*ByteLength
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadByteAndCheckValue: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("ReadByteAndCheckValue: StatStepCheck is error")

    if not StartBytePosition:
        StartBytePosition = 3
    if not ByteLength:
        ByteLength = 1
    if not BitMask:
        BitMask = "FF"
        BitMask *= ByteLength
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
    ReadValue = ""
    #IsReadSuccessful==False?
    if IsReadSuccessful==False:
        #IsCheckSuccessful=False StatStepCheck.Value=NOK@ECU.Name+“ Not Correct Response”
        IsCheckSuccessful=False
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.MeasureValue = ReadValue
            StatStepCheck.SetValue = TargetValue
            StatStepCheck.Format = 'HEX'
            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    else:
        #VciResultRead.length<StartBytePosition+TargetValue.Length?
        vciarray:bytearray= bytearray.fromhex(VciResult)
        Targetarray:bytearray= bytearray.fromhex(TargetValue)
        VciResultLen = len(vciarray)
        TargetValueLen = len(Targetarray)
        StopBytePosition = StartBytePosition+ByteLength
        if not VciResultLen < StopBytePosition:
            #从VciResult的StartBytePosition开始，截取TargetValue.Length个Byte的值，存为局部变量ReadValue
            ReadValue = vciarray[StartBytePosition:StopBytePosition]
            ReadValue = ReadValue.hex()
            #（ReadValue&Mask）==（TargetValue&Mask）？，&表示按位与运算
            if (int(ReadValue,16)&int(BitMask,16)) == (int(TargetValue,16)&int(BitMask,16)):
                #IsCheckSuccessful=TrueStatStepCheck.Value=OK@ECU.Name+“ Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValue
                IsCheckSuccessful=True
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.MeasureValue = ReadValue
                    StatStepCheck.SetValue = TargetValue
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
            else:
                #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValue
                IsCheckSuccessful = False
                if isStatStepCheck:
                    StatStepCheck.Result = 'NOK'
                    StatStepCheck.MeasureValue = ReadValue
                    StatStepCheck.SetValue = TargetValue
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
        else:
        #IsCheckSuccessful=FalseStatStepCheck.Value=NOK@ECU.Name+“ Not Match Response,Response Too Short”
            IsCheckSuccessful=False
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.MeasureValue = ReadValue
                StatStepCheck.SetValue = TargetValue
                StatStepCheck.Format = 'HEX'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    if isStatStepCheck:
        MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                  'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                  'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                  'StatStepCheck.SetValue': StatStepCheck.SetValue})
    return IsReadSuccessful, IsCheckSuccessful, VciResult

