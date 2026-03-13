from .log_info import handler_exception_decorator,MSGLogger
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from .LocalVariable import StatStep,ecuDtObj
from .utils.util_helper import DealASCIIToChar
@handler_exception_decorator(length=3)
def ReadAndUpLoadID(ECUDtObj,DID:str, StatStepRead):
    if ECUDtObj == None or type(ECUDtObj) != ecuDtObj:
        MSGLogger.error("ReadAndUpLoadID InputParam errors:ECUDtObj is error")
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if DID == None or type(DID) != str:
        MSGLogger.error("ReadAndUpLoadID InputParam errors:DID is error")
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("ReadAndUpLoadID InputParam errors:StatStepRead is error")
    DID = DID.lower()
    ResultID = ""
    IsExcuteSuccessful:bool = False
    RequestID = ECUDtObj.RequestID
    ResponseID = ECUDtObj.ResponseID
    FrameType = ECUDtObj.FrameType
    RequestString = "22" + DID
    TargetResponseString = "62" + DID
    LeadingByteLength=TargetResponseString.__len__()
    MSGLogger.debug({'ECU':ECUDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    [IsExcuteSuccessful,IsExcuteSuccessful,VciResult] = ExcuteServiceAndCheckLeadingResponse_P(ECUDtObj,RequestString,TargetResponseString,LeadingByteLength,StatStepRead,StatStepRead)
    if IsExcuteSuccessful:
        try:
            temp = VciResult[2:]
            lenth = len(DID)
            temp = temp[lenth:]
            ResultID = temp
            StatStepRead.SetValue = TargetResponseString.upper()
            StatStepRead.MeasureValue = ResultID
        except:
            MSGLogger.debug({'ECU':ECUDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    if isinstance(StatStepRead,StatStep):
        MSGLogger.debug({'StatStepRead.Value':StatStepRead.Value,'IsExcuteSuccessful':IsExcuteSuccessful,'VciResult':VciResult,"ResultID":ResultID})
    else:
        MSGLogger.debug(
            {'IsExcuteSuccessful': IsExcuteSuccessful, 'VciResult': VciResult,
             "ResultID": ResultID})
    return ResultID,IsExcuteSuccessful,VciResult