from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringToFloat(string):
    if string == None:
        MSGLogger.error("StringToFloat:input param is error")
        string = ""
        return 0.0
    if type(string) != str:
        string = str(string)
        MSGLogger.error("StringToFloat:input param is error")
    return float(string)
