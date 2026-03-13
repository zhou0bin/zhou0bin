import json
import os
import threading
import logging
def SECURE_STARTUP():
    if os.path.exists("/data/SecureFlag"):
        return True
    else:
        return False
IS_SECURE_STARTUP = SECURE_STARTUP()
class PathGet(object):
    if IS_SECURE_STARTUP:
        _pathconfig_path = os.path.join(os.path.dirname(__file__),"SecureDiagParsingConfig.json")
    else:
        _pathconfig_path = os.path.join(os.path.dirname(__file__),"DiagParsingConfig.json")
    _instance_lock = threading.Lock()
    json_fd = None
    def __new__(cls,*args,**kwargs):
        if not hasattr(PathGet, "_instance"):
            with PathGet._instance_lock:
                if not hasattr(PathGet, "_instance"):
                    PathGet._instance = object.__new__(cls)
        return PathGet._instance
    def __init__(self):
        with open(PathGet._pathconfig_path,"r") as fd:
            PathGet.json_fd = json.load(fd)
    def GetStringValue(self,strkey):
        if strkey in PathGet.json_fd.keys():
            return PathGet.json_fd[strkey]
        else:
            return ""
PATHCONFIG = PathGet()
def PathConfirm():
    if not os.path.exists(PATHCONFIG.GetStringValue("DiagScriptPackagePath")):
        os.mkdir(PATHCONFIG.GetStringValue("DiagScriptPackagePath"))
    if not os.path.exists(PATHCONFIG.GetStringValue("DLogPath")):
        os.mkdir(PATHCONFIG.GetStringValue("DLogPath"))
class DiagMessageLog():
    _instance_me_lock = threading.Lock()
    _initflag = False
    def __init__(self):
        with DiagMessageLog._instance_me_lock:
            if not DiagMessageLog._initflag:
                self.callerinfo = {}#{"thread_id":{"function_name":"True or False"}}
                if IS_SECURE_STARTUP:
                    PathConfirm()
                    self.messagepath = PATHCONFIG.GetStringValue("MessageLogPath")
                else:
                    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
                    mesconfigobj = json.load(open(os.path.join(os.path.dirname(__file__),"DiagParsingConfig.json"),"r"))
                    self.messagepath = root + "/" + mesconfigobj["MessageLogPath"]
                
                if os.path.exists(self.messagepath):
                    os.remove(self.messagepath)
                # self.fd = open(messagepath,mode="a")
                self.logger = logging.getLogger("message_log")
                self.logger.setLevel(logging.DEBUG)
                loghd = logging.FileHandler(self.messagepath,encoding="utf-8")
                logformatter = logging.Formatter("[%(asctime)s]-[%(threadName)s-%(levelname)s-%(filename)s-%(funcName)s-%(lineno)d:]%(message)s")
                loghd.setFormatter(logformatter)
                self.logger.addHandler(loghd)
                DiagMessageLog._initflag = True
                if IS_SECURE_STARTUP:
                    self.logger.debug("SECURE STARTUP!")
    def __new__(cls, *args, **kwargs):
        if not hasattr(DiagMessageLog, "_instance"):
            with DiagMessageLog._instance_me_lock:
                if not hasattr(DiagMessageLog, "_instance"):
                    DiagMessageLog._instance = object.__new__(cls)
        return DiagMessageLog._instance

MSGLogger = DiagMessageLog().logger

class DiagPDULog(object):
    _instance_PDU_lock = threading.Lock()
    _isinit = False

    def __new__(cls, *args, **kwargs):
        if not hasattr(DiagPDULog, "_instance"):
            with DiagPDULog._instance_PDU_lock:
                if not hasattr(DiagPDULog, "_instance"):
                    DiagPDULog._instance = object.__new__(cls)
        return DiagPDULog._instance

    def __init__(self):
        with DiagPDULog._instance_PDU_lock:
            if not DiagPDULog._isinit:
                if IS_SECURE_STARTUP:
                    PathConfirm()
                    pdupath = PATHCONFIG.GetStringValue("PUDLogPath")
                else:
                    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
                    pduconfigobj = json.load(open(os.path.join(os.path.dirname(__file__),"DiagParsingConfig.json"),"r"))
                    pdupath = root + "/" + pduconfigobj["PUDLogPath"]
                if os.path.exists(pdupath):
                    os.remove(pdupath)
                self.logger = logging.getLogger("pdu_log")
                self.logger.setLevel(logging.DEBUG)
                loghd = logging.FileHandler(pdupath,encoding="utf-8")
                logformatter = logging.Formatter("[%(asctime)s4]-[%(threadName)s:%(filename)s:%(funcName)s:%(lineno)s]%(message)s")
                loghd.setFormatter(logformatter)
                self.logger.addHandler(loghd)
                DiagPDULog._isinit = True


PDULOGER = DiagPDULog().logger

class DiagResultLog(object):
    _instance_res_lock = threading.Lock()
    if IS_SECURE_STARTUP:
        PathConfirm()
        resultpath = PATHCONFIG.GetStringValue("DiagResultLogPath")
    else:
        root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
        # configobj = json.load(open(os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__),'DiagParsingConfig.json'),"r"))
        configobj = json.load(open(os.path.join(os.path.dirname(__file__),"DiagParsingConfig.json"),"r"))
        resultpath = root + "/" + configobj["DiagResultLogPath"]

    def __init__(self):
        pass

    def __new__(cls, *args, **kwargs):
        if not hasattr(DiagResultLog, "_instance"):
            with DiagResultLog._instance_res_lock:
                if not hasattr(DiagResultLog, "_instance"):
                    DiagResultLog._instance = object.__new__(cls)
                    if os.path.exists(DiagResultLog.resultpath):
                        os.remove(DiagResultLog.resultpath) 
        return DiagResultLog._instance
    def PrinDiagResult(self,data):
        file = open(DiagResultLog.resultpath,mode="a",encoding="utf-8")
        json.dump(data,file,indent=4,ensure_ascii=False,sort_keys=False)
        file.write("\n")
        file.close()



def PrinDiagResult(data):
    resultlog = DiagResultLog()
    if resultlog:
        resultlog.PrinDiagResult(data)



