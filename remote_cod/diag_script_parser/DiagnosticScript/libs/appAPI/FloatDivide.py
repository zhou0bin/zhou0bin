#浮点数相除
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def FloatDivide(num1=None,num2=None):
    if num1 == None:
        num1 = 0.0
        MSGLogger.error("FloatDivide:input parameter error")
    if num2 == None or num2 == 0:
        num2 = 0.000001
        MSGLogger.error("FloatDivide:input parameter error")
    if type(num1) == int:
        MSGLogger.error("FloatDivide:input parameter error")
        num1 = float(num1)
    if type(num2) == int:
        MSGLogger.error("FloatDivide:input parameter error")
        num2 = float(num2)

    Result = num1/num2
    # print(Result)
    return Result