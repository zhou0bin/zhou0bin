import os
from ctypes import *
from libs.appAPI.log_info import MSGLogger,handler_exception_decorator,IS_SECURE_STARTUP
from libs.appAPI.utils.util_helper import GetScriptRoot
from enum import Enum
import traceback
class ERROR(Enum):
    NoError  =     0x00
    VendorNotFound = 0x01
    VehicleNotFound = 0x02
    ECUNotFound = 0x03
    InvalidSeed = 0x04
    InvalidPIN = 0x05
    InvalidSecurityLevel = 0x06
    InvalidOutParameter = 0x07
    InvalidExtraParameter = 0x08
    KeyCalculateError = 0x09
    InvalidInputParameter = 0x10
    StringNotMatch = 0x11
    ExceptionOccurred = 0x80
    UnkownError = 0x81

def GetLibAsfSA(rootpath):
    path = ""
    filePaths = []
    for root,dirs,files in os.walk(rootpath):
        for file in files:
            filePaths.append(os.path.join(root,file))
    for path in filePaths:
        if path.endswith("libasf_SA.so"):
            return path
    return path

def GenerateKeyFromSeed_P(EcuSection:str,Seed:str,Level:int,iVariant:str):
    if type(EcuSection) != str:
        MSGLogger.error("GenerateKeyFromSeed:EcuSection is error")
        return ERROR.InvalidInputParameter.value,""
    if type(Seed) != str:
        MSGLogger.error("GenerateKeyFromSeed:Seed is error")
        return ERROR.InvalidInputParameter.value,""
    if type(Level) != int:
        MSGLogger.error("GenerateKeyFromSeed: Level Is error")
        return ERROR.InvalidInputParameter.value,""
    if type(iVariant) != str:
        MSGLogger.error("GenerateKeyFromSeed: iVariant Is error")
        iVariant = ""
    path = os.getenv("NEUSAR_DEPLOYMENT_PATH", "")
    rootpath = f"{path}/neusar/usr/lib"
    abspath = GetLibAsfSA(rootpath)
    if abspath == "":
        MSGLogger.error("GenerateKeyFromSeed: No asf_SA library")
        return ERROR.UnkownError.value,""
    libso = cdll.LoadLibrary(abspath)
    seed_key_size_16 = True
    try:
        if seed_key_size_16:
            lib_ecu = c_char_p(EcuSection.encode())
            lib_seed = (c_ubyte*16)(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
            i = 0
            loop = 0
            while (i < 16):
                lib_seed[i] = int(Seed[loop:loop+2],16)
                loop += 2
                i += 1
            i = 0
            loop = 0
            lib_iVariant = (c_ubyte*16)(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
            if len(iVariant) != 0:
                while (i < 16):
                    lib_iVariant[i] = int(iVariant[loop:loop+2],16)
                    loop += 2
                    i += 1
            iLevel = c_uint(Level)
            seed2key = libso.Seed2Key
            seed2key.restype = c_int
            key = (c_ubyte*16)()
            res = seed2key(lib_ecu,lib_seed,key,iLevel,lib_iVariant)
            if res == ERROR.NoError.value:
                return res,bytearray(key).hex().upper()
            else:
                return res,""
        else:
            lib_ecu = c_char_p(EcuSection.encode())
            lib_seed = (c_ubyte*4)(0,0,0,0)
            i = 0
            loop = 0
            while (i < 4):
                lib_seed[i] = int(Seed[loop:loop+2],16)
                loop += 2
                i += 1
            i = 0
            loop = 0
            lib_iVariant = (c_ubyte*4)(0,0,0,0)
            if len(iVariant) != 0:
                while (i < 4):
                    lib_iVariant[i] = int(iVariant[loop:loop+2],16)
                    loop += 2
                    i += 1
            iLevel = c_uint(Level)
            seed2key = libso.Seed2Key
            seed2key.restype = c_int
            key = (c_ubyte*4)()
            res = seed2key(lib_ecu,lib_seed,key,iLevel,lib_iVariant)
            if res == ERROR.NoError.value:
                return res,bytearray(key).hex().upper()
            else:
                return res,""
    except:
        MSGLogger.critical(traceback.format_exc())
        return ERROR.ExceptionOccurred.value,""

@handler_exception_decorator(length = 2)
def GenerateKeyFromSeed(EcuSection:str,Seed:str,Level:int,iVariant:str):
    return GenerateKeyFromSeed_P(EcuSection,Seed,Level,iVariant)
