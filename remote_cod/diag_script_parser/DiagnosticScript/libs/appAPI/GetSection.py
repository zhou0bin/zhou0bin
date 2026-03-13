from .log_info import handler_exception_decorator,MSGLogger

import re    
#获取车辆数据某一字段
def GetSectionValue_P(IniFilePath:str,SectionName:str,Key:str)->str:
    #init SectionValue
    SectionValue=''
    with open(IniFilePath,'r') as f:
        ini_read = f.read()

        # #reg for section name
        regSectionNameStr='['+(SectionName).strip()+']'

        # #reg for key name
        regSectionKeyStr=(Key).strip()


        SectionNameIndex=ini_read.find(regSectionNameStr)
        if SectionNameIndex != -1:
            key_value = ini_read[SectionNameIndex+len(regSectionNameStr):]
            keyvalueindex = key_value.find("[")
            if keyvalueindex != -1:
                key_value = key_value[:keyvalueindex]
            strs= key_value.split('\n')
            for item in strs:
                SectionValueindex = item.find(regSectionKeyStr)
                if SectionValueindex != -1:
                    KeySectionValue = item.split('=')
                    SectionValue = KeySectionValue[1]
                    break
    return SectionValue.strip()


@handler_exception_decorator(length=1)
def GetSection(IniFilePath:str,SectionName:str,Key:str)->str:
    return GetSectionValue_P(IniFilePath,SectionName,Key)

# def GetSectionValue_P(IniFilePath:str,SectionName:str,Key:str)->str:
#     with open(IniFilePath,'r') as f:
#         #init SectionValue
#         SectionValue=''

#         #reg for section name
#         regSectionNameStr='\['+(SectionName).strip()+'\]'
#         regSectionName=re.compile(regSectionNameStr)

#         #reg for key name
#         regSectionKeyStr=(Key).strip()
#         regSectionKey=re.compile(regSectionKeyStr)
#         isFindSerctionName=False
        
#         #search file lines
#         while True:
#             line=f.readline()
#             if line:
#                 line=line.strip()
#                 if line.startswith('['):
#                     if isFindSerctionName==False and regSectionName.fullmatch(line):
#                             #find section name
#                             # print('regSectionName.fullmatch(line):'+str(regSectionName.fullmatch(line)))
#                             isFindSerctionName=True
#                             continue
#                     elif isFindSerctionName==True:
#                         break
#                 elif isFindSerctionName==True and regSectionKey.match(line):
#                         #find key name get value
#                         strs= line.split('=')
#                         SectionValue=strs[1]
#                         break
#                 else:
#                     continue
#             else:
#                 break
#     return SectionValue.strip()



