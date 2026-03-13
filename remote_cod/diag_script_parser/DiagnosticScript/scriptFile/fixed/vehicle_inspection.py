import os
import sys
sys.path[0]=(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))
from libs.appAPI.LocalVariable import *
from libs.appAPI.log_info import *
from libs.communication.pdu_logical_link import PDUOpenChannel,PDUCloseChannel 
from libs.util.dserver.odx_system import CallOdxServiceByShortName ,LoadOdx,UnloadOdx 
from libs.appAPI.DtcReadByStatusMask import *
from libs.appAPI.BlankStep import *
statblockobj = StatBlock(1,"Combined Service - VIN and Fault Memory","综合服务-VIN和故障存储器",True,True)
dtc_mask_value = "01"
mask_dtcs = ""
IsSuccess = False
PDUChancelHandle = -1
vciResult = ""
ExtendedDiagnosticSession = "ExtendedDiagnosticSession"
ReadDTCInformation_ReportDTCByStatusMask = "ReadDTCInformation_ReportDTCByStatusMask"
VINDataIdentifier_F190_read = "VINDataIdentifier_F190_read"
systemSupplierIdentifierDataIdentifier_F18A_read = "systemSupplierIdentifierDataIdentifier_F18A_read"
vehicleManufacturerSparePartNumberDataIdentifier_F187_read = "vehicleManufacturerSparePartNumberDataIdentifier_F187_read"
SystemName_F197_read = "SystemName_F197_read"
vehicleManufacturerECUSoftwareVersionNumberDataIdentifier_F189_read = "vehicleManufacturerECUSoftwareVersionNumberDataIdentifier_F189_read"
vehicleManufacturerECUHardwareVersionNumberDataIdentifier_F089_read = "vehicleManufacturerECUHardwareVersionNumberDataIdentifier_F089_read"
read_num = "{'TestFailed':'true', 'ConfirmedDtc':'true'}"
CallOdxResult = -1
OdxPositionResponse = None
Str_OdxNegtiveResponse = ""
DummyLAS = LAS("$T","object","BsVar_00000000000000000000000000000000","$T","LAS","")
Result = False
Handle = -1
stat_step_id = 0

INSPECTION_ECU_LIST = [
	{
		"ecu_name": "FSWM",
		"source_address": 1908,
		"target_address": 1916
    }
]

# 写入并检查 VIN
def VehicleInspcetion(EcuDtObj: ecuDtObj):
    res = GenerateStatStep(EcuDtObj.Name)
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,ExtendedDiagnosticSession,"",res[0])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,VINDataIdentifier_F190_read,"",res[1])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,systemSupplierIdentifierDataIdentifier_F18A_read,"",res[2])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,vehicleManufacturerSparePartNumberDataIdentifier_F187_read,"",res[3])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,SystemName_F197_read,"",res[4])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,vehicleManufacturerECUSoftwareVersionNumberDataIdentifier_F189_read,"",res[5])
    [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,vehicleManufacturerECUHardwareVersionNumberDataIdentifier_F089_read,"",res[6])
    [EmptyOutParam,EmptyOutParam,vciResult,StpResult]= DtcReadByStatusMask(EcuDtObj,"AUTOX_INSP",dtc_mask_value,mask_dtcs,res[7])
    # [CallOdxResult,OdxPositionResponse,Str_OdxNegtiveResponse,vciResult,StpResult]= CallOdxServiceByShortName(EcuDtObj,ReadDTCInformation_ReportDTCByStatusMask,read_num,res[8])

@handler_exception_decorator(2)
def WholeVehicleInspection(
    EcuDtObjList: list[ecuDtObj]):
    # 生成StatStep
    for ecu in EcuDtObjList:
        VehicleInspcetion(ecu)

def GenerateStatStep(ecu_name: str):
    global stat_step_id
    stat_step_id = stat_step_id + 1
    StatStepExtendedSession = StatStep(stat_step_id,ecu_name,"Enter Extended Session","进入扩展诊断模式",True,False,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepVIN = StatStep(stat_step_id,ecu_name,"Read VIN","读取VIN",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepSystemSupplierIdentifier = StatStep(stat_step_id,ecu_name,"Read System Supplier Identifier","读取系统供应商标识符",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    PartNumberDataIdentifier_Read = StatStep(stat_step_id, ecu_name,"Read Part Number Identifier","读取零件号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    SystemNameDataIdentifier = StatStep(stat_step_id,ecu_name,"Read ECU Name Identifier","读ECU名",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    SoftwareVersionNumberDataIdentifier = StatStep(stat_step_id,ecu_name,"Read Software Version Identifier","读软件版本号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    DID_0xF19F = StatStep(stat_step_id,ecu_name,"Read Hardware Version Identifier","读硬件版本号",True,True,"object","","","StatStep",None,"1")
    stat_step_id = stat_step_id + 1
    StatStepGetDTCs = StatStep(stat_step_id, ecu_name, "Get DTCs","获取故障码",True,False,"object","","","StatStep",None,"1")
    return StatStepExtendedSession, StatStepVIN, StatStepSystemSupplierIdentifier, PartNumberDataIdentifier_Read,SystemNameDataIdentifier,SoftwareVersionNumberDataIdentifier,DID_0xF19F,StatStepGetDTCs

def GetInspectionECUList():
	global INSPECTION_ECU_LIST
	ecu_list = []
	for config in INSPECTION_ECU_LIST:
		if "ecu_name" in config and "source_address" in config and "target_address" in config:	
			ecu_list.append(ecuDtObj(config["ecu_name"],int(config["source_address"]),int(config["target_address"]),0,"object","","","ecuDtObj",None))
	return ecu_list

def main():
	RecordStatblock_(statblockobj)
	[IsSuccess,PDUChancelHandle,StpResult]= PDUOpenChannel()
	[Result,Handle,StpResult] = LoadOdx("Autox_INSP")
	inspection_ecu_list = GetInspectionECUList()
	WholeVehicleInspection(inspection_ecu_list)
	[Result,StpResult]= UnloadOdx(Handle)
	[IsSuccess,StpResult]= PDUCloseChannel(PDUChancelHandle)
	EndCollectInfos_()


if ( __name__ == "__main__"):
	response = {'code': 0, 'message': '操作成功'}
	try:
		main()
	except Exception as e:
		response["code"] = 1
		response["message"] = f"操作失败: {str(e)}"
	print(json.dumps(response, ensure_ascii=False))
	sys.exit(response["code"])
