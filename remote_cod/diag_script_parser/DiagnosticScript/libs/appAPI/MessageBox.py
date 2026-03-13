from .socketTransfer import SendAndReceiveMSG,ConfirmMessageIntegrate_T,StreamUnpack
from .log_info import MSGLogger,handler_exception_decorator
from .utils.util_helper import *
import math
# int Result=MessageBox(string Title ， string Message ， int MessagBoxButton ,int
# MeesageBoxIcon ,float TimeOut)
@handler_exception_decorator(length=1)
def MessageBox( Title, Message, MessagBoxButton ,MeesageBoxIcon, UserData,TimeOut):
    if Title == None or type(Title) != str:
        Title = ""
        MSGLogger.error("MessageBox InputParams error:Title is error")
    if Message == None or type(Message) != str:
        Message = ""
        MSGLogger.error("MessageBox InputParams error:Message is error")
    if MessagBoxButton == None or type(MessagBoxButton) != int:
        MessagBoxButton=0
        MSGLogger.error("MessageBox InputParams error:MessagBoxButton is error")
    if MeesageBoxIcon == None or type(MeesageBoxIcon) != int:
        MeesageBoxIcon=0
        MSGLogger.error("MessageBox InputParams error:MeesageBoxIcon is error")
    if UserData == None or type(UserData) != str:
        UserData = ""
        MSGLogger.error("MessageBox InputParams error:UserData is error")
    if type(TimeOut) != float and type(TimeOut) != int:
        TimeOut = 5.0
        MSGLogger.error("MessageBox InputParams error:TimeOut is error")
    if type(TimeOut) == float:
        TimeOut = math.ceil(TimeOut)
    msg = ConfirmMessageIntegrate_T(Title, Message, MessagBoxButton ,MeesageBoxIcon,UserData,TimeOut)
    result = -1
    ack = {}
    flag = False
    flag,stream= SendAndReceiveMSG(msg,TimeOut,"MessageBox")
    if flag:
        ack = StreamUnpack(stream)
        result = ack["result"]
    MSGLogger.debug({'Title':Title,'Message':Message,'MessagBoxButton':MessagBoxButton,'MeesageBoxIcon':MeesageBoxIcon,'TimeOut':TimeOut,'result':result})
    MSGLogger.debug("MessageBox rec pdu:"+str(ack))
    return result