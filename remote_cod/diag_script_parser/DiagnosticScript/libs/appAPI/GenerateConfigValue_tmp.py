
# from lxml import etree
from xml.dom import minidom
from .log_info import handler_exception_decorator,MSGLogger
from .Evaluate import Evaluate
import re
import traceback

def Calculate_P(BaseConfigValue, bytepos, mask, startbit, bitlen, bitvalue):   
    bitlen = int(bitlen,10)
    #如果mask缺失，生成mask
    if mask == "" or mask == None:
        mask = bin(0b11111111)[2:]
        vst = ""
        #如果bitvalue是两位及以上的0
        if int(bitvalue,2) == 0 and bitlen >1:
            vst = "0"*bitlen
        else:
            vst = bin(int(bitvalue,2))[2:]
        m1 = mask[0:8-int(startbit,10)-bitlen]
        m2 = mask[8-int(startbit,10):]
        mask = hex(int(m1 + vst + m2,2))
    BaseConfigBytes = bytearray.fromhex(BaseConfigValue)
    # 截取基础配置码bytepos处的一个byte，记为Base Byte
    Base_Byte = BaseConfigBytes[int(bytepos)]
    # 在bitvalue的右侧添加startbit个0，左侧添加8-startbitbitlen个0，得到Value_Byte
    Value_Bit = int(bitvalue,2)
    #如果bitvalue是两位及以上的0
    if Value_Bit == 0 and bitlen > 1:
        Value_Bit = "0"*bitlen
    else:
        Value_Bit = bin(Value_Bit)[2:]
    leftstr = "0"*(8-int(startbit,10)-bitlen)
    rightstr = "0"*int(startbit,10)
    Value_Bit = leftstr + Value_Bit + rightstr
    # 将Base_Bytet和Value_Byte进行按位或运算，再和mask进行按位与运算，得到Result Byte
    Result_Byte = (Base_Byte | int(Value_Bit,2)) & int(mask,16)
    #用Result Byte替換掉基础配置码bytepos处的一个字节得到选配配置码
    BaseConfigBytes[int(bytepos)] = Result_Byte
    #字节数组转字符串，输出字符串
    BaseConfigBytes = BaseConfigBytes.hex()
    return BaseConfigBytes

def GenerateConfigValue_p(Project:str, Ecu:str, Did:str, BaseConfigValue:str, OptionConfigFilePath:str, LasList=None):
    OptionConfigValue = BaseConfigValue
    re_did = "did"+Did
    xmlstr=""
    try:
        try:
            fd=open(OptionConfigFilePath, "r",encoding="utf-8")
            xmlstr = fd.read()
            fd.close()
        except:
            MSGLogger.critical(traceback.format_exc())
            try:
                fd = open(OptionConfigFilePath, "r")
                xmlstr = fd.read()
                fd.close()
            except:
                MSGLogger.critical(traceback.format_exc())
                fd = open(OptionConfigFilePath, "r",encoding='gbk')
                xmlstr = fd.read()
                fd.close()

        if(xmlstr==""):
            return ""

        reg = "<ECU id=\""+ Ecu +"\".*?(<RECORD id=\"" + re_did + "\".*?</RECORD>).*?</ECU>"
        reg_xml = re.search(reg,xmlstr,re.S|re.M|re.I)
        if reg_xml==None:
            return BaseConfigValue
        elif len(reg_xml.groups())>=1:
            xmlstr=str(reg_xml.groups(0)[0])
        else:
            return BaseConfigValue
        #print(xmlstr)
        ValueDoms = minidom.parseString(xmlstr).documentElement.getElementsByTagName("VALUE")
        for ValueDom in ValueDoms:
            # 取第i个<VALUE>元素的bytepos, mask,startbit，bitvalue, bitlen，las属性
            Las = ValueDom.getAttribute("las")
            if Evaluate(Las,LasList):
                bytepos = ValueDom.getAttribute("bytepos")
                mask = ValueDom.getAttribute("mask")
                startbit = ValueDom.getAttribute("startbit")
                bitlen = ValueDom.getAttribute("bitlen")
                bitvalue = ValueDom.getAttribute("bitvalue")
                OptionConfigValue = Calculate_P(OptionConfigValue, bytepos, mask, startbit, bitlen,bitvalue)
        return OptionConfigValue
    except:
        MSGLogger.critical(traceback.format_exc())
        return ""
# #计算选配配置码
# def GenerateConfigValue_p(Project:str, Ecu:str, Did:str, BaseConfigValue:str, OptionConfigFilePath:str, LasList=None):
#     #选配规则文件路径
#     xmlFilePath = OptionConfigFilePath
#     # 选配规则文件XSD文路径
#     OptionConfigValue = BaseConfigValue
#     Ecu=Ecu.lower()
#     Did = 'did' + Did.lower()
#     #在xml中按照<ECU>的id属性搜索ECU
#     DOMTree = minidom.parse(xmlFilePath)
#     collection = DOMTree.documentElement
#     EcuDoms = collection.getElementsByTagName("ECU")
#     for EcuDom in EcuDoms:
#         if EcuDom.getAttribute("id").lower()  == Ecu:
#             RecordDoms = EcuDom.getElementsByTagName("RECORD")
#             for RecordDom in RecordDoms:
#                 #在xml中按照<RECORD>的id属性搜索DID
#                 if RecordDom.getAttribute("id").lower() == Did:
#                     # 提取<RECORD>的所有<VALUE>子节点，总数为N，令i=0
#                     ValueDoms = RecordDom.getElementsByTagName("VALUE")
#                     for ValueDom in ValueDoms:
#                         # 取第i个<VALUE>元素的bytepos, mask,startbit，bitvalue, bitlen，las属性
#                         Value = ValueDom.getAttribute("las")
#                         if Evaluate(Value,LasList):
#                             bytepos = ValueDom.getAttribute("bytepos")
#                             mask = ValueDom.getAttribute("mask")
#                             startbit = ValueDom.getAttribute("startbit")
#                             bitlen = ValueDom.getAttribute("bitlen")
#                             bitvalue = ValueDom.getAttribute("bitvalue")
#                             OptionConfigValue = Calculate_P(OptionConfigValue, bytepos, mask, startbit, bitlen, bitvalue)
#                     break
#             break
#     return OptionConfigValue


@handler_exception_decorator(length=6)
def GenerateConfigValue(Project:str, Ecu:str, Did:str, BaseConfigValue:str, OptionConfigFilePath:str, LasList=None):
    return GenerateConfigValue_p(Project, Ecu, Did, BaseConfigValue, OptionConfigFilePath, LasList)
