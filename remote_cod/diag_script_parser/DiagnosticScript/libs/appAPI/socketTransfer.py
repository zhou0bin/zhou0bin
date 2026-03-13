# -*- coding: utf-8 -*-  
#from socket import *
from socket import *
import os
import json
import struct
import traceback
# from click import confirm
from .log_info import MSGLogger,RegistInteractCallback,DeleteRegist,PATHCONFIG,IS_SECURE_STARTUP
from enum import Enum
class Interact(Enum):
    MESSAGEBOX = 0X01
    INPUTBOX = 0x02
    SELECTION = 0X03
    INFORMATION = 0X04
    OTHER = 0X05

def recvMsg(clientfd):
    
    valueLen = clientfd.recv(1)
    if valueLen:
        valueLen=int.from_bytes(valueLen, byteorder='big')
        data = clientfd.recv(valueLen)
        data = data.hex()
    return data

def socketTransfer(msg, TimeOut):
    sock = socket.socket()
    try:
        HOST, PORT = 'localhost', 13400
        msgLen = len(msg).to_bytes(1, byteorder='big')
        msgBytearray =  bytearray()
        msgBytearray = msgLen + msg
        sock.connect((HOST, PORT))
        sock.send(msgBytearray)
        sock.settimeout(TimeOut)
        received = recvMsg(sock)
        sock.close()
        return received
    except Exception:
        sock.close()
        MSGLogger.error({'The connetion fails'})
        raise
    finally:
        pass

def StopSocket(fd):
    try:
        fd.shutdown(SHUT_RDWR)
    except:
        MSGLogger.error("Shoutdown Fail")
        pass

def SendAndReceiveMSG(msg,timeout,types,needrecv = True):
    try:
        root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        if IS_SECURE_STARTUP:
            root = os.path.join(root,"util","SecureDiagParsingConfig.json")
        else:
            root = os.path.join(root,"util","DiagParsingConfig.json")
        lockpath = ""
        if os.path.exists(root):
            with open(root,"r") as fd:
                js = json.load(fd)
                lockpath = js["Interact_Pip_File"]
        else:
            MSGLogger.error("SendAndReceiveMSG:no Interact_Pip_File")
            return False,None
        clientfd = socket(AF_UNIX,SOCK_STREAM)
        RegistInteractCallback(types,StopSocket,clientfd)
        if timeout:
            clientfd.settimeout(timeout)
        clientfd.connect(lockpath)
        lent = clientfd.send(msg)
        if lent == len(msg):
            MSGLogger.debug("SendAndReceiveMSG:send msg success!"+str(msg))
        else:
            MSGLogger.debug("SendAndReceiveMSG:send msg fail!"+str(msg))
        if needrecv:
            RawData = clientfd.recv(4)
            if(len(RawData) < 4):
                MSGLogger.error("SendAndReceiveMSG:recv data error")
                DeleteRegist(types)
                return False,None
            lent = struct.unpack("<i",RawData[0:4])[0]
            RawData += clientfd.recv(lent-4)
            if(len(RawData) < lent):
                MSGLogger.error("SendAndReceiveMSG:recv data error")
                DeleteRegist(types)
                return False,None
            MSGLogger.debug("SendAndReceiveMSG::raw data:" + str(RawData))
            DeleteRegist(types)
            return True,RawData
        else:
            DeleteRegist(types)
            return True,None
    except:
        MSGLogger.critical(traceback.format_exc())
        DeleteRegist(types)
        return False,None
def StreamUnpack(stream:bytearray):
    MSGLogger.debug("StreamUnpack::raw data:" + str(stream))
    ackst = {}
    ackst["tollen"] = 0
    ackst["acktype"] = Interact.OTHER
    ackst["result"] = -1
    ackst["selectedIndex"] = -1
    ackst["infoNum"] = 0
    ackst["infos"] = []
    pos = 0
    if len(stream):
        ackst["tollen"] = struct.unpack("<i",stream[pos:pos+4])[0]
        pos+=4
        type = struct.unpack("<i",stream[pos:pos+4])[0]
        if type == Interact.MESSAGEBOX.value:
            ackst["acktype"] = Interact.MESSAGEBOX
        if type == Interact.INPUTBOX.value:
            ackst["acktype"] = Interact.INPUTBOX
        if type == Interact.SELECTION.value:
            ackst["acktype"] = Interact.SELECTION
        if type == Interact.INFORMATION.value:
            ackst["acktype"] = Interact.INFORMATION
        if type == Interact.OTHER.value:
            ackst["acktype"] = Interact.OTHER
        pos+=4
        ackst["result"] = struct.unpack("<i",stream[pos:pos+4])[0]
        pos+=4
        ackst["selectedIndex"] = struct.unpack("<i",stream[pos:pos+4])[0]
        pos+=4
        ackst["infoNum"] = struct.unpack("<i",stream[pos:pos+4])[0]
        pos+=4
        for i in range(ackst["infoNum"]):
            lent = struct.unpack("<i",stream[pos:pos+4])[0]
            pos+=4
            ackst["infos"].append(stream[pos:pos + lent].decode())
            pos+=lent

    return ackst


def InputMessageIntegrate_T(Title, Message, NumberOfInputs,MessagBoxButton , VarType, regExpression,UserData,TimeOut):
    message = bytes()
    mesdic = {}
    try:
        mesdic["taskTimeout"] = TimeOut
        mesdic["needResponse"] = 1
        mesdic["interactParameter"] = {}
        mesdic["interactParameter"]["interactType"] = "2"
        mesdic["interactParameter"]["title"] = Title
        mesdic["interactParameter"]["message"] = Message
        mesdic["interactParameter"]["numberOfInputs"] = NumberOfInputs
        mesdic["interactParameter"]["messageBoxButton"] = MessagBoxButton
        mesdic["interactParameter"]["varType"] = VarType
        mesdic["interactParameter"]["regExpression"] = regExpression
        mesdic["interactParameter"]["userData"] = UserData
        message += Interact.INPUTBOX.value.to_bytes(4,byteorder = "little") 
        # message += int(TimeOut).to_bytes(1,byteorder = "little")
        message += struct.pack("<f",TimeOut)
        message += bytes(json.dumps(mesdic,ensure_ascii=False),"utf-8")
    except:
        message = bytes()
    MSGLogger.debug("InputMessageIntegrate_T::pack message:" + str(message))
    return message


def ConfirmMessageIntegrate_T(Title, Message, MessagBoxButton ,MeesageBoxIcon, UserData,TimeOut):
    message = bytes()
    mesdic = {}
    try:
        mesdic["taskTimeout"] = TimeOut
        mesdic["needResponse"] = 1
        mesdic["interactParameter"] = {}
        mesdic["interactParameter"]["interactType"] = "1"
        mesdic["interactParameter"]["title"] = Title
        mesdic["interactParameter"]["message"] = Message
        mesdic["interactParameter"]["messageBoxButton"] = MessagBoxButton
        mesdic["interactParameter"]["meesageBoxIcon"] = MeesageBoxIcon
        mesdic["interactParameter"]["userData"] = UserData
        message += Interact.MESSAGEBOX.value.to_bytes(4,byteorder = "little") 
        message += struct.pack("<f",TimeOut)
        message += bytes(json.dumps(mesdic,ensure_ascii=False),"utf-8")
    except:
        message = bytes()
    MSGLogger.debug("ConfirmMessageIntegrate_T::pack message:" + str(message))
    return message

def InformationIntegrate_T(Title,Message,UserData):
    message = bytes()
    mesdic = {}
    try:
        mesdic["taskTimeout"] = -1
        mesdic["needResponse"] = 0
        mesdic["interactParameter"] = {}
        mesdic["interactParameter"]["interactType"] = "4"
        mesdic["interactParameter"]["title"] = Title
        mesdic["interactParameter"]["message"] = Message
        mesdic["interactParameter"]["userData"] = UserData
        message += Interact.INFORMATION.value.to_bytes(4,byteorder = "little") 
        message += struct.pack("<f",-1.0)
        message += bytes(json.dumps(mesdic,ensure_ascii=False),"utf-8")
    except:
        message = bytes()
    MSGLogger.debug("InformationIntegrate_T::pack message:" + str(message))
    return message


def SelectionListIntegrate_T(Title, Message, MessagBoxButton , SelectionItems, VarName, UserData,TimeOut):
    message = bytes()
    mesdic = {}
    try:
        mesdic["taskTimeout"] = TimeOut
        mesdic["needResponse"] = 1
        mesdic["interactParameter"] = {}
        mesdic["interactParameter"]["interactType"] = "3"
        mesdic["interactParameter"]["title"] = Title
        mesdic["interactParameter"]["message"] = Message
        mesdic["interactParameter"]["messageBoxButton"] = MessagBoxButton
        mesdic["interactParameter"]["varName"] = VarName
        mesdic["interactParameter"]["selectionItems"] = SelectionItems
        mesdic["interactParameter"]["userData"] = UserData
        message += Interact.SELECTION.value.to_bytes(4,byteorder = "little") 
        message += struct.pack("<f",TimeOut)
        message += bytes(json.dumps(mesdic,ensure_ascii=False),"utf-8")
    except:
        message = bytes()
    MSGLogger.debug("SelectionListIntegrate_T::pack message:" + str(message))
    return message