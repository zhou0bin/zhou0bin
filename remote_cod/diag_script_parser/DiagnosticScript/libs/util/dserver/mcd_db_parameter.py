from .mcd_object import MCDDbObject, MCDNamedCollection
from  libs.util.DiagLog import MSGLogger


class Identical():

	def __init__(self,basetype = None,displaytype = None):
		super().__init__()
		self.basetype = basetype
		self.displaytype = displaytype

	def getResult(self, x):
		if self.basetype != None and self.basetype.find("INT") != -1:
			if self.displaytype.find("HEX") != -1:
				return hex(x)[2:]
			else:
				return x
		return x

	def getInternalResult(self, x):
		if self.basetype == None and self.displaytype == None:
			return x
		elif self.basetype != None and self.basetype.find("INT") != -1:
			if self.displaytype.find("HEX") != -1:
				x = int(str(x),16)
				if isinstance(x, int):
					return x
			else:
				x = int(str(x),10)
				return x
		elif self.basetype.find("FLOAT") != -1:
			return float(str(x))
		elif self.basetype.find("BYTE") != -1:
			b = bytearray.fromhex(x)
			# return bytearray(x,encoding="utf-8")
			return b 
		else:
			return x

class Linear():
	def __init__(self, pthn0,pthn1,pthd0,vn0, vn1, vd0=1,basetype = None,displaytype = None,encoding = None):
		super().__init__()
		self.basetype = basetype
		self.displaytype = displaytype
		self._pthn0 = pthn0
		self._pthn1 = pthn1
		self._pthd0 = pthd0
		self._htpn0 = vn0
		self._htpn1 = vn1
		self._htpd0 = vd0
		self._encoding = encoding

	def getResult(self, x):
		if self.basetype != None and self.basetype.find("INT") != -1:
			if self.displaytype != None and self.displaytype.find("HEX") != -1 and self._encoding != "BCD-P":
				return eval(hex((self._htpn0+self._htpn1*x)/self._htpd0)[2:])
			elif self._encoding == "BCD-P":
				x = eval(hex(x)[2:])
				return int((self._htpn0+self._htpn1*x)/self._htpd0)
			else:
				return int((self._htpn0+self._htpn1*x)/self._htpd0)
		return round(((self._htpn0+self._htpn1*x)/self._htpd0),3)

	def getInternalResult(self, y):
		if self.basetype != None and self.basetype.find("INT") != -1:
			if self.displaytype != None and self.displaytype.find("HEX") != -1:
				return eval(hex((self._pthn0+self._pthn1*int(y,16))/self._pthd0)[2:])
			else:
				return int(int(self._pthn0+self._pthn1*int(y,10))/self._pthd0)
		return (self._pthn0+self._pthn1*float(y))/self._pthd0
class ScaleLinear():
	def __init__(self):
		super().__init__()
		self._linear = []
		self._bound = []
	
	def add(self, vn0, vn1, vd0, upper, utype, low, ltype):
		self._linear.append(Linear(vn0, vn1, vd0))
		item = dict(upper=upper, utype=utype, low=low, ltype=ltype)
		self._bound.append(item)
		return self

	def getResult(self, x):
		index = -1
		for i in range(len(self._bound)):
			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "INFINITE":
				if x >= int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "INFINITE":
				if x > int(self._bound['low']):
					index = i

			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "OPEN":
				if x >= int(self._bound['low']) and x < int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "OPEN":
				if x > int(self._bound['low']) and x < int(self._bound['low']):
					index = i

			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "CLOSED":
				if x >= int(self._bound['low']) and x <= int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "CLOSED":
				if x > int(self._bound['low']) and x <= int(self._bound['low']):
					index = i

			return self._linear[index].getResult()

class RationalFunction():
	
	def __init__(self, n, d):
		super().__init__()
		self._n = n
		self._d = d

	def getResult(self, x):
		n = 0
		d = 0
		for i_n in self._n.reverse():
			n = x*i_n + n
		
		for i_d in self._d.reverse():
			d = x*i_d + d

		return n/d	

class ScaleRationalFunction():
	def __init__(self):
		super().__init__()
		self._rationals = []
		self._bound = []
	
	def add(self, upper, utype, low, ltype, r):
		self._bound.append(dict(upper=upper, utype=utype, low=low, ltype=ltype))
		self._rationals.append(r)
		return self

	def getResult(self, x):
		index = -1
		for i in range(len(self._bound)):
			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "INFINITE":
				if x >= int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "INFINITE":
				if x > int(self._bound['low']):
					index = i

			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "OPEN":
				if x >= int(self._bound['low']) and x < int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "OPEN":
				if x > int(self._bound['low']) and x < int(self._bound['low']):
					index = i

			if self._bound['lclose'] == "CLOSED" and self._bound['uclose'] == "CLOSED":
				if x >= int(self._bound['low']) and x <= int(self._bound['low']):
					index = i
			elif self._bound['lclose'] == "OPEN" and self._bound['uclose'] == "CLOSED":
				if x > int(self._bound['low']) and x <= int(self._bound['low']):
					index = i

			return self._rationals[index].getResult()

class TextTable():
	def __init__(self):
		super().__init__()
		self._text = []
		self._bound = []
		self._physicl = dict()

	def add(self, low, upper, t, ltype="CLOSED", utype="CLOSED"):
		self._bound.append(dict(upper=upper, utype=utype, low=low, ltype=ltype))
		self._text.append(t)
		return self
	def addPhysicValue(self,physicalchar,hexvalue):
		self._physicl.update({physicalchar:hexvalue})
		return self
	def getInternalResult(self, x):
		# for i in range(len(self._bound)):
		# 	if x==self._text[i]:
		# 		if self._bound[i]['upper'] == self._bound[i]['low']:
		# 			return self._bound[i]['upper']
		for key in self._physicl.keys():
			if key == x:
				return self._physicl[key]
		return None

	def getResult(self, x):

		for i in range(len(self._bound)):
			#print("table", self._bound[i]['low'], self._bound[i]['upper'], self._text[i], x)
			if self._bound[i]['upper'] == "INFINITE":
				if x > int(self._bound[i]['low']):
					return self._text[i]
				elif self._bound[i]['ltype'] == "CLOSED" and x == int(self._bound[i]['low']):
					return self._text[i]
			elif self._bound[i]['low'] == "INFINITE":
				if x < int(self._bound[i]['upper']):
					return self._text[i]
				elif self._bound[i]['utype'] == "CLOSED" and x == int(self._bound[i]['upper']):
					return self._text[i]
			else:
				if x > int(self._bound[i]['low']) and x < int(self._bound[i]['upper']):
					return self._text[i]
				elif self._bound[i]['ltype'] == "CLOSED" and x == int(self._bound[i]['low']):
					return self._text[i]
				elif self._bound[i]['utype'] == "CLOSED" and x == int(self._bound[i]['upper']):
					return self._text[i]

		return None

class TabInterpolated():
	def __init__(self):
		super().__init__()
		self._kv = dict()

	def add(self, k, v):
		self._kv[k] = v
		return self

	def getResult(self, x):
		return self._kv[x] 

class CompuMethod():
	
	def __init__(self, dbDop):
		super().__init__()
		basetype = None
		displaytype = None
		isHEXDUMP = None
		self.bitLength = dbDop.diagCode.bitLength
		#print(json.dumps(dbDop.conf, indent=2))
		self.dtype = dbDop.diagCode.baseDataType
		self.ptype = "A_UNICODE2STRING"
		dbConf = dbDop.conf
		types = dbConf["PHYSICAL-TYPE"]
		encoding = dbDop.diagCode.encoding if dbDop.diagCode.encoding else None
		if "BASE-DATA-TYPE" in types.keys():
			basetype = types["BASE-DATA-TYPE"]
		if "DISPLAY-RADIX" in types.keys():
			displaytype = types["DISPLAY-RADIX"]
		if dbDop.compuMethod == "IDENTICAL":
			self.compuMethod = Identical(basetype,displaytype)
		elif dbDop.compuMethod == "LINEAR":
			scales = dbConf['HEX-TO-PHYSIC'].split(';')
			htpn0 = float(scales[0])
			htpn1 = float(scales[1])
			htpd0 = float(scales[2])
			scales = dbConf['PHYSIC-TO-HEX'].split(';')
			pthn0 = float(scales[0])
			pthn1 = float(scales[1])
			pthd0 = float(scales[2])
			self.compuMethod = Linear(pthn0,pthn1,pthd0,htpn0, htpn1, htpd0,basetype,displaytype,encoding)
		elif dbDop.compuMethod == "SCALE-LINEAR":
			self.compuMethod = ScaleLinear()
		elif dbDop.compuMethod == "RAT-FUNC":
			self.compuMethod = RationalFunction()
		elif dbDop.compuMethod == "SCALE-RAT-FUNC":
			self.compuMethod = ScaleRationalFunction()
		elif dbDop.compuMethod == "TEXTTABLE":
			self.compuMethod = TextTable()
			for item in dbConf['HEX-TO-PHYSIC']:
				low = int(item.get('MIN'))
				high = int(item.get('MAX'))
				target = item.get('PHYSICAL')
				self.compuMethod.add(low, high, target)
			for item in dbConf["PHYSIC-TO-HEX"]:
				physicalchar = item.get("PHYSICAL")
				hexvalue = int(item.get("HEX"))
				self.compuMethod.addPhysicValue(physicalchar,hexvalue)
		elif dbDop.compuMethod == "TAB-INTP":
			self.compuMethod = TabInterpolated()
		
	
	def getResult(self, x):
		return self.compuMethod.getResult(x)
	
	def getInternalResult(self, x):
		return self.compuMethod.getInternalResult(x)

class DiagCodeType():
	def __init__(self, diagConf):
		#print(json.dumps(diagConf, indent=2))
		conf = diagConf["DIAG-CODED-TYPE"]
		if  not isinstance(conf, dict) or not conf.get('BASE-DATA-TYPE'):
			# LOGGER.fatal("{} BASE-DATA-TYPE not exist,".format(diagConf['SHORT-NAME']))
			MSGLogger.debug("MCD_DB: "+str(diagConf['SHORT-NAME'])+" BASE-DATA-TYPE not exist")
			self.baseDataType = "A_UINT32"
			self.type = "STANDARD-LENGTH-TYPE"
			self.bitLength=8
			self.encoding = ""
			# LOGGER.warning("{} DIAG-CODED-TYPE not found  {},".format(diagConf['SHORT-NAME'], self.type))
			MSGLogger.debug("MCD_DB: "+str(diagConf['SHORT-NAME'])+" DIAG-CODED-TYPE not found"+str(self.type))
			return

		self.baseDataType = conf['BASE-DATA-TYPE']
		self.type = conf['TYPE']
		self.encoding = conf['BASE-TYPE-ENCODING'] if conf.get("BASE-TYPE-ENCODING") else ""
		if self.type == "STANDARD-LENGTH-TYPE":
			if not conf.get('BIT-LENGTH'):
				# LOGGER.warning("{} BIT-LENGTH Is not exist,".format(diagConf['SHORT-NAME']))
				MSGLogger.debug("MCD_DB: "+str(diagConf['SHORT-NAME'])+" BIT-LENGTH Is not exist")
			self.bitLength=int(conf.get('BIT-LENGTH'))
		elif self.type == "MIN-MAX-LENGTH-TYPE":
			self.bitLength=0
			self.minlenth = int(conf["MIN-LENGTH"])
			self.maxlenth = int(conf["MAX-LENGTH"])
		else:
			# LOGGER.warning("{} not support {},".format(diagConf['SHORT-NAME'], self.type))
			MSGLogger.debug("MCD_DB: "+str(diagConf['SHORT-NAME'])+" not support"+str(self.type))
			self.bitLength=0

class PhysicalType():
	def __init__(self, diagConf):
		conf = diagConf["PHYSICAL-TYPE"]
		self.baseDataType = conf['BASE-DATA-TYPE']
		self.radix = conf['DISPLAY-RADIX'] if conf.get('DISPLAY-RADIX') else ""
		self.distype = conf["BASE-DATA-TYPE"] if conf.get("BASE-DATA-TYPE") else ""

class DiagDop(MCDDbObject):
	def __init__(self, conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], "", parent)
		dopConf = conf['current']
		self.conf = conf['current']
		self.dopType = "SIMPLE-DOP"
		self.diagCode = DiagCodeType(dopConf)
		self.physical = PhysicalType(dopConf)
		self.compuMethod = dopConf['COMPU-METHOD']
		self.unit = "" if not dopConf.get('UNIT') else dopConf.get('UNIT')
		self.lower_limit = None
		self.upper_limit = None
		self.specialdisplay = {}
		if "LOWER-LIMIT" in dopConf.keys():
			self.lower_limit = dopConf["LOWER-LIMIT"] if dopConf["LOWER-LIMIT"] == "INFINITE" else int(dopConf["LOWER-LIMIT"])
		if "UPPER-LIMIT" in dopConf.keys():
			self.upper_limit = dopConf["UPPER-LIMIT"] if dopConf["UPPER-LIMIT"] == "INFINITE" else int(dopConf["UPPER-LIMIT"])
		if "IS-ASCII" in dopConf.keys():
			self.specialdisplay.update({"IS-ASCII":dopConf["IS-ASCII"]})
		elif "IS-HEXDUMP" in dopConf.keys():
			self.specialdisplay.update({"IS-HEXDUMP":dopConf["IS-HEXDUMP"]})
	def DopMethodValueOutLimit(self,value):
		if self.lower_limit == None and self.upper_limit == None:
			return "No"
		if self.lower_limit == "INFINITE" and self.upper_limit == "INFINITE":
			return "No"
		
		if self.lower_limit == "INFINITE" and self.upper_limit != "INFINITE":
			if value > self.upper_limit:
				return "Upper"
			else:
				return "No"
		elif self.lower_limit != "INFINITE" and self.upper_limit == "INFINITE":
			if value < self.lower_limit:
				return "Lower"
			else:
				return "No"
		else:
			if value > self.upper_limit:
				return "Upper"
			elif value < self.lower_limit:
				return "Lower"
			else:
				return "No"

class DiagDtc(MCDDbObject):
	def __init__(self, conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], "", parent)
		dtcConf = conf['current']
		self.troubleCode = int(dtcConf['TROUBLE-CODE'])
		self.displayTroubleCode =  dtcConf['DISPLAY-TROUBLE-CODE']
		# self.ti = dtcConf['TI']
		self.text = dtcConf['TEXT']
		self.conf = dtcConf

class DiagDtcDop(MCDDbObject):
	def __init__(self, conf, parent):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], "", parent)
		dopConf = conf['current']
		self.conf = conf['current']
		self.dopType = "SIMPLE-DOP"
		self.diagCode = DiagCodeType(dopConf)
		self.physical = PhysicalType(dopConf)
		self.compuMethod = dopConf['COMPU-METHOD']
		self.dtcs = MCDNamedCollection()
		for dtc in dopConf.get("DTCS"):
			conf['current'] = dtc
			self.dtcs.add(DiagDtc(conf, self))
	def DopMethodValueOutLimit(self,value):
		return "No"
	
class DiagMuxSwitchKey:
    """解析MUX的开关钥匙（SWITCH-KEY）"""
    def __init__(self, switch_conf, parent):
        self.parent = parent
        # 开关值的字节位置
        self.byte_position = int(switch_conf.get("BYTE-POSITION", 0))
        # 关联的DOP引用
        self.dop_ref = switch_conf.get("DOP-REF")
        # 解析后的DOP对象
        self.db_dop = None

    def resolve_dop(self, root_conf):
        """根据DOP-REF从根配置中查找并初始化对应的DOP"""
        if not self.dop_ref:
            return
        
        # 1. 查找SIMPLE-DOP
        for dop in root_conf.get("Data_Object_Props", []):
            if dop.get("ID") == self.dop_ref:
                conf = {"current": dop, "root": root_conf}
                self.db_dop = DiagDop(conf, self)
                return
        
        # 2. 查找DTC-DOP
        for dtc_dop in root_conf.get("DTC-DOPS", []):
            dtc_dop_obj = dtc_dop.get("DTC-DOP")
            if dtc_dop_obj and dtc_dop_obj.get("ID") == self.dop_ref:
                conf = {"current": dtc_dop_obj, "root": root_conf}
                self.db_dop = DiagDtcDop(conf, self)
                return
	
class DiagMuxCase:
    """解析MUX的单个分支案例（CASES中的项）"""
    def __init__(self, case_conf, parent, root_conf):
        self.parent = parent
        self.short_name = case_conf.get("SHORT-NAME")
        self.lower_limit = case_conf.get("LOWER-LIMIT")  # 下限
        self.upper_limit = case_conf.get("UPPER-LIMIT")  # 上限
        self.params = []  # 案例内的参数列表
        self.params_length = 0
        
        # 解析案例内的PARAMS（复用MCDDbParameter）
        for param_conf in case_conf.get("PARAMS", []):
            param_conf_full = {
                "current": param_conf,
                "root": root_conf
            }
            mp = MCDDbParameter(param_conf_full, self)
            self.params_length += mp.getBitLength()
            self.params.append(mp)
	
    def getParamsLength(self):
        return self.params_length
			
class DiagMuxDop(MCDDbObject):
    """MUX节点的根解析类"""
    def __init__(self, conf, parent):
        super().__init__(
            conf['current']['SHORT-NAME'], 
            conf['current']['SHORT-NAME'], 
            "", 
            parent
        )
        self.mux_conf = conf['current']
        self.root_conf = conf['root']
        
        # 基础属性
        self.id = self.mux_conf.get("ID")
        self.short_name = self.mux_conf.get("SHORT-NAME")
        self.byte_position = int(self.mux_conf.get("BYTE-POSITION", 0))
        
        # 1. 解析SWITCH-KEY（开关钥匙）
        self.switch_key = None
        switch_conf = self.mux_conf.get("SWITCH-KEY")
        if switch_conf:
            self.switch_key = DiagMuxSwitchKey(switch_conf, self)
            self.switch_key.resolve_dop(self.root_conf)
        
        # 2. 解析DEFAULT-CASE（默认分支）
        self.default_case = self.mux_conf.get("DEFAULT-CASE", {}).get("SHORT-NAME")
        
        # 3. 解析CASES（分支案例列表）
        self.cases = []
        for case_conf in self.mux_conf.get("CASES", []):
            self.cases.append(DiagMuxCase(case_conf, self, self.root_conf))
        if self.cases:
            self.case_params_length = self.cases[0].getParamsLength()
        else:
            self.case_params_length = 0

    def get_case_params_length(self):
        return self.case_params_length
			
    def get_switch_key_dop(self):
        """获取开关钥匙对应的DOP对象（用于解析开关值）"""
        return self.switch_key.db_dop if self.switch_key else None

    def match_case(self, switch_value):
        """
        根据开关值匹配对应的MUX案例
        :param switch_value: 解析后的开关物理值（如"Fault number1"）
        :return: 匹配的DiagMuxCase对象 / 默认案例名称 / None
        """
        for case in self.cases:
            # 精确匹配（上下限相等）
            if case.lower_limit == case.upper_limit and str(switch_value) == case.lower_limit:
                return case
            
            # 区间匹配（兼容数值类型）
            try:
                val = float(switch_value)
                lower = float(case.lower_limit)
                upper = float(case.upper_limit)
                if lower <= val <= upper:
                    return case
            except (ValueError, TypeError):
                continue
        
        # 无匹配返回默认案例
        return self.default_case

class MCDDbParameter(MCDDbObject):
	def __init__(self, conf, parent, repeatParameter=None):
		super().__init__(conf['current']['SHORT-NAME'], conf['current']['SHORT-NAME'], "", parent)
		#print(json.dumps(conf['current'], indent=2))
		#conf['current']['ID']
		self.__bitPosition = 0
		self.__dtype = 'A_UINT32'
		self.__bitLength = 0
		self.__byteLength = 0
		self.parameters = []
		self.dbParameter = conf['current']
		self.__phisicalType = "A_UNICODE2STRING"
		#self.dbDtc = None
		self.__dbTableKey = dict()
		self.__dbTableStruct = None
		self.__isComplex = True
		self.__defaultValue = None
		self.dbDop = None
		self.dbDtcDop = None
		self.codedValues = list()
		self.__radix = ""
		self.__distype = ''
		self.__encoding = ""
		self.__sementic = None
		self.__dynamicType = None
		self.__dynamicMin = None
		self.__dynamicMax = None
		self.specialdisplay = {}
		parameter = conf['current']
		self.__ptype = parameter.get('TYPE')
		if not parameter.get('BYTE-POSITION'):
			raise RuntimeError(parameter['SHORT-NAME'] + " MCDDbParameter BYTE-POSITION not found")
		self.__bytePosition = int(parameter.get('BYTE-POSITION'))
		if parameter.get('BIT-POSITION'):
			self.__bitPosition = int(parameter.get('BIT-POSITION'))
		if self.__ptype in ["CODED-CONST"]:
			self.__defaultValue = parameter.get('CODED-VALUE')
			self.__isComplex = False
			if not parameter.get('BIT-LENGTH'):
				# LOGGER.warning("{} BIT-LENGTH Is not exist, set to 8".format(self.dbParameter['SHORT-NAME']))
				MSGLogger.debug("MCD_DB: "+str(self.dbParameter['SHORT-NAME'])+" BIT-LENGTH Is not exist, set to 8")
			self.__bitLength = 8
			self.__bitLength = int(parameter.get('BIT-LENGTH'))
		if  self.__ptype == "PHYS-CONST":
			if not parameter.get("PHYS-CONSTANT-VALUE"):
				# LOGGER.warning("{} PHYS-CONSTANT-VALUE Is not exist, set to 8".format(self.dbParameter['SHORT-NAME']))
				MSGLogger.debug("MCD_DB: "+str(self.dbParameter['SHORT-NAME'])+" PHYS-CONSTANT-VALUE Is not exist, set to 8")
			self.__defaultValue = 0
		if self.__ptype in ["VALUE","PHYS-CONST"]:
			#parse DOP
			foundDop = False

			if parameter.get('DOP-REF'):
				for dop in conf['root']['Data_Object_Props']:
					if not dop.get('ID') or dop.get('ID') != parameter.get('DOP-REF'):
						continue
					conf['current'] = dop
					self.dbDop = DiagDop(conf, self)
					if len(self.dbDop.specialdisplay):
						self.specialdisplay.update(self.dbDop.specialdisplay)
					self.__dtype = self.dbDop.diagCode.baseDataType
					self.__bitLength = self.dbDop.diagCode.bitLength
					self.__radix = self.dbDop.physical.radix
					self.__distype = self.dbDop.physical.distype
					if "MIN-MAX-LENGTH-TYPE" == self.dbDop.diagCode.type:
						self.__dynamicType = self.dbDop.diagCode.type
						self.__dynamicMin = self.dbDop.diagCode.minlenth
						self.__dynamicMax = self.dbDop.diagCode.maxlenth
					self.__encoding = self.dbDop.diagCode.encoding
					if parameter.get("PHYSICAL-DEFAULT-VALUE"):
						compu = CompuMethod(self.dbDop)
						self.__defaultValue = compu.getInternalResult(parameter.get("PHYSICAL-DEFAULT-VALUE"))
					if parameter.get("PHYS-CONSTANT-VALUE")	and self.__ptype == "PHYS-CONST":
						compu = CompuMethod(self.dbDop)
						self.__defaultValue = compu.getInternalResult(int(parameter.get("PHYS-CONSTANT-VALUE")))#Temporarily think of it as a dec value
					#print(json.dumps(parameter, indent=2))
					foundDop = True
					break
			
			if not foundDop:
				for dtcDop in conf['root']['DTC-DOPS']:
					if not dtcDop.get("DTC-DOP") or not dtcDop.get("DTC-DOP").get("ID") or dtcDop.get("DTC-DOP").get("ID") != parameter.get('DOP-REF'):
						continue

					conf['current'] = dtcDop.get("DTC-DOP")
					self.dbDtcDop = DiagDtcDop(conf, self)
					self.__dtype = self.dbDtcDop.diagCode.baseDataType
					self.__bitLength = self.dbDtcDop.diagCode.bitLength
					self.__radix = self.dbDtcDop.physical.radix
					self.__distype = self.dbDtcDop.physical.distype
					self.__encoding = self.dbDtcDop.diagCode.encoding
					foundDop = True	

			if not foundDop:
				for muxDop in conf['root']['MUXS']:
					if not muxDop.get("ID") or muxDop.get("ID") != parameter.get('DOP-REF'):
						continue
					conf['current'] = muxDop
					# 初始化MUX DOP对象
					self.dbMuxDop = DiagMuxDop(conf, self)
					self.__bitLength = self.dbMuxDop.get_case_params_length()
					# 从MUX开关钥匙中获取物理类型/编码等信息
					# switch_dop = self.dbMuxDop.get_switch_key_dop()
					# if switch_dop:
					# 	self.__radix = switch_dop.physical.radix
					# 	self.__distype = switch_dop.physical.distype
					# 	self.__encoding = switch_dop.diagCode.encoding
					# 	self.specialdisplay.update(getattr(switch_dop, 'specialdisplay', {}))
					# 标记为已找到
					foundDop = True
					break

			if not foundDop:
				if parameter.get("DIDList"):
					foundDop = True
					for did in parameter.get("DIDList"):
						conf['current'] = did
						ep = MCDDbParameter(conf, self)
						self.parameters.append(ep)
						self.__ptype = "ENVDATA"
						subBitLength = int(ep.getBitPos()) + ep.getBitLength() + (ep.getBytePos()<<3)
						if self.__bitLength < subBitLength:
							self.__bitLength = subBitLength
				
			if not foundDop and parameter.get("PARAMS"):
				for p in parameter.get("PARAMS"):
					#print(json.dumps(parameter, indent=2))
					conf['current'] = p
					ps = MCDDbParameter(conf, self)
					self.parameters.append(ps)
					subBitLength = int(ps.getBitPos()) + ps.getBitLength() + (ps.getBytePos()<<3)
					if self.__bitLength < subBitLength:
							self.__bitLength = subBitLength
					foundDop = True

			if parameter.get('GENERATE'):
				foundDop = True

			if not foundDop:
				#print(json.dumps(parameter, indent=2))
				# LOGGER.warning("{} is DOP-REF {} is not found".format(parameter['SHORT-NAME'],parameter.get('DOP-REF')))
				MSGLogger.debug("MCD_DB: "+str(parameter['SHORT-NAME'])+" is DOP-REF "+str(parameter.get('DOP-REF'))+" is not found")
				#raise RuntimeError(parameter['SHORT-NAME'] + " DOP-REF not found")
				self.__isComplex = False
		elif self.__ptype == "TABLE-KEY":
			if parameter.get('TABLE-ROW-REF'):
				self.__dbTableKey = dict(type='TABLE-ROW-REF', value=parameter['TABLE-ROW-REF'])
				self.__bitLength = 16
			elif parameter.get('TABLE-REF'):
				self.__dbTableKey = dict(type='TABLE-REF', value=parameter['TABLE-REF'])
				self.__bitLength = 16
			else :
				raise RuntimeError()
		elif self.__ptype == "TABLE-STRUCT":
			if not parameter.get('TABLE-KEY-REF'):
				raise RuntimeError(parameter['SHORT-NAME'] + " TABLE-KEY-REF not found")
			tableKeyRef =  parameter.get('TABLE-KEY-REF')[0]
			if not tableKeyRef.get('PARAMS'):
				raise RuntimeError(parameter['SHORT-NAME'] + " TABLE-KEY-REF PARAMS not found")
			for p in tableKeyRef.get('PARAMS'):
				pass

		elif self.__ptype == "MATCHING-REQUEST-PARAM":
			pass
		elif self.__ptype == "NRC-CONST":
			#print(json.dumps(parameter, indent=2))
			self.diagCode = DiagCodeType(parameter)
			self.__bitLength = self.diagCode.bitLength
			self.__dtype = self.diagCode.baseDataType
			codedValues = parameter.get('CODED-VALUE')
			if not codedValues:
				# LOGGER.error("{} CODED-VALUE is missing".format(parameter['SHORT-NAME']))
				MSGLogger.debug("MCD_DB: "+str(parameter['SHORT-NAME'])+" CODED-VALUE is missing")
			if not isinstance(codedValues, list):
				codedValues = [codedValues]
			for cv in codedValues:
				self.codedValues.append(int(cv))
		
		elif self.__ptype == "RESERVED":
			self.__dtype = 'A_BYTEFIELD'
			self.__bitLength = int(parameter.get('BIT-LENGTH'))

		if parameter.get("BYTE-LENGTH"):
			self.__byteLength = int(parameter.get("BYTE-LENGTH"))

		
		
		#print(json.dumps(conf['root']['DTCS'][0], indent=2))

		# if self.getLongName() == "F53574C4634B44b28C332C86E6A43406_431A95832D224055B8D04BC887C8974A":
		# 	print(self.__bitLength)
		# 	print(json.dumps(parameter, indent=2))

		if parameter.get('TYPE') not in ["CODED-CONST", "VALUE", "TABLE-KEY", "TABLE-STRUCT", "MATCHING-REQUEST-PARAM", "NRC-CONST", "RESERVED", "PHYS-CONST"]:
			#print(json.dumps(parameter, indent=2))
			MSGLogger.debug("MCD_DB: "+str(parameter)+" TYPE is invalid")
			pass

	def getBitLength(self):
		#print(json.dumps(self.dbParameter, indent=2))
		return self.__bitLength

	def setBitLength(self, pos):
		self.__bitLength = pos

	def getBitPos(self):
		return self.__bitPosition

	def getByteLength(self):
		return self.__byteLength

	def getBytePos(self):
		return self.__bytePosition
	
	def setBytePos(self, pos):
		self.__bytePosition = pos

	def getCodedDefaultValue(self):
		return self.__defaultValue

	def getDataType(self):
		return self.__dtype
	def getPDataType(self):
		return self.__ptype
	def getDbParameters(self):
		return self.parameters

	def getDbTable(self):
		pass

	def getDbTableKeyParam(self):
		return self.__dbTableKey 

	def getDbTableStructParams(self):
		return self.__dbTableStruct

	def getDbUnit(self):
		return self.dbDop.unit if self.dbDop else ""

	def getDecimalPlaces(self):
		pass

	def getDefaultValue(self):
		return self.__defaultValue

	def getInternalConstraint(self):
		pass

	def getInterval(self):
		pass

	def getKeys(self):
		pass

	def getMaxLength(self):
		pass

	def getMinLength(self):
		pass

	def getParameterType(self):
		return self.__ptype

	def getRadix(self):
		return self.__radix

	def getPhysicalDisplayType(self):
		return self.__distype

	def getEncoding(self):
		return self.__encoding

	def getSemantic(self):
		pass

	def getStructureByKey(self):
		pass

	def getSystemParameterName(self):
		pass

	def getTextTableElements(self):
		pass

	def isComplex(self):
		return self.__isComplex

	def isConstant(self):
		pass

	def isVisible(self):
		pass
	
	def getDynamicLenthType(self):
		if self.__dynamicType == "MIN-MAX-LENGTH-TYPE":
			return self.__dynamicType,self.__dynamicMin,self.__dynamicMax
		else:
			return None,None,None

	def getSpecialDisplay(self):
		return self.specialdisplay

class MCDDbRequestParameter(MCDDbParameter):
	def __init__(self, conf, parent):
		super().__init__(conf, parent)
		self.parent.add(self)


class MCDDbRequestParameters(MCDNamedCollection):
	def __init__(self, conf, parent):
		super().__init__()
		self.parent = parent
		parameters = conf['current']
		for parameter in parameters:
			conf['current'] = parameter
			MCDDbRequestParameter(conf, self)

class MCDDbResponseParameters(MCDNamedCollection):
	def __init__(self, conf, parent, repeatParameter=None):
		super().__init__()
		self.parent = parent
		parameters = conf['current']
		#print(json.dumps(parameters, indent=2))
		for parameter in parameters:
			conf['current'] = parameter
			p = MCDDbResponseParameter(conf, self, repeatParameter)
			if repeatParameter is None or p.getBytePos() < repeatParameter.getBytePos():
				self.add(p)
			else:
				p.setBytePos(p.getBytePos() - repeatParameter.getBytePos())
				repeatParameter.parameters.append(p)
				subBitLength = int(p.getBitPos()) + int(p.getBitLength()) + (p.getBytePos()<<3) - (repeatParameter.getBytePos()<<3)
				if repeatParameter.getBitLength() < subBitLength:
					repeatParameter.setBitLength(subBitLength)


class MCDDbProtocolParameter(MCDDbRequestParameter):
	pass

class MCDDbResponseParameter(MCDDbParameter):

	def __init__(self, conf, parent, repeatParameter=None):
		super().__init__(conf, parent, repeatParameter)


class MCDDbTableParameter(MCDDbParameter):
	def __init__(self):
		super()

	def getKey(self):
		pass

	def getDbParameters(self):
		pass

class MCDDbTableParameters(MCDNamedCollection):

	def __init__(self):
		super()

	def getItemByIndex(self):
		pass

	def getItemByName(self):
		pass

	def getItemByKey(self):
		pass

class MCDDbTable(MCDDbObject):
	
	def __init__(self):
		super()

	def getDbDiagComPrimitiveByConnectorSemantic(self):
		pass

	def getDbDiagComPrimitives(self):
		pass

	def getDbTableRows(self):
		pass

	def getKeys(self):
		pass

	def getSemantic(self):
		pass

class MCDDbTables(MCDNamedCollection):
	
	def __init__(self):
		super()
		self.__dbTables = []

	def getItemByIndex(self):
		pass

	def getItemByName(self):
		pass	