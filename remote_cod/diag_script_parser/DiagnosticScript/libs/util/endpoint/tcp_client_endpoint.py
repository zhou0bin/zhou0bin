import socket
from .client_endpoint import ClientEndpoint
from ...appAPI.log_info import RegistInteractCallback,DeleteRegist

class TcpClient(ClientEndpoint):

	address_family = socket.AF_INET
	socket_type = socket.SOCK_STREAM
	allow_reuse_address = False

	def __init__(self, server_address, DoipHandlerClass):
		super().__init__(server_address, self.address_family, self.socket_type, DoipHandlerClass)

	def connect_server(self):
		# conn_timeout = 5
		if self.allow_reuse_address:
			self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		# import os
		# configfile = os.path.join(os.path.dirname(os.path.dirname(__file__)),"communicationConfig.json")
		# if os.path.exists(configfile):
		# 	import json
		# 	with open(configfile,"r") as fd:
		# 		config = json.load(fd)
		# 		conn_timeout = config["socket_connect_timeout"]
		# 		fd.close()
		RegistInteractCallback("PDUOpenChannel",self.shutdown,None)
		# self.socket.settimeout(conn_timeout)
		ret = self.socket.connect_ex(self.server_address)
		DeleteRegist("PDUOpenChannel")
		self.server_address = self.socket.getsockname()
		# if ret != 0:
			# self.connect_status = False
		return ret

	def shutdown(self,fd):
		try:
			self.socket.shutdown(socket.SHUT_RDWR)
		except:
			pass

	def fileno(self):
		return self.socket.fileno()

	def finish_request(self):
		self.DoipHandlerClass.handle()

	def get_request(self):
		return self.socket, self.server_address

	def process_request(self, request, address):
		try:
			self.finish_request()
		except Exception:
			self.handle_error(address)
			# connect ? 
		finally:
			pass

	def _handle_request_noblock(self):
		try:
			request, address = self.get_request()
		except OSError:
			return
		try:
			self.process_request(request, address)
		except Exception:
			self.handle_error(address)
		except:
			raise
