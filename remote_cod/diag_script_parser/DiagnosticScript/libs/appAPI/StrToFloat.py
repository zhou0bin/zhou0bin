from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def IsFloatNum(Str):
    if(Str[0]=='-' and len(Str)>1):
        Str = Str[1:]
    s=Str.split('.')
    if len(s)>2 or (not isinstance(Str,str)):
        return False
    else:
        for si in s:
            if not si.isdigit():
                return False
        return True
@ToolsFunctionDecorator(rcount = 1)
def StrToFloat(Str:str = ""):
    if Str == None or Str == "":
        MSGLogger.error("StrToFloat:input parameter error")
        Str = 0.0
        return Str
    elif type(Str) == str and len(Str) != 0:
        Str = Str.strip()
        Str = float(eval(Str))
        return Str
    elif type(Str) == int:
        Str = float(Str)
        MSGLogger.error("StrToFloat:input parameter error")
        return Str
    else:
        return float(Str)
