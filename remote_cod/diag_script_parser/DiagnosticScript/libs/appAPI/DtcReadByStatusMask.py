
from typing import List

from .LocalVariable import StatStep,ecuDtObj
from .utils.util_helper import GetProjectName,GetScriptRoot
from .log_info import handler_exception_decorator,MSGLogger,IS_SECURE_STARTUP
from .ExcuteServiceAndCheckLeadingResponse import ExcuteServiceAndCheckLeadingResponse_P
import os
import json, inspect

#读取 DTC 函数定义
@handler_exception_decorator(length=3)
def DtcReadByStatusMask(EcuDtObj,OdxFileName:str,MaskValue:str,MaskedDTCs:str,StatStepRead:StatStep):
    IsReadSuccessful = False
    dtcs = []
    VciResult = ""
    if EcuDtObj == None or type(EcuDtObj) != ecuDtObj:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if type(MaskValue) != str:
        if type(StatStepRead) == StatStep:
            StatStepRead.Result = "NOK"
    if OdxFileName == None or OdxFileName=="":
        MSGLogger.error("DtcReadByStatusMask:OdxFileName is error")
        StatStepRead.Result = "NOK"
    if StatStepRead == None or type(StatStepRead) != StatStep:
        MSGLogger.error("DtcReadByStatusMask:StatStepRead is error")
    if MaskValue == None or MaskValue=="":
        MaskValue = "AF"
    if IS_SECURE_STARTUP:
        odx_path = GetScriptRoot()
    else:
        stack_frame = inspect.stack()[-1]
        configPath = os.path.dirname(os.path.abspath(stack_frame.filename))
        flag = False
        for file in os.listdir(configPath):
            if file.find(OdxFileName) != -1 and file.endswith(".json"):
                odx_path = os.path.join(configPath,file)
                flag = True
                break
        if not flag:
            odx_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))),"Odx")
            if OdxFileName != None and OdxFileName!="":
                for file in os.listdir(odx_path):
                    if file.find(OdxFileName) != -1:
                        odx_path += "/"
                        odx_path += file
                        flag = True
                        break
    # if not flag:
    #     if hasattr(EcuDtObj,"Name"):
    #         for file in os.listdir(odx_path):
    #             if file.find(EcuDtObj.Name) != -1:
    #                 odx_path += "/"
    #                 odx_path += file
    #                 flag = True
    #                 break
    ODX_DTC_list = {}
    if flag:
        with open(odx_path,'r',encoding='utf8')as fp:
            json_Data = json.load(fp)
            lists = json_Data["DTC-DOPS"][0]["DTC-DOP"]["DTCS"]
            for lis in lists:
                tcode = hex(int(lis["TROUBLE-CODE"],10))[2:].upper()
                # dcode = lis["DISPLAY-TROUBLE-CODE"]
                text = lis["TEXT"]
                ODX_DTC_list.update({tcode:text})
    else:
          MSGLogger.error("DtcReadByStatusMask:odx is no find")    

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
            MSGLogger.error("DtcReadByStatusMask:no DTC")
            if ((len(VciResult[6:]) // 2) % 4)!=0:
                MSGLogger.error("DtcReadByStatusMask:the lenth of pdu which received is error")
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
    if MaskedDTCs == "" and len(rec_DTClists) != 0:
        if IsReadSuccessful:
            if isStatStepRead:
                measureValue=''
                r_rec_DTClists = []
                for rdtc in rec_DTClists:
                    r_dtc = rdtc[0:6]
                    displayValue = rdtc[6:]
                    r_rec_DTClists.append(r_dtc)
                    measureValue += r_dtc + "-" + displayValue + "|"
                dtcs = r_rec_DTClists
                measureValue = measureValue[0:measureValue.rfind("|")]
                #StatStepRead.Result=OK
                StatStepRead.Result='OK'
                #StatStepRead.Value=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.Value = ' REQ=' + RequestString + ' ,RESP=' + VciResult
                #StatStepRead.Format=“HEX”
                StatStepRead.Format='HEX'
                #StatStepRead.MeasureValue=”REQ=”+RequestString+”,RESP=”+VciResultRead
                StatStepRead.MeasureValue = measureValue
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
                
    elif len(rec_DTClists) == 0:
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
            if OdxFileName == None or OdxFileName=="":
                measureValue += r_dtc + "-" + displayValue + " "
            else:
                measureValue += r_dtc + "-" + displayValue + "."
                r_dtc = r_dtc.strip('0')
                if r_dtc in ODX_DTC_list.keys():
                    r_dtc = ODX_DTC_list[r_dtc]
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
