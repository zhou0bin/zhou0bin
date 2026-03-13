import time
from .log_info import ToolsFunctionDecorator
@ToolsFunctionDecorator(rcount=1)
def GetTime():
    return int(round(time.time()*1000))

@ToolsFunctionDecorator(rcount=1)
def TimeDifference(StartTime):
    return int(round(time.time()*1000)) - int(round(StartTime))

