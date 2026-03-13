class MCDValue():
	
	def __init__(self):
		#self._state = "uninitialized"
		self.type = "NO_TYPE"
		self.value = None
		self.bs = None
	
	def isValid(self):
		#return self._state != "uninitialized"
		return self.type != "NO_TYPE"
		
	def getValue(self):
		return self.bs

	def getCodeValue(self):
		return self.value

	def setAsciistring(self, value, bs=bytearray()):
		self.type = "ASCIISTRING"
		self.value = value
		self.bs = bs
			 
	def setBitfield(self, value, bs=bytearray()):
		self.type = "BITFIELD"
		self.value = value
		self.bs = bs

	def setBoolean(self, value, bs=bytearray()):
		self.type = "BOOLEAN"
		self.value = value
		self.bs = bs

	def setBytefield(self, value, bs=bytearray()):
		self.type = "BYTEFIELD"
		self.value = value
		self.bs = bs

	def setFloat32(self, value, bs=bytearray()):
		self.type = "FLOAT32"
		self.value = value
		self.bs = bs

	def setFloat64(self, value, bs=bytearray()):
		self.type = "FLOAT64"
		self.value = value
		self.bs = bs

	def setInt8(self, value, bs=bytearray()):
		self.type = "INT8"
		self.value = value
		self.bs = bs

	def setInt16(self, value, bs=bytearray()):
		self.type = "INT16"
		self.value = value
		self.bs = bs

	def setInt32(self, value, bs=bytearray()):
		self.type = "INT32"
		self.value = value
		self.bs = bs

	def setInt64(self, value, bs=bytearray()):
		self.type = "INT64"
		self.value = value
		self.bs = bs

	def setUInt8(self, value, bs=bytearray()):
		self.type = "UINT8"
		self.value = value
		self.bs = bs

	def setUInt16(self, value, bs=bytearray()):
		self.type = "UINT16"
		self.value = value
		self.bs = bs

	def setUInt32(self, value, bs=bytearray()):
		self.type = "UINT32"
		self.value = value
		self.bs = bs

	def setUInt64(self, value, bs=bytearray()):
		self.type = "UINT64"
		self.value = value
		self.bs = bs

	def setUnicode2string(self, value, bs=bytearray()):
		self.type = "UNICODE2STRING"
		self.value = value
		self.bs = bs

	def getInt8(self):
		return self.value
	
	def getInt16(self):
		return self.value

	def getInt32(self):
		return self.value

	def getInt64(self):
		return self.value

	def getUInt8(self):
		return self.value
	
	def getUInt16(self):
		return self.value

	def getUInt32(self):
		return self.value

	def getUInt64(self):
		return self.value

class MCDObjectFactory():

	@staticmethod
	def createValue():
		return MCDValue()