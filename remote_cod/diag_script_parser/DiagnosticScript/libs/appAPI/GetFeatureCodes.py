# from .utils.db_data import DbVehicleData
# from .utils.util_helper import utilVehicleDataFile
from .log_info import MSGLogger,ToolsFunctionDecorator,IS_SECURE_STARTUP,PATHCONFIG
import os
import json

def VehicleDataGet(path):
    with open(path) as fd:
        data = json.load(fd)
        if "LAS" in data.keys():
            return data["LAS"]
        else:
            MSGLogger.error("GetFeatureCodes:Find no LAS")
            return []

@ToolsFunctionDecorator(rcount=1,listflag = True)
def GetFeatureCodes():
    path = ""
    if IS_SECURE_STARTUP:
        path = PATHCONFIG.GetStringValue("VehDataPath")
        if not os.path.exists(path):
            MSGLogger.error("GetFeatureCodes:Find no "+path)
            return []
    else:
        path = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
        path = os.path.join(path,"vehicleData")
        flag = False
        for file in os.listdir(path):
            if file == "vehicle_data.json":
                path = os.path.join(path,file)
                flag = True
        if not flag:
            MSGLogger.error("GetFeatureCodes:Find no vehicle_data.json")
            return [] 
    try:
        return VehicleDataGet(path)
    except Exception as exception:
        MSGLogger.error({'错误':'获取VIN码失败'})
        return []