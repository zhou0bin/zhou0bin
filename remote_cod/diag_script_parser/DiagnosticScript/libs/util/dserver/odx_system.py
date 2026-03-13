import os
import inspect
from  .mcd_system import getMCDSystem
from  .mcd_diag_com_primitive import  *
from  .mcd_db_parameter import CompuMethod
import ast
import json
import traceback
from  libs.appAPI.log_info import handler_exception_decorator,MSGLogger,ecuDtObj,IS_SECURE_STARTUP,PATHCONFIG
# from  libs.appAPI.ExcuteServiceAndCheckResponse import ExcuteServiceAndCheckResponse
import collections
from  libs.appAPI.ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
from libs.appAPI.utils.util_helper import GetScriptRoot

# ======================== 新增：MUX参数解析辅助函数 ========================
def dumpMuxParameter(p):
    """
    解析MUX类型参数（专门处理MUX分支参数）
    :param p: MCDResponseValueParameter（MUX参数对象）
    :return: dict - MUX解析结果
    """
    mux_result = {}
    # 1. 检查是否为MUX参数
    if not hasattr(p, "is_mux_param") or not p.is_mux_param:
        return None

    # 2. 获取MUX解析结果
    mux_parse_result = p.get_mux_parse_result() if hasattr(p, "get_mux_parse_result") else None
    if not mux_parse_result:
        mux_result[p.getShortName()] = "MUX解析失败：未获取到解析结果"
        return mux_result
    mux_base_info = {}
    # 4. 递归解析分支内的参数（复用dumpParameter）
    # case_params = mux_parse_result
    for case_param in mux_parse_result:
        mux_base_info.update(dumpParameter(case_param))
    # 5. 最终返回MUX结构化结果
    mux_result[p.getShortName()] = mux_base_info
    return mux_result

def dumpParameter(p):
	result = dict()
	# ===== 新增：优先处理MUX参数 =====
	mux_dump_result = dumpMuxParameter(p)
	if mux_dump_result:
		return mux_dump_result
	if len(p.parameters) == 0:
		if p.getDbObject().getEncoding().find("BCD-P") != -1:
			if p.getDbObject().getRadix() != "DEC":
				v= p.getValue()
				result[p.getShortName()] = "{}{}".format(p.getValue(), p.getDbUnit())
			else:
				result[p.getShortName()] = "{}{}".format(hex(p.getValue())[2:], p.getDbUnit())
		# elif p.getDbObject().getRadix() == "HEX":
		# 	# if type(p.getValue()) == bytes or type(p.getValue()) == bytearray:
		# 	# 	result[p.getShortName()] = "{}{}".format(p.getValue().decode("ascii").strip('\x00'), p.getDbUnit())
		# 	# else:
		# 	result[p.getShortName()] = "{}{}".format(p.getValue(), p.getDbUnit())
		elif p.getDbObject().getRadix() == "":
			if p.getDbObject().getPhysicalDisplayType() == "A_BYTEFIELD":
				bitlenth = p.getDbObject().getBitLength()
				if bitlenth == 0:
					dynamic = p.getDbObject().getDynamicLenthType()
					if dynamic[0] == "MIN-MAX-LENGTH-TYPE":
						bitlenth = len(p.getValue())
						if bitlenth > dynamic[2]: 
							bitlenth = dynamic[2]
				else:
					if bitlenth%8 == 0:
						bitlenth = bitlenth // 8
					else:
						bitlenth = bitlenth // 8 + 1
				resul = p.getValue()[0:bitlenth]
				isall0:bool = False
				isall0_start = None
				for i in range(len(resul)):
					if resul[i] == 0:
						isall0 = True
						if isall0_start == None:
							isall0_start = i
					else:
						isall0 = False
						isall0_start = None
				if isall0:
					resul = resul[0:isall0_start]
				else:
					resul = resul[0:bitlenth]
				SpecialDisplay = p.getDbObject().getSpecialDisplay()
				if "IS-HEXDUMP" in SpecialDisplay.keys():
					if SpecialDisplay["IS-HEXDUMP"] == "TRUE":
						result[p.getShortName()] = "{}{}".format(resul.hex(), p.getDbUnit())
				else:
					result[p.getShortName()] = "{}{}".format(resul.hex(), p.getDbUnit())
			elif p.getDbObject().getPhysicalDisplayType() == "A_UNICODE2STRING":
				recstr = p.getValue()
				if type(recstr) == bytes or type(recstr) == bytearray:
					bitlenth = p.getDbObject().getBitLength()
					if bitlenth%8 == 0:
						bitlenth = bitlenth // 8
					else:
						bitlenth = bitlenth // 8 + 1
					recstr = recstr[0:bitlenth]
					isall0:bool = False
					isall0_start = None
					for i in range(len(recstr)):
						if recstr[i] == 0:
							isall0 = True
							if isall0_start == None:
								isall0_start = i
						else:
							isall0 = False
							isall0_start = None
					if isall0:
						recstr = recstr[0:isall0_start]
					else:
						recstr = recstr[0:bitlenth]
					recstr = recstr.decode("ascii")
					result[p.getShortName()] = "{}{}".format(recstr, p.getDbUnit())
				else:
					result[p.getShortName()] = "{}{}".format(recstr, p.getDbUnit())
			else:
				result[p.getShortName()] = "{}{}".format(p.getValue(), p.getDbUnit())
		else:
			if p.getDbObject().getRadix() == "DEC":
				result[p.getShortName()] = "{}{}".format(p.getValue(), p.getDbUnit())
			else:
				recstr = p.getValue()
				bitlenth = p.getDbObject().getBitLength()
				lenbitlenth = int(bitlenth/4)
				zfrecstr=str(recstr).zfill(lenbitlenth)
				result[p.getShortName()] = "{}{}".format(zfrecstr, p.getDbUnit())
		if "DTC" in result.keys():
			dop = p.getDbObject().dbDtcDop
			for dtc in dop.dtcs.MCDObjects:
				if dtc.troubleCode == int(p.getValue()):
					result.clear()
					result["SHORT-NAME"] = dtc.getShortName()
					result["TROUBLE-CODE"] = str(dtc.troubleCode)
					result["DISPLAY-TROUBLE-CODE"] = dtc.displayTroubleCode
					# result["TI"] = dtc.ti
					result["TEXT"] = dtc.text
					break
	elif not p.results:
		results = dict()
		for ps in p.parameters:
			results.update(dumpParameter(ps))
			result[p.getShortName()] = results
	else:
		result["DTCAndStatusRecord"] = []
		for r in p.results:
			results = {}
			for ps in r:
				dop = ps.getDbObject().dbDtcDop
				if dop:
					dumpParameter(ps)
					for dtc in dop.dtcs.MCDObjects:
						if dtc.troubleCode == int(ps.getValue()):
							results["SHORT-NAME"] = dtc.getShortName()
							results["TROUBLE-CODE"] = str(dtc.troubleCode)
							results["DISPLAY-TROUBLE-CODE"] = dtc.displayTroubleCode
							# results["TI"] = dtc.ti
							results["TEXT"] = dtc.text
							break
				else:
					results.update(dumpParameter(ps))
			result["DTCAndStatusRecord"].append(results)

	return result

def dserverInit(fd,ODXServer):
	'''
	result=4 unknow err
	result=3 Not enough storage err
	result=2 file format err
	result=1 no file err
	result=0 ok
	'''
	# Result = 4 
	# start_point = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
	# configPath = os.path.join(start_point,"Odx")
	# fileflag = False
	# logicalLinks = None
	# for file in os.listdir(configPath):
	# 	if file.find(Name) == -1:
	# 		continue
	# 	else:
	# 		filenamestr,filetype = os.path.splitext(file)
	# 		if filetype != ".json":
	# 			Result = 2 
	# 			return Result,logicalLinks
	# 		configPath = os.path.join(configPath,file)
	# 		fileflag = True		
	# 		break
	# if not fileflag:
	# 	Result = 1 
	# 	return Result,logicalLinks
	
	try:
		logicalLinks = None
		ecuinfo = {}
		system = getMCDSystem(fd,ODXServer)
		prj = system.getActiveProject()
		dbPrj = prj.getDbProject()
		dbVehicles = dbPrj.getDbVehicleInformations()
		dbLogicalLinks = dbVehicles[0].getDbLogicalLinks()

		logicalLinks = prj.getLogicalLinks()
		ecuinfo = dbPrj.getECUinfos()
		if not logicalLinks:
			logicalLinks.addByDbObject(dbLogicalLinks[0])
		Result = 0
		return Result,(logicalLinks,ecuinfo) 
	except Exception as e:
		# print(e)
		Result = 4 
		return Result,(logicalLinks,ecuinfo) 
def arg2Dict(arg):
	result = dict()
	for k,v in arg.__dict__.items():
		if isinstance(v, int):
			result[k] = v
		elif isinstance(v, float):
			result[k] = v
		elif isinstance(v, str):
			result[k] = v
		elif isinstance(v, bytes):
			result[k] = v
		elif isinstance(v, bytearray):
			result[k] = v
		elif isinstance(v, bool):
			result[k] = v
		elif isinstance(v, memoryview):
			pass
		elif isinstance(v, tuple):
			pass
		elif isinstance(v, list):
			pass
		elif isinstance(v, dict):
			pass
		elif isinstance(v, set):
			pass
		elif isinstance(v, frozenset):
			pass
		elif isinstance(v, enumerate):
			pass
		elif isinstance(v, range):
			pass
		elif isinstance(v, property):
			pass
		else:
			result[k] = arg2Dict(v)

	return result


def dict2Args(resultDict, output):
	for k,v in resultDict.items():
		if isinstance(v, dict):
			output.__dict__[k] = dict2Args(v, output.__dict__[k])
		else:
			output.__dict__[k] = v
		
	return output

def setArgs(args, parameters):
	match = False
	for k, v in args.items():
		p = None
		for p in parameters:
			if p.getShortName() == k:
				# found
				if p.getDbObject().getEncoding() == "BCD-P":
					v = int(str(v),16)
				p.setValue(v)
				match = True
				break
		
		if not match:
			if not p or p.getValue().isValid():
				return True

			# LOGGER.error("not found arguments {}".format(k))
			MSGLogger.error("not found arguments "+str(k))
			return False
	for p in parameters:
		if not p.getValue().isValid():
			# LOGGER.error("parameter {} not set.".format(p.getShortName()))
			MSGLogger.error("parameter "+str(p.getShortName())+" not set.")
			return False
	
	return True

class Programming_Control():
	def __init__(self):
		pass
		# self.P2_Server = 20
		# self.P2Star_Server = 200

_links:dict = {}
# ODXFD = -1
class odxdata():
	def __init__(self) -> None:
		self.key = None
		self.data = None
		self.name = None
	def clear(self):
		self.key = -1
		self.data = None
		self.name = None
# ODXFD = odxdata()
ODXFD = {}
@handler_exception_decorator(length = 2)
def LoadOdx(OdxFileName):
	# path = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
	'''
	result=4 unknow err
	result=3 Not enough storage err
	result=2 file format err
	result=1 no file err
	result=0 ok
	'''
	global ODXFD
	result = 4 
	if IS_SECURE_STARTUP:
		configPath = GetScriptRoot()
	else:
		# 获取执行脚本路径
		stack_frame = inspect.stack()[-1]
		configPath = os.path.dirname(os.path.abspath(stack_frame.filename))
		flag = False
		for file in os.listdir(configPath):
			if file.find(OdxFileName) != -1 and file.endswith(".json"):
				configPath = os.path.join(configPath,file)
				flag = True
				break
		if not flag:
			start_point = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
			configPath = os.path.join(start_point,"Odx")
			if not os.path.exists(configPath):
				result=1
				# ODXFD[-1] = None
				return result,None
			flag = False
			for file in os.listdir(configPath):
				if file.find(OdxFileName) != -1:
					configPath = os.path.join(configPath,file)
					flag = True
					break
			if not flag:
				result=1
				return result,None
	if not configPath.endswith(".json"):
		result=2
		return result,None
	try:
		fd = open(configPath,"rb")
		# ODXFD.data = fd.read()
		# ODXFD.key = id(ODXFD.data)
		# ODXFD.name = OdxFileName
		data = fd.read()
		ODXFD.update({id(data):data})
		fd.close()
		if data:
			result=0
			return result,id(data)
	except:
		result=4
		return result,None
	# result = 4
	# odxname = None
	# global _links
	# if OdxFileName in _links.keys():
	# 	result = 0
	# 	odxname = OdxFileName
	# else:
	# 	result,_links[OdxFileName] = dserverInit(OdxFileName,ODXServerList)
	# if _links[OdxFileName] == None:
	# 	odxname = None
	# else:
	# 	odxname = OdxFileName
	# return result,odxname
	
@handler_exception_decorator(length = 1)
def UnloadOdx(Handle):
	'''
	Result=2 unknow err
	Result=1 no data
	Result=0 ok
	'''
	global ODXFD
	Result = 2
	try:
		if Handle in ODXFD.keys():
			ODXFD.pop(Handle)
			Result=0
			return Result
		else:
			Result=1
			return Result
	except:
		Result=2
		return Result
	# Result = 2
	# try:
	# 	if Handle in _links.keys():
	# 		_links.pop(Handle)
	# 		Result = 0
	# 	else:
	# 		Result = 1
	# 	return Result
	# except Exception as e:
	# 	Result = 2
	# 	return Result

@handler_exception_decorator(length = 1)
def GetECUDtObjFromODX(Handle):
	global ODXFD
	if Handle in ODXFD.keys():
		odx = json.loads(ODXFD[Handle])
		if "Ecu_Name" in odx.keys():
			ecuinfosName = odx["Ecu_Name"]
		if "Request_Address" in odx.keys():
			ecuinfosRequestID = odx["Request_Address"]
		if "Response_Address" in odx.keys():
			ecuinfosResponseID = odx["Response_Address"]
		ecuobj = ecuDtObj(ecuinfosName,ecuinfosRequestID,ecuinfosResponseID,0,"object","ecu_344521486",None,"ecuDtObj",None)
		return ecuobj
	else:
		return None


@handler_exception_decorator(length = 1,listflag = True)
def DictionaryAppend(InputDictArray,InPutDict):
	if type(InPutDict) in [collections.OrderedDict,dict]:
		if type(InputDictArray) == list:
			InputDictArray.append(InPutDict)
			return InputDictArray
		else:
			MSGLogger.error("DictionaryAppend:InputDictArray is not list")
			return []
	else:
		MSGLogger.error("DictionaryAppend:InPutDict is not dict")
		return InputDictArray

#获取指定DTC的发送参数信息	
def ReadDTCParamByDID(tmp_odx, DID, DTC, StatStep):#DID:1904  1906   DTC:D21A87 ...
	service_short_name = ""
	DTCParam_shortname = ""
	dtc_param = ""
	record_number_param_name = ""
	record_number_param_value = ""

	dia_service_list = tmp_odx["Diag_Service"]
	find_service_node = next((s for s in dia_service_list if s.get("SEND") == DID), None)
	if find_service_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到DID:{DID} 对应的服务"
		return [service_short_name, DTCParam_shortname,dtc_param,record_number_param_name,record_number_param_value]
	#找到对应服务的shor-name
	service_short_name = find_service_node["SHORT-NAME"]
	#找到对应服务的请求参数
	request_param = find_service_node["REQUEST"]["PARAMS"]
	DTCParam_shortname = request_param[0]["SHORT-NAME"]
	DTCParam_value_dopid = request_param[0]["DOP-REF"]

	#找到DTCParam_value_dopid对应的Data_Object_Props子节点
	data_object_props_node = next((s for s in tmp_odx["Data_Object_Props"] if s.get("ID") == DTCParam_value_dopid), None)
	dtc_param = ""
	if data_object_props_node:#主要用于扩展数据
		#将DTC十六进制数据转成十进制
		dtc_dec = str(int(DTC, 16))
		dtc_param_node = next((s for s in data_object_props_node["PHYSIC-TO-HEX"] if s.get("HEX") == dtc_dec) , None)
		if dtc_param_node == None:
			StatStep.Result = "NOK"
			StatStep.Value = f"NOK@未找到DTC:{DTC} 对应的PHYSIC-TO-HEX节点"
			return [service_short_name, DTCParam_shortname,dtc_param,record_number_param_name,record_number_param_value]
		dtc_param = dtc_param_node["PHYSICAL"]
	else:#如果不存在Data_Object_Props节点,直接使用DTC的值，主要用于快照
		dtc_param = DTC	

	#找到RecordNumber参数名字及值
	record_number_param_name = find_service_node["REQUEST"]["PARAMS"][1]["SHORT-NAME"]
	record_number_param_dopid = find_service_node["REQUEST"]["PARAMS"][1]["DOP-REF"]
	record_number_props_node = next((s for s in tmp_odx["Data_Object_Props"] if s.get("ID") == record_number_param_dopid), None)
	if record_number_props_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到DTC record number refid:{record_number_param_dopid}对应的Data_Object_Props节点"
		return [service_short_name, DTCParam_shortname,dtc_param,record_number_param_name,record_number_param_value]
	#找到record number参数对应的物理值
	record_number_dop_phy_to_hex_node = next((s for s in record_number_props_node["PHYSIC-TO-HEX"] if s.get("HEX") == "255"), None)
	if record_number_dop_phy_to_hex_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到DTC record number:255 对应record number的Data_Object_Props节点"		
		return [service_short_name, DTCParam_shortname,dtc_param,record_number_param_name,record_number_param_value]	
	record_number_param_value = record_number_dop_phy_to_hex_node["PHYSICAL"]
	return [service_short_name, DTCParam_shortname,dtc_param,record_number_param_name,record_number_param_value]

#没有参数的服务
def CallOdxNoparamService(ecuDtObj, SendID, StatStep):
	ReturnResult = 4 
	VciResult = ""
	PositiveResponseParam = ""
	NegtiveResponseParam = ""

	global ODXFD
	odxdata = None
	for data in ODXFD.values():
		odxdata = data
	#将odxdata转换为字典
	tmp_odx = json.loads(odxdata)
	dia_service_list = tmp_odx["Diag_Service"]
	#找到DID对应的节点
	# next(for s in dia_service_node if s["Short_Name"] == DID)
	# send_id = "22" + DID
	# send_id = DID
	find_service_node = next((s for s in dia_service_list if s.get("SEND") == SendID), None)
	if find_service_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到send_id:{SendID} 对应的服务"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	#找到对应服务的shor-name
	service_short_name = find_service_node["SHORT-NAME"]	

	inpara = ""
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult,StpResult = CallOdxServiceByShortName(ecuDtObj, service_short_name, inpara, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]	

#切换会话
@handler_exception_decorator(length = 4)
def StartExtendedDiagnosticSession(ecuDtObj, StatStep):
	send_id = "1003"
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult = CallOdxNoparamService(ecuDtObj, send_id, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]

#根据DID读DID状态
@handler_exception_decorator(length = 4)
def ReadDIDInfoByID(ecuDtObj, DID, StatStep):
	send_id = "22" + DID
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult = CallOdxNoparamService(ecuDtObj, send_id, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]

#读DTC对应的扩展信息
@handler_exception_decorator(length = 4)
def ReadAllExtendedDataByDTC(ecuDtObj, DTC, StatStep):
	ReturnResult = 4 
	VciResult = ""
	PositiveResponseParam = ""
	NegtiveResponseParam = ""

	DID = "1906"
	global ODXFD
	odxdata = None
	for data in ODXFD.values():
		odxdata = data
	if ODXFD is None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@odx内容为空"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	#将odxdata转换为字典
	tmp_odx = json.loads(odxdata)
	
	service_short_name, DTCParam_shortname, dtc_param, record_number_param_name, record_number_param_value = ReadDTCParamByDID(tmp_odx, DID, DTC, StatStep)
	inpara = f"{{'{DTCParam_shortname}':'{dtc_param}','{record_number_param_name}':'{record_number_param_value}'}}"
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult,StpResult = CallOdxServiceByShortName(ecuDtObj, service_short_name, inpara, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]

#清指定DTC状态
@handler_exception_decorator(length = 4)
def CLearDtcs(ecuDtObj,StatStep):
	ReturnResult = 4 
	VciResult = ""
	PositiveResponseParam = ""
	NegtiveResponseParam = ""
	global ODXFD
	odxdata = None
	for data in ODXFD.values():
		odxdata = data
	#将odxdata转换为字典
	tmp_odx = json.loads(odxdata)
	if tmp_odx == "":
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@odx内容是空的"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	dia_service_list = tmp_odx["Diag_Service"]
	#找到DID对应的节点
	# next(for s in dia_service_node if s["Short_Name"] == DID)
	send_id = "14"
	# send_id = DID
	find_service_node = next((s for s in dia_service_list if s.get("SEND") == send_id), None)
	if find_service_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到send_id:{send_id} 对应的服务"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	#找到对应服务的shor-name
	service_short_name = find_service_node["SHORT-NAME"]	
	#找到对应服务的请求参数
	request_param = find_service_node["REQUEST"]["PARAMS"]
	param_shortname = request_param[0]["SHORT-NAME"]
	param_value_dopid = request_param[0]["DOP-REF"]

	param_value_props_node = next((s for s in tmp_odx["Data_Object_Props"] if s.get("ID") == param_value_dopid), None)
	if param_value_props_node == None:
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@未找到param_value_props_node:{param_value_props_node} 对应的Data_Object_Props节点"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	#找到record number参数对应的物理值
	param_value_dop_phy_to_hex_node = param_value_props_node["PHYSIC-TO-HEX"]
	if type(param_value_dop_phy_to_hex_node) is list:
		param_value_dop_phy_to_hex_node = param_value_dop_phy_to_hex_node[0]

	param_value = param_value_dop_phy_to_hex_node["PHYSICAL"]
	inpara = f"{{'{param_shortname}':'{param_value}'}}"
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult,StpResult = CallOdxServiceByShortName(ecuDtObj, service_short_name, inpara, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]	

@handler_exception_decorator(length = 4)
def ReadSnapshotByID(ecuDtObj, DTC, StatStep):
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult,StpResult = ReadAllSnapshotByDTC(ecuDtObj, DTC, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]

#读DTC对应快照信息
@handler_exception_decorator(length = 4)
def ReadAllSnapshotByDTC(ecuDtObj, DTC, StatStep):
	ReturnResult = 4 
	VciResult = ""
	PositiveResponseParam = ""
	NegtiveResponseParam = ""

	global ODXFD
	odxdata = None
	for data in ODXFD.values():
		odxdata = data
	DID = "1904"
	#将odxdata转换为字典
	tmp_odx = json.loads(odxdata)
	if tmp_odx == "":
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@odx内容是空的"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	service_short_name, DTCParam_shortname, dtc_param, record_number_param_name, record_number_param_value = ReadDTCParamByDID(tmp_odx, DID, DTC, StatStep)
	if service_short_name == "" or DTCParam_shortname == "" or dtc_param == "" or record_number_param_name == "" or record_number_param_value == "":
		StatStep.Result = "NOK"
		StatStep.Value = f"NOK@部分参数是空的,如服务名,dtc名,dtc值,记录号名,记录号值"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]	
	inpara = f"{{'{DTCParam_shortname}':'{dtc_param}','{record_number_param_name}':'{record_number_param_value}'}}"
	ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult,StpResult = CallOdxServiceByShortName(ecuDtObj, service_short_name, inpara, StatStep)
	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]	

@handler_exception_decorator(length = 4)
def CallOdxServiceByShortName(ecuDtObj,ServiceShortName,InputPara,StatStep):
	# ReturnResult = 0����Ӧ��ReturnResult = 1����Ӧ��ReturnResult = 2��������
	# global _links
	global ODXFD
	'''
	ReturnResult=4 unknow err
	ReturnResult=3 pdu lenth err
	ReturnResult=2 input param err
	ReturnResult=1 negative response
	ReturnResult=0 positive response
	'''
	odxdata = None
	for data in ODXFD.values():
		odxdata = data
	links = dserverInit(odxdata,ServiceShortName)
	ReturnResult = 4 
	inputparam = None
	internals = dict()

	VciResult = ""
	PositiveResponseParam = ""
	NegtiveResponseParam = ""
	if ecuDtObj == None:
		MSGLogger.error("InputParam error:ecuDtObj is None")
		StatStep.Result = "NOK"
		StatStep.Value = "NOK@ecuDtObj is None"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	if ServiceShortName == None:
		MSGLogger.error("InputParam error:ServiceShortName is None")
		StatStep.Result = "NOK"
		StatStep.Value = "NOK@ServiceShortName is None"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	if StatStep == None:
		MSGLogger.error("InputParam error:StatStep is None")
		StatStep.Result = "NOK"
		StatStep.Value = "NOK@StatStep is None"
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	ectinfos = dict()
	ectinfos["ECU"] = ecuDtObj.__dict__
	MSGLogger.debug(str(ectinfos)+" "+str(ServiceShortName))
	# loadresult,logicalLinks = LoadOdx(ecuDtObj.Name)
	# if ecuDtObj.Name not in _links.keys():
		# StatStep.Value = "NOK@ Can Not find the Odx File For " +str(ecuDtObj.Name)
		# StatStep.Result = "NOK"
		# MSGLogger.error("Can Not find the Odx File For "+str(ecuDtObj.Name))
		# return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	logicalLinks = links[1][0]
	if logicalLinks == None:
		StatStep.Value = "NOK@ Can Not Get the Odx Which Is Loaded  For " +str(ecuDtObj.Name)
		StatStep.Result = "NOK"
		MSGLogger.error("Can Not Get the Odx Which Is Loaded  For " +str(ecuDtObj.Name))
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	# if loadresult != 0:
	# 	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	# if logicalLinks == None:
	# 	StatStep.Value = "NOK@ Can Not find the Odx File For " +str(ecuDtObj.Name)
	# 	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	services = logicalLinks[0].getDiagComPrimitives()
	service = services.getItemByName(ServiceShortName)
	if service is None:
		StatStep.Value = "NOK@ Can Not find the Service For "+str(ecuDtObj.Name)+str(ServiceShortName)
		StatStep.Result = "NOK"
		MSGLogger.error("Can Not find the Service For "+str(ecuDtObj.Name))
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	semantic = service.getDbObject().getSemantic()
	responsedtypelist = service.getDbObject().getResponsePrefix()

	request = service.getRequest()
	parameters = request.getRequestParameters()
	if InputPara == None:
		if semantic == "DEFAULT-FAULT-COUNT" or semantic == "DEFAULT-FAULT-READ":
			InputPara = '{"Test_failed":"True","Test_failed_this_operation_cycle_1_Bit":"True","Pending_DTC_1_Bit":"True","Confirmed_DTC":"True","Test_not_completed_since_last_clear":"True","Test_failed_since_last_clear_1_Bit":"True","Test_not_completed_this_operation_cycle_1_Bit":"True","Warning_indicator_requested_1_Bit":"True"}'
		else:
			InputPara = ""
	if InputPara == '':
		if semantic == "DEFAULT-FAULT-COUNT" or semantic == "DEFAULT-FAULT-READ":
			InputPara = '{"Test_failed":"True","Test_failed_this_operation_cycle_1_Bit":"True","Pending_DTC_1_Bit":"True","Confirmed_DTC":"True","Test_not_completed_since_last_clear":"True","Test_failed_since_last_clear_1_Bit":"True","Test_not_completed_this_operation_cycle_1_Bit":"True","Warning_indicator_requested_1_Bit":"True"}'
	if len(InputPara) != 0:
		inputparam = ast.literal_eval(InputPara)
		if semantic == "DEFAULT-FAULT-COUNT" or semantic == "DEFAULT-FAULT-READ":
			for k,v in inputparam.items():
				if v == "True":
					inputparam[k] = "true"
					continue
				if v == "False":
					inputparam[k] = "false"
					continue
		internals = dict()
		for paramter in parameters:
			if paramter.getDbObject().getDefaultValue():
				internals[paramter.getShortName()] = paramter.getDbObject().getDefaultValue()
				continue
			dop = paramter.getDbObject().dbDop if paramter.getDbObject().dbDop else paramter.getDbObject().dbDtcDop
			if dop:
				cm = CompuMethod(dop)
				internal= cm.getInternalResult(inputparam[paramter.getShortName()])
			else:
				if paramter.getDbObject().getPDataType() == "RESERVED":
					internal = 0
			internals[paramter.getShortName()] = internal
			# if paramter.getDataType() in ["A_ASCIISTRING"]:
			# 	internals[paramter.getShortName()] = internal 
			# else:
			# 	if type(internal)==str:
			# 		try:
			# 			internals[paramter.getShortName()] = int(internal,16)
			# 		except:
			# 			internals[paramter.getShortName()] = int(internal)
			# 	elif type(internal)==int:
			# 		internals[paramter.getShortName()] = internal
			# 	elif type(internal)==float:
			# 		internals[paramter.getShortName()] = float(internal)
			# isoutlimit = dop.DopMethodValueOutLimit(internal)
			# if isoutlimit != "No":
			# 	ReturnResult = 2
			# 	StatStep.Result = "NOK"
			# 	StatStep.Value = "NOK@Param is out limit"
			# 	MSGLogger.error("Param is out limit ")
			# 	return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	# if internals:
	ret = setArgs(internals, parameters)
	if not ret:
		ReturnResult = 2
		StatStep.Result = "NOK"
		MSGLogger.error("Param set fail")
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	try:
		syncresults = service.executeSync()

		RequestString = syncresults[0].hex()
		TargetResponseString = (service.getDbObject().posResponsePrefix).hex()
		IsExcuteSuccessful = False
		LeadingByteLength=TargetResponseString.__len__()
		returns = ExcuteServiceAndCheckLeadingResponse_P(ecuDtObj,RequestString,TargetResponseString,LeadingByteLength,StatStep,StatStep)
		VciResult = returns[2]
		IsExcuteSuccessful = returns[0]
		MSGLogger.debug("StatStep.Value= "+str(StatStep.Value)+" IsExcuteSuccessful= "+str(IsExcuteSuccessful)+ " VciResult= "+str(VciResult))
		if not IsExcuteSuccessful and len(VciResult)==0:
			StatStep.Result = "NOK"
			StatStep.Value = "NOK@"+str(ecuDtObj.Name)+" "+str(ServiceShortName)+" Service Failed,Excepted="+str(TargetResponseString)+" Actual "+str(VciResult)
			MSGLogger.error("No receive pdu")
			return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
		table = syncresults[2]
		ret, results ,responsedtype,respudlenErr= HanderResponseRequest.processResponse(table,VciResult)
		if responsedtypelist[0] == responsedtype:
			ReturnResult = 0
		elif responsedtypelist[1] == responsedtype:
			ReturnResult = 1
		if not respudlenErr:
			ReturnResult = 3
		serviceResult = dict(service=service.getShortName(), responses=[])
		for res in results:
			for r in res.responseCollection:		
				responseResult = dict()
				for p in r.getResponseParameters():
					if p.getDbObject().getPDataType() == "RESERVED":
						responseResult.update({p.getDbObject().getShortName():"RESERVED"})
					else:
						responseResult.update(dumpParameter(p))
			
			serviceResult["responses"].append(responseResult)

		if ReturnResult == 0 :
			PositiveResponseParam = serviceResult["responses"][0]
			StatStep.Result = "OK"
			StatStep.Value = str(PositiveResponseParam)
		if ReturnResult == 1:
			NegtiveResponseParam = serviceResult["responses"][0]
			StatStep.Result = "NOK"
			StatStep.Value = str(NegtiveResponseParam)
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
	except Exception as e:
		StatStep.Result = "NOK"
		StatStep.Except = True
		traceinfo= traceback.format_exc()
		MSGLogger.critical(traceinfo)
		return [ReturnResult,PositiveResponseParam,NegtiveResponseParam,VciResult]
@handler_exception_decorator(length = 1)
def GetResultByCallODX(ECUname:str,ShortName:str,Result:int,PositiveResponseParam:dict,NegtiveResponseParam:dict,VciResult:str):
	resultpack = collections.OrderedDict()
	if type(ECUname) == ecuDtObj:
		resultpack["ECU"] = ECUname.Name
	elif type(ECUname) == str:
		resultpack["ECU"] = ECUname
	else:
		resultpack["ECU"] = "unknow"
	if ShortName and type(ShortName) == str:
		resultpack[ShortName] = collections.OrderedDict()
		if Result == 0:
			resultpack[ShortName]["PositiveResponseParam"] = ""
			resultpack[ShortName]["NegtiveResponseParam"] = ""
			if type(PositiveResponseParam) == dict:
				resultpack[ShortName]["PositiveResponseParam"] = PositiveResponseParam 
		elif Result == 1:
			resultpack[ShortName]["PositiveResponseParam"] = ""
			resultpack[ShortName]["NegtiveResponseParam"] = ""
			if type(NegtiveResponseParam) == dict:
				resultpack[ShortName]["NegtiveResponseParam"] =  NegtiveResponseParam
		else:
			resultpack[ShortName]["PositiveResponseParam"] = ""
			resultpack[ShortName]["NegtiveResponseParam"] = ""

		resultpack[ShortName]["VciResult"] = VciResult
		resultpack[ShortName]["Result"] = Result
	return resultpack

class N121eInput():
	def __init__(self):
		self.GroupOfDtc = 2
class cinput():

	def __init__(self):
		self.CRC_Value = 0x01020304
class arg4():
	def __init__(self):
		self.arg5 = 6
	
	def __str__(self):
		return str(self.__dict__)

class inputData():
	def __init__(self):
		pass

# testing()
