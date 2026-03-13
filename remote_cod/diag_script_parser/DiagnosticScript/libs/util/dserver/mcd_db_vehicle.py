from .mcd_object import MCDDbObject
from .mcd_object import MCDNamedCollection
from .mcd_db_logical_link import MCDDbLogicalLinks

class MCDDbVehicleInformation(MCDDbObject):

	def __init__(self, shortName, longName, desc, conf, parent,ODXServer):
		super().__init__(shortName, longName, desc, parent)
		self.parent.add(self)
		self.__dbLogicalLinks = MCDDbLogicalLinks(conf, self,ODXServer)

	def getDbLogicalLinks(self):
		return self.__dbLogicalLinks

class MCDDbVehicleInformations(MCDNamedCollection):

	def __init__(self, conf, parent,ODXServer):
		super().__init__()
		self.parent = parent
		#only a DbVehicleInformation
		if not conf.get('Ecu_Name'):
			raise RuntimeError("Ecu_Name not found")

		#only a dbVehicleInformation
		ecuVarient = conf['Ecu_Name']
		MCDDbVehicleInformation(ecuVarient, ecuVarient, ecuVarient, conf, self,ODXServer)
