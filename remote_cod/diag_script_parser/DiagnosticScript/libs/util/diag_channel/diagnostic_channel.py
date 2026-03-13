import threading
from ctypes import *
from ..DiagLog import MSGLogger,PDULOGER
from enum import IntEnum
from concurrent.futures import _base
try:
    from uds_common.uds_channel import DiagnosticChannel, UDSResult
except (ModuleNotFoundError, ImportError) as e:
    pass

def recordMessage(sa, ta, payload):
    sourceAddress = bytes.fromhex('{:04X}'.format(sa))
    targetAddress = bytes.fromhex('{:04X}'.format(ta))
    msg = sourceAddress + targetAddress + bytes.fromhex(payload)
    PDULOGER.debug(format(msg.hex()))

class DiagChannel():
    def __init__(self):
        self.__comParam = dict(DiagnosticAddress="0e80", FunctionalAddress="e400")
        self.__sourceAddr = int(self.__comParam['DiagnosticAddress'], 16)
        self.__destAddr = int(self.__comParam['FunctionalAddress'], 16)
        self.__lock = threading.RLock()
        self.__channel = DiagnosticChannel()

    def start(self):
        return self.__channel.ChannelOpen() 

    def stop(self):
        return self.__channel.ChannelClose()
    
    def send(self, sa, ta, payload):
        return False

    def setCallbackClass(self, callbackClass):
        pass

    def setTimeout(self, p2_client_max, p2_star_client_max):
        pass

    def setComParam(self, param):
        self.__comParam = param

    def getComParam(self):
        return self.__comParam
    
    def TransmitAndReceive(self, pdu, ResponseID, RequestID, TargetResponse):
        sendSa = ResponseID
        if not sendSa:
            sendSa = self.__sourceAddr
        sendTa = RequestID
        if not sendTa:
            sendTa = self.__destAddr 

        self.__lock.acquire()
        recordMessage(sendSa, sendTa, pdu)
        res = self.__channel.TransmitAndReceiveService(sendSa, sendTa, pdu, TargetResponse)
        if res[0] and not res[1]:
            recordMessage(sendTa, sendSa, res[2])
        self.__lock.release()
        return res

    def startComPrimitives(self, pdu, RequestFrameType, ResponseID, RequestID, sendAndRecv):
        sendSa = ResponseID
        if not sendSa:
            sendSa = self.__sourceAddr
        sendTa = RequestID
        if not sendTa:
            sendTa = self.__destAddr

        if sendAndRecv:
            self.__lock.acquire()
            recordMessage(sendTa, sendSa, pdu)
            res = self.__channel.TransmitAndReceiveService(sendSa, sendTa, pdu)
            if res[0] and not res[1]:
                recordMessage(sendTa, sendSa, res[2])
            self.__lock.release()
            future = _base.Future()
            future.set_result(bytes.fromhex(res[2]))
            res[2] = future
            return res
        else:
            if self.__channel.SendService(sendSa, sendTa, pdu) == UDSResult.Success:
                recordMessage(sendSa, sendTa, pdu)
                return True, False, None, None
            else:
                return False, False, None, None
