
#字符串拼接
from .log_info import MSGLogger,ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount = 1)
def StringCat(Str1=None, Str2=None):
    if Str1 == None:
        Str1 = ''
        MSGLogger.error("StringCat:input parameter Str1 None")
    if type(Str1) != str:
        Str1 = str(Str1)
        MSGLogger.error("StringCat:input parameter Str1 error")
    if Str2 == None:
        Str2 = ''
        MSGLogger.error("StringCat:input parameter Str2 None")
    if type(Str2) != str:
        Str2 = str(Str2)
        MSGLogger.error("StringCat:input parameter Str2 error")
    OutStr = Str1+Str2
    # print(OutStr)
    return OutStr