
from .log_info import MSGLogger,StatStep,handler_exception_decorator

@handler_exception_decorator(length=1)
def FloatToleranceCheck(LowerLimit:float,UpperLimit:float,Value:float,statStep:StatStep):
    if type(LowerLimit) != float and type(LowerLimit) != int:
        MSGLogger.error("FloatToleranceCheck:input param LowerLimit error")
        if type(statStep) == StatStep:
            StatStep.Result = "NOK"
        return False
    if type(UpperLimit) != float and type(UpperLimit) != int:
        MSGLogger.error("FloatToleranceCheck:input param UpperLimit error")
        if type(statStep) == StatStep:
            StatStep.Result = "NOK"
        return False
    if type(Value) != float and type(Value) != int:
        MSGLogger.error("FloatToleranceCheck:input param Value error")
        if type(statStep) == StatStep:
            StatStep.Result = "NOK"
        return False
    if type(statStep) != StatStep:
        MSGLogger.error("FloatToleranceCheck:input param statStep error")
        return False
    
    statStep.Minimum = float(LowerLimit)
    statStep.Maximum = float(UpperLimit)
    statStep.MeasureValue = float(Value)
    statStep.Value = float(Value)
    if Value >= LowerLimit and Value <= UpperLimit:
        statStep.Result = "OK"
        return True
    else:
        statStep.Result = "NOK"
        return False