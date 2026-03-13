/*****************************************************************************
* @file     diag_file.h
* @brief    
* @author   kevin
* @date     2025/4/24
*****************************************************************************/

#ifndef VD_DIAG_FILE_H_
#define VD_DIAG_FILE_H_

#include <string>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <atomic>
#include <iostream>
#include <time.h>
#include <chrono>
#include <iomanip>
#include <optional>


#include "common/system_cmd.h"
#include "Poco/Delegate.h"
#include "Poco/Zip/Decompress.h"
#include "Poco/Zip/ZipLocalFileHeader.h"
#include "midware/crypto_wrapper/crypto_services_wrapper.h"
#ifdef AP_CRYPTO
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_provider.h"

using namespace ara::crypto;
using namespace cryp;
using namespace keys;
#endif

#define SO_FILE_NAME "encrypted_libSA.so"
#define KEY_FILE_NAME "Publickey.txt"

enum FileType : uint
{
    TempScriptPackage = 0,
    FixedScriptPackage = 1,
    Odx = 2,
    SAFile = 3,
    MTOConfig = 5,
    OptionConfig = 8,
    OptionCodeMap = 9,
    EcuidMtoc = 10,
    EventScriptPackage = 11,
    PeriodScriptPackage = 12
};

class DiagFile
{
public:
#ifdef MIDWARE_CRYPTO
    using CryptoServicesWrapper = middleware::cryptowrapper::CryptoServicesWrapper;
    using CryptoMWAlgoNid = middleware::cryptowrapper::CryptoMWAlgoNid;
    using CryptoMWError = middleware::cryptowrapper::CryptoMWError;
#endif

    DiagFile() = default;
    
    ~DiagFile() noexcept
    {};

    void DecompressAndDistributeFile(std::string packageName,FileType fileType);

    int CompressPackage(std::string resultPath, std::string resultPackageName);

    std::optional<std::string> DecryptFileByAES(const std::string& public_key_string, std::string key1, std::string key0 = "0000000000000000");

    std::optional<std::string> DecryptFileByRSA(const std::string& scriptPackageSign, const std::string& publicKey);

    std::vector<uint8_t> DecryptCMACKeyFile(const std::vector<uint8_t>& file, bool& result);

    std::vector<uint8_t> CalculateFileSha256(std::vector<uint8_t> scriptPackage);

    std::vector<uint8_t> CalculateFileMd5(std::vector<uint8_t> file);

    std::vector<uint8_t> CalculateCMACValue(const std::vector<uint8_t>& context, const std::string& key, bool& result);

    std::vector<uint8_t> CryptFileByAES(std::vector<uint8_t> resultPackage, std::string key1, std::string key0 = "0000000000000000");

    bool CompareSha256(std::vector<uint8_t> fileSha256, std::string stringSha256);

    void GetCryptoProvider();

#if 0
    bool VerifySign(std::string contextStr, std::string signStr);

    bool VerifyCertificate(std::string ClientCertificate);
#else
    bool VerifyCertChainSign(std::string ClientCertificate, std::string contextStr, std::string signStr);

    bool VerifySignByContext(const std::vector<uint8_t> &context, const std::vector<uint8_t>& sign, const std::string& public_key);
#endif

    bool PublickeyWrite(std::string publickey);

    bool PublickeyRead(std::string &Public_key);

    bool SoCryptoWrite(std::string filepath);

    bool SoDecryptoRead(std::string filepath);

    bool ExtractZip(const std::string& sourcePath, const std::string& extractionDirectory);

    void ExtractFile(const std::vector<uint8_t>& scriptPackageMerge, std::vector<uint8_t>& scriptPackageSign,std::vector<uint8_t>& scriptPackage);

    long int FileSize(const char *fname);

    void ProcessFile2BigDataFolder(const std::string script_result_path, const std::string packageName);

    long GetFileSize(const std::string& filePath);

    size_t GetFloderSize(const std::string &folder);

    std::string GetBigDataFormatFileName();

    std::string CompareFileGenTime(const std::string& file1, const std::string& file2);

    bool RenameFile(const std::string& oldName, const std::string& newName);

    std::string RandomKey();

    std::string ConvertKey(const std::string key0);

    bool Base64Decode(const std::string& src, std::vector<unsigned char>& out);

#ifdef AP_CRYPTO
    CryptoProvider::Sptr cryptoProvider_sptr = nullptr;
#endif
    SystemCmd file_opreation_instance;

#ifdef MIDWARE_CRYPTO
    CryptoServicesWrapper crypto_service_wrapper;
#endif

private:
    void SignStr2Vector(std::string signStr, std::vector<uint8_t> &signVetvor);

#ifdef AP_CRYPTO
    void initMsgRecoveryPublic(MsgRecoveryPublicCtx &msgRecoveryCtx, const PublicKey &key);
#endif

    void processPublicKeyforRSA(std::string &publicKey);

    void CopyFile(const std::string& source, const std::string& dest, const std::string& wildcard);
    size_t const maxblock = 1024;
    int coutnum = 64;
    static int result_file_num;
    const std::string aes_key = "zwl6YyG97Wfax6fH";
    const std::vector<uint8_t> cmac_file_iv = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                                          0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
    const unsigned char cmac_file_key_h[6] = {0xff,0xfe,0xfd,0xfc,0xfb,0xfa};
    const unsigned char cmac_file_key_l[10] = {0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf3,0xf2,0xf1,0xf0};
};

#endif // VD_DIAG_FILE_H_
