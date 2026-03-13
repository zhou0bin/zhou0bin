from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#车辆数据写入比对函数定义
@handler_exception_decorator(length=5)
def WriteAndCheckVehicleData(EcuDtObj, DID:str, VehicleDataSectionValue:str, StatStepWrite:StatStep, StatStepRead:StatStep, StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("WriteAndCheckVehicleData: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("WriteAndCheckVehicleData: StatStepCheck is error")
    if StatStepWrite == None or type(StatStepWrite) != StatStep:
        MSGLogger.error("WriteAndCheckVehicleData: StatStepWrite is error")
    if type(DID) != str or DID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if type(VehicleDataSectionValue) != str or VehicleDataSectionValue == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsWriteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VciResultWrite=''
    #从车辆数据中依据VehicleDataSectionValue获取车辆数据VehicleData
    DbVehicle = DbVehicleData(utilVehicleDataFile())
    if isinstance(VehicleDataSectionValue,str) and len(VehicleDataSectionValue)>0:
        VehicleData=DbVehicle.getKeyValue(VehicleDataSectionValue)
    else:
        VehicleData = ''
    #设置局部变量
    # VehicleData = ConvertStrASCII(VehicleData)
    VehicleData = VehicleData
    RequestString='2E'+DID+VehicleData
    TargetResponseString='6E'+DID
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsWriteSuccessful, IsWriteSuccessful, VciResultWrite = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepWrite, StatStepWrite)

    #StatStepWrite.Value，IsWriteSuccessful，VciResultWrite
    if isStatStepWrite:
        MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value, 'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})
    else:
        MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful, 'VciResultWrite':VciResultWrite})

    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+VehicleData
    TargetResponseString='62'+DID+VehicleData
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'Ecu':EcuDtObj, 'RequestString':RequestString, 'TargetResponseStrig':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsReadSuccessful, IsCheckSuccessful, VciResultRead = ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead, StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    MSGLogger.debug(
        {'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResultRead': VciResultRead})

    return IsWriteSuccessful, IsReadSuccessful, IsCheckSuccessful, VciResultWrite, VciResultRead