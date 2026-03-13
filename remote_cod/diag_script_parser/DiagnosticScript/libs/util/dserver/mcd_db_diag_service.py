
from .mcd_object import MCDNamedCollection

from .mcd_object import MCDNamedCollection
from .mcd_object import MCDDbObject

class MCDDbDiagComPrimitive(MCDDbObject):
	pass

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
	pass

class MCDDbDiagService(MCDDbDataPrimitive):
	pass

class MCDDbService(MCDDbDiagService):
	pass

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