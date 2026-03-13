from xml.dom import minidom
from .log_info import handler_exception_decorator,MSGLogger,IS_SECURE_STARTUP
from .utils.db_data import DbVehicleData
from .utils.util_helper import utilVehicleDataFile,GetScriptRoot
import os
# 计算选配映射码
def GenerateOptionCodeValue_P(OptionCodeMapFilePath:str):
    #设置Result为空字符串
    if type(OptionCodeMapFilePath) != str or OptionCodeMapFilePath == None:
        MSGLogger.error("GenerateOptionCodeValue:input param is error")
    if IS_SECURE_STARTUP:
        xml_path = GetScriptRoot()
    else:
        xml_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))),"vehicleConfig")
    flag = False
    if OptionCodeMapFilePath != None and OptionCodeMapFilePath!="":
        for file in os.listdir(xml_path):
            if file.find(OptionCodeMapFilePath) != -1:
                xml_path += "/"
                xml_path += file
                flag = True
                break
    Result = ''
    if flag:
        LasDom=minidom.parse(xml_path)
        Lascodepair=LasDom.getElementsByTagName("LASCODEPAIR")
        #从车辆数据中获取JSON数据
        DbVehicle = DbVehicleData(utilVehicleDataFile())
        LasList = DbVehicle.getLAS()
        #依据LasList读取LAS列表，长度为N 依次从LasList取出每个LAS，判断是否等于XML某一个LASCODEPair节点的las属性值
        Lascodepair_ele = ""
        for i in range(len(Lascodepair)):
            if "$" in Lascodepair[i].getAttribute("las"):
                Lascodepair_ele = Lascodepair[i].getAttribute("las").replace("$","")
            else:
                Lascodepair_ele = Lascodepair[i].getAttribute("las")
            for ch in LasList:
                #如果值相等 Result=Result+该LASCODEPair的code属性值
                if Lascodepair_ele == ch:
                    Result += Lascodepair[i].getAttribute("code")
                    break
        #在Result的末尾补0，凑足50个字节,一个字节在字符串中占两个字符
        if Result.__len__()<600:
            #Result=Result+'-'
            Result=Result.ljust(600,'0')
    else:
        raise FileExistsError
    return Result
@handler_exception_decorator(length=1)
def GenerateOptionCodeValue(OptionCodeMapFilePath:str):
    return GenerateOptionCodeValue_P(OptionCodeMapFilePath)
