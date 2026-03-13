import json
from typing import Dict, List

class DbDataReader(object):
    def __init__(self,dataFilePath:str) -> None:
        self.DataFilePath=dataFilePath
        self.Data={}

    def setDataFilePath(self,dataFilePath:str):
        self.DataFilePath=dataFilePath
        self.loadData()
    def getData(self):
        return self.Data

    def loadData(self):
        try:
            with open(self.DataFilePath,'r',encoding='utf8')as fp:
                self.Data = json.load(fp)
        except:
            self.Data = {}

    def getKeyValue(self,key:str):
        if self.Data.get(key):
            return self.Data[key]
        return ''

class DbVehicleData(DbDataReader):
    def __init__(self,dataFilePath:str) -> None:
        # print(dataFilePath)
        super().__init__(dataFilePath)
        self.loadData()
        self.VINKey:str='VIN'
        self.MTOCKey:str='MTOC'
        self.PROJECTNAMEKey:str='ProjectName'
        self.LASKey='LAS'
    def getVIN(self)->str:
        return self.getKeyValue(self.VINKey)
    
    def getMTOC(self)->str:
        return self.getKeyValue(self.MTOCKey)
    
    def getPROJECTNAME(self)->str:
        return self.getKeyValue(self.PROJECTNAMEKey)
    
    def getLAS(self)->List[str]:
        return self.getKeyValue(self.LASKey)

class DtcDefinition:
    def __init__(self,TROUBLECODE:str,DISPLAYTROUBLECODE:str,TEXT:str,TI:str) -> None:
        self.TROUBLECODE=TROUBLECODE
        self.DISPLAYTROUBLECODE=DISPLAYTROUBLECODE
        self.TEXT=TEXT
        self.TI=TI

class Dtc:
    def __init__(self,TROUBLECODE:str,DTCSTATECODE:str,dtcDefine:DtcDefinition) -> None:
        self.TROUBLECODE=TROUBLECODE
        self.DTCSTATECODE=DTCSTATECODE
        self.DisplayValue=None
        self.EnglishTranslation=None
        self.ChineseTranslation=None
        self.FIXMESSAGE=None
        if dtcDefine:
            self.DisplayValue=dtcDefine.DISPLAYTROUBLECODE
            self.EnglishTranslation=dtcDefine.TEXT
            self.ChineseTranslation=dtcDefine.TI
            self.FIXMESSAGE=dtcDefine.TI
        
class DbODXData(DbDataReader):
    def __init__(self,dataFilePath:str) -> None:
        super.__init__(dataFilePath)
        self.loadData()
        tmpDTCDefines=self.getData['DTC-DOPS']['DTC-DOP']['DTCS']
        self.dtcDefinesList=List[DtcDefinition]
        self.dtcDefinesDic={}
        for item in tmpDTCDefines:
            #数据源为10进制需要转为16进制
            dtcDefine=DtcDefinition(hex(item["TROUBLE-CODE"]),item["DISPLAY-TROUBLE-CODE"],item["TEXT"],"")
            troublecode=hex(item["TROUBLE-CODE"])
            self.dtcDefinesList.append(dtcDefine)
            self.dtcDefinesDic[troublecode]=dtcDefine

    def getDTCDefineList(self)->List[DtcDefinition]:         
        return self.dtcDefinesList

    def getDTCDefineListDic(self)->Dict[str,DtcDefinition]:
        self.dtcDefinesDic
        return

    def getDtcDefineByCode(self,troublecode:str)->DtcDefinition:
        result=self.dtcDefinesDic[troublecode]
        return result

    def getDtcCount(self)->int:
        return len(self.dtcDefinesList)
