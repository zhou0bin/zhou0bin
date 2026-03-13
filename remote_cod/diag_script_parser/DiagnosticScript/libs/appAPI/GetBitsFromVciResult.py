
from .log_info import MSGLogger,ToolsFunctionDecorator

@ToolsFunctionDecorator(rcount=1)
def GetBitsFromVciResult(VciResult:str,BytePosition:int,StartBit:int,BitLength:int):
    result = -1
    if VciResult == None or type(VciResult) != str or VciResult == "":
        MSGLogger.error("GetBitsFromVciResult:input param VciResult error")
        return result
    if BytePosition == None or type(BytePosition) != int:
        MSGLogger.error("GetBitsFromVciResult:input param BytePosition error")
        return result
    if StartBit == None or type(StartBit) != int:
        MSGLogger.error("GetBitsFromVciResult:input param StartBit error")
        return result
    if type(StartBit) == int:
        if StartBit < 0 or StartBit > 7:
            MSGLogger.error("GetBitsFromVciResult:input param StartBit error")
            return result
    if BitLength == None or type(BitLength) != int:
        MSGLogger.error("GetBitsFromVciResult:input param BitLength error")
        return result
    if type(BitLength) == int:
        if BitLength < 1 or BitLength > 8:
            MSGLogger.error("GetBitsFromVciResult:input param BitLength error")
            return result
    if len(VciResult)//2 < (BytePosition + 1) or BytePosition < 0:
        MSGLogger.error("GetBitsFromVciResult:input param BytePosition error")
        return result
    if (StartBit + BitLength)>8:
        MSGLogger.error("GetBitsFromVciResult:input param StartBit and BitLength error")
        return result
    cutvci = bin(int(VciResult[BytePosition*2:BytePosition*2+2],16))[2:]
    cutvci = cutvci.rjust(8,"0")
    value = ""
    for i in range(BitLength):
        value += cutvci[7-(StartBit + i)]
    temp = ""
    lent = len(value)
    for i in range(lent):
        temp += value[lent-1-i]
    temp = temp.rjust(8,"0")
    result = int(temp,2)
    return result

