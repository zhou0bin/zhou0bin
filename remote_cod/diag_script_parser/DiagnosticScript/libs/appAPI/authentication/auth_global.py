import os
from enum import Enum

AUTOX_ON = True
GUANGBEN_ON = False
COMMON_ON = False

DEFAULT_29_TOOL = "UDS29ServiceTool.exe"
# DEFAULT_AUTH_CONFIG_PATH = "auth_config.json"
DEFAULT_AUTH_CONFIG_PATH = os.path.join(os.path.join(os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(__file__)))), "config"), "auth"), "auth_config.json")

# -------------------------- 标准报文格式配置 --------------------------
class UDSConfig:
    """UDS 0x29服务核心配置(ISO 14229-1:2020)"""
    # 服务ID与子功能
    AUTH_SERVICE_ID = "29"                          # 29服务
    DEAUTHENTICATE = "00"                           # 取消认证
    VERIFY_CERT_BIDIRECTIONAL = "02"                # 双向认证
    PROOF_OF_OWNERSHIP = "03"                       # 所有权认证
    REQUEST_CHALLENGE_FOR_AUTHENTICATION = "05"     # 请求认证挑战数
    VERIFY_PROOF_OF_OWNERSHIP_UNIDIRECTIONAL = "06" # 单向认证所有权
    DE_AUTHENTICATE = "00"                          # 终止认证

    # 通信常量
    SID_RESPONSE = 0x40                             # 响应SID偏移（0x29+0x40=0x69）

    # 报文字段配置
    COMM_CONFIG = 0                                 # 通信配置：0x01=ECC+SHA256（与ECU协商）
    CHALLENGE_LEN = 32                              # 挑战值固定长度（32字节）

    # 报文长度要求
    VERIFY_CERT_BIDIRECTIONAL_MIN                           = 11
    REQUEST_CHALLENGE_FOR_AUTHENTICATION_MIN                = 23
    REQUEST_VERIFY_PROOF_OF_OWNERSHIP_UNIDIRECTIONAL_MIN    = 21

class AuthResult(Enum):
    """认证错误码枚举（适配 UDS 29服务故障场景）"""
    Success                                     = 0x00             # 成功
    VerifyServerCertAndProofOfOwnershipFail     = 0x01             # 验证服务端证书失败
    VerifyCertBidirectionalFail                 = 0x02             # 双向认证失败
    ProofOfOwnershipFail                        = 0x03             # 验证所有权失败
    RequestChallengeForAuthenticationFail       = 0x04             # 请求挑战数认证
    SignChallengeFail                           = 0x05             # 签名挑战数失败
    VerifyProofOfOwnershipUnidirectionalFail    = 0x06             # 认证单向所有权失败
    DeAuthenticateFail                          = 0x08             # 取消认证失败
    Timeout                                     = 0x09             # 超时
    NegativeResponse                            = 0x7f             # 否定响应
    ExceptionOccurred                           = 0x80             # 发生异常
    UnkownError                                 = 0x81             # 未知错误  
    AuthNoSupport                               = 0x82             # 认证不支持

    # 为错误码添加名称映射（避免name属性报错）
    @classmethod
    def get_name(cls, code):
        for name, value in cls.__dict__.items():
            if value == code and not name.startswith('_'):
                return name
        return "UnknownError"