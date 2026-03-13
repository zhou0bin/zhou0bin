from .mcd_object import MCDObject
from .mcd_db_project import MCDDbProjectConfiguration
from .mcd_logical_link import MCDLogicalLinks

class MCDProject(MCDObject):
	
	def __init__(self, dbProject):
		super().__init__()
		self.__dbProject = dbProject
		self.__dbVehicleInformation = None
		self.__logicalLinks = MCDLogicalLinks()
			

	def addLogicalLinks(self, dbLogicalLink):
		self.__logicalLinks.add(dbLogicalLink)

	def getLogicalLinks(self):
		return self.__logicalLinks 

	def getDbProject(self):
		return self.__dbProject

	def selectDbVehicleInformation(self, dbVehicleInformation):
		self.__dbVehicleInformation = dbVehicleInformation

	def getDbVehicleInformation(self, dbVehicleInformation):
		return self.__dbVehicleInformation

class MCDSystem(MCDObject):
	def __init__(self, conf,ODXServer):
		super().__init__()
		self.__dbProjectConfigration = MCDDbProjectConfiguration(conf)
		self.__project = MCDProject(self.__dbProjectConfigration.load(ODXServer))

	def configureEventHandler(self, hander):
		pass

	def deselectProject(self):
		pass

	def getAbsoluteActivationTime(self):
		pass

	def getActiveProject(self):
		return self.__project

	def getASAMMCDVersion(self):
		pass

	def getClientNames(self):
		pass

	def getConnectedInterfaces(self):
		pass

	def getCurrentInterfaces(self):
		pass

	def getDbProjectConfiguration(self):
		return self.__dbProjectConfigration

	def getDbProjectDescriptions(self):
		pass

	def getInterfaceNumber(self):
		pass

	def getLockState(self):
		pass

	def getMaximumBufferSize(self):
		pass

	def getMaxNoOfClients(self):
		pass

	def getMonitoringLinks(self):
		pass

	def getObjectFactory(self):
		pass

	def getProperty(self):
		pass

	def getPropertyNames(self):
		pass

	def getServerType(self):
		pass

	def getState(self):
		pass

	def getSupportedODXVersions(self):
		pass

	def getSystemParameter(self):
		pass

	def getVersion(self):
		pass

	def hasActiveProject(self):
		pass

	def isModifiedByOtherClient(self):
		pass

	def isUnsupportedComParametersAccepted(self):
		pass

	def lock():
		pass

	def prepareVciAccessLayer(self):
		pass

	def releaseEventHandler(self):
		pass

	def resetProperty(self):
		pass

	def selectProject(self):
		pass

	def selectProjectByName(self):
		pass

	def setClientName(self):
		pass

	def setEventHandler(self):
		pass

	def setProperty(self):
		pass

	def unlock(self):
		pass

	def unlockTree(self):
		pass

	def unprepareVciAccessLayer(self):
		pass

	def unsupportedComParametersAccepted(self):
		pass

# __system = None
def getMCDSystem(conf,ODXServer):
	__system = None
	if __system is None:
		__system = MCDSystem(conf,ODXServer)
	return __system
