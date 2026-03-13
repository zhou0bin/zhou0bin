from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,ConvertStrASCII,DealASCIIToChar

#写入并检查 VIN
@handler_exception_decorator(length=5)
def WriteAndCheckVIN(EcuDtObj,VinValue:str,StatStepWrite:StatStep,StatStepRead:StatStep,StatStepCheck:StatStep):
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
  
    if not VinValue:
        VinValue = ''
    #定义局部变量DID=F190

    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsWriteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VciResultWrite=''
    DID:str='F190'
    VinValue=VinValue.strip()
    if not VinValue:
        #从车辆数据中获取VIN值
        VehicleData:DbVehicleData=DbVehicleData(utilVehicleDataFile())
        VinValue=VehicleData.getVIN()
    #VINValue转为大写后转为ASCII格式
    VinValue=ConvertStrASCII(VinValue.upper())
    #设置局部变量RequestString=“2E”+DID+VINValue
    RequestString='2E'+DID+VinValue
    #设置局部变量TargetResponseString=“6E”+DID
    TargetResponseString='6E'+DID
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsWriteSuccessful,IsWriteSuccessful,VciResultWrite]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepWrite,StatStepWrite)
    #打印VciResultWrite.Value，IsWriteSuccessful，VciResultWrite
    if isStatStepWrite:
        MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value,'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})
    else:
        MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})

    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+VINValue
    TargetResponseString='62'+DID+VinValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    [IsReadSuccessful,IsCheckSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if IsReadSuccessful:
            StatStepCheck.MeasureValue = DealASCIIToChar(StatStepCheck.MeasureValue[len(RequestString):])
        StatStepCheck.SetValue = DealASCIIToChar(StatStepCheck.SetValue[len(RequestString):])
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    MSGLogger.debug(
        {'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResultRead': VciResultRead})
    return [IsWriteSuccessful,IsReadSuccessful,IsCheckSuccessful,VciResultWrite,VciResultRead]