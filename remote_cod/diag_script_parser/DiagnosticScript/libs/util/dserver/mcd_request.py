from .mcd_object import MCDObject
from .mcd_parameter import MCDRequestParameters

class MCDRequest(MCDObject):
	
	def __init__(self, dbRequest):
		super().__init__()
		self.__dbRequest = dbRequest
		self.__pdu = None
		self.__parameterCollection = MCDRequestParameters(self.__dbRequest.getDbParameters())

	def getDbObject(self):
		return self.__dbRequest

	def getRequestParameters(self):
		return self.__parameterCollection

	# hex service
	def getPDU(self):
		return self.__pdu

	def hasPDU(self):
		return self.__pdu != None

	def setPDU(self, pdu):
		self.__pdu = pdu