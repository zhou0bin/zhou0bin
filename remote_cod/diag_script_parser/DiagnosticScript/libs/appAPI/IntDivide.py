# 整数相除
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def IntDivide(num1=None, num2=None):
    if num1 == None:
        MSGLogger.error("IntDivide:input param is error")
        num1 = 0
    if num2 == None or num2 ==0:
        MSGLogger.error("IntDivide:input param is error")
    if type(num1) != int:
        MSGLogger.error("IntDivide:input param is error")
        num1 = int(num1)
    if type(num2) != int:
        MSGLogger.error("IntDivide:input param is error")
        num2 = int(num2)
    if num2 == None or num2 ==0:
        num2 = 0.000001
    Result = int(num1/num2)   
    # print(Result)
    return Result
