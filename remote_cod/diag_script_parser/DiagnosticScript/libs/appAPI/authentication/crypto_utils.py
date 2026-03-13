import time
from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend
from cryptography.exceptions import InvalidSignature

# -------------------------- 加密工具类 --------------------------
class CryptoUtils:
    """加密工具：证书加载/验证、所有权证明签名/验签"""
    # @staticmethod
    # def LoadCertificate(cert_path):
    #     """加载X.509证书（PEM格式），返回证书对象+原始字节"""
    #     with open(cert_path, "rb") as f:
    #         cert_data = f.read()
    #     cert = x509.load_pem_x509_certificate(cert_data, default_backend())
    #     return cert, cert_data

    # @staticmethod
    # def load_pem_certificate_from_bytes(cert_bytes):
    #     """从字节流解析证书"""
    #     return x509.load_pem_x509_certificate(cert_bytes, default_backend())

    @staticmethod
    def LoadCertificate(cert_path):
        """加载X.509证书（PEM格式），返回证书对象"""
        with open(cert_path, "rb") as f:
            cert_data = f.read()
        cert = x509.load_pem_x509_certificate(cert_data, default_backend())
        return cert

    @staticmethod
    def LoadCertificateBytes(cert_path):
        """返回证书原始字节"""
        with open(cert_path, "rb") as f:
            cert_data = f.read()
        return cert_data
    
    @staticmethod
    def load_der_certificate_from_bytes(cert_bytes):
        """从字节流解析证书"""
        return x509.load_der_x509_certificate(cert_bytes, default_backend())
    
    @staticmethod
    def LoadCertificateFromBytes(cert_bytes):
        """从字节流解析证书"""
        return x509.load_pem_x509_certificate(cert_bytes, default_backend())

    @staticmethod
    def VerifyCertificate(cert, ca_cert):
        """验证证书有效性（CA签名+有效期）"""
        try:
            # 验证CA签名
            ca_pub_key = ca_cert.public_key()
            ca_pub_key.verify(
                cert.signature,
                cert.tbs_certificate_bytes,
                ec.ECDSA(hashes.SHA256()),
                cert.signature_hash_algorithm
            )
            # 验证有效期
            now = time.datetime.utcnow()
            if cert.not_valid_before > now or cert.not_valid_after < now:
                raise ValueError("证书过期或未生效")
            return True
        except Exception as e:
            print(f"证书验证失败: {e}")
            return False

    @staticmethod
    def SignProofOfOwnership(private_key, server_challenge):
        """生成所有权证明（仅签名服务器挑战值，无临时公钥）"""
        signature = private_key.sign(
            server_challenge,
            ec.ECDSA(hashes.SHA256())
        )
        return signature

    @staticmethod
    def VerifyProofSignature(pub_key, challenge, signature):
        """验证所有权证明签名"""
        try:
            pub_key.verify(signature, challenge, ec.ECDSA(hashes.SHA256()))
            return True
        except InvalidSignature:
            return False