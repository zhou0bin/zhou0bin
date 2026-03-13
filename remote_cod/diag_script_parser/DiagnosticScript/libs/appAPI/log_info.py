import traceback
import signal
# from LocalVariable import ecuDtObj,vciResult,LocalVariable
from .LocalVariable import ecuDtObj,StatStep
from ..util.DiagLog import MSGLogger,PrinDiagResult,PATHCONFIG,IS_SECURE_STARTUP
import inspect
from ..appAPI.LocalVariable import *
import threading
from enum import Enum
import json
import os
import collections
import time
StepResult=Enum('StepResult',('Ok','Nok','Cancel','None','Abort'))
PyPath = ""
MySiganl = False
Stop_Inter_Callback = {}
Inter_Lock = threading.Lock()
def RegistInteractCallback(types,function,arg):
    global Inter_Lock
    global Stop_Inter_Callback
    global MySiganl
    Inter_Lock.acquire()
    try:
        if not MySiganl:
            Stop_Inter_Callback.update({types:(function,arg)})
        MSGLogger.debug(Stop_Inter_Callback)
        Inter_Lock.release()
    except:
        if Inter_Lock.locked():
            Inter_Lock.release()
def DeleteRegist(types):
    global Inter_Lock
    global Stop_Inter_Callback
    Inter_Lock.acquire()
    try:
        if types in Stop_Inter_Callback.keys():
            MSGLogger.debug("DeleteRegist::Delete " + types)
            Stop_Inter_Callback.pop(types)
        Inter_Lock.release()
    except:
        if Inter_Lock.locked():
            Inter_Lock.release()
# def SignalHandler(a,b):
#     signal.signal(signal.SIGUSR1,signal.SIG_IGN)
#     global Inter_Lock
#     global MySiganl
#     global Stop_Inter_Callback
#     MSGLogger.debug("Receive Signal to Stop Runing")
#     Inter_Lock.acquire()
#     try:
#         MySiganl = True
#         if len(Stop_Inter_Callback) > 0:
#             for key,value in Stop_Inter_Callback.items():
#                 value[0](value[1])
#                 MSGLogger.debug("Stop A Interact:"+key)
#         Stop_Inter_Callback.clear()
#         if IS_SECURE_STARTUP:
#             stopflag = PATHCONFIG.GetStringValue("StopFlag")
#             if os.path.exists(stopflag):
#                 os.remove(stopflag)
#             with open(stopflag,mode="a") as fd:
#                 pass
#         else:
#             if os.path.exists("/tmp/asf_ds_stop_flag.txt"):
#                 os.remove("/tmp/asf_ds_stop_flag.txt")
#             with open("/tmp/asf_ds_stop_flag.txt",mode="a") as fd:
#                 pass
#         Inter_Lock.release()
#     except:
#         traceback.print_exc()
#         if Inter_Lock.locked():
#             Inter_Lock.release()
# signal.signal(signal.SIGUSR1,SignalHandler)

def GetScriptRoot():
    stack = traceback.extract_stack()
    stackfilepath = ""
    for i in range(stack.__len__()):
        file = stack[i].filename
        if file.find("temporary")!=-1 or file.find("fixed")!=-1:
            stackfilepath = os.path.dirname(traceback.extract_stack()[i].filename)
            break
    return stackfilepath

def GetInputInfos(realinput,realcount,defcount,definput,defaultinput):
    inputcount = realcount
    deal_inputs = []
    deal_defualt = []
    for i in range(defcount):
        inp = params()
        inp.param_name = definput[i]
        if len(realinput)!=0 and (i+1 <= inputcount):
            inp.param_value = realinput[i]
        else:
            inp.param_value = None
        deal_inputs.append(inp)
    if defaultinput:
        for i in range(inputcount-len(defaultinput)):
            deal_defualt.append(None)
        for i in range(len(defaultinput)):
            deal_defualt.append(defaultinput[i])
        if inputcount != 0:
            for i in range(inputcount):
                if deal_inputs[i].param_value == None:
                    deal_inputs[i].param_value = deal_defualt[i]
        else:
            for i in range(defcount):
                deal_inputs[i].param_value = deal_defualt[i]
    else:
        for i in range(inputcount):
            deal_inputs[i].param_value = realinput[i]
    return deal_inputs
#异常横向拦截装饰器
def handler_exception_decorator(length:int,listflag = False):
    def handler_exception_inner(func):
        tname = threading.current_thread().name
        tid = str(threading.current_thread().ident)
        def handler_exception(*args):
            try:
                global MySiganl
                if (not MySiganl) or ((MySiganl == True) and (func.__code__.co_name == "PDUCloseChannel" or func.__code__.co_name == "UnloadOdx")):
                    bStep = False
                    result:list=[]
                    # deal_input = GetInputInfos(args,len(args),func.__code__.co_argcount,func.__code__.co_varnames,func.__defaults__) 
                    # PrintFunctionInBoundary(tname,tid,func.__name__,deal_input)
                    MSGLogger.debug(">>>>Enter : " + func.__code__.co_name)
                    resultInfo=func(*args)
                    if isinstance(resultInfo,tuple):
                        result=list(resultInfo)
                    elif isinstance(resultInfo,list):
                        result=resultInfo
                    else:
                        if(length!=0):
                            result.append(resultInfo)
                    if listflag:
                        tempresult = []
                        tempresult.append(result)
                        result = tempresult
                    result.append(StepResult.Ok)
                    for arg in args:
                        if type(arg) == StatStep:
                            RecordStatStep_(arg)
                    # PrintFunctionOutBoundary(tname,tid,func.__name__)
                    MSGLogger.debug("<<<<leave : " + func.__code__.co_name)
                    return result
                else:
                    MSGLogger.debug("Skip:"+func.__code__.co_name)
                    result=[]
                    for i in range(length):
                        result.append(None)
                    if listflag:
                        tempresult = []
                        tempresult.append(result)
                        result = tempresult
                    result.append(StepResult.Nok)
                    return result
            except Exception as exception:
                traceback.print_exc()
                traceinfo= traceback.format_exc()
                MSGLogger.critical(traceinfo)
                # PrintFunctionOutBoundary(tname,tid,func.__name__)
                for arg in args:
                    if type(arg) == StatStep:
                        arg.Result = "NOK"
                        arg.Except = True
                        RecordStatStep_(arg)
                result=[]
                for i in range(length):
                    result.append(None)
                if listflag:
                    tempresult = []
                    tempresult.append(result)
                    result = tempresult
                result.append(StepResult.Nok)
                MSGLogger.debug("<<<<Leave : " + func.__code__.co_name)
                # PDUOpenChannel 函数抛出异常时直接向上传播
                if func.__code__.co_name == "PDUOpenChannel":
                    EndCollectInfos_()
                    raise
                return result
        return handler_exception
    return handler_exception_inner

def ToolsFunctionDecorator(rcount:int,listflag = False):
    """  [rcount] : number of return values"""
    def InternalDecorator(func):
        def interdecorator(*args):
            try:
                global MySiganl
                if not MySiganl:
                    result = []
                    MSGLogger.debug(">>>>Enter : " + func.__code__.co_name)
                    res = func(*args)
                    if type(res) == list:
                        result = res
                    elif type(res) == tuple:
                        result = list(res)
                    else:
                        if rcount != 0:
                            result.append(res)
                    if listflag:
                        tempresult = []
                        tempresult.append(result)
                        result = tempresult
                    result.append(StepResult.Ok) 
                    MSGLogger.debug("<<<<leave : " + func.__code__.co_name) 
                    return result   
                else:
                    MSGLogger.debug("Receive stop signal")
                    result=[]
                    for i in range(rcount):
                        result.append(None)
                    if listflag:
                        tempresult = []
                        tempresult.append(result)
                        result = tempresult
                    result.append(StepResult.Nok)
                    return result
            except:
                traceinfo= traceback.format_exc()
                MSGLogger.critical(traceinfo)
                traceback.print_exc()
                result = []
                for i in range(rcount):
                    result.append(None)
                if listflag:
                    tempresult = []
                    tempresult.append(result)
                    result = tempresult
                result.append(StepResult.Nok)
                MSGLogger.debug("<<<<Leave : " + func.__code__.co_name)
                return result
        return interdecorator
    return InternalDecorator
def DoCheck():
    resultdir = os.path.dirname(os.path.dirname((os.path.dirname(__file__))))
    tempcheck = os.path.join(resultdir,"result","tempcheckresult.json")
    if os.path.exists(tempcheck):
        return TempCheckResultGet(tempcheck)
    else:
        global PyPath
        return ConditionCheck(PyPath)
def TempCheckResultGet(filepath):
    flag = True
    notpassnum = 0
    
    tresu = {}
    try:
        if os.path.exists(filepath):
            with open(filepath,"r") as fd:
                tresu = json.load(fd)
    except:
        print("no tempresult")
        return flag,tresu
    if len(tresu) != 0:
        for item in tresu.items():
            if item[1]["Enable"] == "True":
                if item[0] == "VehicleSpeedCheck":
                    if item[1]["SetValue"] != item[1]["MeasureValue"]:
                        notpassnum += 1
                if item[0] == "SocCheck":
                    if item[1]["SetValue"] != item[1]["MeasureValue"]:
                        notpassnum += 1
                if item[0] in ["VehicleModeCheck","UsageModeCheck"]:
                    if item[1]["MeasureValue"] not in item[1]["SetValue"]:
                        notpassnum += 1
            # else:
            #     notpassnum += 1
    if notpassnum != 0:
        flag = False
    os.remove(filepath)
    return flag,tresu
            

def ConditionCheck(filepath):
    #iscloud==True,for cloud call;iscloud==False,for local call
    checkresult:dict = {}
    result:bool = False
    isfindfile = False
    start_point = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
    needcondition:list = ["VehicleSpeedCheck","VehicleModeCheck","UsageModeCheck","SocCheck"]
    losedcondition:list = []
    notpass = 0
    # path = os.path.join(start_point,filepath)
    path = ""
    if IS_SECURE_STARTUP:
        path = os.path.join(filepath,"condition_check.json")
        if not os.path.exists(path):
            MSGLogger.error("no "+path)
            path = os.path.join(GetScriptRoot(),"condition_check.json")
            if not os.path.exists(path):
                MSGLogger.error("no "+path)
                path = os.path.join(start_point,"conditioncheck/condition_check.json")
                if not os.path.exists(path):
                    MSGLogger.error("no "+path)
                    return result,checkresult
    else:
        path = filepath
        
        for file in os.listdir(path):
            if file.find("condition_check") != -1:
                isfindfile = True
                path = os.path.join(path,file)
                break
        if not isfindfile:
            result = False
            return result,checkresult
    fd = open(path,"r")
    checkresult = json.load(fd)
    fd.close()

    i = 0
    for i in range(len(needcondition)):
        if needcondition[i] not in checkresult.keys():
            losedcondition.append(needcondition[i]) 
        i += 1
    findlocalcond:bool = False
    if len(losedcondition):
        path = os.path.join(start_point,"conditioncheck")
        for condf in os.listdir(path):
            if condf.find("condition_check") != -1:
                findlocalcond = True
                path = os.path.join(path,condf)
        if not findlocalcond:
            result = False
            return result,checkresult
        localfd = open(path,"r")
        localcond = json.load(localfd)
        localfd.close()
        for losecond in losedcondition:
            if losecond in localcond.keys():
                checkresult[losecond] = localcond[losecond]
    for conds in checkresult.items():
        if conds[0] == "VehicleSpeedCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = VehicleSpeedCheck_()
                if conds[1]["MeasureValue"] == "":
                    notpass += 1
                else:
                    if float(conds[1]["MeasureValue"]) > float(conds[1]["SetValue"]):
                        notpass += 1
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "VehicleModeCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = VehicleModeCheck_()
                if conds[1]["MeasureValue"] == "":
                    notpass += 1
                else:
                    if conds[1]["MeasureValue"] not in conds[1]["SetValue"]:
                        notpass += 1
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "UsageModeCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = UsageModeCheck_()
                if conds[1]["MeasureValue"] == "":
                    notpass += 1
                else:
                    if conds[1]["MeasureValue"] not in conds[1]["SetValue"]:
                        notpass += 1
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "SocCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = SocCheck_()
                if conds[1]["MeasureValue"] == "":
                    notpass += 1
                else:
                    if float(conds[1]["MeasureValue"]) < float(conds[1]["SetValue"]):
                        notpass += 1
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
    # for conds in checkresult.items():
    #     if conds[1]["Enable"] == "True":
    #         if conds[0] != 
    #         result = False
    #         return result,checkresult
    # result = True
    if notpass == 0:
        result = True
    return result,checkresult
    
    
def VehicleSpeedCheck_():
    return ""
def VehicleModeCheck_():
    return ""
def UsageModeCheck_():
    return ""
def SocCheck_():
    return ""


class DiagResultDate(object):
    IsCreate:bool = False
    _instance_me_lock = threading.Lock()
    totalresultdate = collections.OrderedDict()
    statblock = list()
    isconstruct = False
    hasblock = False
    pypath = ""
    ref_count = 0
    connect_status = "Success"
    def __init__(self):
        pass

    def __new__(cls, *args, **kwargs):
        if not hasattr(DiagResultDate, "_instance"):
            with DiagResultDate._instance_me_lock:
                if not hasattr(DiagResultDate, "_instance"):
                    DiagResultDate._instance = object.__new__(cls)
        return DiagResultDate._instance

    def GenerateVehicleData(self):
        filefind:bool = False
        path=""
        if IS_SECURE_STARTUP:
            path = PATHCONFIG.GetStringValue("VehDataPath")
            if os.path.exists(path):
                filefind = True
        else:
            start_path = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
            path = os.path.join(start_path,"vehicleData")
            for name in os.listdir(path):
                if name.find("vehicle_data.json") != -1:
                    path = os.path.join(path,name)
                    filefind = True
        if not filefind:
            DiagResultDate.totalresultdate["VIN"] = ""
            diagtime = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
            DiagResultDate.totalresultdate["TimeStamp"] = diagtime
            DiagResultDate.totalresultdate["MT"] = ""
            DiagResultDate.totalresultdate["OptionCodes"] = ""
            return
        vehiclefile = open(path,"r")
        params = json.load(vehiclefile)
        vehiclefile.close()
        if params.get("VIN"):
            DiagResultDate.totalresultdate["VIN"] = params["VIN"]
        diagtime = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
        DiagResultDate.totalresultdate["TimeStamp"] = diagtime
        if params.get("MTOC"):
            DiagResultDate.totalresultdate["MT"] = params.get("MTOC")
        if params.get("LAS"):
            DiagResultDate.totalresultdate["OptionCodes"] = params.get("LAS") 

    def GetLocation(self):
        return "Init"

    def GetReleaseNumber(self):
        return "1611900407"

    def UpdateTotalResult(self,result):
        DiagResultDate.totalresultdate["Result"]["TotalResult"] = result

    def SetConnectStatus(self,status):
        DiagResultDate.connect_status = status

    def GetPackageName(self):
        return "A26 init package"


    def GenerateConditionResultData(self):
        global PyPath
        DiagResultDate.totalresultdate["Result"] = collections.OrderedDict()
        DiagResultDate.totalresultdate["Result"]["ExceptStep"] = []
        DiagResultDate.totalresultdate["Result"]["Location"] = self.GetLocation()
        DiagResultDate.totalresultdate["Result"]["TotalResult"] = "OK"
        DiagResultDate.totalresultdate["Result"]["ReleaseNumber"] = self.GetReleaseNumber()
        DiagResultDate.totalresultdate["Result"]["SessionID"] = "123"
        packname = PyPath.split("/")[-1]
        DiagResultDate.totalresultdate["Result"]["PackageName"] = packname
        # DiagResultDate.totalresultdate["Result"]["ConditionChecks"] = collections.OrderedDict()
        
        # if IS_SECURE_STARTUP:
        #     condpath = PATHCONFIG.GetStringValue("ConditionCheckPath")
        # else:
        #     condpath = PyPath
        # checkresult,checkconditions = ConditionCheck(condpath)
        # if checkresult:
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["TotalResult"] = "OK"
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["Conditions"] = checkconditions
        # else:
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["TotalResult"] = "NOK"
        #     self.UpdateTotalResult("NOK")
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["Conditions"] = checkconditions

    def RecordPYPath(self,pypath):
        DiagResultDate.pypath = pypath

    def FindBlockID(self,statblock):
        flag = 0
        for item in DiagResultDate.statblock:
            if item["ID"] == statblock.ID:
                flag+=1
        if flag >0:
            return True
        else:
            return False

    def RecordStatblock(self,statblock):
        if not self.FindBlockID(statblock):
            block = collections.OrderedDict()
            block["ID"] = statblock.ID
            if hasattr(statblock,"Result"):
                block["Result"] = statblock.Result
            else:
                block["Result"] = "OK"
            if hasattr(statblock,"ChineseTitle"):
                block["ChineseTranslation"] = statblock.ChineseTitle
            if hasattr(statblock,"EnglishTitle"):
                block["EnglishTranslation"] = statblock.EnglishTitle
            block["StatSteps"] = list()
            DiagResultDate.statblock.append(block)
            DiagResultDate.hasblock = True
     
    def RecordStatStep(self,statstep):
        if not DiagResultDate.hasblock:
            return
        for block in DiagResultDate.statblock:
            if block["ID"] != int(statstep.DefaultValue):
                continue
            for item in block["StatSteps"]:
                if item["ID"] == statstep.ID:
                    block["StatSteps"].remove(item)
                    break
                
            step = collections.OrderedDict()
            step["ID"] = statstep.ID
            if hasattr(statstep,"ECUName"):
                step["ECUName"] = statstep.ECUName
            else:
                step["ECUName"] = ""
            if hasattr(statstep,"Error"):
                step["Error"] = statstep.Error
            if hasattr(statstep,"Result"):
                step["Result"] = statstep.Result
            else:
                step["Result"] = "OK"
            step["ChineseTranslation"] = statstep.ChineseTranslation
            step["EnglishTranslation"] = statstep.EnglishTranslation
            if hasattr(statstep,"Format"):
                step["Format"] = statstep.Format
            else:
                step["Format"] = "HEX"
            if hasattr(statstep,"Value"):
                step["Value"] = statstep.Value
            else:
                step["Value"] = ""
            if hasattr(statstep,"SetValue"):
                step["SetValue"] = statstep.SetValue
            else:
                step["SetValue"] = ""
            if hasattr(statstep,"Minimum"):
                step["Minimum"] = statstep.Minimum
            else:
                step["Minimum"] = ""
            if hasattr(statstep,"MeasureValue"):
                step["MeasureValue"] = statstep.MeasureValue
            else:
                step["MeasureValue"] = ""
            if hasattr(statstep,"Maximum"):
                step["Maximum"] = statstep.Maximum
            else:
                step["Maximum"] = ""
            if hasattr(statstep,"Units"):
                step["Units"] = statstep.Units
            else:
                step["Units"] = ""
            if hasattr(statstep,"PrintAtNok"):
                step["PrintAtNok"] = statstep.PrintAtNok
            if hasattr(statstep,"PrintAtOK"):
                step["PrintAtOK"] = statstep.PrintAtOK
            if hasattr(statstep,"Except"):
                step["Except"] = statstep.Except
            block["StatSteps"].append(step)

    def EndCollectInfos(self):
        hasExceptFlag = False
        if DiagResultDate.isconstruct:
            for block in DiagResultDate.statblock:
                if len(block["StatSteps"])==0:
                    block["Result"] = "NOK"
                    self.UpdateTotalResult("NOK")
                for step in block["StatSteps"]:
                    if "Except" in step.keys():
                        hasExceptFlag = True
                        DiagResultDate.totalresultdate["Result"]["ExceptStep"].append({"StatBlock":block["ID"],"StatSteps":step["ID"]})
                        block["Result"] = "NOK"
                        self.UpdateTotalResult("NOK")
                        # step.pop("Except")
                        continue
                    if step["Result"] == "NOK":
                        block["Result"] = "NOK"
                        self.UpdateTotalResult("NOK")
                        
        if not hasExceptFlag:
            DiagResultDate.totalresultdate["Result"].pop("ExceptStep",-1)
        DiagResultDate.totalresultdate["Result"]["ConnectStatus"] = DiagResultDate.connect_status
        DiagResultDate.totalresultdate["Result"]["StatBlocks"] = DiagResultDate.statblock
        

    def GeneratePrecInfos(self):
        resultpath = ""
        if IS_SECURE_STARTUP:
            resultpath = PATHCONFIG.GetStringValue("TmpCheckResult")
        else:
            path = os.getenv("NEUSAR_DEPLOYMENT_PATH", "")
            resultpath = f"{path}/opt/DiagnosticScript/result/tempcheckresult.json"
        if os.path.exists(resultpath):
            with open(resultpath,"r") as fd:
                params = json.load(fd)
                DiagResultDate.totalresultdate["VIN"] = params["VIN"]
                DiagResultDate.totalresultdate["TimeStamp"] = params["TimeStamp"]
                DiagResultDate.totalresultdate["MT"] = params["MT"]
                DiagResultDate.totalresultdate["OptionCodes"] = params["OptionCodes"]
                DiagResultDate.totalresultdate["Result"] = collections.OrderedDict()
                DiagResultDate.totalresultdate["Result"]["ExceptStep"] = []
                DiagResultDate.totalresultdate["Result"]["Location"] = params["Result"]["Location"]
                DiagResultDate.totalresultdate["Result"]["TotalResult"] = "OK"
                DiagResultDate.totalresultdate["Result"]["ReleaseNumber"] = params["Result"]["ReleaseNumber"]
                DiagResultDate.totalresultdate["Result"]["PackageName"] = params["Result"]["PackageName"]
                # DiagResultDate.totalresultdate["Result"]["ConditionChecks"] = collections.OrderedDict()
                # DiagResultDate.totalresultdate["Result"]["ConditionChecks"] = params["Result"]["ConditionChecks"]
            try:
                os.remove(resultpath)
            except:
                pass 
        else:        
            self.GenerateVehicleData()
            self.GenerateConditionResultData()
        DiagResultDate.isconstruct = True

    def GetResultPrintInfo(self):
        return DiagResultDate.totalresultdate
    
    def GenerateResultFile(self):
        PrinDiagResult(self.GetResultPrintInfo())


def GetDiagResultInstance():
    log = DiagResultDate()
    if not log.isconstruct:
        log.GeneratePrecInfos()
    return log

def RecordStatblock_(block):
    global PyPath
    stack = inspect.stack()[1].filename
    pos = stack.rfind(os.sep)
    PyPath = stack[0:pos]
    log = GetDiagResultInstance()
    log.RecordStatblock(block)
    log.ref_count += 1

def RecordStatStep_(step):
    log = GetDiagResultInstance()
    log.RecordStatStep(step)

def EndCollectInfos_():
    log = GetDiagResultInstance()
    if log.ref_count == 1:
        log.EndCollectInfos()
        log.GenerateResultFile()
    log.ref_count -= 1