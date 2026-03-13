#字符串转大写
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringToUpper(Str=None):
    if isinstance(Str,str):
        OutStr = Str.upper()
    else:
        OutStr = ''
        MSGLogger.error("StringToUpper:input parameter error")
    # print(OutStr)
    return OutStr

# StringToUpper()