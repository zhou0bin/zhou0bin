
from .mcd_object import MCDNamedCollection
from .mcd_object import MCDDbObject
from .mcd_db_request import MCDDbRequest
from .mcd_db_response import MCDDbResponse

class MCDDbDiagComPrimitive(MCDDbObject):
	
	def __init__(self, conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], parent)
		self.__preConditionStateRef = []
		self.__request = None
		self.__posResponse = None
		self.__negResponse = None
		self.__sementic = None
		service = conf['current']
		if service.get('SEMANTIC'):
			self.__sementic = service.get('SEMANTIC')
		if service.get('PRE-CONDITION-STATE-REFS'):
			for state in service.get('PRE-CONDITION-STATE-REFS'):
				self.__preConditionStateRef = state.get('ID-REF')

		self.__stateTransitionRef = []
		if service.get('STATE-TRANSITION-REFS'):
			for state in service.get('STATE-TRANSITION-REFS'):
				self.__stateTransitionRef = state.get('ID-REF')

		request = service.get('REQUEST')
		if not request:
			raise RuntimeError(conf['current']['SHORT-NAME'] + " request not found")
		conf['current'] = request
		self.__request = MCDDbRequest(conf, self)

		posResponse = service.get('POS-RESPONSE')
		if not posResponse:
			raise RuntimeError(conf['current']['SHORT-NAME'] + " request not found")
		conf['current'] = posResponse
		self.__posResponse = MCDDbResponse(conf, self)
		# self.posTargetBytelen = None
		# self.negTargetBytelen = None
		# if len(self.__posResponse.parameters.MCDObjects) != 0:
		# 	self.posTargetBytelen = self.__posResponse.parameters.MCDObjects[-1].getBytePos()
		# 	if self.__posResponse.parameters.MCDObjects[-1].getBitLength() <= 8:
		# 		self.posTargetBytelen += 1
		# 	elif self.__posResponse.parameters.MCDObjects[-1].getBitLength() % 8 >0:
		# 		self.posTargetBytelen += (self.__posResponse.parameters.MCDObjects[-1].getBitLength() // 8 + 1)
		# 	else:
		# 		self.posTargetBytelen += self.__posResponse.parameters.MCDObjects[-1].getBitLength() // 8
		
		negResponse = service.get('NEG-RESPONSE')
		if negResponse:
			conf['current'] = negResponse
			self.__negResponse = MCDDbResponse(conf, self)
			# if len(self.__negResponse.parameters.MCDObjects) != 0:
			# 	self.negTargetBytelen = self.__negResponse.parameters.MCDObjects[-1].getBytePos()
			# 	if self.__negResponse.parameters.MCDObjects[-1].getBitLength() <= 8:
			# 		self.negTargetBytelen += 1
			# 	elif self.__negResponse.parameters.MCDObjects[-1].getBitLength() % 8 >0:
			# 		self.negTargetBytelen += (self.__negResponse.parameters.MCDObjects[-1].getBitLength() // 8 + 1)
			# 	else:
			# 		self.negTargetBytelen += self.__negResponse.parameters.MCDObjects[-1].getBitLength() // 8

		#print(json.dumps(service, indent=2))
		self.requestPrefix = bytes.fromhex(service.get('SEND'))
		self.posResponsePrefix = bytes.fromhex(service.get('POSRESPONSE'))
		self.negResponsePrefix = bytes.fromhex(service.get('NEGRESPONSE'))

	def getDbRequest(self):
		return self.__request

	def getRequestPrefix(self):
		return self.requestPrefix

	def getDbResponse(self):
		return self.__posResponse

	def getResponsePrefix(self):
		return [self.posResponsePrefix,self.negResponsePrefix]

	def getSemantic(self):
		return self.__sementic
		
	def getDbResponses(self):
		#return [self.__posResponse]
		if self.__negResponse:
			return [(self.posResponsePrefix, self.__posResponse), (self.negResponsePrefix, self.__negResponse)]
		else:
			return [(self.posResponsePrefix, self.__posResponse)]
	# def getTargetBytelen(self):
	# 	if self.__negResponse:
	# 		return [self.posTargetBytelen,self.negTargetBytelen]
	# 	else:
	# 		return [self.posTargetBytelen]
# control primitive
class MCDDbControlPrimitive(MCDDbDiagComPrimitive):
	pass

class MCDDbStartCommunication(MCDDbControlPrimitive):
	pass

class MCDDbStopCommunication(MCDDbControlPrimitive):
	pass

class MCDDbProtocolParameterSet(MCDDbControlPrimitive):
	pass

# data primitive
class MCDDbDataPrimitive(MCDDbDiagComPrimitive):
	def __init__(self, conf, parent):
		super().__init__(conf, parent)

class MCDDbDiagService(MCDDbDataPrimitive):
	def __init__(self, conf, parent):
		super().__init__(conf, parent)

class MCDDbService(MCDDbDiagService):

	def __init__(self,conf, parent):
		super().__init__(conf, parent)
		self.parent.add(self)


# primitive collection
class MCDDbControlPrimitives(MCDNamedCollection):
	pass

class MCDDbDataPrimitives(MCDNamedCollection):
	pass

class MCDDbDiagComPrimitives(MCDNamedCollection):
	pass


class MCDDbDiagServices(MCDNamedCollection):
	
	def __init__(self):
		super()
		self.__dbDiagServices = []

class MCDDbServices(MCDNamedCollection):
	
	def __init__(self, conf, parent,ODXServer):
		super().__init__()
		self.parent = parent

		services = conf['current']
		for service in services:
			if not service.get('SHORT-NAME'):
				raise RuntimeError("Not found service short name")
			if service.get('SHORT-NAME') != ODXServer:
				continue
			# if not service.get('ID'):
			# 	raise RuntimeError("Not found service id name")

			# if not service.get('OID'):
			# 	raise RuntimeError("Not found service OID name")

			conf['current'] = service
			MCDDbService(conf, self)
