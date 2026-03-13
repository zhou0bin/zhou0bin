
from .LocalVariable import StatStep,ecuDtObj

from .log_info import handler_exception_decorator,MSGLogger

from .utils.db_data import DbVehicleData
from .utils.util_helper import utilInitDataFile, utilVehicleDataFile,utilOptionFile 

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P
from .GetSection import GetSectionValue_P
from .GenerateConfigValue_tmp import GenerateConfigValue_p
import os
#写入并检查配置码
@handler_exception_decorator(length=5)
def WriteAndCheckConfig(EcuDtObj,DID:str,ConfigValue:str,StatStepWrite:StatStep,StatStepRead:StatStep,StatStepCheck:StatStep):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None: 
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("WriteAndCheckConfig: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("WriteAndCheckConfig: StatStepCheck is error")
    if StatStepWrite == None or type(StatStepWrite) != StatStep:
        MSGLogger.error("WriteAndCheckConfig: StatStepWrite is error")
    if type(DID) != str or DID == None:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if type(ConfigValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
        if type(StatStepWrite) == StatStep:
            StatStepWrite.Result = "NOK"
    if not ConfigValue:
        ConfigValue = ""
    ConfigValue=ConfigValue.strip()
    isStatStepWrite: bool = isinstance(StatStepWrite, StatStep)
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsWriteSuccessful=False
    IsCheckSuccessful=False
    VciResultRead=''
    VciResultWrite=''
    if not ConfigValue:
        try:
            vehicleData:DbVehicleData=DbVehicleData(utilVehicleDataFile())
            #从车辆数据中获取车型项目名称ProjectName
            lowerProjectName=vehicleData.getPROJECTNAME().lower()
            #获取ECU.Name,,转换为小写例如ccu
            lowerEcuName=str.lower(EcuDtObj.Name)
            #拼接字符串Section=ProjectName_ECU.Name_did+DID,，转为小写 
            Section=str.lower(lowerProjectName+'_'+lowerEcuName+'_did'+DID)   
            #init 文件位置固定
            #调用GetSection函数从配置码ini文件中提取Section对应的基础配置码
            mtoc:str=vehicleData.getMTOC()
            initFilePath=utilInitDataFile()
            BaseConfigValue=GetSectionValue_P(initFilePath,Section,mtoc)
            if not BaseConfigValue:
                Section=str(lowerProjectName+'_'+lowerEcuName+'_did'+DID.upper())
                BaseConfigValue=GetSectionValue_P(initFilePath,Section,mtoc)
            #依据基础配置码，车辆数据和选配规则文件计算选配配置码，赋值给ConfigValue 车辆LAS集合列表
            optionConfigFilePath=utilOptionFile()
            if not (os.path.exists(optionConfigFilePath) and optionConfigFilePath.endswith(".xml")):
                StatStepRead.Result = "NOK"
                StatStepCheck.Result = "NOK"
            lasList=vehicleData.getLAS()
            ConfigValue=GenerateConfigValue_p(lowerProjectName,lowerEcuName,DID.lower(),BaseConfigValue,optionConfigFilePath,lasList)
            if ConfigValue.strip() == BaseConfigValue.strip():
                ConfigValue = ""
        except:
            if len(ConfigValue) == 0:
                ConfigValue = ""
    #设置局部变量RequestString=“2E”+DID+ConfigValue
    RequestString='2E'+DID.upper()+ConfigValue.upper()
    #设置局部变量TargetResponseString=“6E”+DID
    TargetResponseString='6E'+DID.upper()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    [IsWriteSuccessful,IsWriteSuccessful,VciResultWrite]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepWrite,StatStepWrite)
    if isStatStepWrite:
        MSGLogger.debug({'StatStepWrite.Value':StatStepWrite.Value,'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})
    else:
        MSGLogger.debug({'IsWriteSuccessful':IsWriteSuccessful,'VciResultWrite':VciResultWrite})

    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID.upper()
    #设置局部变量TargetResponseString=“62”+DID+ConfigValue
    TargetResponseString='62'+DID.upper()+ConfigValue.upper()
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsReadSuccessful,IsCheckSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
        if IsReadSuccessful:
            StatStepCheck.MeasureValue = StatStepCheck.MeasureValue[len(RequestString):]
        StatStepCheck.SetValue = StatStepCheck.SetValue[len(RequestString):]
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'IsCheckSuccessful': IsCheckSuccessful, 'VciResultRead': VciResultRead})
    return [IsWriteSuccessful,IsReadSuccessful,IsCheckSuccessful,VciResultWrite,VciResultRead]