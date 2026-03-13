from .mcd_object import MCDNamedCollection, MCDNamedObject, MCDObject
from .mcd_request import MCDRequest
from .mcd_response import MCDResponse, MCDResult, MCDResults

class HanderResponseRequest(MCDObject):
	
	def __init__(self, args, msg):
		super().__init__()
		self.processResponse(args, msg)


	@staticmethod
	def processResponse(args, messages):
		msg = bytes.fromhex(messages)
		service = args[0]
		resTable = args[1]
		requestPdu = memoryview(args[2])
		results = MCDResults()
		mem = memoryview(msg)
		responsetype = None
		flag = True
		for t in resTable:
			prefix = t[0]
			dbResponse = t[1]
			#check prefix
			if len(msg) < len(prefix):
				continue
			if prefix != msg[:len(prefix)]:
				continue
			#check parameters
			response = MCDResponse(dbResponse)
			parameters = response.getResponseParameters()
			
			for p in parameters:
				if not p.deserialize(mem, requestPdu):
					flag = False
					break

			if flag == True:
				#get a response, add to results
				res = MCDResult(service)
				res.responseCollection.add(response)
				results.add(res)
				responsetype = prefix
				break
		
		ret = 0 if len(results) !=0 else 1
		return ret, results,responsetype,flag


class MCDDiagComPrimitive(MCDNamedObject):

	def __init__(self, dbDiagComPrimitive):
		super().__init__(dbDiagComPrimitive.getShortName(), dbDiagComPrimitive.getLongName(), dbDiagComPrimitive.getDescription())
		self.__dbDiagComPrimitive = dbDiagComPrimitive
		self.__request = MCDRequest(self.__dbDiagComPrimitive.getDbRequest())
		#self.__response = MCDResponse(self.__dbDiagComPrimitive.getDbResponse())

	def cancel(self):
		pass

	def configureEventHandler(self):
		pass

	def executeSync(self):

		request = self.getRequest()
		parameters = request.getRequestParameters()
		prefix = self.getDbObject().requestPrefix
		mem = memoryview(bytearray(prefix + bytearray(65536)))
		# mem = memoryview(bytearray(prefix + bytearray(1500)))
		g = None
		for p in parameters:
			g = p.serialize(mem)
		
		pdu = prefix if g is None else g
		# prepare response table
		resTable = self.__dbDiagComPrimitive.getDbResponses()
		# bytelen = self.__dbDiagComPrimitive.getTargetBytelen()
		return (pdu, HanderResponseRequest, (self, resTable, pdu))
		# fut = dpdu.PDUStartComPrimitive(pdu, HanderResponseRequest, (self, resTable, pdu))

		# return fut.result(10)


	def getCooperationLevel(self):
		pass

	def getDbObject(self):
		return self.__dbDiagComPrimitive

	def getErrors(self):
		pass

	def getLockState(self):
		pass

	def getRequest(self):
		return self.__request

	def getSemantic(self):
		return self.__dbDiagComPrimitive.getSemantic()
		
	def getState(self):
		pass

	def isModifiedByOtherClient(self):
		pass

	def isOwned(self):
		pass

	def lock(self):
		pass

	def releaseEventHandler(self):
		pass

	def resetToDefaultValues(self):
		pass

	def setEventHandler(self):
		pass

	def unlock(self):
		pass

#control primitive
class MCDControlPrimitive(MCDDiagComPrimitive):
	
	def __init__(self):
		super()

	def getProtocolParameters(self):
		pass


class MCDProtocolParameterSet(MCDControlPrimitive):
	
	def  __init__(self):
		super()

	def fetchValueFromInterface(self):
		pass

	def fetchValuesFromInterface(self):
		pass

class MCDStartCommunication(MCDControlPrimitive):
	
	def __init__(self):
		super()

	def hasSuppressPositiveResponseCapability():
		pass

	def isSuppressPositiveResponse(self):
		pass

	def setSuppressPositiveResponse(self):
		pass


class MCDStopCommunication(MCDControlPrimitive):
	
	def __init__(self):
		super()

	def hasSuppressPositiveResponseCapability(self):
		pass

	def isSuppressPositiveResponse(self):
		pass

	def setSuppressPositiveResponse(self):
		pass

# data primitive
class MCDDataPrimitive(MCDDiagComPrimitive):
	
	def __init__(self, dbDiagComPrimitive):
		super().__init__(dbDiagComPrimitive)
		

	def executeAsync(self):
		# prepare pdu
		request = self.getRequest()
		parameters = request.getRequestParameters()
		mem = memoryview(bytearray(1500))
		for p in parameters:
			g = p.serialize(mem)

		#generate response table


	def fetchResults(self):
		pass

	def getFilter(self):
		pass

	def getNewAsciiStrings(self):
		pass

	def getNumberOfResults(self):
		pass

	def getRepetitionTime(self):
		pass

	def getResultBufferSize(self):
		pass

	def isFilterModeSet(self):
		pass

	def removeFilter(self):
		pass

	def setFilter(self):
		pass

	def setRepetitionTime(self):
		pass

	def setResultBufferSize(self):
		pass

	def startRepetition(self):
		pass

	def stopRepetition(self):
		pass

	def updateRepetitionParameters(self):
		pass

class MCDDiagService(MCDDataPrimitive):
	
	def __init__(self, shortName, longName, desc, dbDiagComPrimitive):
		super().__init__(shortName, longName, desc, dbDiagComPrimitive)

	def getProtocolParameters(self):
		pass


class MCDService(MCDDiagService):
	
	def __init__(self, shortName, longName, desc, dbDiagComPrimitive):
		super().__init__(self, shortName, longName, desc, dbDiagComPrimitive)

	def getDefaultResultBufferSize(self):
		pass

	def isSuppressPositiveResponse(self):
		pass

	def setRuntimeTransmissionModeNotApplicabl(self):
		pass

	def setSuppressPositiveResponse(self):
		pass

# delay
class MCDDelay(MCDDiagComPrimitive):
	pass

# primitive colloction
class MCDControlPrimitives(MCDNamedCollection):
	
	def __init__(self):
		super()

	def addByDbObject(self):
		pass

	def addByName(self):
		pass

	def addBySemanticAttribute(self):
		pass

	def addByType(self):
		pass

	def addDiagVariableServiceByRelationType(self):
		pass

	def addDynIdComPrimitiveByTypeAndDefinitionMode(self):
		pass

	def configureEventHandler(self):
		pass

	def getItemByIndex(self):
		pass

	def getItemByName(self):
		pass

	def releaseEventHandler(self):
		pass

	def remove(self):
		pass

	def removeAll(self):
		pass

	def removeByIndex(self):
		pass

	def removeByName(self):
		pass

	def setPrimitiveEventHandler(self):
		pass

