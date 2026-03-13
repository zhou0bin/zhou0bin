import socket
import selectors
import sys
import threading
from io import BufferedIOBase
from time import monotonic as time

if hasattr(selectors, 'PollSelector'):
	_ServerSelector = selectors.PollSelector
else:
	_ServerSelector = selectors.SelectSelector

class ClientEndpoint(object):
	timeout = None
	thread_cnt = 0
	wbufsize = 0
	def __init__(self, server_address, address_family, socket_type, DoipHandlerClass):
		super().__init__()
		self.server_address = server_address
		# self.DoipHandlerClass = DoipHandlerClass
		self.__thread = threading.Thread(target = self.handle_receive)
		self.__thread.setName("Recv[]"+ str(ClientEndpoint.thread_cnt))
		ClientEndpoint.thread_cnt += 1
		self.__stop = False
		self.connect_status = False
		self.socket = socket.socket(address_family, socket_type)
		if self.wbufsize == 0:
				self.wfile = _SocketWriter(self.socket)
		else:
			self.wfile = self.socket.makefile('wb', self.wbufsize)
		self.DoipHandlerClass = DoipHandlerClass(self.socket)

	def send(self, data):
		try:
			if self.connect_status == False:
				return False
			self.wfile.write(data)
			self.wfile.flush()
			return True
		except OSError as e:
			raise OSError

	def handle_receive(self):
		timeout = self.socket.gettimeout()
		if timeout is None:
			timeout = self.timeout
		elif self.timeout is not None:
			timeout = min(timeout, self.timeout)
		if timeout is not None:
			deadline = time() + timeout
		with _ServerSelector() as selector:
			selector.register(self, selectors.EVENT_READ)
			while not self.__stop:
				ready = selector.select(1)
				if ready:
					self._handle_request_noblock()
				else:
					if timeout is not None:
						timeout = deadline - time()
						if timeout < 0:
							self.handle_timeout()

	def handle_error(self, address):
		print('-'*40, file=sys.stderr)
		print('Exception occurred during processing of request ', address, file=sys.stderr)
		print('-'*40, file=sys.stderr)

	def handle_timeout(self):
		pass

	def finish_request(self, request, address):
		# self.DoipHandlerClass(request, address)
		pass

	def start(self):
		self.__thread.setDaemon(True)
		self.__thread.start()

	def stop(self):
		try:
			self.__stop = True
			# may use pipe to substitute
			self.socket.shutdown(socket.SHUT_RDWR)
			self.socket.close()
			if self.connect_status:
				self.connect_status = False
			self.__thread.join()
		except:
			pass
		return True

	def client_close(self):
		self.socket.close()
		if self.connect_status:
			self.connect_status = False

	def setUdsHandlerClass(self, UdsHandlerClass):
		self.UdsHandlerClass = UdsHandlerClass

class BaseRequestHandler:
	def __init__(self, request):
		self.request = request
		self.totalLen = 0
		self.rawRequest = bytes()
		self.setup()
		# try:
		# 	self.handle()
		# finally:
		# 	self.finish()

	def setup(self):
		pass

	def handle(self):
		pass

	def finish(self):
		pass

class StreamRequestHandler(BaseRequestHandler):
	rbufsize = -1
	timeout = None

	disable_nagle_algorithm = False

	def __init__(self, request):
		super().__init__(request)

	def setup(self):
		self.connection = self.request
		if self.timeout is not None:
			self.connection.settimeout(self.timeout)
		if self.disable_nagle_algorithm:
			self.connection.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, True)
		self.rfile = self.connection.makefile('rb', self.rbufsize)

	def finish(self):
		pass

class _SocketWriter(BufferedIOBase):
    def __init__(self, sock):
        self._sock = sock

    def writable(self):
        return True

    def write(self, b):
        self._sock.sendall(b)
        with memoryview(b) as view:
            return view.nbytes

    def fileno(self):
        return self._sock.fileno()
