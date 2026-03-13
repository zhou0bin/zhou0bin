from typing import Tuple

from time import sleep
from .log_info import  handler_exception_decorator,MSGLogger,StatStep,ecuDtObj
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P


#DiaglogHandler 代表显示用户提示信息的对话框句柄 保留
# class DiaglogHandler:
#     pass

#周期性读取Byte并比对值
@handler_exception_decorator(length=3)
def PeriodicallyReadByteAndCheckValue(EcuDtObj,DID:str,StartBytePosition:int,ByteLength:int,TargetValue:str,Mask:str,TimeOutMillionSeconds:int,TimeGapMillionSeconds:int,MaximumTimesNok:int,MinimumTimesOk:int,DlgHandler,UserInfo,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[bool,bool,str]:
    if ecuDtObj != type(EcuDtObj) or EcuDtObj == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if DID == None or type(DID) != str:
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
    if TargetValue == None or type(TargetValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if Mask == None:
        Mask = "FF"
    if type(Mask) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TimeOutMillionSeconds == None or type(TimeOutMillionSeconds) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TimeGapMillionSeconds == None or type(TimeGapMillionSeconds) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MaximumTimesNok == None or type(MaximumTimesNok) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MinimumTimesOk == None or type(MinimumTimesOk) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("PeriodicallyReadByteAndCheckValue:StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("PeriodicallyReadByteAndCheckValue:StatStepCheck is error")
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsExcuteSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID
    TargetResponseString='62'+DID
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString，TimeOutSeconds,TimeGapMillionSeconds,MaximumNOK,MinimumOK，StartBytePosition，ByteLength，TargetValue，Mask
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString,'TimeOutMillionSeconds':TimeOutMillionSeconds,'TimeGapMillionSeconds':TimeOutMillionSeconds,'MaximumTimesNok':MaximumTimesNok,'MinimumTimesOk':MinimumTimesOk,'StartBytePosition':StartBytePosition,'ByteLength':ByteLength,'TargetValue':TargetValue,'Mask':Mask})

    IsExcuteSuccessful:bool
    IsCheckSuccessful:bool
    VciResult:str
    ResultReadValue = ""
    if TimeGapMillionSeconds==0:
        #打印错误：时间间隔不能为0
        MSGLogger.error({'错误':'时间间隔不能为0'})
        #0StatStepExcute.Value=NOK@ECU.Name+“ Excute ”+RequestString+”Failed“StatStepCheck.Value=NOK@ECU.Name+“ No Response”IsExcuteSuccessful=FalseIsCheckSuccessful=FalseVciResult=
        if isStatStepRead:
            StatStepRead.Result = 'NOK'
            StatStepRead.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepRead.MeasureValue = ''
            StatStepRead.SetValue = TargetResponseString
            StatStepRead.Format = 'HEX'
            StatStepRead.Minimum = ""
            StatStepRead.Maximum = ""
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepCheck.MeasureValue = ResultReadValue
            StatStepCheck.SetValue = TargetValue
            StatStepCheck.Format = 'HEX'
            StatStepRead.Minimum = ""
            StatStepRead.Maximum = ""
        IsReadSuccessful = False
        IsCheckSuccessful = False
        VciResult = ''
        if isStatStepRead:
            MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'StatStepRead.Result': StatStepRead.Result,
                      'StatStepRead.Value': StatStepRead.Value, 'StatStepRead.Format': StatStepRead.Format,
                      'StatStepRead.MeasureValue': StatStepRead.MeasureValue,
                      'StatStepRead.SetValue': StatStepRead.SetValue})
        if isStatStepCheck:
            MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                      'StatStepCheck.SetValue': StatStepCheck.SetValue})
    else:
        #设置计数器OkCounter=0;NokCounter=0
        OkCounter=0
        NokCounter=0
        #设置MaximumNOK=Min(MaximumNOK,(TimeOutSeconds*1000)/TimeGapMillionSeconds） 是否乘以1000 errno
        MaximumNOK=min(MaximumTimesNok,(TimeOutMillionSeconds*1000)/TimeGapMillionSeconds)
        while True:
            #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepRead，StatStepCheck，IsReadSuccessful，IsCheckSuccessful，VciResult)
            [IsExcuteSuccessful,IsCheckSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj, RequestString, TargetResponseString, LeadingByteLength, StatStepRead, StatStepCheck)
            if isStatStepRead:
                MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
            if isStatStepCheck:
                MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
            MSGLogger.debug({'IsExcuteSuccessful': IsExcuteSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
                      'VciResult': VciResult})

            ResultReadValue:str=''
            if IsExcuteSuccessful==False:
                #NokCounter++
                NokCounter+=1
            else:
                vciarray=bytearray.fromhex(VciResult)
                targetArray=bytearray.fromhex(TargetValue)
                #VciResultRead.length<StartBytePosition+TargetValue.Length
                if len(vciarray) < StartBytePosition+ByteLength:
                    #NokCounter++
                    NokCounter+=1
                else:
                    #从VciResult的StartBytePosition开始，截取TargetValue.Length个Byte的值，存为局部变量ReadValue 包含StartBytePosition向后截取Length
                    ReadValue=vciarray[StartBytePosition:StartBytePosition+ByteLength]
                    ResultReadValue=ReadValue.hex()
                    #（ReadValue&Mask）==（TargetValue&Mask）&表示按位与运算
                    if (int(ResultReadValue,base=16)&int(Mask,base=16))==(int(TargetValue,base=16)&int(Mask,base=16)):
                        OkCounter+=1
                    else:
                        NokCounter+=1
            if OkCounter>=MinimumTimesOk:
                #StatStepCheck.Value=OK@ECU.Name+”Target:”+TargetValue+”；”+”Actual:”+ReadValueIsCheckSuccessful=true VciResult=ResponseFrame
                if isStatStepCheck:
                    if type(StatStepCheck) == StatStep:
                        StatStepCheck.Result = 'OK'
                        StatStepCheck.Value='REQ='+RequestString+';RESP='+VciResult
                        StatStepCheck.MeasureValue = ResultReadValue
                        StatStepCheck.SetValue = TargetValue
                        StatStepCheck.Format = 'HEX'
                    if type(StatStepRead) == StatStep:
                        StatStepRead.Minimum = ""
                        StatStepRead.Maximum = ""

                IsCheckSuccessful=True
                break
            else:
                if NokCounter>=MaximumNOK:
                    #StatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValueIsCheckSuccessful=FalseVciResult=ResponseFrame
                    if isStatStepCheck:
                        if type(StatStepCheck) == StatStep:
                            StatStepCheck.Result = 'NOK'
                            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                            StatStepCheck.MeasureValue = ResultReadValue
                            StatStepCheck.SetValue = TargetValue
                            StatStepCheck.Format = 'HEX'
                        if type(StatStepRead) == StatStep:
                            StatStepRead.Minimum = ""
                            StatStepRead.Maximum = ""
                    IsCheckSuccessful=False
                    break
                else:
                    sleep(TimeGapMillionSeconds/1000)
        if isStatStepCheck:
            MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                      'StatStepCheck.SetValue': StatStepCheck.SetValue})
    return [IsExcuteSuccessful,IsCheckSuccessful,VciResult]
