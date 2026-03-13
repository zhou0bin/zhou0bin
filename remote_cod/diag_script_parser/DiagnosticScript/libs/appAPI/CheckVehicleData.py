from typing import Tuple

from .LocalVariable import StatStep,ecuDtObj

from .log_info import  handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#车辆数据比对函数定义
@handler_exception_decorator(length=3)
def CheckVehicleData(EcuDtObj,DID:str,VehicleDataSectionValue:str,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[bool,bool,str]:
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(DID) != str or DID == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(VehicleDataSectionValue) != str or VehicleDataSectionValue == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("CheckVehicleData: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("CheckVehicleData: StatStepCheck is error")
    isStatStepExist=False
    if isinstance(StatStepRead,StatStep) and isinstance(StatStepCheck,StatStep):
        isStatStepExist = True
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VehicleDataDB:DbVehicleData=DbVehicleData(utilVehicleDataFile())
    #从车辆数据中依据VehicleDataSectionValue获取车辆数据VehicleData
    if isinstance(VehicleDataSectionValue,str) and len(VehicleDataSectionValue)>0:
        VehicleData=VehicleDataDB.getKeyValue(VehicleDataSectionValue)
    else:
        VehicleData = ''
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+VehicleData
    TargetResponseString='62'+DID+VehicleData
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    IsReadSuccessful:bool
    IsCheckSuccessful:bool
    VciResultRead:str
    #调用ExcuteServiceAndCheckResponse函数
    [IsReadSuccessful,IsCheckSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    if isinstance(StatStepRead, StatStep):
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isinstance(StatStepCheck, StatStep):
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResult': VciResultRead})

    return [IsReadSuccessful,IsCheckSuccessful,VciResultRead]