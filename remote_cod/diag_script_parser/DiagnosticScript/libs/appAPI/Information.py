from .socketTransfer import SendAndReceiveMSG,InformationIntegrate_T
from .log_info import MSGLogger,handler_exception_decorator

@handler_exception_decorator(length=0)
def Information(Title:str,Message:str,UserData:str):
    if Title == None:
        Title = ""
        MSGLogger.error("Information input param Title is None")
    if type(Title) != str and Title != None:
        Title = str(Title)
        MSGLogger.error("Information input param Title is not string type")
    if Message == None:
        Message = ""
        MSGLogger.error("Information input param Message is None")
    if type(Message) != str and Message != None:
        Message = str(Message)
        MSGLogger.error("Information input param Message is not string type")
    if UserData == None:
        UserData = ""
        MSGLogger.error("Information input param UserData is None")
    if type(UserData) != str and UserData != None:
        UserData = str(UserData)
        MSGLogger.error("Information input param UserData is not string type")
    timeout = None
    msg = InformationIntegrate_T(Title,Message,UserData)
    flag = False
    stream = None
    flag,stream = SendAndReceiveMSG(msg,timeout,"Information",False)
    if flag:
        MSGLogger.debug("Information run OK")
    else:
        MSGLogger.error("Information run NOK")

    
