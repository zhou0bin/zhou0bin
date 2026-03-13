import json
import os
import traceback
from ..log_info import MSGLogger,PATHCONFIG,IS_SECURE_STARTUP
STATSTEP_STR_OK=' OK@ '
STATSTEP_STR_NOK=' NOK@ '
SpecialASCII = [161 ,162 ,163 ,165 ,167 ,168 ,169 ,170 ,171 ,176 ,177 ,
                    178 ,179 ,181 ,182 ,183 ,185 ,186 ,187 ,188 ,189 ,191 ,
                    192 ,193 ,194 ,195 ,196 ,197 ,198 ,199 ,200 ,201 ,202 ,
                    203 ,204 ,205 ,206 ,207 ,209 ,210 ,211 ,212 ,213 ,214 ,
                    215 ,216 ,217 ,218 ,219 ,220 ,221 ,223 ,224 ,225 ,226 ,
                    227 ,228 ,229 ,230 ,231 ,232 ,233 ,234 ,235 ,236 ,237 ,
                    238 ,239 ,241 ,242 ,243 ,244 ,245 ,246 ,247 ,248 ,249 ,
                    250 ,251 ,252 ,253 ]
def DealASCIIToChar(ast:str):
    if type(ast) != str or len(ast) == 0:
        return ""
    else:
        loop = 0
        alist = []
        astring = ""
        for i  in range(len(ast)//2):
            st = ast[loop:loop+2]
            loop += 2
            asnum = int(st,16)
            alist.append(asnum)
        for ele in alist:
            if ele >= 32 and ele <= 126:
                astring += chr(ele)
            elif ele in SpecialASCII:
                astring += chr(ele)
            else:
                astring += "\\x"  
                astring += "%02x"%ele
        return astring
def DealStrToASCII(Str:str):
    if len(Str) == 0 or type(Str) != str:
        return ""
    else:
        charlist = ""
        for i in range(len(Str)):
            charlist += ("%02x"%ord(Str[i]))
        return charlist
#字符串转ASCII
def ConvertStrASCII(Str:str)->str:
    return ConvertStrFormat(Str,'ascii')

#字符串格式转换
def ConvertStrFormat(Str:str,Format:str)->str:
    tmpByteData = Str.encode('utf-8','strict') # utf-8 转成 Unicode，decode(解码)需要注明当前编码格式
    result=tmpByteData.decode(Format,'strict')
    result= result.encode().hex()
    return result

def GetScriptRoot():
    stack = traceback.extract_stack()
    stackfilepath = ""
    for i in range(stack.__len__()):
        file = stack[i].filename
        if file.find("temporary")!=-1 or file.find("fixed")!=-1:
            stackfilepath = os.path.dirname(traceback.extract_stack()[i].filename)
            break
    return stackfilepath

def GetProjectName():
    ProjectName = ""
    if (not os.path.exists(PATHCONFIG.GetStringValue("VehDataPath"))) or (not IS_SECURE_STARTUP):
        MSGLogger.error("GetProjectName:no /data/DiagnosticScript/vehicleData/vehicle_data.json")
        MSGLogger.error("GetProjectName:use backup vehicle_data.json file")
        rootpath = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
        vehdatapath = os.path.join(rootpath,"vehicleData")
        files = os.listdir(vehdatapath)
        if(files.__len__()==0):
            return ProjectName
        for file in os.listdir(vehdatapath):
            if file.find("vehicle_data.json")!=-1:
                vehdatapath = os.path.join(vehdatapath,file)
                break
    else:
        vehdatapath = PATHCONFIG.GetStringValue("VehDataPath")
    vehfd = open(vehdatapath,"r")
    data = json.load(vehfd)
    vehfd.close()
    if "ProjectName" in data.keys():
        ProjectName = data["ProjectName"]
    return ProjectName

def utilResFiles(desDir:str, suffix:str)->str:
    desDirPath = ""
    if IS_SECURE_STARTUP:
        if suffix == "vehicle_data":
            return PATHCONFIG.GetStringValue("VehDataPath")
        else:
            tarpath = GetScriptRoot()
            for file in os.listdir(tarpath):
                if file.find(suffix) != -1:
                    desDirPath = os.path.join(tarpath,file)
            return desDirPath
    else:
        recPath = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../..'))
        desDirPath = os.path.join(recPath,desDir)
        files = os.listdir(desDirPath)
        if(files.__len__()==0):
            return ""
            # raise FileExistsError()
        for file in files:
            if suffix in os.path.splitext(file)[0]:#目录下包含.json的文件
                desDirPath = os.path.join(desDirPath,file)
                break
        return desDirPath

def utilVehicleDataFile()->str:
    return utilResFiles('vehicleData','vehicle_data')

def utilInitDataFile()->str:
    pjname = GetProjectName()
    target = pjname + "_MTOConfig_"
    return utilResFiles('vehicleConfig',target)
## no use no need
def utilOptionCodeMapFile()->str:
    pjname = GetProjectName()
    target = pjname + "_OptionCodeMap_"
    return utilResFiles('vehicleConfig',target)

def utilOptionFile()->str:
    pjname = GetProjectName()
    target = pjname + "_OptionConfig_"
    return utilResFiles('vehicleConfig',target)
## no use no need
def utilOdxDataFile()->str:
    return utilResFiles('Odx','_vcu_')

def utilEcuIDFile()->str:
    pjname = GetProjectName()
    target = pjname + "_ecuid_mtoc_"
    return utilResFiles('VehicleVersion',target)

def getSTATSTEP_STR_OK()->str:
    return STATSTEP_STR_OK

def getSTATSTEP_STR_NOK()->str:
    return STATSTEP_STR_NOK

#获取字节某一位的值

def get_BIT_VAL(byte,index):
    if byte & (1 << index):
        return 1
    else:
        return 0

#设置字节某一位的值
def set_BIT_VAL(byte, index,val):
    if val:
        return byte | (1 << index)
    else:
        return byte & ~(1 << index)

def bit_OP_AND(left:str,right:str)->int:
    return int(left,base=2) & int(right,base=16)