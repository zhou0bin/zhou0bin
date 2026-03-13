import os
# import imp
import importlib
from ..appAPI.log_info import StepResult,MSGLogger
import sys
import traceback
# @handler_exception_decorator(length = 1)
def CallProcedure(GUID = ""):
    if GUID == None or type(GUID) != str:
        return [StepResult.Nok]
    caller = sys._getframe().f_back.f_code.co_filename
    path = caller[0:caller.rfind("/")]

    if type(GUID) != str or (type(GUID) == str and len(GUID) == 0):
        MSGLogger.error("CallProcedure Input: GUID Error")
        return [StepResult.Nok]
    if type(path) != str or (type(path) == str and len(path) == 0):
        MSGLogger.error("CallProcedure Input: Path Error")
        return [StepResult.Nok]
    if not os.path.isdir(path):
        MSGLogger.error("CallProcedure Input: Path Is Not A Valid Folder")
        return [StepResult.Nok]
    isfind = False
    for item in os.listdir(path):
        if item.split(".py")[0] == GUID:
            isfind = True
            break

    if not isfind:
        MSGLogger.error("CallProcedure Not Find The Script")
        return [StepResult.Nok]
    else:
        try:
            sys.path.append(path)
            modul = importlib.import_module(GUID)
            if hasattr(modul,"main"):
                try:
                    modul.main()
                    MSGLogger.debug("CallProcedure: Run Script Success")
                    sys.path.remove(path)
                    return [StepResult.Ok]
                except:
                    MSGLogger.error("CallProcedure: "+GUID+" main() Has Error:"+traceback.format_exc())
                    sys.path.remove(path)
                    return [StepResult.Nok]
            else:
                MSGLogger.error("CallProcedure: "+GUID+" Has No main()")
                sys.path.remove(path)
                return [StepResult.Nok]
        except:
            MSGLogger.error("CallProcedure:Find No Module")
            try:
                sys.path.remove(path)
                return [StepResult.Nok]
            except:
                return [StepResult.Nok]
        
# CallProcedure("1testCCU1")
    

