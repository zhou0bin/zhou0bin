import xml.dom.minidom 
import xpath
import os
from libs.appAPI.log_info import MSGLogger,ToolsFunctionDecorator

@ToolsFunctionDecorator(rcount=1)
def XMLGetAttributeAsString(XMLelement,AttributeName:str):
    if XMLelement == None:
        MSGLogger.error("XMLGetAttributeAsString:input param XMLelement error")
        return ""
    if AttributeName == None or AttributeName == "":
        MSGLogger.error("XMLGetAttributeAsString:input param AttributeName error")
        return ""
    if XMLelement.hasAttribute(AttributeName):
        return XMLelement.getAttribute(AttributeName)
    else:
        MSGLogger.error("XMLGetAttributeAsString:has no AttributeName")
        return ""

@ToolsFunctionDecorator(rcount=1)
def XMLGetChildNodesCount(xmlElement):
    if xmlElement == None:
        MSGLogger.error("XMLGetChildNodesCount:input param xmlElement error")
        return 0
    childs = xmlElement.childNodes
    child_count = 0
    for node in childs:
        if node.nodeType == node.ELEMENT_NODE:
            child_count += 1
    return child_count

@ToolsFunctionDecorator(rcount=1)
def XMLGetChildByPath(xmlElement,XPath:str):
    if xmlElement == None:
        MSGLogger.error("XMLGetChildByPath:input param xmlElement error")
        return None
    if XPath == "" or XPath == None or type(XPath) != str:
        MSGLogger.error("XMLGetChildByPath:input param XPath error")
        return None
    result = xpath.find(XPath,xmlElement)
    return result

@ToolsFunctionDecorator(rcount=1)
def LoadXML(XmlFilePath:str):
    if type(XmlFilePath) != str or XmlFilePath == None or XmlFilePath == "":
        MSGLogger.error("LoadXML:input param XmlFilePath error")
        return None
    
    if os.path.exists(XmlFilePath):
        return xml.dom.minidom.parse(XmlFilePath)
    else:
        MSGLogger.error("LoadXML:the file XmlFilePath is not exist")
        return None

@ToolsFunctionDecorator(rcount=1)
def XMLGetRootElement(XmlDoc):
    if XmlDoc == None:
        MSGLogger.error("XMLGetRootElement:input param XmlDoc error")
        return None
    return XmlDoc.documentElement

@ToolsFunctionDecorator(rcount=1)
def XMLGetNodeByIndex(XmlNodes,Index:int):
    # print(len(XmlNodes.childNodes))
    if XmlNodes == None:
        MSGLogger.error("XMLGetNodeByIndex:input param XmlNodes error")
        return None
    if len(XmlNodes.childNodes) == 0:
        MSGLogger.error("XMLGetNodeByIndex:input param XmlNodes error")
        return 0
    if Index == None or type(Index) != int:
        MSGLogger.error("XMLGetNodeByIndex:input param Index error")
        return None
    if len(XmlNodes.childNodes) < Index or Index < 0:
        MSGLogger.error("XMLGetNodeByIndex:input param Index error")
        return None
    return XmlNodes.childNodes[Index]

