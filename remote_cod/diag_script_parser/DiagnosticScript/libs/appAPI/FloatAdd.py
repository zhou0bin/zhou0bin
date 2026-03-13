#浮点数相加
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def FloatAdd(num1=None, num2=None):
    if num1 == None:
        num1 = 0.0
        MSGLogger.error("FloatAdd:input parameter error")
    if num2 == None:
        MSGLogger.error("FloatAdd:input parameter error")
        num2 = 0.0
    if type(num1) == int:
        MSGLogger.error("FloatAdd:input parameter error")
        num1 = float(num1)
    if type(num2) == int:
        MSGLogger.error("FloatAdd:input parameter error")
        num2 = float(num2)
    Result = float(num1 + num2)
    # print(Result)
    return Result
