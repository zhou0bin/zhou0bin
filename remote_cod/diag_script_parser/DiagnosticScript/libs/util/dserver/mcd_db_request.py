from .mcd_object import MCDDbObject
from .mcd_db_parameter import MCDDbRequestParameters


class MCDDbRequest(MCDDbObject):
	
	def __init__(self,conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], parent)
		request = conf['current']

		conf['current'] = request['PARAMS'] if request.get('PARAMS') else []
		self.parameters = MCDDbRequestParameters(conf, self)

	
	def getDbParameters(self):
		return self.parameters