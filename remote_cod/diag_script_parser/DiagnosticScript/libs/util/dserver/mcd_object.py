
class MCDObject(object):
	pass

class MCDCollection(MCDObject):
	
	def __init__(self):
		super(MCDCollection, self).__init__()
		self.MCDObjects = list()

	def getCount(self):
		return len(self.MCDObjects)

	def add(self, mcdObiect):
		self.MCDObjects.append(mcdObiect)

	def getItemByIndex(self, i):
		return self.MCDObjects[i]

	def __getitem__(self, i):
		return self.MCDObjects[i]
	
	def __len__(self):
		return len(self.MCDObjects)

class MCDNamedObject(MCDObject):
	def __init__(self, shortName, longName, desc):
		super().__init__()
		self.__shortName = shortName
		self.__longName = longName
		self.__desc = desc

	def getDescription(self):
		return self.__desc

	def getLongName(self):
		return self.__longName

	def getShortName(self):
		return self.__shortName

class MCDNamedCollection(MCDCollection):
	
	def __init__(self):
		super(MCDNamedCollection, self).__init__()

	'''
	def add(self, namedObject):
		if not isinstance(namedObject, MCDNamedObject):
			raise RuntimeError()
		self.__list.append(namedObject)

	def getCount(self):
		return len(self.__list)
	'''

	def getNames(self):
		return [one.getShortName() for one in self.MCDObjects]

	def getItemByName(self, name):
		for one in self.MCDObjects:
			if one.getShortName() == name:
				return one
		return None

class MCDDbObject(MCDNamedObject):

	def __init__(self, shortName, longName, desc, parent=None):
		super().__init__(shortName, longName, desc)
		self.parent = parent

	def getObjectType(self):
		pass

	def getParent(self):
		return self.parent

	def getDbSDGs():
		pass

	def getDescriptionID():
		pass

	def getLongNameID():
		pass
	
	def getUniqueObjectIdentifier():
		pass
