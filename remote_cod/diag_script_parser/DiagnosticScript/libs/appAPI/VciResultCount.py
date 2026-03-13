from .log_info import ToolsFunctionDecorator,MSGLogger

@ToolsFunctionDecorator(rcount=1)
def VciResultCount(VciResult):
    if VciResult == None:
        MSGLogger.error("VciResultCount:inputparam error")
        return 0
    # if type(VciResult) != str:
    #     MSGLogger.error("VciResultCount:inputparam error")
    #     return 0
    if type(VciResult) == str:
        VciResult.strip()
        if len(VciResult) == 0:
            MSGLogger.error("VciResultCount:inputparam error")
            return 0
        else:
            if len(VciResult)%2 == 0:
                return len(VciResult)//2
            else:
                MSGLogger.error("VciResultCount:inputparam error")
                return 0
    elif type(VciResult) == bytes or type(VciResult) == bytearray:
        return len(VciResult)
    else:
        return 0
  