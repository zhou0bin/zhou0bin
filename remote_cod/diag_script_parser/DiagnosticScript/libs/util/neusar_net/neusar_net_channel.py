import os
import threading
from ctypes import *
from concurrent.futures import _base
from ..DiagLog import MSGLogger,PDULOGER
from enum import IntEnum

class FrameType(IntEnum):
    CAN = 0
    CANFD = 1
    DOIP = 2

UdsAddressLen = 4

lib_uds_api = None
neusar_net_channels = dict()

CallbackType = CFUNCTYPE(None, c_int, c_int, c_int, POINTER(c_ubyte))

def callback(arg1, arg2, arg3, arg4):
    if len(neusar_net_channels.values()) != 1:
        raise RuntimeError()
    # list(neusar_net_channels.values())[0].onMessage(arg1, arg2, arg3, arg4)
    list(neusar_net_channels.values())[0].handleMessage(arg1, arg2, arg3, arg4)

callbackFunc = CallbackType(callback)

def recordMessage(sa, ta, payload):
    doipVersion = bytearray(b'\x02\xfd')
    payloadType = bytearray(b'\x80\x01')
    length = UdsAddressLen + len(payload)
    payloadLength = bytes.fromhex('{:08X}'.format(length))
    sourceAddress = bytes.fromhex('{:04X}'.format(sa))
    targetAddress = bytes.fromhex('{:04X}'.format(ta))
    msg = doipVersion + payloadType + payloadLength + sourceAddress + targetAddress + payload
    PDULOGER.debug(format(msg.hex()))

def GetLibUdsApi(rootpath):
    path = ""
    filePaths = []
    for root,dirs,files in os.walk(rootpath):
        for file in files:
            filePaths.append(os.path.join(root,file))
    for path in filePaths:
        if path.endswith("libasf_UdsInterfaceCtype.so"):
            return path
    return path

class NeusarNetChannel():
    def __init__(self, localadd):
        self.__localAddr = c_char_p(localadd.encode())
        self.__comParam = dict(DiagnosticAddress="0e80", FunctionalAddress="e400")
        self.__sourceAddr = int(self.__comParam['DiagnosticAddress'], 16)
        self.__destAddr = int(self.__comParam['FunctionalAddress'], 16)
        self.__pendingRequest = None
        self.__pendingRequestDpdu = None
        self.__pending = None
        self.__p2_client_max = None
        self.__p2_star_client_max = None
        self.__lock = threading.RLock()
        self.__rawpud = None
        self.__future = None
        self.__recv_future = None
        global neusar_net_channels
        neusar_net_channels[self.__sourceAddr] = self

    def start(self):
        global lib_uds_api
        path = os.getenv("NEUSAR_DEPLOYMENT_PATH", "")
        rootpath = f"{path}/usr/lib"
        abspath = GetLibUdsApi(rootpath)
        if abspath == "":
            MSGLogger.error("NeusarNetChannel: No UdsApi library")
            return False
        lib_uds_api = cdll.LoadLibrary(abspath)
        if lib_uds_api is None:
            return False
        lib_uds_api.Initialize(self.__localAddr)
        lib_uds_api.RegisterCallback(self.__sourceAddr, callbackFunc)
        lib_uds_api.Offer(self.__sourceAddr)
        return True

    def stop(self):
        if lib_uds_api is None:
            return False
        lib_uds_api.StopOffer(self.__sourceAddr)
        return True
    
    def send(self, sa, ta, payload):
        size = len(payload)
        data = (c_ubyte * size).from_buffer_copy(payload)
        if lib_uds_api is not None:
            if lib_uds_api.Request(sa, ta, size, data):
                return True
        return False

    def setCallbackClass(self, callbackClass):
        pass

    def setComParam(self, param):
        self.__comParam = param

    def getComParam(self):
        return self.__comParam

    def setTimeout(self, p2_client_max, p2_star_client_max):
        self.__p2_client_max = p2_client_max
        self.__p2_star_client_max = p2_star_client_max

    def startComPrimitive(self, pdu, HanderResponseRequest, args, type="PDU_COPT_SENDRECV"):
        # testing
        future =  _base.Future()
        if type == "PDU_COPT_SENDRECV":
            self.__pendingRequest = (HanderResponseRequest, args, future)
        self.sendMessage(pdu)
        return future

    def sendMessage(self, data):
        sa = bytes.fromhex('{:04X}'.format(self.__sourceAddr))
        ta = bytes.fromhex('{:04X}'.format(self.__destAddr))
        if self.send(sa, ta, data):
            recordMessage(sa, ta, data)

    def onMessage(self, sa, ta, msg):
        if self.__pendingRequest:
            self.__rawpud = msg
            ret, res = self.__pendingRequest[0].processMessage(self.__pendingRequest[1], sa, ta, msg)
            if not ret:
                print("set")
                self.__pendingRequest[2].set_result(res)
                self.__pendingRequest = None

    def getRawPdu(self):
        return self.__rawpud

    def startComPrimitives(self, pdu, RequestFrameType, ResponseID, RequestID, sendAndRecv):
        # sa = ResponseID, ta = RequestID
        # replace send sa to default sa (0x0e80), CAN/CANFD recv sa = sa
        realSA = ResponseID
        if not realSA:
            realSA = self.__sourceAddr

        sendSa = self.__sourceAddr

        sendTa = RequestID
        if not sendTa:
            sendTa = self.__destAddr

        # payload
        payload = bytes.fromhex(pdu)

        # service id
        request_sid = payload[0:1]
        request_sid_int_value = int.from_bytes(request_sid, byteorder='little')
        response_sid_int_value = request_sid_int_value + 0x40
        response_sid = response_sid_int_value.to_bytes(1, byteorder='big')

        # send and recv
        sendResult = False
        isTimeout = False
        self.__pending = False

        if sendAndRecv:
            self.__lock.acquire()
            self.__future =  _base.Future()
            self.__pendingRequestDpdu = (realSA, sendSa, sendTa, request_sid, response_sid, self.__future)
            if self.send(sendSa, sendTa, payload):
                recordMessage(realSA, sendTa, payload)
                sendResult = True

            if sendResult == False:
                return sendResult, isTimeout

            while not isTimeout:
                if not self.__pending:
                    timeout = self.__p2_client_max / 1000
                else:
                    timeout = self.__p2_star_client_max / 1000
                try:
                    self.__future.result(timeout)
                except _base.TimeoutError:
                    MSGLogger.error("R_TimeoutError,The Request:" + str(pdu) + " Response TimeOut!")
                    isTimeout = True
                else:
                    if self.__pending:
                        self.__future =  _base.Future()
                        self.__pendingRequestDpdu = (realSA, sendSa, sendTa, request_sid, response_sid, self.__future)
                        if (self.__recv_future != None and (not self.__recv_future.done())):
                            self.__recv_future.set_result(True)
                    else:
                        break
            self.__lock.release()
            return sendResult, isTimeout, self.__future

        else:
            if self.send(sendSa, sendTa, payload):
                recordMessage(realSA, sendTa, payload)
                sendResult = True
            return sendResult, False, None

    def handleMessage(self, sa, ta, size, data):
        if size <= 0:
            msg = b''
        else:
            array_type = c_ubyte * size
            byte_array = cast(data, POINTER(array_type)).contents
            msg = bytes(byte_array)
        # MSGLogger.error("handleMessage: " + str(msg))

        if self.__pendingRequestDpdu:
            realSa = self.__pendingRequestDpdu[0]
            SendSa = self.__pendingRequestDpdu[1]
            SendTa = self.__pendingRequestDpdu[2]
            RequestSid = self.__pendingRequestDpdu[3]
            ResponseSid = self.__pendingRequestDpdu[4]
            recv, pending = self.processMessage(realSa, SendSa, SendTa, RequestSid, ResponseSid, sa, ta, msg)
            if recv:
                self.__pending = pending
                self.__recv_future = _base.Future()
                if (self.__pendingRequestDpdu[5] != None and (not self.__pendingRequestDpdu[5].done())):
                    self.__pendingRequestDpdu[5].set_result(msg)
                if self.__pending:
                    self.__recv_future.result()

    def processMessage(self, realSa, SendSa, SendTa, RequestSid, ResponseSid, sa, ta, msg):
        ReceviedMessage = False
        PendingMessage = False
        if (ta == SendSa):
            recordMessage(sa, realSa, msg)
        if (sa == SendTa) and (ta == SendSa):
            if (msg[0:1] == ResponseSid):
                ReceviedMessage = True
            elif (msg[0:1] == b'\x7f') and (msg[1:2] == RequestSid):
                ReceviedMessage = True
                if (msg[2:3] == b'\x78'):
                    PendingMessage = True
            else:
                MSGLogger.error('data service error!')
        else:
            MSGLogger.error('data address error!')
        return ReceviedMessage, PendingMessage
    
    def TransmitAndReceive(self, pdu, ResponseID, RequestID, TargetResponse):
        return False, False, None, None
