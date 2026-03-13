
from .mcd_object import MCDDbObject
from .mcd_object import MCDCollection
from .mcd_db_location import  MCDDbLocation

class MCDDbLogicalLink(MCDDbObject):
	
	def __init__(self, shortName, longName, desc, conf, parent,ODXServer):
		super().__init__(shortName, longName, desc, parent)
		self.__dbLocation = MCDDbLocation(conf['Ecu_Name'], conf['Ecu_Name'], conf['Ecu_Name'], conf, self,ODXServer)

	def getDbLocation(self):
		return self.__dbLocation
		
class MCDDbDLogicalLink(MCDDbLogicalLink):
	
	def __init__(self, shortName, longName, desc, conf, parent,ODXServer):
		super().__init__(shortName, longName, desc, conf, parent,ODXServer)
		parent.add(self)

		if not conf.get('Request_Address'):
			raise RuntimeError("Not found Request_Address")
		
		Request_Address = int(conf.get('Request_Address'))

		if not conf.get('Response_Address'):
			raise RuntimeError("Not found Response_Address")
		
		Response_Address = int(conf.get('Response_Address'))

		self.__dbProtocolStack = dict(Request_Address=Request_Address, Response_Address=Response_Address)	
	

	def isAccessedViaGateway(self):
		pass

	def getDbLogicalLinksOfGateways(self):
		pass

	def getDbProtocolStack(self):
		return self.__dbProtocolStack 

	def getDbPhysicalVehicleLink(self):
		pass

class MCDDbLogicalLinks(MCDCollection):

	def __init__(self, conf, parent,ODXServer):
		super().__init__()
		self.parent = parent
		if not conf.get('Ecu_Name'):
			raise RuntimeError("Ecu_Name not found")

		#only a dbLogicalLink
		ecuVarient = conf['Ecu_Name']

		MCDDbDLogicalLink(ecuVarient, ecuVarient, ecuVarient, conf, self,ODXServer)

# class MCDDbDLogicalLinks(MCDCollection):
# 	pass
