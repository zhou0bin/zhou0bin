from .log_info import  handler_exception_decorator,MSGLogger,StatStep,ecuDtObj
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from time import sleep

#周期性读取Byte并比对范围
@handler_exception_decorator(length=3)
def PeriodicallyReadByteAndCheckRange(EcuDtObj, DID:str, StartBytePosition:int, ByteLength:int, MaxValue:float, MinValue:float, coefficient_a:float, coefficient_b:float, TimeOutMillionSeconds:int, TimeGapMillionSeconds:int, MaximumTimesNok:int, MinimumTimesOk:int, DlgHandler, UserInfo, StatStepRead:StatStep, StatStepCheck:StatStep):
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
        MSGLogger.error("PeriodicallyReadByteAndCheckRange: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("PeriodicallyReadByteAndCheckRange: StatStepCheck is error")
        
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    #依据局部变量ECU获取ECU的请求地址RequestID,回复地址ResponseID，请求类型FrameType
    #设置局部变量RequestString=“22”+DID
    RequestString = '22' + DID
    #设置局部变量TargetResponseString=“62”+DID
    TargetResponseString = '62' + DID
    LeadingByteLength=TargetResponseString.__len__()
    if not isinstance(StartBytePosition,int):
        StartBytePosition = 3
    #打印局部变量ECU，RequestString，TargetResponseString，TimeOutSeconds,TimeGapMillionSeconds,MaximumNOK,MinimumOK,MaxValue,MinValue,Coeficent_a,Coeficent_b,ByteLength
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseponseString':TargetResponseString, 'TimeOutMillionSeconds':TimeOutMillionSeconds, 
    'TimeGapMillionSeconds':TimeGapMillionSeconds, 'MaximumTimesNok':MaximumTimesNok, 'MinimumTimesOk':MinimumTimesOk, 'MaxValue':MaxValue, 
    'MinValue':MinValue, 'coefficient_a':coefficient_a, 'coefficient_b':coefficient_b, 'ByteLength':ByteLength})
    #TimeGapMillionSeconds==0?
    if TimeGapMillionSeconds == 0:
        #打印错误：时间间隔不能为0StatStepExcute.Value=NOK@ECU.Name+“ Excute ”+RequestString+”Failed“StatStepCheck.Value=NOK@ECU.Name+“ No Response” IsExcuteSuccessful=False IsCheckSuccessful=False VciResult=“”
        if isStatStepRead:
            StatStepRead.Result = 'NOK'
            StatStepRead.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepRead.MeasureValue = ''
            StatStepRead.SetValue = TargetResponseString
            StatStepRead.Format = 'DOUBLE'
            StatStepRead.Minimum = MinValue
            StatStepRead.Maximum = MaxValue
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.Value = 'Sys Error:Time Gap Can not be zero!'
            StatStepCheck.MeasureValue = ''
            StatStepCheck.SetValue = ""
            StatStepCheck.Format = 'DOUBLE'
            StatStepCheck.Minimum = MinValue
            StatStepCheck.Maximum = MaxValue
        IsReadSuccessful= False
        IsCheckSuccessful= False
        VciResult= ''
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
        #设置计数器OkCounter=0;NokCounter=0;设置MaximumNOK=Min(MaximumNOK,(TimeOutSeconds*1000)/TimeGapMillionSeconds）
        OkCounter = 0
        NokCounter = 0
        MaximumTimesNok=min(MaximumTimesNok, (TimeOutMillionSeconds*1000)/TimeGapMillionSeconds)
        while True:
            ReadValue = ''
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
            if IsReadSuccessful==False:
                #NokCounter++
                NokCounter+=1
            else:
                #VciResultRead.length<StartBytePosition+ByteLength?
                vciarray=bytearray.fromhex(VciResult)
                if len(vciarray) < StartBytePosition + ByteLength:
                    #NokCounter++
                    NokCounter+=1
                else:
                    #从VciResult的StartBytePosition开始，截取ByteLength个Byte的值，存为局部变量ReadValue;
                    # ReadValue=ReadValue*Coeficent_a+Coeficent_b
                    ReadValue = vciarray[StartBytePosition:StartBytePosition+ByteLength]
                    ReadValue = int(ReadValue.hex(),16)
                    ReadValue=ReadValue*coefficient_a+coefficient_b
                    #MinValue=<ReadValue<=MaxValue？
                    if MinValue<=ReadValue and ReadValue<=MaxValue:
                        #OkCounter++
                        OkCounter+=1
                    else:
                        #NokCounter++
                        NokCounter+=1
            #OkCounter>=MininumOK?
            if OkCounter>=MinimumTimesOk:
                #StatStepCheck.Value=OK@ECU.Name+”MinValue:”+MinValue+”<=”+”Actual:”+ReadValue+”< =MaxValue：”+MaxValue
                # IsCheckSuccessful=true
                # VciResult=ResponseFrame
                if isStatStepCheck:
                    StatStepCheck.Result = 'OK'
                    StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                    StatStepCheck.MeasureValue = ReadValue
                    StatStepCheck.Format = 'DOUBLE'
                    StatStepCheck.Minimum = MinValue
                    StatStepCheck.Maximum = MaxValue
                IsCheckSuccessful=True
                # VciResult=ResponseFrame
                break
            else:
                #NOkCounter<=MaximumNOK?
                if NokCounter>=MaximumTimesNok:
                    # StatStepCheck.Value=NOK@ECU.Name+“ Not Match Response”+”MinValue:”+MinValue+”<=”+”Actual:”+ReadValue+”<=MaxValue:”+MaxValue
                    # IsCheckSuccessful=False
                    # VciResult=ResponseFrame
                    if isStatStepCheck:
                        StatStepCheck.Result = 'NOK'
                        StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
                        StatStepCheck.MeasureValue = ReadValue
                        StatStepCheck.Format = 'DOUBLE'
                        StatStepCheck.Minimum = MinValue
                        StatStepCheck.Maximum = MaxValue
                    IsCheckSuccessful=False
                    # VciResult=ResponseFrame
                    break
                else:
                    sleep(TimeGapMillionSeconds/1000)
        if isStatStepCheck:
            StatStepCheck.SetValue = ""
            MSGLogger.debug({'IsCheckSuccess': IsCheckSuccessful, 'StatStepCheck.Result': StatStepCheck.Result,
                      'StatStepCheck.Value': StatStepCheck.Value, 'StatStepCheck.Format': StatStepCheck.Format,
                      'StatStepCheck.MeasureValue': StatStepCheck.MeasureValue,
                      'StatStepCheck.Minimum': StatStepCheck.Minimum, 'StatStepCheck.Maximum': StatStepCheck.Maximum})


    return IsReadSuccessful, IsCheckSuccessful, VciResult
