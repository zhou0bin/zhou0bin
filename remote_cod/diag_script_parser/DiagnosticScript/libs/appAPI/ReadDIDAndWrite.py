from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#读取 DID 并写入
@handler_exception_decorator(length=5)
def ReadDIDAndWrite(SourceEcuDtObj, TragetEcuDtObj, SourceDID:str, TargetDID:str, StatStepRead:StatStep, StatStepWrite:StatStep, StatStepCheck:StatStep):
    if ecuDtObj != type(SourceEcuDtObj) or SourceEcuDtObj == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if ecuDtObj != type(TragetEcuDtObj) or TragetEcuDtObj == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if str != type(SourceDID) or SourceDID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if str != type(TargetDID) or TargetDID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadDIDAndWrite: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("ReadDIDAndWrite: StatStepCheck is error")
    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsWriteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VciResultWrite=''
    isTargetDID = False
    if isinstance(TargetDID,str) and len(TargetDID)>0:
        isTargetDID = True
    #依据局部变量SourceECU和TargetECU获取SourceECU和TargetECU的请求地址RequestID,回复地址ResponseID，请求类型FrameType
    #获取局部变量SourceDID和targetDID
    #设置局部变量RequestString=“22”+SourceDID
    RequestString= '22'+SourceDID
    #设置局部变量TargetResponseString=“62”+SourceDID
    TargetResponseString= '62'+SourceDID
    #打印局部变量SourceECU，TargetECU，RequestString，TargetResponseString
    MSGLogger.debug({'SourceEcuDtObj':SourceEcuDtObj, 'TragetEcuDtObj':TragetEcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数 ExcuteServiceAndCheckResponse(SourceECU,RequestString,TargetResponseString,StatStepRead，StatStepRead，IsReadSuccessful，IsReadSuccessful，VciResultRead)
    LeadingByteLength=TargetResponseString.__len__()
    IsReadSuccessful, IsReadSuccessful, VciResultRead = ExcuteServiceAndCheckLeadingResponse_P(SourceEcuDtObj, RequestString, TargetResponseString,LeadingByteLength, StatStepRead,StatStepRead)
    #打印VciResultRead.Value，IsReadSuccessful，VciResultRead
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value':StatStepRead.Value, 'IsReadSuccessful':IsReadSuccessful, 'VciResultRead':VciResultRead})
    else:
        MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful, 'VciResultRead':VciResultRead})
    #IsReadSuccessful==True?
    if IsReadSuccessful == True and isTargetDID:
        CofigValue = VciResultRead
        #VciResultRead去掉22和SourceDID，保存为局部变量CofigValue
        if "62" in CofigValue:
            CofigValue = CofigValue.replace('62','',1)
        if SourceDID in CofigValue:
            CofigValue = CofigValue.replace(SourceDID,'',1)
        #设置局部变量RequestString=“2E”+TargetDID+CofigValue
        RequestString = '2E'+TargetDID+CofigValue
        #设置局部变量TargetResponseString=“6E”+TargetDID
        TargetResponseString = '6E'+TargetDID
        #打印局部变量TargetECU，RequestString，TargetResponseString
        MSGLogger.debug({'TragetEcuDtObj':TragetEcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
        #调用ExcuteServiceAndCheckResponse函数 ExcuteServiceAndCheckResponse(TargetECU,RequestString,TargetResponseString,StatStepWrite，StatStepWrite，IsWriteSuccessful，IsWriteSuccessful，VciResultWrite)
        IsWriteSuccessful, IsWriteSuccessful, VciResultWrite = ExcuteServiceAndCheckResponse_P(TragetEcuDtObj, RequestString, TargetResponseString, StatStepWrite,StatStepWrite)
        #打印StatStepWrite.Value，IsWriteSuccessfulVciResultWrite
        if isStatStepWrite:
            MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value, 'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})
        else:
            MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})

        #设置局部变量RequestString=“22”+TargetDID
        RequestString='22'+TargetDID
        #设置局部变量TargetResponseString=“62”+TargetDID++CofigValue
        TargetResponseString='62'+TargetDID+CofigValue
        #打印局部变量TargetECU，RequestString，TargetResponseString
        MSGLogger.debug({'TragetEcuDtObj':TragetEcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
        #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(TargetECU,RequestString,TargetResponseString,StatStepCheck，StatStepCheck，IsCheckSuccessful，IsCheckSuccessful，VciResultRead)
        IsCheckSuccessful, IsCheckSuccessful, VciResultRead = ExcuteServiceAndCheckResponse_P(TragetEcuDtObj, RequestString, TargetResponseString, StatStepCheck,StatStepCheck)
        #打印StatStepCheck.Value，IsCheckSuccessfulVciResultRead
        if isStatStepCheck:
            MSGLogger.debug({'StatStepCheck.Value':StatStepCheck.Value, 'IsCheckSuccessful':IsCheckSuccessful, 'VciResultRead':VciResultRead})
        else:
            MSGLogger.debug({'IsCheckSuccessful':IsCheckSuccessful, 'VciResultRead':VciResultRead})

    else:
        #StatStepRead.Value=NOK@SourceECU.Name+”Read ”+SourceDID+”Failed” StatStepWrite.Value=NOK@SourceECU.Name+”Read ”+SourceDID+”Failed” StatStepCheck.Value=NOK@SourceECU.Name+”Read ”+SourceDID+”Failed”
        # IsWriteSuccessful=False IsCheckSuccessful=False VciResultRead=VciResultRead VciResultWrite=””
        if isStatStepRead:
            StatStepRead.Result = 'NOK'
            StatStepRead.MeasureValue = VciResultRead.upper()
            StatStepRead.SetValue = TargetResponseString.upper()
            StatStepRead.Format = 'HEX'
            StatStepRead.Value = 'REQ=' + RequestString + ';RESP=' + VciResultRead
        if isStatStepWrite:
            StatStepWrite.Result = 'NOK'
            StatStepWrite.MeasureValue = ''.upper()
            StatStepWrite.SetValue = ('6E'+ TargetDID).upper()
            StatStepWrite.Format = 'HEX'
            StatStepWrite.Value = 'REQ='+'2E'+TargetDID+';RESP='
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.MeasureValue = ''.upper()
            StatStepCheck.SetValue = ('62'+ TargetDID).upper()
            StatStepCheck.Format = 'HEX'
            StatStepCheck.Value = 'REQ='+'22'+TargetDID+';RESP='
        IsWriteSuccessful=False
        IsCheckSuccessful=False
        VciResultWrite=''

    MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful,'IsCheckSuccessful':IsCheckSuccessful,'VciResultRead':VciResultRead,'VciResultWrite':VciResultWrite})

    return IsReadSuccessful, IsWriteSuccessful,IsCheckSuccessful , VciResultRead, VciResultWrite
