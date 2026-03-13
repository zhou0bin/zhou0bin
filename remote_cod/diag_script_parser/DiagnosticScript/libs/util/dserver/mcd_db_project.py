
import json
from .mcd_object import MCDObject
from .mcd_db_vehicle import MCDDbVehicleInformations

class MCDDbProjectConfiguration(MCDObject):

	def __init__(self, conf):
		super()
		self.__conf = conf
		self.__dbProject = None

	def add(self):
		pass

	def close(self):
		pass

	def getActiveDbProject(self):
		pass

	def getAdditionalConfigurationDataNames(self):
		pass

	def getAdditionalEcuMemNames(self):
		pass

	def load(self,ODXServer):
		self.__dbProject = MCDDbProject(self.__conf,ODXServer)
		return self.__dbProject

	def removeByIndex():
		pass

	def removeByName():
		pass

class MCDDbProject(MCDObject):
	def __init__(self, conf,ODXServer):
		super().__init__()
		self.__parseODX(conf,ODXServer)

	def __parseODX(self, conf,ODXServer):
		#consider only a logical link in odx
		# with open(conf, 'rb') as f:
			
		odx = json.loads(conf)
		#print(json.dumps(odx,  indent=2))
		if not odx.get('Ecu_Name'):
			raise RuntimeError("Ecu_Name not found")

		#only a dbVehicleInformation
		ecuVarient = odx['Ecu_Name']
		self.__dbVehicleInformations = MCDDbVehicleInformations(odx, self,ODXServer)
		self.__ecuinfos = {}
		self.__ecuinfos['Ecu_Name'] = odx['Ecu_Name']
		if "Request_Address" in odx.keys():
			self.__ecuinfos["Request_Address"] = odx["Request_Address"]
		if "Response_Address" in odx.keys():
			self.__ecuinfos["Response_Address"] = odx["Response_Address"]
			#dbVehicleInformation = MCDDbVehicleInformation(ecuVarient, ecuVarient, ecuVarient):
			#dbLocation = MCDDbLocation(ecuVarient, ecuVarient, ecuVarient)
			#dbLogicalLink = MCDDbDLogicalLink(ecuVarient, ecuVarient, ecuVarient, dbLocation)
			#self.__logicLinks.add(dbLogicalLink)

			#print(odx['Ecu_Name'])
	def getECUinfos(self):
		return self.__ecuinfos
	def getAccessKeys(self):
		pass

	def getDbConfigurationDatas(self):
		pass
	def getDbEcuBaseVariants(self):
		pass
	def getDbEcuMems(self):
		pass
	def getDbElementByAccessKey(self):
		pass
	def getDbFunctionalGroups(self):
		pass
	def getDbFunctionDictionaries(self):
		pass
	def getDbMultipleEcuJobLocation(self):
		pass
	def getDbODXFiles(self):
		return self.__conf
	def getDbPhysicalVehicleLinks(self):
		pass
	def getDbProtocolLocations(self):
		pass
	def getDbVehicleInformations(self):
		return self.__dbVehicleInformations

	def getDbVehicleInformationsByInfoComponentValues(self):
		pass
	def getVersion(self):
		pass
	def loadNewConfigurationData(self):
		pass
	def loadNewConfigurationDatasByFileName(self):
		pass
	def loadNewEcuMem(self):
		pass
	def loadNewEcuMemsByFilename(self):
		pass
	def removeConfigurationDataByName(self):
		pass
	def removeEcuMemByName(self):
		pass
