#整数相减函数定义
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def IntSubtract(num1=None, num2=None):
    if num1 == None:
        MSGLogger.error("IntSubtract:input param is error")
        num1 = 0
    if num2 == None:
        MSGLogger.error("IntSubtract:input param is error")
    if type(num1) != int:
        MSGLogger.error("IntSubtract:input param is error")
        num1 = int(num1)
    if type(num2) != int:
        MSGLogger.error("IntSubtract:input param is error")
        num2 = int(num2)
    Result = num1 - num2
    return Result