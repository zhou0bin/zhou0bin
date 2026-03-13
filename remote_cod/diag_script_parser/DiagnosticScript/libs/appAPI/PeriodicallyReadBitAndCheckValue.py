from typing import Tuple

from time import sleep
from .log_info import  handler_exception_decorator,MSGLogger,StatStep,ecuDtObj
from .utils.util_helper import get_BIT_VAL
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
import math
#DiaglogHandler 代表显示用户提示信息的对话框句柄 預留
        
# 周期性读取 Bit 进行比对 函数定义 
@handler_exception_decorator(length=3)
def PeriodicallyReadBitAndCheckValue(EcuDtObj,DID:str,StartBytePosition:int,StartBitPosition:int,BitLength:int,TargetValue:str,Mask:str,TimeOutMillionSeconds:int,TimeGapMillionSeconds:int,MaximumTimesNok:int,MinimumTimesOk:int,DlgHandler,UserInfo:str,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[bool,bool,str]:
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
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
    if StartBitPosition == None:
        StartBitPosition = 0
    if type(StartBitPosition) != int:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if BitLength == None:
        BitLength = 1
    if type(BitLength) != int:
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
        MSGLogger.error("PeriodicallyReadBitAndCheckValue: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("PeriodicallyReadBitAndCheckValue: StatStepCheck is error")
    isStatStepRead:bool=isinstance(StatStepRead,StatStep)
    isStatStepCheck:bool=isinstance(StatStepCheck,StatStep)
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID
    TargetResponseString='62'+DID
    LeadingByteLength=TargetResponseString.__len__()
    #打印局部变量ECU，RequestString，TargetResponseString，TimeOutSeconds,TimeGapMillionSecondsMaximumNOK,MinimumOK，StartBytePosition，StartBitPosition,TargetValue,BitMask
    MSGLogger.debug({'ECU':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString,'TimeOutMillionSeconds':TimeOutMillionSeconds,'TimeGapMillionSeconds':TimeGapMillionSeconds,'MaximumTimesNok':MaximumTimesNok,'MinimumTimesOk':MinimumTimesOk,'StartBytePosition':StartBytePosition,'StartBitPosition':StartBitPosition,'TargetValue':TargetValue,'Mask':Mask})
    IsReadSuccessful:bool=False
    IsCheckSuccessful:bool=False
    VciResult:str=''
    ResultReadValue = ""
    if TimeGapMillionSeconds==0:
        # 打印错误：时间间隔不能为0 用中文？ erro
        MSGLogger.error({'错误':'时间间隔不能为0'})
        #0StatStepExcute.Value=NOK@ECU.Name+“ Excute ”+RequestString+”Failed“
        if isStatStepRead:
            StatStepRead.Result='NOK'
            StatStepRead.Value='Sys Error:Time Gap Can not be zero!'
            StatStepRead.MeasureValue=''
            StatStepRead.SetValue=TargetResponseString
            StatStepRead.Format='ASCII'
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepCheck.MeasureValue = ResultReadValue
            StatStepCheck.SetValue = TargetValue
            StatStepCheck.Format = 'ASCII'
        IsReadSuccessful=False
        IsCheckSuccessful=False
        VciResult=''
    else:
        #设置计数器OkCounter=0;NokCounter=0;
        OkCounter= 0
        NokCounter=0
        # 设置MaximumNOK=Min(MaximumNOK,(TimeOutSeconds*1000)/TimeGapMillionSeconds） TimeOutMillionSeconds还需要乘以1000么 errno
        MaximumNOK=min(MaximumTimesNok,(TimeOutMillionSeconds*1000)/TimeGapMillionSeconds)
        while True:
            #调用ExcuteServiceAndCheckResponse函数
            [IsReadSuccessful,IsCheckSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj,RequestString,TargetResponseString,LeadingByteLength,StatStepRead,StatStepCheck)
            ResultReadValue:str=''
            if isStatStepRead:
                MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
            if isStatStepCheck:
                MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
            MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
                      'VciResult': VciResult})

            if(IsReadSuccessful):
                # VciResultRead.length<StartBytePosition+(StartBitPosition+TargetValue.Length)/8+1
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
                    if (int(ResultReadValue,base=2) & int(Mask,base=16))==(int(TargetValue,base=2)&int(Mask,base=16)):
                        OkCounter+=1
                    else:
                        NokCounter+=1
                else:
                   NokCounter+=1 
            else:
                NokCounter+=1
            #OkCounter>=MininumOK?
            if OkCounter>=MinimumTimesOk:
                #StatStepCheck.Value=OK@ECU.Name+”Target:”+TargetValue+”；”+”Actual:”+ReadValue IsCheckSuccessful=true VciResult=ResponseFrame ? VciResult=ResponseFrame ReadValue在那是个局部变量  errno
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.Value = 'REQ='+RequestString+';RESP='+VciResult
                    StatStepCheck.MeasureValue = ResultReadValue
                    StatStepCheck.SetValue = TargetValue
                    StatStepCheck.Format = 'HEX'
                IsCheckSuccessful=True
                break
            else:
                #NOkCounter<=MaximumNOK?
                if(NokCounter>=MaximumNOK):
                    #StatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”Target:”+TargetValue+”；”+”Actual:”+ReadValueIsCheckSuccessful=FalseVciResult=ResponseFrame ReadValue在那是个局部变量 errno
                    if isStatStepCheck:
                        StatStepCheck.Result = 'NOK'
                        StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                        StatStepCheck.MeasureValue = ResultReadValue
                        StatStepCheck.SetValue = TargetValue
                        StatStepCheck.Format = 'HEX'
                    IsCheckSuccessful=False
                    break
                else:
                    sleep(TimeGapMillionSeconds/1000)
    if isStatStepRead:
        MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'StatStepRead.Result': StatStepRead.Result,
                  'StatStepRead.Value': StatStepRead.Value, 'StatStepRead.Format': StatStepRead.Format,
                  'StatStepRead.MeasureValue': StatStepRead.MeasureValue,
                  'StatStepRead.SetValue': StatStepRead.SetValue})

    if isStatStepCheck:
        MSGLogger.debug({'IsCheckSuccess': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                  'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                  'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                  'StatStepCheck.SetValue': StatStepCheck.SetValue})
    return [IsReadSuccessful,IsCheckSuccessful,VciResult]

