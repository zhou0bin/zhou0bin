
from typing import List

from .LocalVariable import StatStep,ecuDtObj
from .utils.util_helper import GetProjectName,GetScriptRoot
from .log_info import handler_exception_decorator,MSGLogger,IS_SECURE_STARTUP
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
import os
from xml.dom import minidom 
import traceback
#读取 DTC 函数定义
@handler_exception_decorator(length=3)
def DtcReadByStatusMask2(EcuDtObj,XmlFileName:str,MaskValue:str,MaskedDTCs:str,StatStepRead:StatStep):
    IsReadSuccessful = False
    dtcs = []
    VciResult = ""
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if type(MaskValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if XmlFileName == None or XmlFileName=="":
        StatStepRead.Result = "NOK"
        MSGLogger.error("DtcReadByStatusMask2:XmlFileName is error")
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("DtcReadByStatusMask2:StatStepRead is error")
    if MaskValue == None or MaskValue=="":
        MaskValue = "AF"
    # xml_path = os.path.dirname(os.path.dirname(os.path.dirname(__file__))) + "/Xml"
    # xml_path = os.path.dirname(traceback.extract_stack()[0].filename)
    objlen = traceback.extract_stack().__len__()
    for i in range(objlen):
        file = traceback.extract_stack()[i].filename
        if file.find("temporary")!=-1 or file.find("fixed")!=-1:
            xml_path = os.path.dirname(traceback.extract_stack()[i].filename)
            # print(xml_path)
            break
    flag = False
    if XmlFileName != None and XmlFileName!="":
        for file in os.listdir(xml_path):
            if file.find(XmlFileName) != -1:
                xml_path += "/"
                xml_path += file
                flag = True
                break
    # for file in os.listdir(xml_path):
    #     if file.endswith('.xml'):
    #         xml_path += "/"
    #         xml_path += file
    #         flag = True
    #         break
    XML_DTC_list = {}
    if flag:
        try:
            xml_data = minidom.parse(xml_path)
            #xml结构变更，原xml根目录下无DTC标签
            root = xml_data.documentElement
            doc_dtcs = root.getElementsByTagName('DTCS')
            doc_dtc = doc_dtcs[0].getElementsByTagName('DTC')
            for dtc_1 in doc_dtc:
                code = dtc_1.getAttribute('code')
                code1 = hex(int(code,10))[2:].upper()
                text = dtc_1.getAttribute('text')
                XML_DTC_list.update({code1:text})
        except:
            XML_DTC_list = {}
    else:
          MSGLogger.error("DtcReadByStatusMask2:xml is no find")    

    MaskStatus = MaskValue
    params = MaskedDTCs.split("|")
    inputStatus = list(map(lambda p:p.upper(),params))

    #设置局部变量RequestString=“1902”+MaskStatus
    RequestString='1902'+MaskStatus.upper()
    #设置局部变量TargetResponseString=“5902”
    TargetResponseString='5902'
    MSGLogger.debug({'ECU':EcuDtObj,'RequestString':RequestString,'TargetResponseString':TargetResponseString})
    #调用ExcuteServiceAndCheckResponse函数ExcuteServiceAndCheckResponse(ECU,RequestString,TargetResponseString,StatStepRead，StatStepRead，IsReadSuccessful，IsReadSuccessful，VciResult)
    LeadingByteLength=TargetResponseString.__len__()
    [IsReadSuccessful,IsReadSuccessful,VciResult]=ExcuteServiceAndCheckLeadingResponse_P(EcuDtObj,RequestString,TargetResponseString,LeadingByteLength,StatStepRead,StatStepRead)
    #打印StatStepRead.Value，IsReadSuccessful，VciResult
    isStatStepRead:bool = False
    if isinstance(StatStepRead,StatStep):
        isStatStepRead=True
        MSGLogger.debug({'StatStepRead.Value':StatStepRead.Value,'IsReadSuccessful':IsReadSuccessful,'VciResult':VciResult})
    else:
        MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'VciResult':VciResult})
    rec_DTCinfos = ""
    rec_DTClists = []
    ECU_avaialblestatus = ""
    if IsReadSuccessful:
        l = RequestString.__len__()
        if VciResult.__len__()>l:   
            MSGLogger.error("DtcReadByStatusMask2:no DTC")
            if ((len(VciResult[6:]) // 2) % 4)!=0:
                MSGLogger.error("DtcReadByStatusMask2:the lenth of pdu which received is error")
                StatStepRead.Result = "NOK"
                return [IsReadSuccessful,dtcs,VciResult]
        
        ECU_avaialblestatus = VciResult[4:6]
        rec_DTCinfos = VciResult[6:]
    offset = 0
    #提取VciResult中的每个DTC，形成DTC清单
    for i in range(len(rec_DTCinfos)//8):
        DTC4byte = rec_DTCinfos[offset:offset + 8]
        if DTC4byte[:6].upper() in inputStatus:
            pass
        else:
            rec_DTClists.append(DTC4byte.upper())
        offset += 8
    # for ele in inputStatus:
    #     if ele in rec_DTClists:
    #         rec_DTClists.remove(ele)
    ISO_CODE = {"0":"P0","1":"P1","2":"P2","3":"P3","4":"C0","5":"C1","6":"C2","7":"C3",
                "8":"B0","9":"B1","A":"B2","B":"B3","C":"U0","D":"U1","E":"U2","F":"U3"}
    #DTC清单为空？
    if len(rec_DTClists) == 0:
        if IsReadSuccessful:
            if isStatStepRead:
                #StatStepRead.Result=OK
                StatStepRead.Result='OK'
                #StatStepRead.Value=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                #StatStepRead.Format=“HEX”
                StatStepRead.Format='HEX'
                #StatStepRead.MeasureValue=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.MeasureValue ='REQ ='+RequestString.upper() +', RESP ='+VciResult.upper()
                #StatStepRead.SetValue=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.SetValue = "5902"
        else:
            if isStatStepRead:
                # StatStepRead.Result=NOK
                StatStepRead.Result = 'NOK'
                # StatStepRead.Value=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                # StatStepRead.Format=“HEX”
                StatStepRead.Format = 'HEX'
                # StatStepRead.MeasureValue=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.MeasureValue = 'REQ =' + RequestString.upper() + ', RESP =' + VciResult.upper()
                # StatStepRead.SetValue=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.SetValue = "5902"
    else:
        measureValue=''
        r_rec_DTClists = []
        for rdtc in rec_DTClists:
            r_dtc = rdtc[0:6]
            displayValue = rdtc[6:]
            r_rec_DTClists.append(r_dtc)
            if int(displayValue,16)%2:
                displayValue += " Present"
            else:
                displayValue += " History"
            if XmlFileName == None or XmlFileName=="":
                measureValue += r_dtc + "-" + displayValue + " "
            else:
                measureValue += r_dtc + "-" + displayValue + "."
                if r_dtc in XML_DTC_list.keys():
                    r_dtc = XML_DTC_list[r_dtc]
                else:
                    if r_dtc[0] in ISO_CODE.keys():
                        r_dtc = ISO_CODE[r_dtc[0]] + r_dtc[1:]
                measureValue += r_dtc + "\n"
        dtcs = r_rec_DTClists
        measureValue = measureValue[0:measureValue.rfind("\n")]

        if isStatStepRead:
            #StatStepRead.Result = NOK
            StatStepRead.Result = 'NOK'
            #StatStepRead.Value =”REQ =”+RequestString +”, RESP =”+VciResultRead
            StatStepRead.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
            #StatStepRead.Format =“HEX”
            StatStepRead.Format = 'HEX'
            #StatStepRead.MeasureValue = DTC1.DisplayValue + DTC1.ChineseTranslation + DTC1.EnglishTranslation +“\n”+DTC2.DisplayValue + DTC2.ChineseTranslation + DTC2.EnglishTranslation +..
            StatStepRead.MeasureValue=measureValue
            #StatStepRead.SetValue = StatStepRead.MeasureValue
            StatStepRead.SetValue = "5902"
            MSGLogger.debug({'IsReadSuccessful':IsReadSuccessful,'StatStepRead.Result':StatStepRead.Result,'StatStepRead.Value':StatStepRead.Value,'StatStepRead.Format':StatStepRead.Format,
                    'StatStepRead.MeasureValue':StatStepRead.MeasureValue,'StatStepRead.SetValue':StatStepRead.SetValue})
        #StatStepRead.Value=NOK@ECU.Name+“ DTCs”+DTC1.DisplayValue+DTC1.ChineseTranslation+DTC1.EnglishTranslation+“\n”+DTC1.DisplayValue+DTC1.ChineseTranslation+DTC1.EnglishTranslation+..
        #dtcsStr='DTCs'
        #for dtc in dtcs:
        #    dtcsStr+= dtc.DisplayValue+dtc.ChineseTranslation+dtc.EnglishTranslation+'\n'
        #StatStepRead.Value=getSTATSTEP_STR_NOK()+EcuDtObj.Name+dtcsStr
    #返回DTC清单，IsReadSuccessful [bool IsReadSuccessful, List <DTC DtcList vciResult VciResult Read ] 返回值是按照文档还是流程图? erro
    return [IsReadSuccessful,dtcs,VciResult]
