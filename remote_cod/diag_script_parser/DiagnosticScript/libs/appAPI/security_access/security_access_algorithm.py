#!/usr/bin/python3
import os, re, json, sys, importlib

from typing import List, Dict, Any
from pathlib import Path
# import pandas as pd
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.cmac import CMAC
from libs.appAPI.security_access.security_access_global import *
from libs.appAPI.log_info import MSGLogger

class SecurityAccessAlgorithm:
    def __init__(self, program: str, config_path: str = ""):
        """
        初始化安全访问算法实例
        Args:
            program: 项目名称
            sa_config_dir: 配置文件目录
        """
        self.program = program
        self.ecu_sa_config = []
        self.global_sa_config = {}
        self.ParseConfig(program, config_path)
        self.key = b""
        self.seed = b""
        self.iv = DEFAULT_AES128_IV
        self.mask = b""

    def ParseConfig(self, program, sa_config: str):
        if sa_config == "":
            sa_config = DEFAULT_SA_CONFIG_PATH
        sa_dir = os.path.dirname(sa_config)
        print(sa_config)
        with open(sa_config, "r", encoding="utf-8") as f:
            config_list = json.load(f)
        for s_config in config_list:
            if s_config["program"] == program:
                # 需要读取配置
                if "config" in s_config:
                    config = s_config["config"]
                    method = config["method"]
                    # if method == "excel":
                    #     if "path" in config and config["path"] != "":
                    #         path = config["path"]
                    #     else:
                    #         path = os.path.join(sa_dir, "security_mask.xlsx")
                    #     self.ecu_sa_config = self.ParseSecurityExcel(path)
                    #     MSGLogger.info(f"Security access config excel initialized successfully, loaded config: {path}")
                    if method == "module":
                        if "path" in config and config["path"] != "":
                            path = config["path"]
                        else:
                            path = os.path.join(sa_dir, "security_access_config.py")
                        self.ParseSecurityModule(path)
                        MSGLogger.info(f"Security access config module initialized successfully, loaded config: {path}")
                    else:
                        raise ValueError(f"Security access config method is not support, method is {method}")
                else:
                    raise ValueError("Security access config must have config item.")
                if "algorithm" in s_config:
                    self.algorithm = s_config["algorithm"]
                else:
                    raise ValueError(f"Security access config algorithm must")
 
    def ParseSecurityModule(self, module_path: str):
        """
        ParseSecurityModule 的 Docstring
        
        :param self: 说明
        :param module_path: 说明
        :type module_path: str
        """
        # 1. 标准化路径，分离"文件名"和"后缀"
        file_path = Path(module_path).resolve()
        module_dir = str(file_path.parent)  # 模块所在目录
        print(module_dir)
        module_name = file_path.stem        # 模块名（去掉后缀）
        print(module_name)

        # 2. 定义要检查的文件类型（优先级：py > pyc）
        check_files = [
            file_path.with_suffix(".py"),   # 拼接成 .py 文件路径
            file_path.with_suffix(".pyc")   # 拼接成 .pyc 文件路径
        ]
        # 3. 查找存在的文件（按优先级）
        target_file = None
        for f in check_files:
            print(f)
            if f.exists() and f.is_file():
                target_file = f
                break
        if not target_file:
            raise ValueError(f"SecurityAccessAlgorithm::ParseSecurityModule: not find {module_name}.py or {module_name}.pyc")
        # 4. 将模块目录加入Python搜索路径
        if module_dir not in sys.path:
            sys.path.append(module_dir)
        # file_suffix = target_file.suffix
        # 5. 动态导入模块（核心：导入时只需模块名，无需后缀）
        try:
            module = importlib.import_module(module_name)
            if hasattr(module, "global_sa_config"):
                self.global_sa_config = module.global_sa_config
            if hasattr(module, "ecu_sa_config"):
                self.ecu_sa_config = module.ecu_sa_config
        except Exception as e:
            raise ValueError(f"SecurityAccessAlgorithm::ParseSecurityModule: import {module_name} fail.")

    # def ParseSecurityExcel(self, excel_path: str) -> List[Dict[str, Any]]:
    #     """
    #     解析ECU安全掩码Excel文件
    #     Args:
    #         excel_path: Excel文件路径
    #     Returns:
    #         结构化的ECU安全掩码配置
    #     """
    #     if not os.path.exists(excel_path):
    #         raise FileNotFoundError(f"Excel文件不存在：{excel_path}")

    #     # 读取Excel，跳过标题行（第0-2行）
    #     df = pd.read_excel(excel_path, sheet_name='安全掩码', header=None, skiprows=3)
    #     ecu_config_list = []

    #     for _, row in df.iterrows():
    #         # 提取ECU名称（第2列，索引从0开始）
    #         ecu_name = row.iloc[2] if len(row) > 2 else None
    #         if pd.isna(ecu_name) or str(ecu_name).strip() == "":
    #             continue
    #         ecu_name = str(ecu_name).strip()

    #         # 提取安全掩码（Level1-第5列，Level9-第6列）
    #         security_masks = []
    #         # Level 1
    #         mask1 = row.iloc[5] if len(row) > 5 else None
    #         if pd.notna(mask1) and str(mask1).strip() not in ["", "0x01", "安全掩码"]:
    #             security_masks.append({
    #                 "level": 1,
    #                 "mask": str(mask1).strip()
    #             })
    #         # Level 9
    #         mask9 = row.iloc[6] if len(row) > 6 else None
    #         if pd.notna(mask9) and str(mask9).strip() not in ["", "0x09"]:
    #             security_masks.append({
    #                 "level": 9,
    #                 "mask": str(mask9).strip()
    #             })

    #         # 提取AES密钥（第7列）
    #         aes_key = row.iloc[7] if len(row) > 7 else None
    #         aes_key_str = str(aes_key).strip() if pd.notna(aes_key) and str(aes_key).strip() != "研发阶段AES-KEY" else ""

    #         # 仅添加有有效配置的ECU
    #         if security_masks:
    #             ecu_config_list.append({
    #                 "ECU": ecu_name,
    #                 "security_mask": security_masks,
    #                 "key": aes_key_str
    #             })

    #     MSGLogger.debug(f"Parsed security configuration for {len(ecu_config_list)} ECUs")
    #     return ecu_config_list

    def GetECUSAConfig(self, ecu: str, level: int) -> tuple[SecurityAccessResult, bytes, bytes]:
        """
        获取ECU指定安全等级的AES密钥和掩码（转换为字节类型）
        Returns:
            (错误码, AES密钥字节, 掩码字节)
        """
        for ecu_item in self.ecu_sa_config:
            if ecu_item["ECU"].lower() == ecu.lower():
                # 校验AES密钥
                key_str = ecu_item.get("key", "")
                # if not key_str:
                #     MSGLogger.error(f"ECU[{ecu}] has no AES key configured")
                #     return SecurityAccessResult.KeyCalculateError, b"", b""
                # 校验掩码
                security_level = ecu_item.get("security_level", [])
                mask_str = ""
                if security_level != []:
                    for level_item in security_level:
                        if level_item["level"] == level:
                            mask_str = level_item.get("mask", "")
                            if "key" in level_item:
                                key_str = level_item["key"] 
                            break
                    # if not mask_str:
                    #     MSGLogger.error(f"ECU[{ecu}] has no mask configuration for security level [{level}]")
                    #     return SecurityAccessResult.InvalidSecurityLevel, b"", b""
                if not key_str:
                    MSGLogger.error(f"ECU[{ecu}] has no AES key configured")
                    return SecurityAccessResult.KeyCalculateError, b"", b""
                # 转换为字节类型
                try:
                    key_bytes = bytes.fromhex(key_str)
                    if mask_str != "":
                        mask_bytes = bytes.fromhex(mask_str)
                    else:
                        mask_bytes = b""
                    if len(key_bytes) != 16:
                        MSGLogger.error(f"ECU[{ecu}] AES key length error (required 16 bytes, actual {len(key_bytes)})")
                        return SecurityAccessResult.KeyCalculateError, b"", b""
                    return SecurityAccessResult.Success, key_bytes, mask_bytes
                except ValueError as e:
                    MSGLogger.error(f"ECU[{ecu}] key/mask conversion failed: {str(e)}")
                    return SecurityAccessResult.KeyCalculateError, b"", b""
                
        MSGLogger.error(f"No configuration found for ECU[{ecu}]")
        return SecurityAccessResult.ECUNotFound, b"", b""

    def XorBytes(self, seed_bytes: bytes, mask_bytes: bytes) -> bytes:
        """
        按字节异或，短字节补0对齐
        """
        max_len = max(len(seed_bytes), len(mask_bytes))
        seed_padded = seed_bytes.ljust(max_len, b'\x00')
        mask_padded = mask_bytes.ljust(max_len, b'\x00')
        xor_result = bytes([s ^ m for s, m in zip(seed_padded, mask_padded)])
        MSGLogger.debug(
            f"XOR operation - Seed: {seed_bytes.hex().upper()}, Mask: {mask_bytes.hex().upper()}, "
            f"Result: {xor_result.hex().upper()}"
        )
        return xor_result

    def GenerateIV(self, mode: str) -> bytes:
        """
        生成符合加密模式的IV
        Args:
            mode: ecb/cbc/gcm/ctr
        Returns:
            IV字节串（ECB返回空，GCM返回12字节，CBC/CTR返回16字节）
        """
        if mode == "ecb":
            return b""  # ECB模式无需IV
        elif mode in ("gcm", "cbc", "ctr"):
            return self.iv
        else:
            raise ValueError(f"不支持的加密模式：{mode}")

    def Aes128Cmac(self, data: bytes, key: bytes) -> tuple[SecurityAccessResult, bytes | None]:
        """
        AES-128 CMAC计算（固定16字节输出）
        """
        if len(key) != 16:
            MSGLogger.error(f"AES-CMAC key length error: required 16 bytes, actual {len(key)}")
            return SecurityAccessResult.KeyCalculateError, None
        
        try:
            cmac = CMAC(algorithms.AES(key))
            cmac.update(data)
            result = cmac.finalize()
            MSGLogger.debug(f"AES-CMAC calculation succeeded, result: {result.hex().upper()}")
            return SecurityAccessResult.Success, result
        except Exception as e:
            MSGLogger.error(f"AES-CMAC calculation failed: {str(e)}", exc_info=True)
            return SecurityAccessResult.KeyCalculateError, None

    def Aes128Encrypt(
        self,
        plaintext: bytes,
        key: bytes,
        mode: str = "ecb"
    ) -> tuple[SecurityAccessResult, bytes | None, bytes | None, bytes | None]:
        """
        AES-128加密（适配UDS场景，支持ECB/CBC/GCM/CTR）
        """
        # 密钥长度校验
        if len(key) != 16:
            MSGLogger.error(f"AES-128 key length error: required 16 bytes, actual {len(key)}")
            return SecurityAccessResult.KeyCalculateError, None, None, None

        # PKCS7填充
        try:
            padder = padding.PKCS7(128).padder()
            padded_plaintext = padder.update(plaintext) + padder.finalize()
            MSGLogger.debug(f"PKCS7 padding completed: original {len(plaintext)} bytes → padded {len(padded_plaintext)} bytes")
        except Exception as e:
            MSGLogger.error(f"PKCS7 padding failed: {str(e)}")
            return SecurityAccessResult.KeyCalculateError, None, None, None

        # 生成IV
        try:
            iv = self.GenerateIV(mode)
        except Exception as e:
            MSGLogger.error(f"IV generation failed: {str(e)}")
            return SecurityAccessResult.KeyCalculateError, None, None, None

        # 初始化加密器
        try:
            if mode == "ecb":
                cipher = Cipher(algorithms.AES(key), modes.ECB())
            elif mode == "cbc":
                cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
            elif mode == "gcm":
                cipher = Cipher(algorithms.AES(key), modes.GCM(iv))
            elif mode == "ctr":
                cipher = Cipher(algorithms.AES(key), modes.CTR(iv))
            else:
                MSGLogger.error(f"Unsupported encryption mode: {mode}")
                return SecurityAccessResult.KeyCalculateError, None, None, None
        except Exception as e:
            MSGLogger.error(f"Encryptor initialization failed: {str(e)}", exc_info=True)
            return SecurityAccessResult.KeyCalculateError, None, None, None

        # 执行加密
        try:
            encryptor = cipher.encryptor()
            ciphertext = encryptor.update(padded_plaintext) + encryptor.finalize()
            tag = encryptor.tag if mode == "gcm" else None
            MSGLogger.debug(
                f"AES-128 encryption succeeded (mode={mode}): ciphertext {len(ciphertext)} bytes, "
                f"IV={iv.hex().upper() if iv else 'None'}, TAG={tag.hex().upper() if tag else 'None'}"
            )
            return SecurityAccessResult.Success, ciphertext, iv, tag
        except Exception as e:
            MSGLogger.error(f"AES-128 encryption failed: {str(e)}", exc_info=True)
            return SecurityAccessResult.KeyCalculateError, None, None, None
        
    def GetSAConfig(self, ecu, level):
        if ecu != "" and level > 0 and self.ecu_sa_config:
            result, self.key, self.mask = self.GetECUSAConfig(ecu, level)
            return result
        elif self.global_sa_config:
            if "key" in self.global_sa_config:
                self.key = bytes.fromhex(self.global_sa_config["key"])
            else:
                return SecurityAccessResult.KeyCalculateError
            if "iv" in self.global_sa_config:
                self.iv = bytes.fromhex(self.global_sa_config["iv"])
            return SecurityAccessResult.Success
        else:
            return SecurityAccessResult.Success
        
    def AdditionalProcess(self):
        # Seed与掩码异或
        self.seed = self.XorBytes(self.seed, self.mask)

    def GenerateKeyFromSeed(
        self,
        seed: str,
        ecu: str = "",
        level: int = -1,
        algorithm: str = ""
    ) -> tuple[SecurityAccessResult, str]:
        """
        核心接口：从Seed生成UDS安全访问Key
        Args:
            seed: 种子（十六进制字符串，支持0x前缀）
            ecu: ECU名称
            level: 安全等级
            algorithm: 算法类型（aes_128_cmac/aes_128_ecb/aes_128_cbc/aes_128_gcm/aes_128_ctr）
        Returns:
            (错误码, 十六进制Key字符串)
        """
        # 1. Seed格式校验
        try:
            seed_stripped = seed.strip()
            if seed_stripped.startswith(('0x', '0X')):
                seed_clean = seed_stripped[2:]
            else:
                seed_clean = seed_stripped
            # seed_clean = seed.strip().lstrip("0x").lstrip("0X")
            if not re.match(r"^[0-9A-Fa-f]+$", seed_clean):
                MSGLogger.error(f"Invalid Seed format: {seed} (must be hexadecimal string)")
                return SecurityAccessResult.InvalidSeed, ""
            self.seed = bytes.fromhex(seed_clean)
        except Exception as e:
            MSGLogger.error(f"Seed parsing failed: {str(e)}", exc_info=True)
            return SecurityAccessResult.InvalidSeed, ""
        
        # 2. level校验
        if level <= 0:
            MSGLogger.error(f"requires specified security level[{level}]")
            return SecurityAccessResult.InvalidSecurityLevel, ""
        
        # 3. 获取配置 key, mask(可选)
        result = self.GetSAConfig(ecu, level)
        if result != SecurityAccessResult.Success:
            return result, ""

        # 4、调用算法前的额外处理, 用于各个厂商添加自己的处理
        self.AdditionalProcess()

        # 5. 算法执行
        try:
            if algorithm == "":
                algorithm = self.algorithm
            if algorithm == "aes_128_cmac":
                error, ciphertext = self.Aes128Cmac(self.seed, self.key)
            elif re.match(r"aes_128_(ecb|cbc|gcm|ctr)", algorithm, re.IGNORECASE):
                # 提取加密模式（ecb/cbc/gcm/ctr）
                mode = re.search(r"(ecb|cbc|gcm|ctr)", algorithm, re.IGNORECASE).group(1).lower()
                error, ciphertext, _, _ = self.Aes128Encrypt(self.seed, self.key, mode)
            else:
                MSGLogger.error(f"Unsupported algorithm: {algorithm}")
                return SecurityAccessResult.KeyCalculateError, ""

            if error != SecurityAccessResult.Success:
                MSGLogger.error(f"Algorithm execution failed: {SecurityAccessResult.get_name(error)}")
                return error, ""

            # 转换为大写十六进制Key
            key_hex = ciphertext.hex().upper()
            MSGLogger.info(
                f"Key generation succeeded | Project={self.program} | ECU={ecu} | Level={level} | "
                f"Algorithm={algorithm} | Seed={seed} | Key={key_hex}"
            )
            return SecurityAccessResult.Success, key_hex
        except Exception as e:
            MSGLogger.error(f"Key generation exception: {str(e)}", exc_info=True)
            return SecurityAccessResult.ExceptionOccurred, ""