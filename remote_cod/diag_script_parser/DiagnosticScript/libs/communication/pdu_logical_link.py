from ..util.doip.doip_channel import DoipChannel
from ..util.neusar_net.neusar_net_channel import NeusarNetChannel
from ..util.diag_channel.diagnostic_channel import DiagChannel
from enum import IntEnum
from threading import Timer
import os
import sys
import traceback
import importlib
import json
from ..appAPI.log_info import handler_exception_decorator,MSGLogger,IS_SECURE_STARTUP,PATHCONFIG,GetDiagResultInstance

sys.path[0]=(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

class FrameType(IntEnum):
    CAN = 0
    CANFD = 1
    DOIP = 2

class StartStopType(IntEnum):
    STOP = 0
    START = 1

TestPresentRequestFrameType = None
TestPresentRequestID = None
TestPresentResponseID = None
TestPresentPeriod = None
TestPresentIsRunning = None
TestPresentTimer = None
if IS_SECURE_STARTUP:
    __engineTime = PATHCONFIG.GetStringValue("EngineParameterPath")
else:
    path = os.getenv("NEUSAR_DEPLOYMENT_PATH", "")
    __engineTime = f"{path}/opt/DiagnosticScript/engineTimeParameter/engineTimeParameter.json"
__channel = None
__ip = None
__port = None
__local_sockaddr = None
__communication = None

def GetengineTime(path:str):
    p2clienttime = 500
    p2sclienttime = 3000
    if os.path.exists(path):
        with open(path,"r") as fd:
            try:
                infos = json.load(fd)
                if "diagEngine" in infos.keys():
                    times = infos["diagEngine"]
                    if "p2Client" in times.keys() and "p2StarClient" in times.keys():
                        p2time = times["p2Client"]
                        p2stime = times["p2StarClient"]
                        if type(p2time) == int and type(p2stime) == int:
                            p2clienttime = p2time
                            p2sclienttime = p2stime
                        else:
                            MSGLogger.error("GetengineTime:engineTimeParameter.json value error")
                    else:
                        MSGLogger.error("GetengineTime:engineTimeParameter.json key error")
                else:
                    MSGLogger.error("GetengineTime:engineTimeParameter.json key error")
            except:
                MSGLogger.error("GetengineTime:engineTimeParameter.json format error")
    else:
        MSGLogger.error("GetengineTime:engineTimeParameter.json not exist")
    return p2clienttime,p2sclienttime

@handler_exception_decorator(length = 2)
def PDUOpenChannel(p2_client_max = 500, p2_star_client_max = 3000):
    global __ip
    global __port
    global __local_sockaddr
    global __communication
    __ip, __port, __local_sockaddr, __communication = PDUGetConfig()
    p2_client_max, p2_star_client_max = GetengineTime(__engineTime)
    PDUCreateComLogicalLink()
    if PDUConnect():
        PDUSetTimeout(p2_client_max, p2_star_client_max)
        return True,id(__channel)
    else:
        MSGLogger.error('error:Channel establishment failed')
        GetDiagResultInstance().SetConnectStatus("Fail")
        raise ConnectionError(255)
        # return False, -1
@handler_exception_decorator(length = 1)
def PDUCloseChannel(handle):
    global __channel
    if handle == None or type(handle) != int:
        MSGLogger.error('InputParam errors')
        return False
    elif id(__channel) != handle:
        MSGLogger.error('InputParam errors')
        return False
    return PDUDisconnect(handle)

def PDUGetConfig():
    conparentpath = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
    configfp = open(os.path.join(conparentpath,"libs","util","communicationConfig.json"),"r")
    configobj = json.load(configfp)
    targetIP = configobj["target_ip"]
    targetPort = configobj["target_port"]
    local_sockaddr = configobj["target_add"]
    # local_sock = configobj["local_socket"]
    communication = configobj["communication"]
    return targetIP, targetPort, local_sockaddr, communication

def PDUCreateComLogicalLink():
    global __channel
    if __communication == "uds_channel":
        __channel = DiagChannel()
    elif __communication == "local":
        __channel = NeusarNetChannel(__local_sockaddr)
    else:
        __channel = DoipChannel(__ip, __port)

def PDUGetUniqueRespIdTable():
    pass

def PDUGetComParam():
    return __channel.getComParam()

def PDUSetComParam(parameters):
    return __channel.setComParam(parameters)

def PDUSetUniqueRespIdTable():
    pass

def PDUConnect():
    global __channel
    return __channel.start()

def PDUGetRawMessage():
    return __channel.getRawPdu()

# def PDUStartComPrimitive(pdu, HanderResponseRequest, resTable):
#     global __channel
#     return __channel.startComPrimitive(pdu, HanderResponseRequest, resTable)

def PDUStartComPrimitives(pdu, RequestFrameType, ResponseID, RequestID, sendAndRecv):
    global __channel
    return __channel.startComPrimitives(pdu, RequestFrameType, ResponseID, RequestID, sendAndRecv)

def PDUDisconnect(handle):
    global __channel
    if id(__channel) == handle:
        flag = __channel.stop()
        if flag:
            __channel = None
            return True
        else:
            return False
    else:
        if __channel == None:
            return True
        else:
            MSGLogger.error('Close Channel exception')
            return False

def PDUDestroyComLogicalLink():
    global __channel
    __channel.stop()
    __channel = None

def PDURegisterEventCallback(callback):
    global __channel
    __channel.setCallbackClass(callback)

def PDUSetTimeout(p2_client_max, p2_star_client_max):
    global __channel
    __channel.setTimeout(p2_client_max, p2_star_client_max)

def NewTestPresentTimer(function):
    global TestPresentTimer
    TestPresentTimer = Timer(TestPresentPeriod, function)

def StopSendTestPresent():
    global TestPresentTimer
    global TestPresentIsRunning
    if TestPresentIsRunning:
        TestPresentIsRunning = False
        TestPresentTimer.cancel()
    return True

def RepeatSendTestPresentWithoutResponse():
    if TestPresentIsRunning:
        res = PDUStartComPrimitives("3e80", TestPresentRequestFrameType, TestPresentResponseID, TestPresentRequestID, False)
        if not res[0]:
            MSGLogger.error('error:TransmitRequestFrameFailed')
        NewTestPresentTimer(RepeatSendTestPresentWithoutResponse)
        TestPresentTimer.start()
    return

def RepeatSendTestPresentWithResponse():
    if TestPresentIsRunning:
        res = PDUStartComPrimitives("3e00", TestPresentRequestFrameType, TestPresentResponseID, TestPresentRequestID, True)
        if not res[0]:
            MSGLogger.error('error:TransmitRequestFrameFailed')
        elif res[1]:
            MSGLogger.error('error:ResponseTimeOut')
        NewTestPresentTimer(RepeatSendTestPresentWithResponse)
        TestPresentTimer.start()
    return

def TransmitMessage(RequestID, RequestFrameType, RequestFrame):
    IsRequestSuccess = False
    ErrorMessage = ""
    RequestFramelist = ["0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"]
    if not RequestID:
        ErrorMessage = "InalidRequestID"
    elif RequestFrameType != FrameType.CAN and RequestFrameType != FrameType.CANFD and RequestFrameType != FrameType.DOIP:
        ErrorMessage = "InalidFrameType"
    elif not RequestFrame:
        ErrorMessage = "InalidRequestFrame"
    elif len(RequestFrame)%2 :
        ErrorMessage = "InalidRequestFrame"
    elif any(R.upper() not in RequestFramelist for R in RequestFrame):
        ErrorMessage = "InalidRequestFrame"
    else:
        res = 0
        if RequestFrameType == FrameType.DOIP:
            if RequestID == 0 or RequestID == "":
                ErrorMessage = "InalidRequestID"
                IsRequestSuccess = False
            else:
                res = PDUStartComPrimitives(RequestFrame, RequestFrameType, None, RequestID, False)
        if not res[0] and (RequestID != 0 and RequestID != ""):
            ErrorMessage = "TransmitRequestFrameFailed"
        else:
            IsRequestSuccess = True
    return IsRequestSuccess, ErrorMessage
__globalrep__DiagScript_Bool_RepeatRequest = True
__globalrep__DiagScript_Int_RepeatRequestTimes = 3
__globalrep__getinfo_flag = False 
def RepeatRequest():
    global __globalrep__DiagScript_Bool_RepeatRequest
    global __globalrep__DiagScript_Int_RepeatRequestTimes
    global __globalrep__getinfo_flag
    target_path = ""
    find_success = False
    stack=traceback.extract_stack()
    file_name = ""
    for ele in range(stack.__len__()):
        file = stack[ele].filename
        if file.find("temporary")!=-1:
            target_path = os.path.dirname(file)
            for pyfile in os.listdir(target_path):
                if pyfile.find("PrepareFile.py") != -1:
                    file_name = pyfile.split(".py")[0]
                    find_success = True
                    break
            if find_success:
                break
        if file.find("fixed")!=-1:
            break
    try:
        if find_success:
            sys.path.append(target_path)
            modul = importlib.import_module(file_name)
            __globalrep__DiagScript_Bool_RepeatRequest = modul._DiagScript_Bool_RepeatRequest
            __globalrep__DiagScript_Int_RepeatRequestTimes = modul._DiagScript_Int_RepeatRequestTimes
            __globalrep__getinfo_flag = True
        else:
            MSGLogger.error("Find not AXX_PrepareFile.py")
            conparentpath = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
            configfp = open(os.path.join(conparentpath,"libs","util","communicationConfig.json"),"r")
            configobj = json.load(configfp)
            configfp.close()
            __globalrep__DiagScript_Bool_RepeatRequest = configobj["need_repeat"]
            __globalrep__DiagScript_Int_RepeatRequestTimes = configobj["default_time"]
            __globalrep__getinfo_flag = True
    except:
        MSGLogger.critical(traceback.format_exc())
        __globalrep__DiagScript_Bool_RepeatRequest = True
        __globalrep__DiagScript_Int_RepeatRequestTimes = 3
        __globalrep__getinfo_flag = True
def TransmitAndReceive(RequestID, ResponseID, RequestFrameType, RequestFrame, TargetResponseFrame, LeadingByteLength):
    IsRequestSuccess = False
    IsNRC = False
    IsCheckSuccess = False
    ResponseFrame = b''
    ErrorMessage = ""
    RequestFramelist = ["0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"]
    global __globalrep__DiagScript_Bool_RepeatRequest
    global __globalrep__DiagScript_Int_RepeatRequestTimes
    global __globalrep__getinfo_flag
    if not RequestID:
        ErrorMessage = "InalidRequestID"
    elif RequestFrameType != FrameType.CAN and RequestFrameType != FrameType.CANFD and RequestFrameType != FrameType.DOIP:
        ErrorMessage = "InalidFrameType"
    elif not RequestFrame:
        ErrorMessage = "InalidRequestFrame"
    elif len(RequestFrame)%2 :
        ErrorMessage = "InalidRequestFrame"
    elif any(R.upper() not in RequestFramelist for R in RequestFrame):
        ErrorMessage = "InalidRequestFrame"
    else:
        remain_repet_time = 0
        if not __globalrep__getinfo_flag:
            RepeatRequest()
        if __globalrep__DiagScript_Bool_RepeatRequest:
            remain_repet_time = __globalrep__DiagScript_Int_RepeatRequestTimes -1
        while True:
            res = PDUStartComPrimitives(RequestFrame, RequestFrameType, ResponseID, RequestID, True)
            if not res[0]:
                ErrorMessage = "TransmitRequestFrameFailed"
                break
            elif res[1]:
                if __globalrep__DiagScript_Bool_RepeatRequest and remain_repet_time>0:
                    remain_repet_time -= 1
                    MSGLogger.debug(RequestFrame+" repeat request!")
                    continue
                else:
                    ErrorMessage = "ResponseTimeOut"
                    IsRequestSuccess = True
                    break
            else:
                udsMessage = res[2]._result
                udsMessageStr =  udsMessage.hex()
                if udsMessageStr[0:2] == "7f" and udsMessageStr[2:4] != "78":
                    ErrorMessage = "NegtiveResponse"
                    IsRequestSuccess = True
                    IsNRC = True
                    IsCheckSuccess = False
                    ResponseFrame = udsMessage
                elif len(udsMessageStr) < LeadingByteLength:
                    ErrorMessage = "ResponseTooShort"
                    IsRequestSuccess = True
                    IsNRC = False
                    IsCheckSuccess = False
                    ResponseFrame = udsMessage
                elif udsMessageStr[0:LeadingByteLength].lower() != TargetResponseFrame.lower():
                    ErrorMessage = "ResponseNotMatch"
                    IsRequestSuccess = True
                    IsNRC = False
                    IsCheckSuccess = False
                    ResponseFrame = udsMessage
                else:
                    ErrorMessage = ""
                    IsRequestSuccess = True
                    IsNRC = False
                    IsCheckSuccess = True
                    ResponseFrame = udsMessage
                break
    return IsRequestSuccess, IsNRC, IsCheckSuccess, ResponseFrame, ErrorMessage

def FunctionalTransmitMessage(RequestFrameType, RequestFrame):
    IsRequestSuccess = False
    ErrorMessage = ""
    if RequestFrameType != FrameType.CAN and RequestFrameType != FrameType.CANFD and RequestFrameType != FrameType.DOIP:
        ErrorMessage = "InalidFrameType"
    elif not RequestFrame:
        ErrorMessage = "InalidRequestFrame"
    else:
        res = 0
        if RequestFrameType == FrameType.DOIP:
            res = PDUStartComPrimitives(RequestFrame, RequestFrameType, None, None, False)
        if not res[0]:
            ErrorMessage = "TransmitRequestFrameFailed"
        else:
            IsRequestSuccess = True
    return IsRequestSuccess, ErrorMessage

def TestPresent(RequestID, ResponseID, RequestFrameType, StartOrStop, IsResponseSuppressable, Period):
    IsSuccess = False
    ErrorMessage = ""
    global TestPresentRequestFrameType
    global TestPresentRequestID
    global TestPresentResponseID
    global TestPresentPeriod
    global TestPresentIsRunning
    if not RequestID:
        ErrorMessage = "InalidRequestID"
    elif RequestFrameType != FrameType.CAN and RequestFrameType != FrameType.CANFD and RequestFrameType != FrameType.DOIP:
        ErrorMessage = "InalidFrameType"
    elif StartOrStop != StartStopType.START and StartOrStop != StartStopType.STOP:
        ErrorMessage = "InalidStartStopType"
    elif StartOrStop == StartStopType.START:
        TestPresentRequestFrameType = RequestFrameType
        TestPresentRequestID = RequestID
        TestPresentResponseID = ResponseID
        TestPresentPeriod = Period / 1000
        if IsResponseSuppressable:
            res = PDUStartComPrimitives("3e80", RequestFrameType, ResponseID, RequestID, False)
            if not res[0]:
                ErrorMessage = "TransmitRequestFrameFailed"
            else:
                if Period != 0:
                    NewTestPresentTimer(RepeatSendTestPresentWithoutResponse)
                    TestPresentTimer.start()
                    TestPresentIsRunning = True
                IsSuccess = True
        else:
            res = PDUStartComPrimitives("3e00", RequestFrameType, ResponseID, RequestID, True)
            if not res[0]:
                ErrorMessage = "TransmitRequestFrameFailed"
            elif res[1]:
                ErrorMessage = "ResponseTimeOut"
            else:
                IsSuccess = True
                if Period != 0:
                    NewTestPresentTimer(RepeatSendTestPresentWithResponse)
                    TestPresentTimer.start()
                    TestPresentIsRunning = True
    else:
        if not StopSendTestPresent():
            ErrorMessage = "CannotStopTestPresent"
            IsSuccess = False
        else:
            IsSuccess = True
    return IsSuccess, ErrorMessage
