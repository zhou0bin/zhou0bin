from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,ConvertStrASCII,DealASCIIToChar

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#检查 VIN
@handler_exception_decorator(length=3)
def CheckVIN(EcuDtObj, VinValue:str, StatStepRead:StatStep, StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    # if type(VinValue) != str or VinValue == None: 
    #     if type(StatStepRead) == StatStep:
    #         StatStepRead.Result = "NOK"
    #     if type(StatStepCheck) == StatStep:
    #         StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("CheckVIN: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("CheckVIN: StatStepCheck is error")
    IsExcuteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    #定义局部变量DID=F190
    DID = 'F190'
    DbVehicle = DbVehicleData(utilVehicleDataFile())
    #VINValue为空字符串？
    if not VinValue:
        #从车辆数据中获取VIN值，转为大写后再转为ASCII格式，赋值给VINValue
        VinValue = DbVehicle.getVIN()

    #VINValue转为大写，后再转为ASCII格式，赋值给VINValue   
    VinValue = ConvertStrASCII(VinValue.upper())
    # print('正常： ', VinValue)
    #设置局部变量RequestString=“22”+DID
    RequestString = '22' + DID
    #设置局部变量TargetResponseString=“62”+DID+VINValue
    TargetResponseString = '62' + DID + VinValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj, 'RequestString':RequestString, 'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    IsExcuteSuccessful, IsCheckSuccessful, VciResultRead = ExcuteServiceAndCheckResponse_P(EcuDtObj, RequestString, TargetResponseString, StatStepRead, StatStepCheck)
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    if isinstance(StatStepRead, StatStep):
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isinstance(StatStepCheck, StatStep):
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if IsExcuteSuccessful:
            StatStepCheck.MeasureValue = DealASCIIToChar(StatStepCheck.MeasureValue[len(RequestString):])
        StatStepCheck.SetValue = DealASCIIToChar(StatStepCheck.SetValue[len(RequestString):])
    MSGLogger.debug({'IsExcuteSuccessful': IsExcuteSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResult': VciResultRead})
    return IsExcuteSuccessful, IsCheckSuccessful, VciResultRead