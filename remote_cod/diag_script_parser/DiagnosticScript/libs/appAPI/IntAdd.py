# 整数相加函数定义
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def IntAdd(num1= None,num2=None):
    if num1 == None:
        MSGLogger.error("IntAdd:input param is error")
        num1 = 0
    if num2 == None:
        MSGLogger.error("IntAdd:input param is error")
        num2 = 0
    if type(num1) != int:
        MSGLogger.error("IntAdd:input param is error")
        num1 = int(num1)
    if type(num2) != int:
        MSGLogger.error("IntAdd:input param is error")
        num2 = int(num2)
    Result = num1 + num2
    # print(Result)
    return Result