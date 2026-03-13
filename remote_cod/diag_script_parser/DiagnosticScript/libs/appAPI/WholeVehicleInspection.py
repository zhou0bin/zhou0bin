from .LocalVariable import StatStep,ecuDtObj
from .log_info import  handler_exception_decorator,MSGLogger,RecordStatStep_
from .ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse_P

stat_step_id = 0

@handler_exception_decorator(2)
def WholeVehicleInspection(
    EcuDtObjList: list[ecuDtObj]):
    IsReadSuccessful = False
    VciResultRead = ""
    # 生成StatStep
    for ecu in EcuDtObjList:
        try:
            [IsReadSuccessful, VciResultRead] = VehicleInspection(ecu)
        except:
            pass
    return IsReadSuccessful, VciResultRead

def GenerateStatStep(ecu_name: str):
    global stat_step_id
    stat_step_id = stat_step_id + 1
    StatStepExtendedSession = StatStep(stat_step_id,ecu_name,"Enter Extended Session","进入扩展诊断模式",True,False,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepVIN = StatStep(stat_step_id,ecu_name,"Read VIN","读取VIN",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepSystemSupplierIdentifier = StatStep(stat_step_id,ecu_name,"Read System Supplier Identifier","读取系统供应商标识符",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    PartNumberDataIdentifier_Read = StatStep(stat_step_id, ecu_name,"Read Part Number Identifier","读取零件号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    SystemNameDataIdentifier = StatStep(stat_step_id,ecu_name,"Read ECU Name Identifier","读ECU名",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    SoftwareVersionNumberDataIdentifier = StatStep(stat_step_id,ecu_name,"Read Software Version Identifier","读软件版本号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    DID_0xF19F = StatStep(stat_step_id,ecu_name,"Read Hardware Version Identifier","读硬件版本号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepGetDTCs = StatStep(stat_step_id, ecu_name, "Get DTCs","获取故障码",True,False,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepFaultMemoryRead = StatStep(stat_step_id,ecu_name,"Read Fault Memory","读取故障存储器",True,True,"object","","","StatStep",None,"1")
    return StatStepExtendedSession, StatStepVIN, StatStepSystemSupplierIdentifier, PartNumberDataIdentifier_Read,SystemNameDataIdentifier,SoftwareVersionNumberDataIdentifier,DID_0xF19F,StatStepGetDTCs,StatStepFaultMemoryRead

#写入并检查 VIN
@handler_exception_decorator(2)
def VehicleInspection(EcuDtObj:ecuDtObj):
    res = GenerateStatStep(EcuDtObj.Name)
    RequestString='1003'
    TargetResponseString='5003'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[0],res[0])
    MSGLogger.debug({'StatStepExtendedSession.Value':res[0].Value,'IsSessionSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[0].SetValue = "5003"
    if IsExcuteSuccessful:
        res[0].MeasureValue = VciResultRead[4:]
        if VciResultRead[0:4].upper() == "5003":
            res[0].Result = "OK"
            res[0].MeasureValue = VciResultRead[4:]
        else:
            res[0].Result = "NOK"
            res[0].MeasureValue = VciResultRead
    else:
        res[0].Result = "NOK"
        res[0].SetValue = VciResultRead

    RequestString='22f190'
    TargetResponseString='62f190'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[1], res[1])
    MSGLogger.debug({'StatStepVIN.Value':res[1].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[1].SetValue = "62F190"
    if IsExcuteSuccessful:
        res[1].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F190":
            res[1].Result = "OK"
            res[1].MeasureValue = VciResultRead[6:]
        else:
            res[1].Result = "NOK"
            res[1].MeasureValue = VciResultRead
    else:
        res[1].Result = "NOK"
        res[1].SetValue = VciResultRead

    RequestString='22f18a'
    TargetResponseString='62f18a'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[2],res[2])
    MSGLogger.debug({'StatStepSystemSupplierIdentifier.Value':res[2].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[2].SetValue = "62F18A"
    if IsExcuteSuccessful:
        res[2].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F18A":
            res[2].Result = "OK"
            res[2].MeasureValue = VciResultRead[6:]
        else:
            res[2].Result = "NOK"
            res[2].MeasureValue = VciResultRead
    else:
        res[2].Result = "NOK"
        res[2].SetValue = VciResultRead

    RequestString='22f187'
    TargetResponseString='62f187'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[3],res[3])
    MSGLogger.debug({'PartNumberDataIdentifier_Read.Value':res[3].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[3].SetValue = "62F187"
    if IsExcuteSuccessful:
        res[3].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F187":
            res[3].Result = "OK"
            res[3].MeasureValue = VciResultRead[6:]
        else:
            res[3].Result = "NOK"
            res[3].MeasureValue = VciResultRead
    else:
        res[3].Result = "NOK"
        res[3].SetValue = VciResultRead

    RequestString='22f197'
    TargetResponseString='62f197'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[4],res[4])
    MSGLogger.debug({'SystemNameDataIdentifier.Value':res[4].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[4].SetValue = "62F197"
    if IsExcuteSuccessful:
        res[4].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F197":
            res[4].Result = "OK"
            res[4].MeasureValue = VciResultRead[6:]
        else:
            res[4].Result = "NOK"
            res[4].MeasureValue = VciResultRead
    else:
        res[4].Result = "NOK"
        res[4].SetValue = VciResultRead

    RequestString='22f189'
    TargetResponseString='62f189'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[5],res[5])
    MSGLogger.debug({'DID_0xF19F.Value':res[5].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[5].SetValue = "62F189"
    if IsExcuteSuccessful:
        res[5].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F189":
            res[5].Result = "OK"
            res[5].MeasureValue = VciResultRead[6:]
        else:
            res[5].Result = "NOK"
            res[5].MeasureValue = VciResultRead
    else:
        res[5].Result = "NOK"
        res[5].SetValue = VciResultRead

    RequestString='22f089'
    TargetResponseString='62f089'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[6],res[6])
    MSGLogger.debug({'DID_0xF19F.Value':res[6].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultRead':VciResultRead})
    res[6].SetValue = "62F089"
    if IsExcuteSuccessful:
        res[6].MeasureValue = VciResultRead[6:]
        if VciResultRead[0:6].upper() == "62F089":
            res[6].Result = "OK"
            res[6].MeasureValue = VciResultRead[6:]
        else:
            res[6].Result = "NOK"
            res[6].MeasureValue = VciResultRead
    else:
        res[6].Result = "NOK"
        res[6].SetValue = VciResultRead

    RequestString='190201'
    TargetResponseString='5902'
    MSGLogger.debug({'EcuDtObj':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsReadSuccessful,VciResultRead]=ExcuteServiceAndCheckResponse_P(EcuDtObj,RequestString,TargetResponseString,res[7],res[7])
    MSGLogger.debug({'StatStepGetDTCs.Value':res[6].Value,'IsReadSuccessful':IsReadSuccessful,'VciResultSession':VciResultRead})
    res[7].SetValue = "5902"
    if IsExcuteSuccessful:
        res[7].MeasureValue = VciResultRead[4:]
        if VciResultRead[0:4].upper() == "5902":
            res[7].Result = "OK"
            res[7].MeasureValue = VciResultRead[4:]
        else:
            res[7].Result = "NOK"
            res[7].MeasureValue = VciResultRead
    else:
        res[7].Result = "NOK"
        res[7].SetValue = VciResultRead
    RecordStatStep_(res[0])
    RecordStatStep_(res[1])
    RecordStatStep_(res[2])
    RecordStatStep_(res[3])
    RecordStatStep_(res[4])
    RecordStatStep_(res[5])
    RecordStatStep_(res[6])
    RecordStatStep_(res[7])
    RecordStatStep_(res[8])
    return [IsReadSuccessful,VciResultRead]