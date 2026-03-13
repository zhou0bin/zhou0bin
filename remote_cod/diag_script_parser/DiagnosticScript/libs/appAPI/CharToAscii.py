from .log_info import handler_exception_decorator,MSGLogger

@handler_exception_decorator(length=1)
def CharToAscii(AsciiCode):
    if AsciiCode == None:
        AsciiCode = 0
        MSGLogger.error("Inputparam error")
        return chr(AsciiCode)
    if type(AsciiCode) == str:
        AsciiCode = int(AsciiCode,16)
        return chr(AsciiCode)
    elif type(AsciiCode) == int:
        return chr(AsciiCode)
    else:
        MSGLogger.error("Inputparam error")
        return None