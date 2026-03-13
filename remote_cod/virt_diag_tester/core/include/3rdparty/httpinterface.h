// #ifndef VCPROXYSERVICEWAPPER_HTTPEXPORFUNTION_H
// #define VCPROXYSERVICEWAPPER_HTTPEXPORFUNTION_H

// #include <map>
// #include <vector>
// #include <future>
// #include "common.h"

// #include "asf/aag/http_file_interface.h"

// using namespace asf::aag;

// namespace middleware
// {
//     namespace vcproxyservicewapper
//     {
//         class CHttpClient;

//         class HttpClient
//         {
//         private:
//             CHttpClient * m_pHttpClient;

//         public:
//             HttpClient(/* args */);
//             ~HttpClient();
//             /**
//              * @brief 执行curl
//              * @param int 返回执行正确值:200 (其他返回值请参考curl和http错误码)
//              */
//             int Call();
//             /**
//              * @brief 设置请求类型
//              * @param  type            请求方式类型POST、GET、DOWNLOAD、UPLOAD
//              */
//             void SetRequestType(const Type type);
//             /**
//              * @brief 获取请求类型
//              * @return const Type    请求方式类型POST、GET、DOWNLOAD、UPLOAD
//              */
//             const Type GetRequestType() const;
//             /**
//              * @brief 设置网卡
//              * @param  vlan   网卡(默认值:vlan = "vlan8")
//              */
//             void SetVLan(const std::string &vlan);
//             /**
//              * @brief 获取网卡
//              * @return int   网卡
//              */
//             std::string &GetVLan();

//             /**
//              * @brief 设置域名
//              * @param  domain   域名
//              */
//             void SetDomain(std::string &&domain);
//             /**
//              * @brief 获取域名
//              * @return string   域名
//              */
//             std::string &GetDomain();

//             /**
//              * @brief 设置超时时间
//              * @param  time             超时时间(默认值:time = 60秒)
//              */
//             void SetTimeOut(long time);
//             /**
//              * @brief 获取超时时间
//              * @return long            超时时间
//              */
//             long GetTimeOut();
//             /**
//              * @brief 设置重试次数
//              * @param  tries        重试次数(默认值:tries = 3次)
//              */
//             void SetTries(int tries);
//             /**
//              * @brief 获取重试次数
//              * @return int          重试次数
//              */
//             int GetTries();
//             /**
//              * @brief 设置重试间隔时间
//              * @param  time         重试间隔时间(默认值:time = 5秒)
//              */
//             void SetReDelay(long time);
//             /**
//              * @brief 获取重试间隔时间
//              * @return long         重试间隔时间
//              */
//             long GetReDelay();

//             /**
//              * @brief 设置认证方式（默认不需要认证，支持单项认证和双项认证）
//              * @param  authsModeType        AuthsModeTypeEN认证模式(三种模式：无、单项认证、双项认证)
//              */
//             void SetAuthsMode(AuthsModeTypeEN authsModeType);
//             /**
//              * @brief 获取认证方式
//              * @return AuthsModeTypeEN   认证枚举类型
//              */
//             AuthsModeTypeEN GetAuthsMode() const;

//             /**
//              * @brief 设置URL路径
//              * @param  url              URL路径
//              */
//             void SetUrlPath(const std::string &url);
//             /**
//              * @brief 设置URL路径
//              * @param  url              URL路径
//              */
//             void SetUrlPath(std::string &&url);
//             /**
//              * @brief 获取URL路径
//              * @return const char*    URL路径
//              */

//             const char *GetUrl();

//             /**
//              * @brief 设置请求的Body体
//              * @param  bodyData         Body体
//              */
//             void SetBodys(std::map<std::string, std::string> &&bodyData);
//             /**
//              * @brief 获取Body体
//              * @return const std::map<std::string, std::string>
//              */
//             const std::map<std::string, std::string> &GetBodys();
//             /**
//              * @brief 设置Header
//              * @param  headers          Body体
//              */
//             void SetHeaders(std::map<std::string, std::string> &&headers);
//             /**
//              * @brief 获取Header
//              * @return const std::map<std::string, std::string>   Header
//              */
//             const std::map<std::string, std::string> GetHeaders() const;

//             /**
//              * @brief 获取文件操作指针
//              * @return std::shared_ptr<FileOperateInterface>    文件操作指针
//              */
//             std::shared_ptr<FileOperateInterface> GetFileOperatePtr();

//             /**
//              * @brief 设置文件操作指针
//              * @tparam T
//              * @param  pFileOperate     继承FileOperateInterface的子类指针
//              */

//             void SetFileOperatePtr(std::shared_ptr<FileOperateInterface> pFileOperate);
//             /**
//              * @brief 获取请求数据大小
//              * @return string   数据大小
//              */
//             size_t GetRequestDataSize() const;
//             /**
//              * @brief 获取请求数据
//              * @return string   数据
//              */
//             std::string GetRequestData();
//             /**
//              * @brief 设置本地文件路径
//              * @param  strFilePath     本地文件路径
//              */
//             void SetLocalFilePath(const std::string &strFilePath);
//             /**
//              * @brief 获取本地文件路径
//              * @return string   文件路径
//              */
//             std::string GetLocalFilePath();
//             /**
//              * @brief 设置本地文件名称
//              * @param  strFileName     文件名称
//              */
//             void SetLocalFileName(const std::string &strFileName);
//             /**
//              * @brief 获取本地名称
//              * @return string   文件名称
//              */
//             std::string GetLocalFileName();
//             /**
//              * @brief 设置ContentType
//              * @param  strContentType     请求类型
//              */
//             void SetContentType(const std::string &strContentType);
//             /**
//              * @brief 获取ContentType
//              * @return string   请求类型
//              */
//             std::string GetContentType();
//             /**
//              * @brief 设置下载状态
//              * @param  iPause     类型(0:默认下载 1:暂停下载 2:恢复下载)
//              */
//             void SetPauseType(int iPause);
//             /**
//              * @brief 获取下载状态
//              * @param  int 返回状态结果
//              */
//             int GetPauseType();
// #if 0
//             /**
//              * @brief 设置证书名称
//              * @param  strFileName     继承FileOperateInterface的子类指针
//              */

//             void SetCertificateName(const std::string &strFileName);
//             /**
//              * @brief 获取证书名称
//              * @return string   证书名称
//              */
//             std::string &GetCertificateName();
//             /**
//              * @brief 设置证书路径(使用本地证书就传入带有路径，使用neusar则传入文件名)
//              * @param  mCertificatePath          证书路径(双向证书时，最后一个传入证书密码)
//              */
//             void SethttpCertificatePath(std::vector<std::string> &&mCertificatePath);
//             /**
//              * @brief 获取证书路径
//              * @param  
//              */
//             std::vector<std::string> &GethttpCertificatePath();
//             /**
//              * @brief 设置证书类型
//              * @param  strCertificateType          支持 PEM DER ENG （默认PEM）
//              */
//             void SetCertificateType(const std::string &strCertificateType);
//             /**
//              * @brief 获取证书类型
//              * @param  
//              */
//             std::string GetCertificateType();            
//              /**
//              * @brief 设置是否使用本地证书，此开关隔离neusar证书使用
//              * @param  bflag （false:使用本地证书， true:使用neusar）
//              */           
//             void SetIsUerLocalCertificate(bool bflag);    
//              /**
//              * @brief 获取是否使用本地证书
//              * @param  
//              */   
//             bool GetIsUerLocalCertificate();
// #endif
//             /**
//              * @brief 设置下载状态
//              * @param  iPause     类型(0:默认下载 1:暂停下载 2:恢复下载)
//              */
//             void IsPauseRecover(int iPause);
//             /**
//              * @brief 设置限速
//              * @param  iSpeed （限速 KB/s）(默认值:iSpeed = 1000)
//              */           
//             void SetSpeedLimit(int iSpeed);       
//             /**
//              * @brief 获取限速
//              * @param  
//              */           
//             int GetSpeedLimit();
//              /**
//              * @brief 获取响应云端信息
//              * @param  
//              */            
//             std::string ResponseDataAsString();
//              /**
//              * @brief 获取响应
//              * @param
//              */       
//             std::string GetErrorBuffer();
//             /**
//              * @brief 获取响应码
//              * @return long    响应码(正确值:200 其他返回值请参考curl和http错误玛)
//              */
//             long GetResponseCode();
//             /**
//              * @brief 设置连接超时间
//              * @param  time             连接超时时间(默认值:time = 30秒)
//              */
//             void SetConnectTimeOut(long time);
//             /**
//              * @brief 获取连接超时时间
//              * @return long            超时时间
//              */           
//             long GetConnectTimeOut();
//             /**
//              * @brief 获取是否请求成功
//              * @return true    成功
//              * @return false   失败
//              */            
//             bool IsSucceed() const;
//             /**
//              * @brief 获取响应数据
//              * @return std::vector<char>*  响应数据
//              */
//             std::vector<char> *GetResponseData();
//             /**
//              * @brief 设置根证书短名
//              * @param  strFileName     上位机配置出的短名(默认值:strCacertName = "pre_rsa_trustca")
//              */
//             void SetCacertName(const std::string &strCacertName);
//             /**
//              * @brief 获取根证书短名
//              * @return string   根证书短名
//              */
//             std::string &GetCacertName();
//             /**
//              * @brief 设置客户端证书短名
//              * @param  strClientcertName     上位机配置出的短名(默认值:strClientcertName = "pre_rsa_ccu")
//              */
//             void SetClientcertName(const std::string &strClientcertName);
//             /**
//              * @brief 获取客户端证书短名
//              * @return string   客户端证书短名
//              */
//             std::string &GetClientcertName();       
//             /**
//              * @brief 设置客户端证书key短名
//              * @param  strCertKey     上位机配置出的短名(默认值:strCertKey = "nap_kms")
//              */
//             void SetCertKey(const std::string &strCertKey);
//             /**
//              * @brief 获取客户端证书key短名
//              * @return string   客客户端证书key短名
//              */
//             std::string &GetCertKey();                    
//             /**
//              * @brief 设置客户端密码(客户端如果需要秘密，可以填写。默认无密码)
//              * @param  strClientPassword     客户端密码
//              */
//             void SetClientPassword(const std::string &strClientPassword);
//             /**
//              * @brief 获取客户端证书密码
//              * @return string   客户端证书密码
//              */
//             std::string &GetClientPassword(); 
//         };
        

        
//     }
// }

// #endif // VCPROXYSERVICEWAPPER_HTTPEXPORFUNTION_H
