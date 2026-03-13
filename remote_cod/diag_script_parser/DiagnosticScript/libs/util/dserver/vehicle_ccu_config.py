
import inspect
import json
import os
from libs.util.dserver.vehicle_ecu_config_define import ECU_CONFIG_LIST
from libs.util.DiagLog import IS_SECURE_STARTUP
from libs.appAPI.utils.util_helper import GetScriptRoot


	
class VehicleCcuConfig(object):
    def __init__(self, vehicle_name = "Model-M", ecu_config_list = None):
        self.vehicle_name = vehicle_name
        self.ecu_config_list = ECU_CONFIG_LIST
        if ecu_config_list is not None:
            if self.is_list_of_dicts(ecu_config_list):
                self.ecu_config_list = ecu_config_list
            elif self.is_list_of_strings(ecu_config_list):
                self.ecu_config_list = []
                #遍历ecu_config_list中的字符串
                for ecuname in ecu_config_list:
                    tmp_dict = {"EcuName":ecuname}
                    self.ecu_config_list.append(tmp_dict)

    def is_list_of_strings(self, lst):
        return isinstance(lst, list) and all(isinstance(item, str) for item in lst)

    def is_list_of_dicts(self, lst):
        return isinstance(lst, list) and all(isinstance(item, dict) for item in lst)

    def GetFrameType(self, ecu_name):
        for ecu_config in self.ecu_config_list:
            if ecu_config["EcuName"] == ecu_name:
                frame_type = ecu_config.get("FrameType", 1)
                return frame_type
        return 1

    def GetEcuCommParams(self, ecu_name):   #获取信息用于生成EcuObject 
        # [RequestID,ResponseID,FrameType]= GetEcuCommParams(EcuName)
        ecuinfosRequestID = ""
        ecuinfosResponseID = ""
        frame_type = None
        #在列表中找到该ECU的字典
        find_ecu_node = next((s for s in self.ecu_config_list if s.get("EcuName") == ecu_name), None)
        if find_ecu_node == None:
            return [ecuinfosRequestID,ecuinfosResponseID,frame_type]
        
        if "RequestID" in find_ecu_node and "ResponseID" in find_ecu_node:#如果字典中已经定义了RequestID和ResponseID，则直接使用
            if "RequestID" in find_ecu_node:
                ecuinfosRequestID = find_ecu_node["RequestID"]
            if "ResponseID" in find_ecu_node:
                ecuinfosResponseID = find_ecu_node["ResponseID"]
        else:#ecu字典中没找到RequestID和ResponseID时，再从ODX中查找
            # 获取odx文件名
            OdxFileName = self.GetEcuOdxFile(ecu_name)
            result, odxdata = self.CusLoadOdx(OdxFileName)
            if result != 0:
                print("CusLoadOdx err:",result)
                return [ecuinfosRequestID,ecuinfosResponseID,frame_type]       
            
            #判断odx_system.ODXFD是否为空字典
            if not odxdata:
                print("odxdata is empty") 
                return [ecuinfosRequestID,ecuinfosResponseID,frame_type]
            #将odxdata转换为字典
            tmp_odx_dict = json.loads(odxdata)
            if "Ecu_Name" in tmp_odx_dict.keys():
                ecuinfosName = tmp_odx_dict["Ecu_Name"]
            if "Request_Address" in tmp_odx_dict.keys():
                ecuinfosRequestID = tmp_odx_dict["Request_Address"]
            if "Response_Address" in tmp_odx_dict.keys():
                ecuinfosResponseID = tmp_odx_dict["Response_Address"]
        
        frame_type = self.GetFrameType(ecu_name)
			
        return [ecuinfosRequestID,ecuinfosResponseID,frame_type]   
        
    def GetEcuOdxFile(self, ecu_name):   #获取odx文件名
        # OdxFile = GetEcuOdxFile(EcuName)
        for ecu_config in self.ecu_config_list:
            if ecu_config["EcuName"] == ecu_name:
                odx_file_name = ecu_config.get("OdxFileName", f"{self.vehicle_name}_{ecu_name}")
                return odx_file_name
            
    def GetDidList(self, ecu_name):   #获取ECU的did列表，类初始化时在每个ECU对应的Odx文件中搜索22服务的所有DID
        # DidList = GetDidList(EcuName)
        did_list = []
        OdxFileName = self.GetEcuOdxFile(ecu_name)
        result, odxdata = self.CusLoadOdx(OdxFileName)
        if result != 0:
            print("CusLoadOdx err:",result)
            return None        
        #判断odx_system.ODXFD是否为空字典
        if not odxdata:
            print("odxdata is empty") 
            return None

        tmp_odx_dict = json.loads(odxdata)
        diag_service_list = tmp_odx_dict["Diag_Service"]
        for diag_service in diag_service_list:
            if diag_service["SEND"].startswith("22"):
                did_list.append(diag_service["SEND"][2:])
        return did_list
    
    def GetEcuNameList(self):
         return [ecu_config["EcuName"] for ecu_config in self.ecu_config_list]
    
    def CusLoadOdx(self, OdxFileName):
        # path = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
        '''
        result=4 unknow err
        result=3 Not enough storage err
        result=2 file format err
        result=1 no file err
        result=0 ok
        '''
        ODXFD_json = {}
        result = 4 
        configPath = ""
        if IS_SECURE_STARTUP:
            configPath = GetScriptRoot()
        else:
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
            data = fd.read()
            # ODXFD_json.update({id(data):data})
            fd.close()
            if data:
                result=0
                return result,data
        except:
            result=4
            return result,None
            