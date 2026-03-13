from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def FloatToStr(num1 = None):
    if num1 == None:
        num1 = 0.0
        MSGLogger.error("FloatToStr:input parameter error")
    if type(num1) == int:
        MSGLogger.error("FloatToStr:input parameter error")
        num1 = float(num1)
    Result = str(num1)
    # print(Result,type(Result))
    return Result
