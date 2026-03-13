#字符串去除首尾空白函数定义
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringTrim(Str = None):
    if isinstance(Str,str):
        OutStr = Str.strip()
    else:
        OutStr = ''
        MSGLogger.error("StringTrim:input parameter error")
    return OutStr