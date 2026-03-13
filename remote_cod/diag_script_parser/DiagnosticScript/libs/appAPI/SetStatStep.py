from .log_info import MSGLogger,handler_exception_decorator,StatStep

@handler_exception_decorator(length=0)
def SetStatStep(statStep:StatStep,Result:bool,Value:str,MeasureValue:str,SetValue:str,MinValue:str,MaxValue:str):
    if type(statStep)!= StatStep or statStep == None:
        MSGLogger.error("SetStatStep:input param statStep error")
        pass
    else:
        if Result == True:
            statStep.Result = "OK"
        elif Result == False:
            statStep.Result = "NOK"
        statStep.Value = str(Value)
        statStep.MeasureValue = str(MeasureValue)
        statStep.SetValue = str(SetValue)
        statStep.Minimum = str(MinValue)
        statStep.Maximum = str(MaxValue)