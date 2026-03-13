from typing import Tuple

from .LocalVariable import StatStep,ecuDtObj
from .log_info import handler_exception_decorator,MSGLogger
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,utilEcuIDFile,DealASCIIToChar,DealStrToASCII

from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P
from .GetSection import GetSectionValue_P
            
# 读取 Bit 并比对范围
def ReadCompareID_P(EcuDtObj,DID:str,IsCompareID:bool,TargetID:str,StatStepRead:StatStep,StatStepCheck:StatStep)->Tuple[str,bool,bool,str]:
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if DID == None or type(DID) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if IsCompareID == None:
        IsCompareID = False
    if type(IsCompareID) != bool:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if type(TargetID) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
        if type(StatStepCheck) == StatStep:
            StatStepCheck.Result = "NOK"
    if TargetID == None:
        TargetID = ""
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadCompareID: StatStepRead is error")
    if StatStepCheck == None or type(StatStepCheck) != StatStep:
        MSGLogger.error("ReadCompareID: StatStepCheck is error")
    isStatStepRead: bool = isinstance(StatStepRead, StatStep)
    isStatStepCheck: bool = isinstance(StatStepCheck, StatStep)
    IsReadSuccessful=False
    IsCheckSuccessful=False
    VciResult=''
    if not TargetID :
        VehicleData:DbVehicleData = DbVehicleData(utilVehicleDataFile())
        lowerProjectName=VehicleData.getPROJECTNAME().lower()
        lowerEcuName=EcuDtObj.Name.lower()
        #拼接字符串Section=ProjectName_ECU.Name_XX,当DID=F187时，XX=id;当DID=F189时，XX=swid;当DID=F17F时，XX=hwid;
        Section=lowerProjectName+'_'+lowerEcuName+'_'
        if DID.upper()=='F187':
            Section+='id'
        elif DID.upper()=='F189':
            Section+='swid'
        elif DID.upper()=='F17F':
            Section+='hwid'
        #将Section转为小写
        Section=Section.lower()
        MTOCValue=VehicleData.getMTOC()
        #调用GetSection函数从版本号ini文件中提取Section对应的TargetID
        if utilEcuIDFile():
            TargetID=GetSectionValue_P(utilEcuIDFile(),Section,MTOCValue)
        MSGLogger.debug({"Section":Section,"MTOCValue":MTOCValue,"TargetID":TargetID})
    if TargetID != "":
        TargetID=TargetID.strip()
    asciiTargetID = DealStrToASCII(TargetID)
    #TargetID转为ASCII格式，赋值给TargetID
    # TargetID=DealStrToASCII(TargetID)
    #设置局部变量RequestString=“22”+DID
    RequestString='22'+DID
    #设置局部变量TargetResponseString=“62”+DID+TargetID
    TargetResponseString='62'+DID+asciiTargetID
    #打印局部变量ECU，RequestString，TargetResponseString
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数
    [IsReadSuccessful,IsCheckSuccessful,VciResult]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,StatStepRead,StatStepCheck)
    if isStatStepRead:
        MSGLogger.debug({'StatStepRead.Value': StatStepRead.Value})
    if isStatStepCheck:
        MSGLogger.debug({'StatStepCheck.Value': StatStepCheck.Value})
    MSGLogger.debug({'IsReadSuccessful': IsReadSuccessful, 'IsCheckSuccessful': IsCheckSuccessful,
              'VciResult': VciResult})
    ResultID:str=''
    if IsReadSuccessful:
        #VciResultRead取掉SID和DID，转换为ASCII码对应的字符，赋值为ResultID
        l = RequestString.__len__()
        if VciResult.__len__()>l:
            ResultID=VciResult[l:]
            ResultID = DealASCIIToChar(ResultID)
            # ResultID=binascii.a2b_hex(ResultID).decode("utf8")
            if isStatStepRead:
                StatStepRead.Result = 'OK'
                StatStepRead.MeasureValue = ResultID
                StatStepRead.SetValue = TargetID
                StatStepRead.Format = 'HEX'
                StatStepRead.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
        else:
            if isStatStepRead:
                StatStepRead.Result = 'OK'
                StatStepRead.MeasureValue = ResultID
                StatStepRead.SetValue = TargetID
                StatStepRead.Format = 'HEX'
                StatStepRead.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    else:
        ResultID=''
        if isStatStepRead:
            StatStepRead.Result = 'NOK'
            StatStepRead.MeasureValue = ResultID
            StatStepRead.SetValue = TargetID
            StatStepRead.Format = 'HEX'
            StatStepRead.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
        if isStatStepCheck:
            StatStepCheck.Result = 'NOK'
            StatStepCheck.MeasureValue = ResultID
            StatStepCheck.SetValue = TargetID
            StatStepCheck.Format = 'HEX'
            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    # MSGLogger.debug({'ResultID':ResultID})
     #IsCheckID==True？ 变量从哪来？ errno 是不是IsCompareID
    if not IsCompareID==True:
        #IsCheckSuccessful=true，StatStepCheck.Value=”OK”+@ECU.Name+“ Match Response”
        if isStatStepCheck:
            StatStepCheck.Result = 'OK'
            StatStepCheck.MeasureValue = ResultID
            StatStepCheck.SetValue = TargetID
            StatStepCheck.Format = 'ASCII'
            StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
    else:
        if IsCheckSuccessful:
            if isStatStepCheck:
                if ResultID == TargetID:
                    StatStepCheck.Result = 'OK'
                else:
                    StatStepCheck.Result = 'NOK'
                StatStepCheck.MeasureValue = ResultID
                StatStepCheck.SetValue = TargetID
                StatStepCheck.Format = 'ASCII'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
        else:
            if isStatStepCheck:
                StatStepCheck.Result = 'NOK'
                StatStepCheck.MeasureValue = ResultID
                StatStepCheck.SetValue = TargetID
                StatStepCheck.Format = 'ASCII'
                StatStepCheck.Value = 'REQ=' + RequestString + ';RESP=' + VciResult
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
    return [ResultID,IsReadSuccessful,IsCheckSuccessful,VciResult]

@handler_exception_decorator(length=4)
def ReadCompareID(EcuDtObj,DID:str,IsCompareID:bool,TargetID:str,StatStepRead,StatStepCheck)->Tuple[str,bool,bool,str]:
    return ReadCompareID_P(EcuDtObj,DID,IsCompareID,TargetID,StatStepRead,StatStepCheck)
    
