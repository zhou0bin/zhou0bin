from .socketTransfer import SelectionListIntegrate_T,SendAndReceiveMSG,StreamUnpack
from .log_info import MSGLogger,handler_exception_decorator
from .utils.util_helper import *
import math
@handler_exception_decorator(length=2)
def SelectionList( Title, Message, MessagBoxButton , SelectionItems, VarName, UserData,TimeOut):
    if Title == None:
        Title = ""
        MSGLogger.error("SelectionList InputParams error:Title is None")
    if type(Title) != str:
        Title = str(Title)
        MSGLogger.error("SelectionList InputParams error:Title is not str")
    if Message == None or type(Message) != str:
        Message = ""
        MSGLogger.error("SelectionList InputParams error:Message is error")
    if MessagBoxButton == None or type(MessagBoxButton) != int:
        MessagBoxButton=0
        MSGLogger.error("SelectionList InputParams error:MessagBoxButton is error")
    if SelectionItems == None or type(SelectionItems)!= list:
        SelectionItems = []
        MSGLogger.error("SelectionList InputParams error:SelectionItems is error")
    if VarName == None or type(VarName) != str:
        VarName = "_Temp_Var"
        MSGLogger.error("SelectionList InputParams error:VarName is error")
    if UserData == None or type(UserData) != str:
        UserData = ""
        MSGLogger.error("SelectionList InputParams error:VarName is error")
    if type(TimeOut) != float and type(TimeOut) != int:
        TimeOut = 5.0
        MSGLogger.error("SelectionList InputParams error:TimeOut is error")
    if type(TimeOut) == float:
        TimeOut = math.ceil(TimeOut)
    result = -1
    SelectedIndex = -1
    value = None
    flag = -1
    result = -1 
    ack = {}
    msg = SelectionListIntegrate_T(Title, Message, MessagBoxButton , SelectionItems, VarName, UserData,TimeOut)
    flag,stream = SendAndReceiveMSG(msg,TimeOut,"SelectionList")
    if flag:
        ack = StreamUnpack(stream)
        result = ack["result"]
        SelectedIndex = ack["selectedIndex"]
        if SelectedIndex>(len(SelectionItems)-1) or SelectedIndex <0:
            MSGLogger.error("SelectionList:Cloude input index out of SelectionItems's size")
    MSGLogger.debug({'Title':Title,'Message':Message,'MessagBoxButton':MessagBoxButton,'SelectionItems':SelectionItems,'VarName':VarName,'TimeOut':TimeOut,'result':result,'SelectedIndex':SelectedIndex})
    MSGLogger.debug("SelectionList rec pdu:"+str(ack))
    return result, SelectedIndex