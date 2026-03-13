from .mcd_object import MCDDbObject
from .mcd_object import MCDNamedCollection
from .mcd_db_service import MCDDbServices

class MCDDbLocation(MCDDbObject):
	
	def __init__(self, shortName, longName, desc, conf, parent,ODXServer):
		super().__init__(shortName, longName, desc, parent)
		services = conf.get('Diag_Service')
		if not services:
			return
		confDict = dict(root=conf, current=services)
		self.__dbSevices = MCDDbServices(confDict, self,ODXServer)

	def GetAccessKey(self):
		pass

	def getAuthorizationMethods(self):
		pass

	def getDbAdditionalAudiences(self):
		pass

	def getDbConfigurationDatas(self):
		pass

	def getDbControlPrimitives(self):
		pass

	def getDbDataPrimitives(self):
		pass

	def getDbDiagComPrimitives(self):
		pass

	def getDbDiagComPrimitivesBySemanticAttribute(self):
		pass

	def getDbDiagComPrimitivesByType(self):
		pass

	def getDbDiagServices(self):
		return self.__dbSevices

	def getDbDiagVariables(self):
		pass

	def getDbDynDefinedSpecTableByDefinitionMode(self):
		pass

	def getDbDynDefinedSpecTableByName(self):
		pass

	def getDbDynDefinedSpecTables(self):
		pass

	def getDbECU(self):
		pass

	def getDbEcuMems(self):
		pass

	def getDbEcuStateCharts(self):
		pass

	def getDbEnvDataDescs(self):
		pass

	def getDbFaultMemories(self):
		pass

	def getDbFunctionalClasses(self):
		pass

	def getDbJobs(self):
		pass

	def getDbLogicalLinks(self):
		pass

	def getDbPhysicalMemories(self):
		pass

	def getDbProtocolStacks(self):
		pass

	def getDbServices(self):
		pass

	def getDbSubComponents(self):
		pass

	def getDbTables(self):
		pass

	def getDbTablesBySemanticAttribute(self):
		pass
	
	def getSemantics(self):
		pass

	def getSupportedDynIds(self):
		pass

	def getType(self):
		pass

	def getUnitGroups(self):
		pass

	def getVersion(self):
		pass

class MCDDbLocations(MCDNamedCollection):

	def __init__(self):
		self.__locations = dict()
		super()

	def getItemByIndex(self):
		pass

	def getItemByName(self):
		pass
