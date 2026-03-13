from enum import Enum
import os

# 默认配置
DEFAULT_AES128_KEY = b"a1b2c3d4e5f6g7h8"  # 16字节示例密钥
# AUTOX_SA_CONFIG = "security_mask.xlsx"
DEFAULT_AES128_IV = b"01010101010101010101010101010101"  # 默认IV值
DEFAULT_SA_CONFIG_PATH = os.path.join(os.path.join(os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__)))), "config"), "security_access"), "sa_config.json")

class SecurityAccessResult(Enum):
    """安全访问错误码枚举（适配 UDS 27服务故障场景）"""
    Success                  = 0x00             # 无错误
    VendorNotFound           = 0x01
    VehicleNotFound          = 0x02
    ECUNotFound              = 0x03             # 未找到目标ECU
    InvalidSeed              = 0x04             # 无效Seed（格式/长度错误）
    InvalidPIN               = 0x05
    InvalidSecurityLevel     = 0x06             # 无效安全等级
    InvalidOutParameter      = 0x07             
    InvalidExtraParameter    = 0x08
    KeyCalculateError        = 0x09             # Key计算失败
    InvalidInputParameter    = 0x10
    StringNotMatch           = 0x11
    Timeout                  = 0x12             # 发送诊断报文超时
    NegativeResponse         = 0x7f             # 否定响应
    ExceptionOccurred        = 0x80             # 发生异常
    UnkownError              = 0x81             # 未知错误  

    # 为错误码添加名称映射（避免name属性报错）
    @classmethod
    def get_name(cls, code):
        for name, value in cls.__dict__.items():
            if value == code and not name.startswith('_'):
                return name
        return "UnknownError"