import sys
from concurrent.futures import _base
from ..endpoint.client_endpoint import  StreamRequestHandler
from ..endpoint.tcp_client_endpoint import TcpClient
from ..DiagLog import MSGLogger,PDULOGER
import threading
from enum import IntEnum

class FrameType(IntEnum):
    CAN = 0
    CANFD = 1
    DOIP = 2

# DoIPProtocolVersion = b'\x02'
# DoIPInvProtocolVersion = b'\xfd'
# DoIPPayloadTypeDiagnostic = b'\x80\x01'

doipHeaderLen = 8
doIpPayloadTypeOffset = 2
doIpPayloadLenOffset = 4

UdsPayloadOffset = 4

client_tcp = None
doip_channels = dict()

def handerMessage(message, idoipHeaderLen, iUdsPayloadOffset):
    if len(doip_channels.values()) != 1:
        raise RuntimeError()
    # list(doip_channels.values())[0].onMessage(message[4:])
    list(doip_channels.values())[0].onDoIPMessage(message, idoipHeaderLen, iUdsPayloadOffset) # Modifications caused by CAN type messages

class DoipMsgHandler(StreamRequestHandler):
    request = bytes()
    def __init__(self, request):
        super().__init__(request)

    def handleOneRequest(self):
        try:
            # read doip header
            if len(DoipMsgHandler.request) < doipHeaderLen:
                request = self.rfile.read(doipHeaderLen - len(DoipMsgHandler.request))
                if len(request) == 0:
                    # shutdown, need a lock
                    list(doip_channels.values())[0].close()
                DoipMsgHandler.request += request

            if len(DoipMsgHandler.request) < doipHeaderLen:
                return

            totalLen = (DoipMsgHandler.request[doIpPayloadLenOffset]<<24) + (DoipMsgHandler.request[doIpPayloadLenOffset+1] << 16) + \
                (DoipMsgHandler.request[doIpPayloadLenOffset+2]<<8) + DoipMsgHandler.request[doIpPayloadLenOffset+3] + doipHeaderLen

            request = self.rfile.read(totalLen - len(DoipMsgHandler.request))
            if len(request) == 0:
                # shutdown, need a lock
                list(doip_channels.values())[0].close()
            DoipMsgHandler.request += request

            if len(DoipMsgHandler.request) < totalLen:
                return
            # PDULOGER.debug(format(DoipMsgHandler.request.hex())) #Modifications caused by CAN type messages
            msgType = (DoipMsgHandler.request[doIpPayloadTypeOffset]<<8) + DoipMsgHandler.request[doIpPayloadTypeOffset+1]
            if msgType == 0x8001:
                # handerMessage(DoipMsgHandler.request[doipHeaderLen:])
                handerMessage(DoipMsgHandler.request, doipHeaderLen, UdsPayloadOffset) #Modifications caused by CAN type messages
                if DoipMsgHandler.request[doipHeaderLen:][4:5] == b'\x7f' and DoipMsgHandler.request[doipHeaderLen:][6:7] == b'\x78':
                    DoipMsgHandler.request = bytes()
                    self.handleOneRequest()
            elif msgType == 0x8002:
                DoipMsgHandler.request = bytes()
                self.handleOneRequest()
            elif msgType == 0x8003:
                DoipMsgHandler.request = bytes()
                self.handleOneRequest()
            elif msgType == 0x0006:
                DoipMsgHandler.request = bytes()
                self.handleOneRequest()
            DoipMsgHandler.request = bytes()
            #print(str(sys._getframe().f_lineno) + " handleOneRequest.")

        except TimeoutError as e:
            MSGLogger.error(e)
            return
        except OSError as e:
            MSGLogger.error(e)
            return
        except Exception as e:
            MSGLogger.error(e)
            return

    def handle(self):
        self.handleOneRequest()

class DoipChannel():
    def __init__(self, address, port):
        super().__init__()
        self.__tcpClient = TcpClient((address, port), DoipMsgHandler)
        self.__sendQueue = []
        self.__comParam = dict(DiagnosticAddress="0e80", FunctionalAddress="e400")
        self.__sourceAddr = bytearray.fromhex(self.__comParam['DiagnosticAddress'])
        self.__destAddr = bytearray.fromhex(self.__comParam['FunctionalAddress'])
        self.__pendingRequest = None
        self.__pendingRequestDpdu = None
        self.__pending = None
        self.__p2_client_max = None
        self.__p2_star_client_max = None
        self.__lock = threading.RLock()
        self.__rawdoippud = None
        key = str(port) + address
        global doip_channels
        doip_channels[key] = self
        self.__iscan = False # Modifications caused by CAN type messages
        self.__realSA = None # Modifications caused by CAN type messages
        self.__future = None
        self.__recv_future = None

    def start(self):
        if 0 == self.__tcpClient.connect_server():
            self.__tcpClient.start()
            self.__tcpClient.connect_status = True
            # self.sendRoutinActivation()
            return True
        else:
            MSGLogger.debug("Connection Warning:Server Not Found")
            return False

    def stop(self):
        return self.__tcpClient.stop()

    def close(self):
        self.__tcpClient.client_close()

    def _send(self, data):
        # construct pdu
        doipVersion = bytearray(b'\x02\xfd')
        payloadType = bytearray(b'\x80\x01')
        length = len(data)
        payloadLength = bytes.fromhex('{:08X}'.format(length))

        # send
        msg = doipVersion + payloadType + payloadLength + self.__sourceAddr + self.__destAddr + data
        PDULOGER.debug(format(msg.hex()),12)
        if self.__tcpClient:
            try:
                self.__tcpClient.send(msg)
            except OSError:
                MSGLogger.debug('error:OSError occurred during processing of send')

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
        self._send(pdu)
        return future

    def onMessage(self, msg):
        if self.__pendingRequest:
            self.__rawdoippud = msg
            ret, res = self.__pendingRequest[0].processResponse(self.__pendingRequest[1], msg)
            if not ret:
                print("set")
                self.__pendingRequest[2].set_result(res)
                self.__pendingRequest = None

    def getRawPdu(self):
        return self.__rawdoippud

    def startComPrimitives(self, pdu, RequestFrameType, ResponseID, RequestID, sendAndRecv):
        # sa = ResponseID, ta = RequestID
        data = bytes()
        if not ResponseID:
            sa = self.__sourceAddr
        else:
            sa = bytes.fromhex('{:04X}'.format(ResponseID))
        if not RequestID:
            ta = self.__destAddr
        else:
            ta = bytes.fromhex('{:04X}'.format(RequestID))

        # replace send sa to default sa (0x0e80), CAN/CANFD recv sa = sa
        if (RequestFrameType == FrameType.CAN) or (RequestFrameType == FrameType.CANFD):
            sendSa = self.__sourceAddr
            recvSa = ta
            self.__iscan = True
            self.__realSA = bytes.fromhex('{:04X}'.format(ResponseID))
        else:
            sendSa = sa
            recvSa = ta
        sendTa = ta
        recvTa = sendSa

        # payload
        payload = bytes.fromhex(pdu)

        # service id
        request_sid = payload[0:1]
        request_sid_int_value = int.from_bytes(request_sid, byteorder='little')
        response_sid_int_value = request_sid_int_value + 0x40
        response_sid = response_sid_int_value.to_bytes(1, byteorder='big')

        # send data
        data = sendSa + sendTa + payload

        # send and recv
        isTimeout = False
        self.__pending = False

        if sendAndRecv:
            self.__lock.acquire()
            self.__future =  _base.Future()
            self.__pendingRequestDpdu = (recvSa, recvTa, request_sid, response_sid, self.__future)
            send = self.sendDoIPMessage(data)

            if not send:
                return send, isTimeout

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
                        self.__pendingRequestDpdu = (recvSa, recvTa, request_sid, response_sid, self.__future)
                        if (self.__recv_future != None and (not self.__recv_future.done())):
                            self.__recv_future.set_result(True)
                    else:
                        break

            self.__lock.release()
            return send, isTimeout, self.__future

        else:
            send = self.sendDoIPMessage(data)
            return send, False, None

    def sendRoutinActivation(self):
        data = b'\x02\xfd\x00\x05\x00\x00\x00\x07' + self.__sourceAddr + b'\x00\x00\x00\x00\x00'
        if self.__tcpClient:
            self.__tcpClient.send(data)
        PDULOGER.debug(format(data.hex()))

    def sendDoIPMessage(self, data):
        # construct pdu
        doipVersion = bytearray(b'\x02\xfd')
        payloadType = bytearray(b'\x80\x01')
        length = len(data)
        payloadLength = bytes.fromhex('{:08X}'.format(length))

        # send
        msg = doipVersion + payloadType + payloadLength + data
        if self.__tcpClient:
            try:
                if not self.__tcpClient.send(msg):
                    return False
                PDULOGER.debug(format(msg.hex()))
            except OSError:
                return False
        return True

    def onDoIPMessage(self, msg, idoipHeaderLen, iUdsPayloadOffset):
        if self.__pendingRequestDpdu:
            recv, pending = self.processResponse(self.__pendingRequestDpdu[0], self.__pendingRequestDpdu[1], self.__pendingRequestDpdu[2], self.__pendingRequestDpdu[3], msg, idoipHeaderLen)
            if recv:
                self.__pending = pending
                self.__recv_future =  _base.Future()
                if (self.__pendingRequestDpdu[4] != None and (not self.__pendingRequestDpdu[4].done())):
                    self.__pendingRequestDpdu[4].set_result((msg[idoipHeaderLen + iUdsPayloadOffset:]))
                if self.__pending:
                    self.__recv_future.result()

    def processResponse(self, sa, ta, request_sid, response_sid, orgmsg, idoipHeaderLen):
        ReceviedMessage = False
        PendingMessage = False
        msg = orgmsg[idoipHeaderLen:]
        if not self.__iscan:
            PDULOGER.debug(format(orgmsg.hex()))
        else:
            mes1 = orgmsg[0:idoipHeaderLen] + self.__realSA + orgmsg[idoipHeaderLen+2:]
            PDULOGER.debug(format(mes1.hex()))
        if (msg[0:2] == sa) and (msg[2:4] == ta):
            if (msg[4:5] == response_sid):
                ReceviedMessage = True
            elif (msg[4:5] == b'\x7f') and (msg[5:6] == request_sid):
                ReceviedMessage = True
                if (msg[6:7] == b'\x78'):
                    PendingMessage = True
            else:
                MSGLogger.error('data service error!')
        else:
            MSGLogger.error('data address error!')
        return ReceviedMessage, PendingMessage
    
    def TransmitAndReceive(self, pdu, ResponseID, RequestID, TargetResponse):
        return False, False, None, None