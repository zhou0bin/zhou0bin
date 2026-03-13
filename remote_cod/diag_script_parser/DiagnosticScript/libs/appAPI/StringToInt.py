
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringToInt(Str:str=""):

    if Str == None or type(Str) != str:
        Str = ""
        MSGLogger.error("StringToInt:input parameter error")
        return 0
    elif type(Str) == str and len(Str) !=0:
        Str = Str.strip()
        # return int(Str,16) //reserve for 16
        # return int(Str,10) //reserve for 16
        return eval(Str)
    else:
        return 0
