from typing import Dict,Tuple
from .LocalVariable import StatStep
from .log_info import  handler_exception_decorator,MSGLogger

from ..communication.pdu_logical_link import TransmitAndReceive
#直接发送指令 函数定义
def ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj,RequestString:str,TargetResponseString:str,LeadingByteLength,StatStepExcute=None,StatStepCheck=None)->Tuple[bool,bool,str]:
    #设置LeadingByteLength=TargetResponseString.Length
    # LeadingByteLength=TargetResponseString.__len__()
    ErrorStr=''
    #ECU为空？
    if len(EcuDtObj.Name) == 0:
        ErrorStr='ECU Not Defined'
    #ECU为类型错误？

    #RequestString为空？
    elif len(RequestString) == 0:
        ErrorStr='RequestString Not Defined'
    #RequestString类型错误？

    #TargetResponseString为空？
    elif len(TargetResponseString) == 0:
        ErrorStr='TargetResponseString Not Defined'
    #TargetResponseString类型错误？
    isStatStepExcute = isinstance(StatStepExcute, StatStep)
    isStatStepCheck = isinstance(StatStepCheck, StatStep)

    if ErrorStr != '':
        MSGLogger.debug({'ErrorStr':ErrorStr})
        if isStatStepExcute:
            StatStepExcute.Result = 'NOK'
            StatStepExcute.Value = 'Sys Error:'+ErrorStr
            StatStepExcute.Format = 'HEX'
            StatStepExcute.SetValue = TargetResponseString.upper()
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'Sys Error:'+ErrorStr
            StatStepCheck.Format = 'HEX'
            StatStepCheck.SetValue = TargetResponseString.upper()
        IsExcuteSuccessful=False
        IsCheckSuccessful=False
        VciResult=''
        if isStatStepExcute:
            MSGLogger.debug({'IsExcuteSuccessful': IsExcuteSuccessful, 'StatStepExcute.Result': StatStepExcute.Result,
                      'StatStepExcute.Value': StatStepExcute.Value, 'StatStepExcute.Format': StatStepExcute.Format,
                      'StatStepExcute.SetValue': StatStepExcute.SetValue})
        if isStatStepCheck:
            MSGLogger.debug({'IsCheckSuccess': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.SetValue': StatStepCheck.SetValue})
        return [IsExcuteSuccessful,IsCheckSuccessful,VciResult]
    #打印局部变量ECU，RequestString，TargetResponseString,LeadingByteLength
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString,'LeadingByteLength':LeadingByteLength})
    IsCheckExist:bool=True if StatStepCheck else False
    #调用TransmitAndReceive函数
    IsRequestSuccess:bool
    IsNRC:bool
    IsCheckSuccess:bool=False
    ResponseFrame:bytearray
    ErrorMessage:str
    [IsRequestSuccess,IsNRC,IsCheckSuccess,ResponseFrame,ErrorMessage]=TransmitAndReceive(EcuDtObj.RequestID,EcuDtObj.ResponseID,EcuDtObj.FrameType,RequestString,TargetResponseString,LeadingByteLength)
    ResponseFrameStr=''
    if ResponseFrame:
        ResponseFrameStr=ResponseFrame.hex()
    #IsRequestSuccess=true
    IsExcuteSuccessful=False
    VciResult:str=ResponseFrameStr
    if IsRequestSuccess:
        #StatStepExcute.Value=OK@ECU.Name+“ Excute ”+RequestStringIsExcuteSuccessful=True
        if isStatStepExcute:
            StatStepExcute.Result='OK'
            StatStepExcute.Value='REQ='+RequestString+';RESP='+ResponseFrameStr
            StatStepExcute.Format='HEX'
            StatStepExcute.MeasureValue=ResponseFrameStr.upper()
            StatStepExcute.SetValue=TargetResponseString.upper()
        IsExcuteSuccessful=True
        VciResult = ResponseFrameStr
        if IsNRC:
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + ResponseFrameStr
                StatStepCheck.Format = 'HEX'
                StatStepCheck.MeasureValue = ResponseFrameStr.upper()
                StatStepCheck.SetValue = TargetResponseString.upper()
            IsCheckSuccess=False
            VciResult=ResponseFrameStr
            IsExcuteSuccessful=False
        else:
            if(ResponseFrameStr[0:LeadingByteLength]==TargetResponseString.lower()):
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.Value='REQ='+RequestString+';RESP='+ResponseFrameStr
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.MeasureValue = ResponseFrameStr[LeadingByteLength:].upper()
                    StatStepCheck.SetValue = TargetResponseString.upper()
                IsCheckSuccess=True
                VciResult=ResponseFrameStr
            else:
                if isStatStepCheck:
                    StatStepCheck.Result = 'NOK'
                    StatStepCheck.Value='REQ='+RequestString+';RESP='+ResponseFrameStr
                    StatStepCheck.Format = 'HEX'
                    StatStepCheck.MeasureValue = ResponseFrameStr.upper()
                    StatStepCheck.SetValue = TargetResponseString.upper()
                IsCheckSuccess=False
                VciResult=ResponseFrameStr
        if ErrorMessage=="ResponseTimeOut":
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + ResponseFrameStr
                StatStepCheck.Format = 'HEX'
                StatStepCheck.MeasureValue = ResponseFrameStr.upper()
                StatStepCheck.SetValue = TargetResponseString.upper()
            IsCheckSuccess=False
            VciResult=ResponseFrameStr
            IsExcuteSuccessful=False
    else:
        #StatStepExcute.Value=NOK@ECU.Name+“ Excute ”+RequestString+”Failed“StatStepCheck.Value=NOK@ECU.Name+“ No Response”IsExcuteSuccessful=FalseIsCheckSuccessful=FalseVciResult=“”
        if isStatStepExcute:
            StatStepExcute.Result = 'NOK'
            StatStepExcute.Value = 'REQ=' + RequestString + ';RESP=' + ErrorMessage
            StatStepExcute.Format = 'HEX'
            StatStepExcute.MeasureValue = ''
            StatStepExcute.SetValue = TargetResponseString.upper()
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + ErrorMessage
            StatStepCheck.Format = 'HEX'
            StatStepCheck.MeasureValue = ''
            StatStepCheck.SetValue = TargetResponseString.upper()
        IsExcuteSuccessful=False
        IsCheckSuccess=False
        #vciResult就是个str?
        VciResult=''
    #打印StatStepExcute.Value，StatStepCheck.Value，IsExcuteSuccessful，IsCheckSuccessful，VciResult
    if isStatStepExcute:
        printInfo:Dict={'StatStepExcute.Value':StatStepExcute.Value,'IsExcuteSuccessful':IsExcuteSuccessful}
    else:
        printInfo:Dict={'IsExcuteSuccessful':IsExcuteSuccessful}
    if isStatStepCheck:
        printInfo['IsCheckSuccess']=IsCheckSuccess
        printInfo['StatStepCheck.Value']=StatStepCheck.Value
    printInfo['VciResult']=VciResult
    MSGLogger.debug(printInfo)
    if isStatStepExcute:
        MSGLogger.debug({'IsExcuteSuccessful': IsExcuteSuccessful, 'StatStepExcute.Result': StatStepExcute.Result,
              'StatStepExcute.Value': StatStepExcute.Value, 'StatStepExcute.Format': StatStepExcute.Format,
              'StatStepExcute.MeasureValue': StatStepExcute.MeasureValue, 'StatStepExcute.SetValue': StatStepExcute.SetValue})
    if isStatStepCheck:
        MSGLogger.debug({'IsCheckSuccess': IsCheckSuccess, 'StatStepCheck.Result': StatStepCheck.Result,
              'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
              'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue, 'StatStepCheck.SetValue': StatStepCheck.SetValue})
    return [IsExcuteSuccessful,IsCheckSuccess,VciResult]

@handler_exception_decorator(length=3)
def ExcuteServiceAndCheckLeadingResponse(EcuDtObj,RequestString:str,TargetResponseString:str,LeadingByteLength,StatStepExcute=None,StatStepCheck=None)->Tuple[bool,bool,str]:
    return ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj,RequestString,TargetResponseString,LeadingByteLength,StatStepExcute,StatStepCheck)
