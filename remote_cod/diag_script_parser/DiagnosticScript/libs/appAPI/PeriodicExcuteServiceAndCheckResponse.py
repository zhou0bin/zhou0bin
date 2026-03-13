from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P
from time import sleep 

#周期性发送指令并检查响应
@handler_exception_decorator(length=3)
def PeriodicExcuteServiceAndCheckResponse(EcuDtObj, RequestString:str, TargetResponseString:str, TimeOutSeconds:int, TimeGapMillionSeconds:int, MaximumNOK:int, MinimumOK:int, DlgHandler, UserInfo, StatStepExcute:StatStep, StatStepCheck:StatStep):
    if ecuDtObj != type(EcuDtObj) or EcuDtObj == None:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if str != type(RequestString) or RequestString == None:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if str != type(TargetResponseString) or TargetResponseString == None:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TimeOutSeconds == None or type(TimeOutSeconds) != int:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TimeGapMillionSeconds == None or type(TimeGapMillionSeconds) != int:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MaximumNOK == None or type(MaximumNOK) != int:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if MinimumOK == None or type(MinimumOK) != int:
        if type(StatStepExcute) == StatStep:
            StatStepExcute.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepExcute == None or type(StatStepExcute) != StatStep:
        MSGLogger.error("PeriodicExcuteServiceAndCheckResponse: StatStepExcute is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("PeriodicExcuteServiceAndCheckResponse: StatStepCheck is error")
    
    
    isStatStepExcute: bool = isinstance(StatStepExcute, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsExcuteSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    #设置LeadingByteLength=TargetResponseString.Length
    LeadingByteLength=len(TargetResponseString)
    #打印局部变量ECU，RequestString，TargetResponseString,LeadingByteLength,TimeOutSeconds,TimeGapMillionSeconds,MaximumNOK,MinimumOK
    MSGLogger.debug({'EcuDtObj':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString, 'LeadingByteLength':LeadingByteLength, 
    'TimeOutSeconds':TimeOutSeconds, 'TimeGapMillionSeconds':TimeGapMillionSeconds, 'MaximumNOK':MaximumNOK, 'MinimumOK':MinimumOK})
    if TimeGapMillionSeconds == 0:
        #打印错误：时间间隔不能为0
        MSGLogger.error({'错误':'时间间隔不能为0'})
        #StatStepExcute.Value=NOK@ECU.Name+“ Excute ”+RequestString+”Failed“StatStepCheck.Value=NOK@ECU.Name+“ No Response”IsExcuteSuccessful=FalseIsCheckSuccessful=FalseVciResult=“”
        if isStatStepExcute:
            StatStepExcute.Result = 'NOK'
            StatStepExcute.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepExcute.MeasureValue = ''
            StatStepExcute.SetValue = TargetResponseString.upper()
            StatStepExcute.Format = 'HEX'
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepCheck.MeasureValue = ''
            StatStepCheck.SetValue = TargetResponseString.upper()
            StatStepCheck.Format = 'HEX'
        IsReadSuccessful = False
        IsCheckSuccessful = False
        VciResult = ''
        if isStatStepExcute:
            MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'StatStepExcute.Result': StatStepExcute.Result,
                      'StatStepExcute.Value': StatStepExcute.Value, 'StatStepExcute.Format': StatStepExcute.Format,
                      'StatStepExcute.MeasureValue': StatStepExcute.MeasureValue,
                      'StatStepExcute.SetValue': StatStepExcute.SetValue})
        if isStatStepCheck:
            MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                      'StatStepCheck.SetValue': StatStepCheck.SetValue})

    else:
        #设置计数器OkCounter=0;NokCounter=0;设置MaximumNOK=Min(MaximumNOK,(TimeOutSeconds*1000)/TimeGapMillionSeconds）
        OkCounter= 0
        NokCounter= 0
        MaximumNOK=min(MaximumNOK, (TimeOutSeconds*1000)/TimeGapMillionSeconds)
        while True:
            #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepExcute，StatStepCheck，IsExcuteSuccessful，IsCheckSuccessful，VciResult)
            IsExcuteSuccessful, IsCheckSuccessful, ResponseFrame = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepExcute, StatStepCheck)
            if isStatStepExcute:
                MSGLogger.debug({'isStatStepExcute.Value': StatStepExcute.Value})
            if isStatStepCheck:
                MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
            MSGLogger.debug({'IsExcuteSuccessful': IsExcuteSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
                      'VciResult': VciResult})
            #判断IsRequestSuccess==True
            if IsExcuteSuccessful == True:
                if IsCheckSuccessful == True:
                    #OkCounter++
                    OkCounter += 1
                else:
                    #NokCounter++
                    NokCounter += 1
            else:
                #NokCounter++
                NokCounter += 1
            #OkCounter>=MinimumOK?
            if OkCounter >= MinimumOK:
                #StatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”Target:”+TargetResponseString+”；”+”Actual:”+ResponseFrameIsCheckSuccessful=FalseVciResult=ResponseFrame
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.MeasureValue = ResponseFrame.upper()
                    StatStepCheck.SetValue = TargetResponseString.upper()
                    StatStepCheck.Format = 'HEX'
                IsCheckSuccessful= True
                VciResult= ResponseFrame
                break
            else:
                #NOkCounter<=MaximumNOK?
                if NokCounter >= MaximumNOK:
                    #StatStepCheck.Value=OK@ECU.Name+”Target:”+TargetResponseString+”；”+”Actual:”+ResponseFrameIsCheckSuccessful=trueVciResult=ResponseFrame
                    if isStatStepCheck:
                        StatStepCheck.Result = 'NOK'
                        StatStepCheck.MeasureValue = ResponseFrame.upper()
                        StatStepCheck.SetValue = TargetResponseString.upper()
                        StatStepCheck.Format = 'HEX'
                    IsCheckSuccessful=False
                    VciResult=ResponseFrame
                    break
                else:
                    sleep(TimeGapMillionSeconds/1000)
        if isStatStepCheck:
            MSGLogger.debug({'IsCheckSuccessful': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                      'StatStepCheck.SetValue': StatStepCheck.SetValue})
    #打印StatStepExcute.Value，StatStepCheck.Value，IsExcuteSuccessful，IsCheckSuccessful，VciResult
    #MSGLogger.debug({'StatStepExcute.Value':StatStepExcute.Value, 'StatStepCheck.Value':StatStepCheck.Value, 'IsExcuteSuccessful':IsExcuteSuccessful,
    #'IsCheckSuccessful':IsCheckSuccessful, 'VciResult':VciResult})
    return IsExcuteSuccessful, IsCheckSuccessful, VciResult