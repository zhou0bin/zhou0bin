
from .mcd_object import MCDObject, MCDCollection
from .mcd_parameter import MCDResponseParameters

class MCDResponse(MCDObject):
	
	def __init__(self, dbResponse):
		super().__init__()
		self.__dbResponse = dbResponse
		self.__parameterCollection = MCDResponseParameters(self.__dbResponse.getDbParameters())

	def getDbObject(self):
		return self.__dbResponse

	def getResponseParameters(self):
		return self.__parameterCollection

class MCDResponses(MCDCollection):
	
	def  __init__(self):
		super().__init__()
	

class MCDResult(MCDObject):
	
	def __init__(self, service=None):
		super().__init__()
		self._service = service
		self.responseCollection = MCDResponses()

	def hasError(self):
		pass

class MCDResults(MCDCollection):
	def __init__(self):
		super().__init__()
	
	def getError(self):
		pass

	def hasOverflow(self):
		pass

	