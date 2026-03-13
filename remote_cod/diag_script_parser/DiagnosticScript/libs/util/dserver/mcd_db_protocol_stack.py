from .mcd_object import MCDNamedCollection
from .mcd_object import MCDDbObject

class MCDDbProtocolStack(MCDDbObject):
	
	def __init__(self):
		super()

	def getDbProtocolType():
		pass

class MCDDbProtocolStacks(MCDNamedCollection):
	
	def __init__(self):
		super()
		self.__dbProtocolStacks = []