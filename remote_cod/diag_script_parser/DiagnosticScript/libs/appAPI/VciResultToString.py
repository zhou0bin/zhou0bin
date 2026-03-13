# vciResult转为 字符串 函数
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount=1)
def VciResultToString(VciResult = None):
    if VciResult == None:
        VciResult = ""
        MSGLogger.error("VciResultToString:input VciResult is error")
    if type(VciResult) != str:
        VciResult = str(VciResult)
        MSGLogger.error("VciResultToString:input VciResult is error")
    VciResult = VciResult.strip()
    return VciResult