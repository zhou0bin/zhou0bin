from .mcd_object import MCDDbObject
from .mcd_db_parameter import MCDDbResponseParameters, MCDDbResponseParameter

class MCDDbResponse(MCDDbObject):
	generatePattern = "generate"
	counter = 0
	def __init__(self, conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], parent)
		reponse = conf['current']

		repeatParameter = None
		if reponse.get("START-REPEAT-BYTE-POSITION"):
			# construct a repeat parameter
			self.counter += 1
			repeatConf = None
			for par in conf['current']["PARAMS"]:
				if par["BYTE-POSITION"] == reponse.get("START-REPEAT-BYTE-POSITION"):
					repeatConf = {"TYPE":"VALUE", "BYTE-POSITION":int(reponse.get("START-REPEAT-BYTE-POSITION")), "GENERATE":"True", "SHORT-NAME":par["SHORT-NAME"]}
					repeatConf.update({"DOP-REF":par["DOP-REF"]})
					break
			conf['current'] = repeatConf
			repeatParameter = MCDDbResponseParameter(conf, self)

		conf['current'] = reponse['PARAMS'] if reponse.get('PARAMS') else []
		self.parameters = MCDDbResponseParameters(conf, self, repeatParameter=repeatParameter)

		if reponse.get("START-REPEAT-BYTE-POSITION"):
			self.parameters.add(repeatParameter)

	def getDbParameters(self):
		return self.parameters