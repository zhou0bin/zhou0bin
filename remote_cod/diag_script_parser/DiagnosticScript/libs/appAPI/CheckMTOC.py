from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,ConvertStrASCII,DealASCIIToChar

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

#检查 MTOC
@handler_exception_decorator(length=3)
def CheckMTOC(EcuDtObj,MtocValue:str,StatStepRead:StatStep,StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    # if type(MtocValue) != str or MtocValue == None: 
    #     if type(StatStepRead) == StatStep:
    #         StatStepRead.Result = "NOK"
    #     if type(StatStepCheck) == StatStep:
    #         StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("CheckConfig: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("CheckConfig: StatStepCheck is error")
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    #定义局部变量DID=F102
    DID='F102'
    #获取局部变量MTOCValue
    MtocValue=MtocValue.strip()   
    if len(MtocValue)==0:
        # 从车辆数据中获取MTOC值
        VehicleData=DbVehicleData(utilVehicleDataFile())
        MtocValue=VehicleData.getMTOC()
    #后面拼接-00000..,总长度为17个字符，转为ASCII格式，赋值给MTOCValue
    if MtocValue.__len__()<17:
        MtocValue=MtocValue+'-'
        MtocValue=MtocValue.ljust(17,'0')
    MSGLogger.debug({'MtocValue':MtocValue})
    #转为ASCII格式，赋值给MTOCValue ?
    MtocValue=ConvertStrASCII(MtocValue)
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID++MTOCValue
    TargetResponseString='62'+DID+MtocValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    IsReadSuccessful,IsCheckSuccessful,VciResultRead =ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    if isinstance(StatStepRead, StatStep):
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isinstance(StatStepCheck, StatStep):
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if IsReadSuccessful:
            StatStepCheck.MeasureValue = DealASCIIToChar(StatStepCheck.MeasureValue[len(RequestString):])
        StatStepCheck.SetValue = DealASCIIToChar(StatStepCheck.SetValue[len(RequestString):])
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful, 'VciResult': VciResultRead})
    return [IsReadSuccessful,IsCheckSuccessful,VciResultRead]
