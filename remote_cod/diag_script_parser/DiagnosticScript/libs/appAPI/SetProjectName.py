import json
import os
from .log_info import handler_exception_decorator,MSGLogger,PATHCONFIG,IS_SECURE_STARTUP


#设置项目名称
@handler_exception_decorator(length=1)
def SetProjectName (ProjectName:str):
    IsSuccess:bool = False
    path = ""
    if type(ProjectName)!=str or ProjectName==None:
        MSGLogger.error("SetProjectName: ProjectName is error")
        return [IsSuccess]
    if IS_SECURE_STARTUP:
        path = PATHCONFIG.GetStringValue("VehDataPath")
        if not os.path.exists(path):
            MSGLogger.error("SetProjectName: no "+path)
            return [IsSuccess]
    else:
        ProjectName = ProjectName.strip()
        start_path = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
        path = os.path.join(start_path,"vehicleData")
        filefind:bool = False
        for name in os.listdir(path):
            if name.find("vehicle_data.json") != -1:
                path = os.path.join(path,name)
                filefind = True
        if not filefind:
            MSGLogger.error("SetProjectName: vehicle_data.json is not found!")
            return [IsSuccess]
    fd = open(path,"r")
    params = json.load(fd)
    fd.close()
    if "ProjectName" in params.keys():
        params["ProjectName"]=ProjectName
        with open(path,"w") as vehiclefileNew:
            json.dump(params,vehiclefileNew,indent=4,ensure_ascii=False,sort_keys=False)
            IsSuccess = True
    else:
        tempparams = {"ProjectName":ProjectName}
        tempparams.update(params)
        with open(path,"w") as vehiclefileNew:
            json.dump(tempparams,vehiclefileNew,indent=4,ensure_ascii=False,sort_keys=False)
            IsSuccess = True
    return [IsSuccess]