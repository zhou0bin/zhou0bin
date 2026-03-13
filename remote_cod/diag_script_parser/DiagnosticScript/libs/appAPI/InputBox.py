from .socketTransfer import SendAndReceiveMSG,InputMessageIntegrate_T,StreamUnpack
from .log_info import MSGLogger,handler_exception_decorator
from .utils.util_helper import *
import math
@handler_exception_decorator(length=2)
def InputBox( Title, Message, NumberOfInputs,MessagBoxButton ,  VarType, regExpression , UserData,TimeOut):
    if Title == None or type(Title) != str:
        Title = ""
        MSGLogger.error("InputBox:Title error")
    if Message == None or type(Message) != list:
        Message = []
        MSGLogger.error("InputBox:Message error")
    if NumberOfInputs == None or type(NumberOfInputs) != int:
        NumberOfInputs = 0
        MSGLogger.error("InputBox:NumberOfInputs error")
    if MessagBoxButton == None or type(MessagBoxButton) != int:
        MessagBoxButton=0
        MSGLogger.error("InputBox:MessagBoxButton error")
    if VarType == None or type(VarType) != int:
        VarType=1
        MSGLogger.error("InputBox:VarType error")
    if regExpression == None:
        regExpression=""
        MSGLogger.error("InputBox:regExpression error")
    if type(regExpression) != str and regExpression != None:
        regExpression = str(regExpression)
        MSGLogger.error("InputBox:regExpression error")
    if type(UserData) != str or UserData == None:
        UserData = ""
        MSGLogger.error("InputBox:UserData error")
    if type(TimeOut) != float and type(TimeOut) != int:
        TimeOut = 5.0
        MSGLogger.error("InputBox:TimeOut error")
    if type(TimeOut) == float:
        TimeOut = math.ceil(TimeOut)
    msg = InputMessageIntegrate_T(Title, Message, NumberOfInputs,MessagBoxButton ,VarType, regExpression,UserData,TimeOut)
    result = -1
    flag = False
    InputValue = []
    ack = {}
    flag,stream = SendAndReceiveMSG(msg,TimeOut,"InputBox")
    
    if flag:
        ack = StreamUnpack(stream)
        result = ack["result"]
        InputValue = ack["infos"]
    MSGLogger.debug({'Title':Title,'Message':Message,"NumberOfInputs":NumberOfInputs,'MessagBoxButton':MessagBoxButton,'VarType':VarType,'regExpression':regExpression,'TimeOut':TimeOut,'result':result,'InputValue':InputValue})
    MSGLogger.debug("InputBox rec pdu:"+str(ack))
    return result, InputValue
