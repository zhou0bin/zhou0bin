from .mcd_object import MCDNamedObject
from .mcd_object import MCDNamedCollection
from .mcd_diag_com_primitive import MCDDataPrimitive
from  ...communication import pdu_logical_link as dpdu

class MCDLogicalLink(MCDNamedObject):
	def __init__(self, dbLogicLink):
		super().__init__(dbLogicLink.getShortName(), dbLogicLink.getLongName(), dbLogicLink.getDescription())
		self.__dbLogicalLink = dbLogicLink

	def configureEventHandler(self):
		pass

	def getCooperationLevel(self):
		pass

	def getDbObject(self):
		return self.__dbLogicalLink

	def getLockState(self):
		pass

	def getState(self):
		pass

	def getType(self):
		pass

	def isOwned(self):
		pass

	def lock(self):
		pass

	def releaseEventHandler(self):
		pass

	def setEventHandler(self):
		pass

	def unlock(self):
		pass

	def unlockTree(self):
		pass

	def isModifiedByOtherClient(self):
		pass

class MCDDLogicalLink(MCDLogicalLink):
	def __init__(self, dbLogicalLink):
		super().__init__(dbLogicalLink)
		self.__dbLogicalLink = dbLogicalLink
		self.__diagComPrimitives = MCDNamedCollection()
		self.__generatePattern = "RtGen_<MCDDbDiagComPrimitiveShortName>_<UniqueNumber>"
		self.__uniqueNumber = 0
		dbLocation =  dbLogicalLink.getDbLocation()
		dbServices = dbLocation.getDbDiagServices()
		for service in dbServices:
			self.__diagComPrimitives.add(MCDDataPrimitive(service))

	def clearQueue(self):
		pass

	def close(self):
		pass

	def disableReducedResults(self):
		pass

	def enableReducedResults(self):
		pass

	def getActivityState(self):
		pass

	def getConfigurationRecords(self):
		pass

	def getDefinableDynIds(self):
		pass

	def getDiagComPrimitives(self):
		return self.__diagComPrimitives

	def getInterfaceResource(self):
		pass

	def getMatchedDbEcuVariantPattern(self):
		pass

	def getMatchingInterfaceResources(self):
		pass

	def getQueueFillingLevel(self):
		pass

	def getQueueSize(self):
		pass

	def getSelectedVariantAccessKeys(self):
		pass

	def getUnitGroup(self):
		pass

	def gotoOffline(self):
		dpdu.PDUDisconnect()
		# pass

	def gotoOnline(self):
		dpdu.PDUSetComParam(self.__dbLogicalLink.getDbProtocolStack())
		dpdu.PDUConnect()
		# pass

	def isIntermediateResultForFunctionalAddressingEnabled(self):
		pass

	def isUnsupportedComParametersAccepted(self):
		pass

	def open(self):
		dpdu.PDUCreateComLogicalLink()
		# pass

	def reset(self):
		pass

	def resume(self):
		pass

	def selectInterfaceResource(self):
		pass

	def sendBreak(self):
		pass

	def setIntermediateResultForFunctionalAddressing(self):
		pass

	def setQueueSize(self):
		pass

	def setUnitGroupByName(self):
		pass

	def suspend(self):
		pass

	def unsupportedComParametersAccepted(self):
		pass


class MCDLogicalLinks(MCDNamedCollection):
	
	def __init__(self):
		super().__init__()

	def addByAccessKeyAndInterfaceResource(self):
		pass

	def addByAccessKeyAndVehicleLink(self):
		pass

	def addByDbObject(self, dbLogicalLink):
		self.add(MCDDLogicalLink(dbLogicalLink))

	def addByName(self):
		pass

	def addByNames(self):
		pass

	def addByObjects(self):
		pass

	def addByVariant(self):
		pass

	def getItemByIndex(self):
		pass

	def remove(self):
		pass

	def remove(self):
		pass

	def removeByIndex(self):
		pass

	def removeByName(self):
		pass