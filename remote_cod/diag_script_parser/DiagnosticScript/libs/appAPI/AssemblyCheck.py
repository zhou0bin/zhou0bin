from libs.appAPI.LocalVariable import StatStep,ecuDtObj
from .ReadCompareID import ReadCompareID_P
from .log_info import handler_exception_decorator,MSGLogger

#检查零件号，软硬件版本号
@handler_exception_decorator(length=9)
def AssemblyCheck (EcuDtObj, StatStepCheckID, StatStepCheckSWID, StatStepCheckHWID):
    if type(EcuDtObj) != ecuDtObj or EcuDtObj == None:
        if type(StatStepCheckID) == StatStep:
            StatStepCheckID.Result = "NOK"
        if type(StatStepCheckSWID) == StatStep:
            StatStepCheckSWID.Result = "NOK"
        if type(StatStepCheckHWID) == StatStep:
            StatStepCheckHWID.Result = "NOK"
    if StatStepCheckID == None or type(StatStepCheckID) != StatStep:
        MSGLogger.error("AssemblyCheck: StatStepCheckID is error")
    if StatStepCheckSWID == None or type(StatStepCheckSWID) != StatStep:
        MSGLogger.error("AssemblyCheck: StatStepCheckSWID is error")
    if StatStepCheckHWID == None or type(StatStepCheckHWID) != StatStep:
        MSGLogger.error("AssemblyCheck: StatStepCheckHWID is error")
    #设置局部变量DID=F187
    DID = 'F187'
    ID = ''
    IsCompareID=True
    TargetID=''
    IsIDOK=False
    VciResultID=''
    #调用ReadCompareID函数
    ID, IsIDOK, IsIDOK, VciResultID = ReadCompareID_P (EcuDtObj, DID, IsCompareID, TargetID, StatStepCheckID, StatStepCheckID)
    #打印StatStepCheckID.Value，ID，IsIDOK，VciResultID
    if isinstance(StatStepCheckID,StatStep):
        MSGLogger.debug({'StatStepCheckID.Value':StatStepCheckID.Value, 'ID':ID, 'IsIDOK':IsIDOK, 'VciResultID':VciResultID})
    else:
        MSGLogger.debug({'ID':ID, 'IsIDOK':IsIDOK, 'VciResultID':VciResultID})
    DID = 'F189'
    SWID = ''
    IsCompareID=True
    TargetID=''
    IsSWIDOK=False
    VciResultSWID=''
    #调用ReadCompareID函数
    SWID, IsSWIDOK, IsSWIDOK, VciResultSWID = ReadCompareID_P (EcuDtObj, DID, IsCompareID, TargetID, StatStepCheckSWID, StatStepCheckSWID)
    if isinstance(StatStepCheckSWID,StatStep):
        MSGLogger.debug({'StatStepCheckSWID.Value':StatStepCheckSWID.Value, 'SWID':SWID, 'IsSWIDOK':IsSWIDOK, 'VciResultSWID':VciResultSWID})
    else:
        MSGLogger.debug({'SWID':SWID, 'IsSWIDOK':IsSWIDOK, 'VciResultSWID':VciResultSWID})
    DID = 'F17F'
    HWID = ''
    IsCompareID=True
    TargetID=''
    IsHWIDOK=False
    VciResultHWID=''
    #调用ReadCompareID函数
    HWID, IsHWIDOK, IsHWIDOK, VciResultHWID = ReadCompareID_P (EcuDtObj, DID, IsCompareID, TargetID, StatStepCheckHWID, StatStepCheckHWID)
    if isinstance(StatStepCheckHWID,StatStep):
        MSGLogger.debug({'StatStepCheckHWID.Value':StatStepCheckHWID.Value, 'HWID':HWID, 'IsHWIDOK':IsHWIDOK, 'VciResultHWID':VciResultHWID})
    else:
        MSGLogger.debug({'HWID':HWID, 'IsHWIDOK':IsHWIDOK, 'VciResultHWID':VciResultHWID})
    return ID, SWID, HWID, IsIDOK, IsSWIDOK, IsHWIDOK, VciResultID, VciResultSWID, VciResultHWID