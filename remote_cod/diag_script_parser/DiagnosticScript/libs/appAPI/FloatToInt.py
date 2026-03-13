from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def FloatToInt(num1 = None):
    if num1 == None:
        num1 = 0.0
        MSGLogger.error("FloatToInt:input parameter error")
    if type(num1) == int:
        MSGLogger.error("FloatToInt:input parameter error")
        num1 = float(num1)
    Result = int(num1)
    # print(Result)
    return Result