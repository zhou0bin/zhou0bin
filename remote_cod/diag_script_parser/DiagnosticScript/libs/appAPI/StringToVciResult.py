# 字符串转为 vciResult 函数
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringToVciResult(Str = None):
    if Str == None:
        Str = ""
        MSGLogger.error("StringToVciResult:input Str is error")
    if type(Str) != str:
        Str = str(Str)
        MSGLogger.error("StringToVciResult:input Str is error")
    Str = Str.strip()
    return Str
