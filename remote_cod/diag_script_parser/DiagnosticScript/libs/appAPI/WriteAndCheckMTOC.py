from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile, ConvertStrASCII,DealASCIIToChar

#写入并检查 MTOC 码
@handler_exception_decorator(length=5)
def WriteAndCheckMTOC(EcuDtObj, MtocValue:str, StatStepWrite:StatStep, StatStepRead:StatStep, StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("WriteAndCheckMTOC: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("WriteAndCheckMTOC: StatStepCheck is error")
    if StatStepWrite == None or type(StatStepWrite) != StatStep:
        MSGLogger.error("WriteAndCheckMTOC: StatStepWrite is error")

    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsWriteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VciResultWrite=''
    #定义局部变量DID=F102
    DID='F102'
    if not MtocValue:
        #从车辆数据中获取MTOC值
        DbVehicle = DbVehicleData(utilVehicleDataFile())
        MtocValue = DbVehicle.getMTOC()
    #转为小写，例如KT1A-0W8-00，后面拼接-00000..,总长度为17个字符，转为ASCII格式，赋值给MTOCValue
    #MtocValue = MtocValue.lower()
    if MtocValue.__len__()<17:
        MtocValue=MtocValue+'-'
        MtocValue=MtocValue.ljust(17,'0')
    MtocValue = ConvertStrASCII(MtocValue)
    MSGLogger.debug({'MtocValue':MtocValue})
    #设置局部变量
    RequestString='2E'+DID+MtocValue
    TargetResponseString='6E'+DID
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsWriteSuccessful, IsWriteSuccessful, VciResultWrite = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepWrite,StatStepWrite)

    #打印StatStepWrite.Value，IsWriteSuccessful，VciResultWrite
    if isStatStepWrite:
        MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value, 'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})
    else:
        MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})

    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+MTOCValue
    TargetResponseString='62'+DID+MtocValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsReadSuccessful, IsCheckSuccessful, VciResultRead = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepRead, StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if IsReadSuccessful:
            StatStepCheck.MeasureValue = DealASCIIToChar(StatStepCheck.MeasureValue[len(RequestString):])
        StatStepCheck.SetValue = DealASCIIToChar(StatStepCheck.SetValue[len(RequestString):])
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResultRead': VciResultRead})

    return IsWriteSuccessful, IsReadSuccessful, IsCheckSuccessful, VciResultWrite, VciResultRead