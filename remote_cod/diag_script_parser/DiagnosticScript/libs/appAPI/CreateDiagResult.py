###############

# unuseful file !!!!!

###############
 
import json
import os
import collections
import time
import threading
import sys
import inspect
sys.path[0] = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
from libs.util.DiagLog import PrinDiagResult

def ConditionCheck(filepath):
    #iscloud==True,for cloud call;iscloud==False,for local call
    checkresult:dict = {}
    result:bool = False
    isfindfile = False
    start_point = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
    # path = os.path.join(start_point,filepath)
    path = filepath
    needcondition:list = ["VehicleSpeedCheck","VehicleModeCheck","UsageModeCheck","SocCheck"]
    losedcondition:list = []
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
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "VehicleModeCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = VehicleModeCheck_()
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "UsageModeCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = UsageModeCheck_()
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
        elif conds[0] == "SocCheck":
            if conds[1]["Enable"] == "True":
                conds[1]["MeasureValue"] = SocCheck_()
                continue
            else:
                conds[1]["MeasureValue"] = ""
                continue
    for conds in checkresult.items():
        if conds[1]["Enable"] != "True":
            result = False
            return result,checkresult
    result = True
    return result,checkresult
    
    
def VehicleSpeedCheck_():
    return 123456
def VehicleModeCheck_():
    return 123456
def UsageModeCheck_():
    return 123456
def SocCheck_():
    return 123456

class DiagResultDate(object):
    IsCreate:bool = False
    _instance_me_lock = threading.Lock()
    totalresultdate = collections.OrderedDict()
    statblock = list()
    isconstruct = False
    hasblock = False
    def __init__(self):
        pass

    def __new__(cls, *args, **kwargs):
        if not hasattr(DiagResultDate, "_instance"):
            with DiagResultDate._instance_me_lock:
                if not hasattr(DiagResultDate, "_instance"):
                    DiagResultDate._instance = object.__new__(cls)
        return DiagResultDate._instance

    def GenerateVehicleData(self):
        start_path = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
        path = os.path.join(start_path,"vehicleData")
        filefind:bool = False
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
    
    def GetPackageName(self):
        return "A26 init package"


    def GenerateConditionResultData(self):
        DiagResultDate.totalresultdate["Result"] = collections.OrderedDict()
        DiagResultDate.totalresultdate["Result"]["Location"] = self.GetLocation()
        DiagResultDate.totalresultdate["Result"]["TotalResult"] = "OK"
        DiagResultDate.totalresultdate["Result"]["ReleaseNumber"] = self.GetReleaseNumber()
        DiagResultDate.totalresultdate["Result"]["SessionID"] = "123"
        DiagResultDate.totalresultdate["Result"]["PackageName"] = self.GetPackageName()
        # DiagResultDate.totalresultdate["Result"]["ConditionChecks"] = collections.OrderedDict()
        # condpath = os.path.dirname(inspect.stack()[4].filename)
        # checkresult,checkconditions = ConditionCheck(condpath)
        # if checkresult:
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["TotalResult"] = "OK"
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["Conditions"] = checkconditions
        # else:
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["TotalResult"] = "NOK"
        #     self.UpdateTotalResult("NOK")
        #     DiagResultDate.totalresultdate["Result"]["ConditionChecks"]["Conditions"] = checkconditions

    def RecordStatblock(self,statblock):
        for item in DiagResultDate.statblock:
            if item["ID"] == statblock.ID:
                DiagResultDate.statblock.remove(item)
                break
        
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
            step["ChineseTranslation"] = statstep.EnglishTranslation
            step["EnglishTranslation"] = statstep.ChineseTranslation
            if hasattr(statstep,"Format"):
                step["Format"] = statstep.Format
            else:
                step["Format"] = ""
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
            if hasattr(statstep,"Maximun"):
                step["Maximun"] = statstep.Maximun
            else:
                step["Maximun"] = ""
            if hasattr(statstep,"Units"):
                step["Units"] = statstep.Units
            else:
                step["Units"] = ""
            if hasattr(statstep,"PrintAtNok"):
                step["PrintAtNok"] = statstep.PrintAtNok
            if hasattr(statstep,"PrintAtOK"):
                step["PrintAtOK"] = statstep.PrintAtOK

            block["StatSteps"].append(step)
    
    def EndCollectInfos(self):
        if DiagResultDate.isconstruct:
            for block in DiagResultDate.statblock:
                for step in block["StatSteps"]:
                    if hasattr(step,"Error"):
                        continue
                    if step["Result"] == "NOK":
                        block["Result"] = "NOK"
                        self.UpdateTotalResult("NOK")
                        break
        DiagResultDate.totalresultdate["Result"]["StatBlock"] = DiagResultDate.statblock
        

    def GeneratePrecInfos(self):
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
    log = GetDiagResultInstance()
    log.RecordStatblock(block)

def RecordStatStep_(step):
    log = GetDiagResultInstance()
    log.RecordStatStep(step)

def EndCollectInfos_():
    log = GetDiagResultInstance()
    log.EndCollectInfos()
    log.GenerateResultFile()

# class StatBlock():
#     def __init__(self,ID,ChineseTitle,EnglishTitle):
#         self.ID = ID
#         self.ChineseTitle = ChineseTitle
#         self.EnglishTitle = EnglishTitle
# class StatStep():
#     def __init__(self,ID,ChineseTitle,EnglishTitle,Result,GUID):
#         self.GUID = GUID
#         self.ID = ID
#         self.ChineseTitle = ChineseTitle
#         self.EnglishTitle = EnglishTitle
#         self.Result = Result
#         self.Format = "HEX"
        
# block = StatBlock("123456","王南","wangnan")
# step = StatStep("8888","王鹏","wangpeng","NOK","123456")

