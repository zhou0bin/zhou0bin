from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
#字符串截取
def StringCut(Str=None, StartPosition=None):
    if Str == None or type(Str) != str:
        MSGLogger.error("StringCut:input parameter Str error")
        return ""
    if StartPosition == None or type(StartPosition) != int:
        MSGLogger.error("StringCut:input parameter StartPosition None")
        return ""
    return Str[StartPosition:]