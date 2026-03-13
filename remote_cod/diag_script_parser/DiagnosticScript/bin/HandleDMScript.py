import os
import sys
sys.path[0]=(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from socket import *
import select
from threading import Thread
import queue
from enum import Enum, unique
import struct
import json
import imp
from libs.util.DiagLog import MSGLogger

from libs.appAPI.CreateDiagResult import ConditionCheck
class EZIPRESULT(Enum):
    NOFILE = 0X01 #no file in the zip
    NOZIPPACKPATH = 0X02  #no zip
    SUCCESS = 0X03 #success
@unique
class EType(Enum):
    CLOUD     = 0X00 #queue is full
    LOCAL     = 0X01 #script execution successfully
@unique
class ErrorEnum(Enum):
    TOO_MANY      = 0X01 #request queue is full
    EXCUTED_OVER  = 0X02 #script had executed
    EXCUTED_FAILE = 0X03 #script had not executed 
    NOSCRIPT      = 0X04 #find no script
    NOZIP         = 0X05 #the scriptzip does not exist
    COND_NOT_ALLOW= 0X06 #conditions do not allow
    DEC_FAILE     = 0X07 #reserved fields: "decompression failed"
    UNKNOWN       = 0X08 #unknown err


#request queue
#element format:(DiagTpye,SourceAddress,ClientSocket)
QueueSize = 5
ReqQueue = queue.Queue(QueueSize)

#name:ExcuteQueueReuqestTask
#function:python script task
def ExcuteQueueReuqestTask():
    while True:
        queueelement = ReqQueue.get()
        result = ExcuteScriptRun(queueelement)
        try:    
            result[0].send(result[1])
        except Exception as e:
            MSGLogger.critical("SERVER_MSG_S:"+"Server Send "+"Diag Result "+"To Client Fail")
def ExcuteScriptRun(queueelement):
    # excute diag script
    queuescriptinfo = queueelement[0]
    scripyname = queuescriptinfo["name"]
    currentpath = os.getcwd()
    currparentpath = os.path.dirname(os.path.abspath('.'))
    diagtype = queuescriptinfo["diagtype"]
    leng = len(scripyname)
    returnfmt = "!1b1b1b%ds"%leng
    scriptabsname = ""
    condallow:bool = False
    # modulename = ""
    # guidpypath = ""
    # err = 0
    if diagtype == 1:
        # status,Decompressfiles = DecompressArchive("DownLoadPacketSample.zip")
        # if status == DECErrEnum.NO_ARCHIVE.value:
        #     buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.NOZIP.value,scripyname.encode("utf-8"))
        #     return queueelement[1],buffer
        # if status == DECErrEnum.DECOMPRESS_FAILE.value:
        #     buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.DEC_FAILE.value,scripyname.encode("utf-8"))
        #     return queueelement[1],buffer 
        cond_path = os.path.join(os.path.dirname(os.path.abspath('.')),"scriptFile","temporary")
        for dirname in os.listdir(cond_path):
            if dirname.find(scripyname) != -1 and os.path.isdir(os.path.join(cond_path,dirname)):
                for file in os.listdir(os.path.join(cond_path,dirname)):
                    if file.find(scripyname + ".py") != -1 and os.path.isfile(os.path.join(cond_path,dirname,file)):
                        cond_path = os.path.join(cond_path,dirname)
                        break
                break
        condallow,conditions = ConditionCheck(cond_path)
        if not condallow:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.COND_NOT_ALLOW.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer
        dir = os.path.join(currparentpath ,"scriptFile","temporary",scripyname)
        for file in os.listdir(dir):
            if file.find(scripyname + ".py") == -1:
                continue
            else:
                scriptabsname = os.path.join(dir,file)
                # modulename = file.split(".")[0]
                # guidpypath = dir
                break
        if scriptabsname == "" :
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.NOSCRIPT.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer
        os.chdir(dir)
        cmd = "python3 " + scriptabsname
        err = os.system(cmd)
        # try:
        #     modul, path_name, description = imp.find_module(modulename,[guidpypath])
        #     callprocedure = imp.load_module(modulename,modul, path_name, description)
        #     modul.close()
        #     callprocedure.main()
        #     err = 0
        # except:
        #     err = -1
        CompressedLog()
        # ClearDecompressfiles(Decompressfiles)
        if err == 0:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.EXCUTED_OVER.value,scripyname.encode("utf-8"))
            os.chdir(currentpath)
            return queueelement[1],buffer
        else:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.EXCUTED_FAILE.value,scripyname.encode("utf-8"))
            os.chdir(currentpath)
            return queueelement[1],buffer
    elif diagtype == 2:
        cond_path = os.path.join(os.path.dirname(os.path.abspath('.')),"scriptFile","fixed")
        for dirname in os.listdir(cond_path):
            if dirname.find(scripyname) != -1 and os.path.isdir(os.path.join(cond_path,dirname)):
                for file in os.listdir(os.path.join(cond_path,dirname)):
                    if file.find(scripyname + ".py") != -1 and os.path.isfile(os.path.join(cond_path,dirname,file)):
                        cond_path = os.path.join(cond_path,dirname)
                        break
                break
        condallow,conditions = ConditionCheck(cond_path)
        if not condallow:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.COND_NOT_ALLOW.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer
        dir = os.path.join(currparentpath ,"scriptFile","fixed",scripyname)
        for file in os.listdir(dir):
            if file.find(scripyname+".py") == -1:
                continue
            else:
                scriptabsname = os.path.join(dir,file)
                # modulename = file.split(".")[0]
                # guidpypath = dir
                break
        if scriptabsname == "" :
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.NOSCRIPT.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer

        os.chdir(dir)
        cmd = "python3 "+ scriptabsname
        err = os.system(cmd)
        os.chdir(currentpath)
        # try:
        #     modul, path_name, description = imp.find_module(modulename,[guidpypath])
        #     callprocedure = imp.load_module(modulename,modul, path_name, description)
        #     modul.close()
        #     callprocedure.main()
        #     err = 0
        # except:
        #     err = -1
        CompressedLog()
        if err == 0:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.EXCUTED_OVER.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer
        else:
            buffer = struct.pack(returnfmt,leng+3,diagtype,ErrorEnum.EXCUTED_FAILE.value,scripyname.encode("utf-8"))
            return queueelement[1],buffer
#name:DiagScriptPorcess
#function:receive request and put request into queue 
#param:Enum=script type,SourceAddress=script SourceAddress,ClientSocket=client'socket which is online
def DiagScriptPorcess(DiagInfo,ClientSocket):
    if ReqQueue.full():
        try:
            scriptname = DiagInfo["name"]
            leng = len(scriptname)
            fmt = "!1b1b1b%ds"%leng
            buffer = struct.pack(fmt,leng+1+1+1,DiagInfo["diagtype"],ErrorEnum.TOO_MANY.value,scriptname.encode("utf-8"))
            ClientSocket.send(buffer)
        except Exception as e:
            MSGLogger.critical("SERVER_MSG_S:"+"Server Send "+str(scriptname)+" Error Code "+str(ErrorEnum.TOO_MANY.value)+"To Client Fail")
    else :
        ReqQueue.put((DiagInfo,ClientSocket))
def ClearDecompressfiles(files:dict,isremoveArchive:bool = False):
    for key,value in files.items():
        if isremoveArchive:
            if key == "ArchivePath" and os.path.exists(value):
                os.remove(value)
        if key == "DecompressFilePath":
            for path in value:
                if os.path.exists(path):
                    os.remove(path)


def HandleInterSocketConnection(ip,port):
    sock_server = socket(AF_INET, SOCK_STREAM)
    sock_server.setsockopt(SOL_SOCKET,SO_REUSEADDR,1)
    sock_server.bind((ip, port))
    sock_server.listen(5)
    #create epoll
    epoll = select.epoll()
    epoll.register(sock_server.fileno(), select.EPOLLIN)
    # create dict:save client socket:key-socket'fd;value-client socket
    sock_dicts = {}
    # create dict:save client socket address:key-socket'fd;value-client socket address
    client_dicts = {}
    print("Inter Server start")
    while True:
        #waiting for socket change;saving socket which had changed in poll_list;return socket'fd and events in poll_list
        poll_list = epoll.poll()
        for sock_fileno, events in poll_list:
            # judge the service socket is actived,if it was actived,it represets there is a new client connection
            if sock_fileno == sock_server.fileno():
                # accpet client and create new client socket
                new_sock, client_info = sock_server.accept()
                # regist new client socket into epoll
                epoll.register(new_sock.fileno(), select.EPOLLIN)
                # add new client socket to socket_dict
                sock_dicts[new_sock.fileno()] = new_sock
                # add new client socket address in client_dicts
                client_dicts[new_sock.fileno()] = client_info
            elif events & select.EPOLLIN:
                # The following is to handle a receive data from client socket
                # deal with client unnormal disconnection
                try:
                    RawData = sock_dicts[sock_fileno].recv(1)
                    if len(RawData) == 0:
                        epoll.unregister(sock_fileno)
                        sock_dicts[sock_fileno].close()
                        del sock_dicts[sock_fileno]
                        del client_dicts[sock_fileno]
                        continue
                    Len = RawData[0]
                    RawData = sock_dicts[sock_fileno].recv(1)
                    diagtype = int.from_bytes(RawData,byteorder='big',signed=False)
                    RawData = sock_dicts[sock_fileno].recv(Len-2)
                    scriptname = bytes.decode(RawData)
                    DiagScriptPorcess({"name":scriptname,"diagtype":diagtype},sock_dicts[sock_fileno])
                except:
                    epoll.unregister(sock_fileno)
                    sock_dicts[sock_fileno].close()
                    # delete refrence data
                    del sock_dicts[sock_fileno]
                    del client_dicts[sock_fileno]
                    continue

            elif events & select.EPOLLHUP:
                #it represets the client had disconnected
                # unregist the socket form epoll
                epoll.unregister(sock_fileno)
                sock_dicts[sock_fileno].close()
                # delete refrence data
                del sock_dicts[sock_fileno]
                del client_dicts[sock_fileno]

def HandleLocalSocketConnection():
    localsockfd = socket(AF_UNIX, SOCK_STREAM)
    localsockfd.setsockopt(SOL_SOCKET,SO_REUSEADDR,1)
    localsockfd.bind(localADD)
    localsockfd.listen(5)
    epoll = select.epoll()
    epoll.register(localsockfd.fileno(), select.EPOLLIN)
    sock_dicts = {}
    client_dicts = {}
    print("Local Server start")
    while True:
        poll_list = epoll.poll()
        for sock_fileno, events in poll_list:
            if sock_fileno == localsockfd.fileno():
                # accpet client and create new client socket
                new_sock, client_info = localsockfd.accept()
                # regist new client socket into epoll
                epoll.register(new_sock.fileno(), select.EPOLLIN)
                # add new client socket to socket_dict
                sock_dicts[new_sock.fileno()] = new_sock
                # add new client socket address in client_dicts
                client_dicts[new_sock.fileno()] = client_info
            elif events & select.EPOLLIN:
                # The following is to handle a receive data from client socket
                # deal with client unnormal disconnection
                try:
                    RawData = sock_dicts[sock_fileno].recv(1)
                    if len(RawData) == 0:
                        epoll.unregister(sock_fileno)
                        sock_dicts[sock_fileno].close()
                        del sock_dicts[sock_fileno]
                        del client_dicts[sock_fileno]
                        continue
                    Len = RawData[0]
                    RawData = sock_dicts[sock_fileno].recv(1)
                    diagtype = int.from_bytes(RawData,byteorder='big',signed=False)
                    RawData = sock_dicts[sock_fileno].recv(Len-2)
                    scriptname = bytes.decode(RawData)
                    
                    DiagScriptPorcess({"name":scriptname,"diagtype":diagtype},sock_dicts[sock_fileno])
                except:
                    epoll.unregister(sock_fileno)
                    sock_dicts[sock_fileno].close()
                    # delete refrence data
                    del sock_dicts[sock_fileno]
                    del client_dicts[sock_fileno]
                    continue

            elif events & select.EPOLLHUP:
                #it represets the client had disconnected
                # unregist the socket form epoll
                epoll.unregister(sock_fileno)
                sock_dicts[sock_fileno].close()
                # delete refrence data
                del sock_dicts[sock_fileno]
                del client_dicts[sock_fileno]

def GetConfig():
    conparentpath = os.path.dirname(os.path.dirname(os.path.abspath('.')))
    configfp = open(os.path.join(conparentpath,"DiagnosticScript","libs","util","DiagParsingConfig.json"),"r")
    configobj = json.load(configfp)
    configfp.close()
    return configobj

def main():
    global localADD
    if os.path.exists(localADD):
        os.remove(localADD)
    RunScriptThread = Thread(target=ExcuteQueueReuqestTask,name = "Task_Thread")
    RunScriptThread.start()
    # future = ThreadPool.submit(ExcuteScriptRun)
    # SocketInfo = (ConfigObj["Ip"],ConfigObj["Port"])
    # Inter_Server = Thread(target = HandleInterSocketConnection,args = SocketInfo,name = "Inter_Server_Thread")
    # Inter_Server.start()
    Local_Server = Thread(target = HandleLocalSocketConnection,name = "Local_Server_Thread")
    Local_Server.start()
ConfigObj = GetConfig()
# localADD = os.path.join(os.path.abspath('.'), "asf_ds_socket_pip_file.sock")
localADD = ConfigObj["Socket_Pip_File"]
main()


