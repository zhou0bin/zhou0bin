from .log_info import MSGLogger,handler_exception_decorator,StatStep

@handler_exception_decorator(length=1)
def StringCompare(str1:str,str2:str,statStep:StatStep):
    if statStep == None or type(statStep) != StatStep:
        MSGLogger.error("StringCompare:input param statStep error")
        return False
    if str1 == None or type(str1) != str:
        MSGLogger.error("StringCompare:input param str1 error")
        statStep.Result = "NOK"
        return False
    if str2 == None or type(str2) != str:
        MSGLogger.error("StringCompare:input param str2 error")
        statStep.Result = "NOK"
        return False
    statStep.MeasureValue = str1
    statStep.SetValue = str2
    statStep.Minimum = ""
    statStep.Maximum = ""
    if str1 == str2:
        statStep.Result = "OK"
        return True
    else:
        statStep.Result = "NOK"
        return False