#字符串转小写
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringToLower(Str = None):
    if isinstance(Str,str):
        OutStr = Str.lower()
    else:
        OutStr = ''
        MSGLogger.error("StringToLower:input parameter error")
    return OutStr