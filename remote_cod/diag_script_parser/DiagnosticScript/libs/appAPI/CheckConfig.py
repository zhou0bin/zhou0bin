from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,utilInitDataFile,utilOptionFile

from .GetSection import  GetSectionValue_P
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P
from .GenerateConfigValue_tmp import GenerateConfigValue_p
import os
#检查配置码    
@handler_exception_decorator(length=3)
def CheckConfig(EcuDtObj, DID:str, ConfigValue:str, StatStepRead:StatStep, StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("CheckConfig: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("CheckConfig: StatStepCheck is error")
    if type(DID) != str or DID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(ConfigValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    if(ConfigValue == None or (type(ConfigValue)==str and ConfigValue.strip() == "")):
        try:
            vehicleData:DbVehicleData=DbVehicleData(utilVehicleDataFile())
            #从车辆数据中获取车型项目名称ProjectName，转为小写 
            lowerProjectName=vehicleData.getPROJECTNAME().lower()
            #获取ECU.Name,,转换为小写例如ccu
            lowerEcuName=str.lower(EcuDtObj.Name)
            #拼接字符串Section=ProjectName_ECU.Name_did+DID,，转为小写 
            Section=str.lower(lowerProjectName+'_'+lowerEcuName+'_did'+DID)
            #init 文件位置固定？
            mtoc:str=vehicleData.getMTOC()
            BaseConfigValue=GetSectionValue_P(utilInitDataFile(),Section,mtoc)
            if not BaseConfigValue:
                Section=str(lowerProjectName+'_'+lowerEcuName+'_did'+DID.upper())
                BaseConfigValue=GetSectionValue_P(utilInitDataFile(),Section,mtoc)
            #依据基础配置码，车辆数据和选配规则文件计算选配配置码，赋值给ConfigValue 车辆LAS集合列表
            lasList=vehicleData.getLAS()
            optpath = utilOptionFile()
            if not (os.path.exists(optpath) and optpath.endswith(".xml")):
                StatStepRead.Result = "NOK"
                StatStepCheck.Result = "NOK"
            ConfigValue=GenerateConfigValue_p(lowerProjectName,lowerEcuName,DID,BaseConfigValue,optpath,lasList)
            if ConfigValue.strip() == BaseConfigValue.strip():
                ConfigValue = ""
        except:
            if len(ConfigValue) == 0:
                ConfigValue = ""
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+ConfigValue
    TargetResponseString='62'+DID+ConfigValue
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'ECU':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse
    #打印StatStepRead.Value，StatStepCheck.Value，IsReadSuccessful，IsCheckSuccessful，VciResultRead
    [IsReadSuccessful,IsCheckSuccessful,VciResultRead] =ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    if isinstance(StatStepRead,StatStep):
        MSGLogger.debug({'StatStepRead.Value':StatStepRead.Value})
    if isinstance(StatStepCheck,StatStep):
        MSGLogger.debug({'StatStepCheck.Value':StatStepCheck.Value})
        if IsReadSuccessful:
            StatStepCheck.MeasureValue = StatStepCheck.MeasureValue[len(RequestString):]
        StatStepCheck.SetValue = StatStepCheck.SetValue[len(RequestString):]
    MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'IsCheckSuccessful':IsCheckSuccessful,'VciResultRead':VciResultRead})
    return [IsReadSuccessful,IsCheckSuccessful,VciResultRead]