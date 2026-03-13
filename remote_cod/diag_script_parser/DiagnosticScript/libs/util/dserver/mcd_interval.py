from .mcd_object import MCDObject

#dop info
class MCDInterval(MCDObject):

	def __init__(self, low, upper):
		super().__init__(low, upper)
		self.__low = low
		self.__upper = upper

	def getLowerLimit(self):
		return self.__low

	def getLowerLimitAsCodedValue(self):
		pass

	def getLowerLimitIntervalType(self):
		pass

	def getUpperLimit(self):
		return self.__upper

	def getUpperLimitAsCodedValue(self):
		pass

	def getUpperLimitIntervalType(self):
		pass


class MCDInternalConstraint(MCDObject):
	def __init__(self):
		self.__interval = MCDInterval()
		pass

	def getInterval(self):
		return self.__interval

	def getScaleConstraints(self):
		pass


class MCDScaleConstraint(MCDObject):

	def __init__(self):
		super()

	def getDescription(self):
		pass

	def getDescriptionID(self):
		pass

	def getInterval(self):
		pass

	def getRangeInfo(self):
		pass

	def getShortLabel(self):
		pass

	def getShortLabelID(self):
		pass


class MCDTextTableElement(MCDObject):

	def  __init__(self):
		super()

	def getInterval(self):
		pass

	def getLongName(self):
		pass

	def getLongNameID(self):
		pass
	