import struct
import sys
import copy
from .mcd_object import  MCDNamedCollection,MCDNamedObject
from .mcd_value import MCDObjectFactory
from .mcd_db_parameter import CompuMethod, DiagMuxDop
from  libs.util.DiagLog import MSGLogger

import json


def bitSet(arr, i):
	pos = i//8
	offset = i%8
	arr[pos] = arr[pos] | (1<<offset)

def bitClear(arr, i):
	pos = i//8
	offset = i%8
	arr[pos] = arr[pos] & ~(1<<offset)

def bitIsSet(arr, i):
	pos = i//8
	offset = i%8
	return arr[pos] & (1<<offset)

class MCDParameter(MCDNamedObject):
	def __init__(self, dbParameter):
		super().__init__(dbParameter.getShortName(), dbParameter.getLongName(), dbParameter.getDescription())
		self.__dbObject = dbParameter
		self.__textTableElement = None
		self.__scaleConstraint = None
		self.__complex = False
		self.__interval = None
		self.value = MCDObjectFactory.createValue()

	def getDataType(self):
		return self.__dbObject.getDataType()

	def getDbDTC(self):
		if self.__dbObject.dbDtcDop:
			for dtc in self.__dbObject.dbDtcDop.dtcs:
				if dtc.troubleCode == self.value.getValue():
					return dtc.conf

		return None

	def getDbObject(self):
		return self.__dbObject

	def getDbUnit(self):
		return self.__dbObject.getDbUnit()

	def getDecimalPlaces(self):
		pass

	def getParameterType(self):
		return self.__dbObject.getParameterType()

	def getScaleConstraint(self):
		return self.__scaleConstraint

	def getSystemParameterName(self):
		return self.__ptype

	def getTextTableElement(self):
		return self.__textTableElement

	def getValue(self):
		if self.isComplex():
			#raise RuntimeError("parameter {} is complex, cant call getParameters".format(self.getShortName()))
			pass
		return self.value

	def getValueRangeInfo(self):
		return self.__interval

	def isComplex(self):
		return self.__dbObject.isComplex()


class MCDRequestParameter(MCDParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.parameters = []

	def addParameters(self, p):
		self.__parameters.append(p)

	def getCodedValue(self):
		#print(json.dumps(self.getDbObject().getDbParameters(), indent=2))
		#print("dbDop", type(self.getDbObject().dbDop))
		if self.getDbObject().dbDop:
			#print(json.dumps(self.getDbObject().dbDop, indent=2))
			pass
		
		return self.value.getValue()
	
	def getLengthKey(self):
		pass

	def getParameters(self):
		if not self.isComplex():
			raise RuntimeError("parameter {} is simple, cant call getParameters".format(self.getShortName()))
		return self.__parameters

	def isVariableLength(self):
		pass

	def setCodedValue(self, value):
		if self.isComplex():
			return
		interval = self.getValueRangeInfo()
		if interval is None or value < interval.getLowerLimit() or value > interval.getUpperLimit():
			return
		scaleConstraint =  self.getScaleConstraint()
		if scaleConstraint is not None or value in scaleConstraint.getScaleConstraints():
			return

		self.__value = value

	# need translate to phisical representation
	def setValue(self, value):
		#LOGGER.info("{} set parameter {}".format(self.getShortName(), str(value)))
		if self.getParameterType() != "VALUE" and self.getParameterType() != "RESERVED":
			# LOGGER.error("{} getParameterType Is not correct".format(self.getShortName()))
			MSGLogger.debug("MCD: "+str(self.getShortName())+" getParameterType Is not correct")
		elif self.getParameterType() != "VALUE":
			return

		self.setValueUnchecked(value)

	def setValueUnchecked(self, value):
		bitLength = self.getDbObject().getBitLength()
		byteOcupied = (bitLength-1)//8+1
		# if int(value) > 255:
		# 	byteOcupied = byteOcupied+1
		if self.getDataType() == 'A_UINT8':
			self.value.setUInt8(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT16':
			self.value.setUInt16(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT32':
			#print(json.dumps(self.getDbObject().getDbParameters(), indent=2))
			self.value.setUInt32(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT64':
			self.value.setUInt64(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_INT8':
			self.value.setInt8(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT16':
			self.value.setInt16(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT32':
			self.value.setInt32(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT64':
			self.value.setInt64(int(value), int(value).to_bytes(byteOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_ASCIISTRING':
			self.value.setAsciistring(value, value.encode('ascii'))
		elif self.getDataType() == 'A_BYTEFIELD':
			self.value.setBytefield(value, value)

	def serialize(self, mem):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1

		value = self.getCodedValue()
		#print(type(value), len(value)<<3, value.hex(), bitLength, bitPos, bytePos)
		#TODO now is bit-copy, may be subtituded by byte-copy
		payload = mem[bytePos:]
		for i in range(min(bitLength, len(value)<<3)):
			if bitIsSet(value, i):
				bitSet(payload, bitPos + i)

		return mem[0:bytePos+byteCount]

class MCDRequestCodedConstParameter(MCDRequestParameter):

	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.setValueUnchecked(self.getDbObject().getCodedDefaultValue())

class MCDRequestPhysConstParameter(MCDRequestParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.setValueUnchecked(self.getDbObject().getCodedDefaultValue())


class MCDRequestValueParameter(MCDRequestParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.parameters = MCDRequestParameters(dbParameter.getDbParameters())
		codedValue = self.getDbObject().getCodedDefaultValue()
		if codedValue is not None:
			self.setValueUnchecked(codedValue)
	def serialize(self, mem):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1

		value = self.getCodedValue()
		if self.getDbObject().dbDop and self.getDbObject().dbDop.diagCode.type == "MIN-MAX-LENGTH-TYPE":
			bitLength = len(value)<<3
			byteCount = (bitPos+bitLength)//8
		#print(type(value), len(value)<<3, value.hex(), bitLength, bitPos, bytePos)
		#TODO now is bit-copy, may be subtituded by byte-copy
		g = None
		if not len(self.parameters):
			payload = mem[bytePos:]
			for i in range(min(bitLength, len(value)<<3)):
				if bitIsSet(value, i):
					bitSet(payload, bitPos + i)
		else:
			count = bytePos
			for p in self.parameters:
				payload = mem[bytePos:]
				g = p.serialize(payload)
				count = count + len(g) 
			byteCount = count

		return mem[0:bytePos+byteCount]


class MCDRequestTableKeyParameter(MCDRequestParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		tbKey = self.getDbObject().getDbTableKeyParam()
		if tbKey['type'] == 'TABLE-ROW-REF':
			#TODO static table key, need key DOP, __parameters compose[key, table-row], table-row maybe contains more parameters
			#to be fixed
			self.setValueUnchecked(int(tbKey['value']))
			pass
		elif tbKey['type'] == 'TABLE-REF':
			#TODO dynamic table key, need table, __parameters compose[key, table-row]
			pass

class MCDRequestReservedParameter(MCDRequestParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		byteCount = (dbParameter.getBitLength()-1)//8 + 1

		bs = bytearray(byteCount)

		self.setValueUnchecked(bs)

class MCDRequestParameters(MCDNamedCollection):
	
	def __init__(self, dbRequestParameters):
		super().__init__()
		self.__dbRequestParameters = dbRequestParameters
		for p in self.__dbRequestParameters:
			if p.getParameterType() == "CODED-CONST":
				self.add(MCDRequestCodedConstParameter(p))
			elif p.getParameterType() == "PHYS-CONST":
				self.add(MCDRequestPhysConstParameter(p))
			elif p.getParameterType() == "VALUE":
				self.add(MCDRequestValueParameter(p))
			elif p.getParameterType() == "TABLE-KEY":
				self.add(MCDRequestTableKeyParameter(p))
			elif p.getParameterType() == "RESERVED":
				self.add(MCDRequestReservedParameter(p))
			else:
				print(p.getParameterType(), str(sys._getframe().f_lineno))
	
	def setParameterByName(self):
		pass
	def getParameters(self):
		return self.__dbRequestParameters

class MCDProtocolParameter(MCDRequestParameter):
	pass

class MCDResponseParameter(MCDParameter):
	
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.parameters = MCDResponseParameters(dbParameter.getDbParameters())
		self.results = []
	def __getitem__(self, i):
		return self.MCDObjects[i][i]

	def getValue(self):
		#print(json.dumps(self.getDbObject().dbDop, indent=2), str(sys._getframe().f_lineno))
		#print(self.getDbObject().getShortName())
		if self.getDbObject().dbDop:
			#print(json.dumps(self.getDbObject().dbDop.conf, indent=2), str(sys._getframe().f_lineno))
			compu = CompuMethod(self.getDbObject().dbDop)
			return compu.getResult(self.value.getCodeValue())
		
		return self.value.getCodeValue()

	def isVariableLength(self):
		pass

	def getLengthKey(self):
		pass

	def setValue(self, value):
		# LOGGER.info("{} set parameter {}".format(self.getShortName(), str(value)))
		MSGLogger.debug("MCD: "+str(self.getShortName())+" set parameter "+str(value))
		if self.getParameterType() != "VALUE":
			# LOGGER.error("{} getParameterType Is not correct".format(self.getShortName()))
			MSGLogger.debug("MCD: "+str(self.getShortName())+" getParameterType Is not correct")
			return
		
		self.setValueUnchecked(value)

	def setValueUnchecked(self, value):
		bitLength = self.getDbObject().getBitLength()
		bitOcupied = ((bitLength-1)//8+1)*8
		if self.getDataType() == 'A_UINT8':
			self.value.setUInt8(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT16':
			self.value.setUInt16(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT32':
			self.value.setUInt32(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_UINT64':
			self.value.setUInt64(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=False))
		elif self.getDataType() == 'A_INT8':
			self.value.setInt8(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT16':
			self.value.setInt16(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT32':
			self.value.setInt32(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_INT64':
			self.value.setInt64(int(value), int(value).to_bytes(bitOcupied, byteorder='big', signed=True))
		elif self.getDataType() == 'A_ASCIISTRING':
			self.value.setAsciistring(value, value.encode('ascii'))
		elif self.getDataType() == 'A_BYTEFIELD':
			self.value.setBytefield(value, value)

	def setValueByBytearray(self, barray):
		bitLength = self.getDbObject().getBitLength()
		if self.getDataType() == 'A_UINT8':		
			integers = struct.unpack('>'+'B', barray[0:1])
			self.value.setUInt8(int(integers[0]), barray)
		elif self.getDataType() == 'A_UINT16':
			integers = struct.unpack('>'+'H', barray[0:2])
			self.value.setUInt16(int(integers[0])>>(16-bitLength), barray)
		elif self.getDataType() == 'A_UINT32':
			integers = struct.unpack('>'+'I', barray[0:4])
			if bitLength == 1:
				self.value.setUInt32(int(integers[0])>>(32-8), barray)
			else:	
				self.value.setUInt32(int(integers[0])>>(32-bitLength), barray)
		elif self.getDataType() == 'A_UINT64':
			integers = struct.unpack('>'+'Q', barray[0:8])
			self.value.setUInt64(int(integers[0]>>(64-bitLength)), barray)
		elif self.getDataType() == 'A_INT8':
			integers = struct.unpack('>'+'b', barray[0:1])
			self.value.setInt8(int(integers[0]), barray)
		elif self.getDataType() == 'A_INT16':
			integers = struct.unpack('>'+'h', barray[0:2])
			self.value.setInt16(int(integers[0])>>(16-bitLength), barray)
		elif self.getDataType() == 'A_INT32':
			integers = struct.unpack('>'+'i', barray[0:4])
			self.value.setInt32(int(integers[0])>>(32-bitLength), barray)
		elif self.getDataType() == 'A_INT64':
			integers = struct.unpack('>'+'q', barray[0:8])
			self.value.setInt64(int(integers[0])>>(64-bitLength), barray)
		elif self.getDataType() == 'A_ASCIISTRING':
			self.value.setAsciistring(barray, barray.decode('ascii'))
		elif self.getDataType() == 'A_BYTEFIELD':
			self.value.setBytefield(barray, barray)

	def deserialize(self, mem, reqPdu):
		pass

class MCDResponseCodedConstParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.setValueUnchecked(self.getDbObject().getCodedDefaultValue())

	def deserialize(self, mem, reqPdu):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1
		#TODO now is bit-copy, may be subtituded by byte-copy
		#print("@"*20, len(mem), (bitLength-1)//8 + 1, mem.tobytes().hex(), reqPdu.hex(), "MCDResponseCodedConstParameter")
		payload = mem[bytePos:]
		
		#check length
		if len(payload) < ((bitLength-1)//8 + 1):
			return False
		
		bs = self.value.getValue()

		for i in range(bitLength):
			if bitIsSet(payload, i) ^ bitIsSet(bs, bitPos + i):
				return False

		return True 


class MCDResponseNrcConstParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.codedValues = []
		
		#self.setValueUnchecked(self.getDbObject().getCodedDefaultValue())

	def deserialize(self, mem, reqPdu):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1
		#TODO now is bit-copy, may be subtituded by byte-copy

		payload = mem[bytePos:]
		
		#check length
		if len(payload) < ((bitLength-1)//8 + 1):
			return False
		
		bs = bytearray(max(byteCount, 8))
		for i in range(bitLength):
			if bitIsSet(payload, bitPos + i):
				bitSet(bs, i)

		self.setValueByBytearray(bs)
		'''
		backup code for bit paras
		v=0
		for i in range(bitLength):
			if bitIsSet(payload, bitPos + i):
				bitSet(bs, i)
				if self.getDataType not in ["A_ASCIISTRING","A_BYTEFIELD"]:
					v = 1<<i + v
		if not v:
			self.setValueUnchecked(v)
		else:
			self.setValueByBytearray(bs)
		'''
		#print(self.getDbObject().codedValues, self.value.getCodeValue(), str(sys._getframe().f_lineno))
		#check nrc value
		return True if self.value.getCodeValue() in self.getDbObject().codedValues else False

class MCDResponsePhysConstParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		self.setValueUnchecked(self.getDbObject().getCodedDefaultValue())
		#TODO translate to internal repretation

	def deserialize(self, mem, reqPdu):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1
		#TODO now is bit-copy, may be subtituded by byte-copy
		payload = mem[bytePos:]
		
		#check length
		if len(payload) < ((bitLength-1)//8 + 1):
			return False
		if self.getDbObject().getParameterType() == "PHYS-CONST":
			return True
		bs = self.value.getValue()
		for i in range(bitLength):
			if bitIsSet(payload, bitPos + i) ^ bitIsSet(bs, i):
				return False

		return True 


class MCDResponseMatchingRequestParamConstParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)

	def deserialize(self, mem, reqPdu):
		bytePos = self.getDbObject().getBytePos()
		byteLength = self.getDbObject().getByteLength()
		bitPos = self.getDbObject().getBitPos()
		bitLength = self.getDbObject().getBitLength()
		byteCount = (bitPos+bitLength)//8
		#byteOcupied = (bitLength-1)//8+1
		#TODO now is bit-copy, may be subtituded by byte-copy
		payload = mem[bytePos:]
		
		#check length
		if len(payload) < ((bitLength-1)//8 + 1):
			return False
		
		bs = reqPdu
		for i in range(bitLength):
			if bitIsSet(payload, bitPos + i) ^ bitIsSet(bs, i):
				return False

		return True 

class MCDResponseValueParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		# 新增：标记是否为MUX参数
		self.is_mux_param = hasattr(dbParameter, "dbMuxDop") and isinstance(dbParameter.dbMuxDop, DiagMuxDop)

	def get_mux_parse_result(self):
		"""
		获取MUX解析结果（仅对MUX参数有效）
		:return: dict - MUX解析结果 / None
		"""
		if self.is_mux_param and hasattr(self, "mux_parse_result"):
			return self.mux_parse_result
		return None
	
	def _parse_mux_switch_value(self, mem):
		"""
		解析MUX开关值（内部方法）
		:param mem: 完整响应数据（字节数组）
		:return: (switch_raw_value, switch_physical_value, mux_raw_data) 或 (None, None, None)
		"""
		mux_dop = self.getDbObject().dbMuxDop

		# 2. 获取MUX开关钥匙配置
		switch_key = mux_dop.switch_key
		if not switch_key or not switch_key.db_dop:
			MSGLogger.error(f"MUX解析失败：{self.getShortName()} 无有效开关钥匙配置")
			return None, None
		# 3. 从MUX数据帧中截取开关值
		switch_byte_pos = switch_key.byte_position
		switch_dop_obj = switch_key.db_dop
		switch_bit_length = switch_dop_obj.diagCode.bitLength
		switch_byte_length = (switch_bit_length + 7) // 8

		if switch_byte_pos + switch_byte_length > len(mem):
			MSGLogger.error(f"MUX解析失败：{self.getShortName()} 开关值字节位置越界")
			return None, None

		# 截取开关值字节并转换为整数
		switch_bytes = mem[switch_byte_pos:switch_byte_pos + switch_byte_length]
		switch_raw_value = int.from_bytes(switch_bytes, byteorder='big')

		# 4. 转换为物理值
		try:
			compu = CompuMethod(switch_dop_obj)
			switch_physical_value = compu.getResult(switch_raw_value)
		except Exception as e:
			MSGLogger.error(f"MUX解析失败：{self.getShortName()} 开关值转换失败，错误={str(e)}")
			switch_physical_value = None
		return switch_raw_value, switch_physical_value
	
	def _parse_mux_case_params(self, mux_raw_data, switch_physical_value):
		"""
		解析匹配的MUX分支参数（内部方法）
		:param mux_raw_data: MUX整体数据帧
		:param switch_physical_value: 开关物理值
		:return: 分支参数列表 或 None
		"""
		mux_dop = self.getDbObject().dbMuxDop
		# 1. 匹配MUX分支
		matched_case = mux_dop.match_case(switch_physical_value)
		if not matched_case:
			MSGLogger.warning(f"MUX解析：{self.getShortName()} 无匹配分支，使用默认分支={mux_dop.default_case}")
			return None
		mux_dop.case_params_length = matched_case.getParamsLength()
		# mux_dop.case_params_length = 2
		# 2. 解析分支内的参数
		case_params = []
		for case_param_db in matched_case.params:
			# 创建分支参数对象
			case_param = MCDResponseValueParameter(case_param_db)
			# 反序列化分支参数（相对于MUX数据帧）
			case_param.deserialize(mux_raw_data, None)
			case_params.append(case_param)

		# self.matched_mux_case_params = case_params
		return case_params

	def deserialize(self, mem, reqPdu):
		# ===== 新增：优先处理MUX参数 =====
		if self.is_mux_param:
			# 1. 解析MUX开关值
			switch_raw_value, switch_physical_value = self._parse_mux_switch_value(mem)
			if not switch_raw_value:
				return False

			MSGLogger.debug(f"MUX解析：{self.getShortName()} 开关原始值=0x{switch_raw_value:X}，物理值={switch_physical_value}")
			# 2. 解析匹配的分支参数
			case_params = self._parse_mux_case_params(mem[self.getDbObject().dbMuxDop.byte_position:], switch_physical_value)
			if not case_params:
				return False

			# ===== 修正：存储原始MUX数据帧到value（和其他参数语义一致）=====
			# self.setValueByBytearray(mux_raw_data)  # 核心修正：存原始字节，而非JSON
			self.mux_parse_result = case_params
			return True
		else:
			bytePos = self.getDbObject().getBytePos()
			#byteLength = self.getDbObject().getByteLength()
			bitPos = self.getDbObject().getBitPos()
			bitLength = self.getDbObject().getBitLength()
			byteCount = (bitPos+bitLength-1)//8+1
			maxByteCount = 8 #long long type
			#byteOcupied = (bitLength-1)//8+1
			#TODO now is bit-copy, may be subtituded by byte-copy
			payload = mem[bytePos:]
			# print(json.dumps(self.getDbObject().dbParameter, indent=2), str(sys._getframe().f_lineno), payload.hex())
			#check length
			
			if not len(self.parameters):
				if self.getDbObject().dbDop and self.getDbObject().dbDop.diagCode.type == "MIN-MAX-LENGTH-TYPE" :
					bitLength = len(payload)<<3
					byteCount = (bitPos+bitLength-1)//8 +1
					maxByteCount = byteCount
				else:
					if len(payload) < ((bitPos + bitLength-1)//8 + 1) :
						return False 
			
				bs = bytearray(max(byteCount, maxByteCount))
				for i in range(bitLength):
					if bitIsSet(payload, bitPos + i):
						bitSet(bs, i)
				self.setValueByBytearray(bs)

				# if self.getShortName() == "DTC":
					# print("="*20, bs, self.getValue())
				return True if self.getValue() is not None else False
			else:
				#get the bytecount of substructure
				maxSubStructureCount = 0
				# for p in self.parameters:
				# 	subStructureCount += p.getDbObject().getBitLength()
				# 	# if subStructureCount > maxSubStructureCount:
				# 	# 		maxSubStructureCount = subStructureCount
				# if subStructureCount % 8 == 0 :
				# 	subStructureCount = subStructureCount // 8
				# 	maxSubStructureCount = subStructureCount
				# else:
				# 	subStructureCount = subStructureCount // 8 + 1 
				# 	maxSubStructureCount = subStructureCount
				payload = mem[bytePos:]
				while  len(payload) > 0:
					subStructureCount = 0
				# while  len(payload) >= 0:
					for p in self.parameters:
						# print(json.dumps(p.getDbObject().dbParameter, indent=2), str(sys._getframe().f_lineno), payload.hex())
						if not p.deserialize(payload, reqPdu):
							return False
						if not hasattr(p, "is_mux_param") or not p.is_mux_param:
							subStructureCount += p.getDbObject().getBitLength()
						else:
							subStructureCount += p.getDbObject().dbMuxDop.get_case_params_length()
					if subStructureCount % 8 == 0 :
						subStructureCount = subStructureCount // 8
						# maxSubStructureCount = subStructureCount
					else:
						subStructureCount = subStructureCount // 8 + 1 
						# maxSubStructureCount = subStructureCount
					payload = payload[subStructureCount:]
					# the end of pdu is saved in results
					par = copy.deepcopy(self.parameters)
					self.results.append(par)
				return True
class MCDResponseReserveParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
	def deserialize(self, mem, reqPdu):
		return True
class MCDResponseTableKeyParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)
		# TABLE-ROW-REF mean static table
		tbKey = self.getDbObject().getDbTableKeyParam()
		if tbKey['type'] == 'TABLE-ROW-REF':
			#TODO static table key, need key DOP, __parameters compose[key, table-row], table-row maybe contains more parameters
			#to be fixed
			self.setValueUnchecked(int(tbKey['value']))
			pass
		elif tbKey['type'] == 'TABLE-REF':
			#TODO dynamic table key, need table, __parameters compose[key, table-row]
			pass
		#print(json.dumps(self.getDbObject().getDbParameters(), indent=2), str(sys._getframe().f_lineno))

	def deserialize(self, mem, reqPdu):
		return True

class MCDResponseEnvDataParameter(MCDResponseParameter):
	def __init__(self, dbParameter):
		super().__init__(dbParameter)

	def deserialize(self, mem, reqPdu):
		bytePos = self.getDbObject().getBytePos()
		#byteLength = self.getDbObject().getByteLength()
		#byteOcupied = (bitLength-1)//8+1
		#TODO now is bit-copy, may be subtituded by byte-copy
		# print(json.dumps(self.getDbObject().dbParameter, indent=2), str(sys._getframe().f_lineno))
		#check length
		
		payload = mem[bytePos:]
		for p in self.parameters:
			if not p.deserialize(payload, reqPdu):
				return False
			
		return True


class MCDResponseParameters(MCDNamedCollection):
	
	def __init__(self, dbResponseParameters):
		super().__init__()
		self.__dbResponseParameters = dbResponseParameters
		for p in self.__dbResponseParameters:
			if p.getParameterType() == "CODED-CONST":
				self.add(MCDResponseCodedConstParameter(p))
			elif p.getParameterType() == "NRC-CONST":
				self.add(MCDResponseNrcConstParameter(p))
			elif p.getParameterType() == "PHYS-CONST":
				self.add(MCDResponsePhysConstParameter(p))
			elif p.getParameterType() == "MATCHING-REQUEST-PARAM":
				self.add(MCDResponseMatchingRequestParamConstParameter(p))
			elif p.getParameterType() == "VALUE":
				self.add(MCDResponseValueParameter(p))
			elif p.getParameterType() == "TABLE-KEY":
				self.add(MCDResponseTableKeyParameter(p))
			elif p.getParameterType() == "RESERVED":
				self.add(MCDResponseReserveParameter(p))
			elif p.getParameterType() == "ENVDATA":
				self.add(MCDResponseEnvDataParameter(p))
			else:
				#self.add(MCDResponseParameter(p))
				print(p.getParameterType(), str(sys._getframe().f_lineno))
				pass

	# for field	
	def addElement(self):
		pass

	# for field
	def addElementWithContent(self):
		pass

	def addEnvDataByDTC(self):
		pass 

	def addMuxBranch(self):
		pass

	def addMuxBranchByIndex(self):
		pass

	def addMuxBranchByIndexWithContent(self):
		pass

	def addMuxBranchByMuxValue(self):
		pass

	def addMuxBranchWithContent(self):
		pass

	# simple dop
	def setParameterByName(self):
		pass
	