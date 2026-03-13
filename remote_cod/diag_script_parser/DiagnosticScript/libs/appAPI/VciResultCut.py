# vciResult转为 字符串 函数
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount=1)
def VciResultCut(InVciResult = None, Position = None,Length = None):
    if isinstance(InVciResult,str) and isinstance(Position,int) and isinstance(Length,int) and (len(InVciResult)>(Position*2+Length*2-1)):
        OutVciResult = InVciResult[Position*2:2*Position+Length*2]
    else:
        OutVciResult = ""
        MSGLogger.error("VciResultCut:input parameter error")

    # print(OutVciResult)
    return OutVciResult

