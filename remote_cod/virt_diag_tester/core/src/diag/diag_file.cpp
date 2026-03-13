/*****************************************************************************
* @file     diag_file.cpp
* @brief    Diagnostic File
* @date     2025/4/24
*****************************************************************************/

#include "diag/diag_file.h"
#include "common/macro.h"
#include "common/log.h"

using std::string;

int DiagFile::result_file_num = 1;

void DiagFile::DecompressAndDistributeFile(std::string packageName, FileType fileType)
{
    //解压缩文件
    //解压：tar zxvf FileName.tar.gz
    //压缩：tar zcvf FileName.tar.gz DirName
    //std::string decompress_cmd = "cd /containers/Container06/work/common/opt/VirtualTesterAPP/shared/temp/ && tar zxvf " 
    //                         + packageName + ".tar.gz";
    //system(decompress_cmd.c_str());
    std::string packagePath = "";
    // packageName += ".tar.gz";
    packagePath = TEMP_STORAGE_FOLDER + packageName + ".tar.gz";
    LOGINFO << "DiagFile::DecompressAndDistributeFile: packagePath is " << packagePath.c_str();
    file_opreation_instance.uncomparss_dir((char*)TEMP_STORAGE_FOLDER, (char*)packagePath.c_str());

    //将解压后的文件放入相应目录，不同的任务类型放入不同的目录
    if(fileType == TempScriptPackage) {
        //这里用的TEMP_SCRIPT_STORAGE_FOLDER多一层/Script是因为解压出来的压缩包有这层目录
        LOGINFO << "DiagFile::DecompressAndDistributeFile: TempScriptPackage.";
        file_opreation_instance.copy_dir(TEMP_SCRIPT_STORAGE_FOLDER, TEMP_SCRIPT_FOLDER);
    }else if(fileType == EventScriptPackage) {
        LOGINFO << "DiagFile::DecompressAndDistributeFile: EventScriptPackage.";
        file_opreation_instance.copy_dir(TEMP_SCRIPT_STORAGE_FOLDER, EVENT_SCRIPT_FOLDER);
    }else if(fileType == PeriodScriptPackage) {
        LOGINFO << "DiagFile::DecompressAndDistributeFile: PeriodScriptPackage.";
        file_opreation_instance.copy_dir(TEMP_SCRIPT_STORAGE_FOLDER, PERIOD_SCRIPT_FOLDER);
    }
    #if 0
    else if(fileType == FixedScriptPackage) {
        file_opreation_instance.copy_dir(TEMP_SCRIPT_STORAGE_FOLDER, LOCAL_SCRIPT_FILE_FOLDER);
    }
    else if(fileType == Odx){
        file_opreation_instance.copy_dir(ODX_FLODER,DSCR_FLODER);
    }
    else if(fileType == SAFile){

    }
    else if(fileType == MTOConfig){
        CopyFile(TEMP_STORAGE_FOLDER,VEHICLE_CONFIG_FOLDER, ".*_MTOConfig_.*.ini");
    }
    else if(fileType == OptionConfig){
        CopyFile(TEMP_STORAGE_FOLDER,VEHICLE_CONFIG_FOLDER, ".*_OptionConfig_.*.xml");
    }
    else if(fileType == OptionCodeMap){
        CopyFile(TEMP_STORAGE_FOLDER,VEHICLE_CONFIG_FOLDER, ".*_OptionCodeMap_.*.xml");
    }
    else if(fileType == EcuidMtoc){
        CopyFile(TEMP_STORAGE_FOLDER,VEHICLE_VERSION_FOLDER, ".*_ecuid_mtoc_.*.ini");
    }
    #endif
    else {
        LOGERROR << "DiagFile::CopyFile: unknow file type.";
    }

    //删除文件
    //system("rm -rf /containers/Container06/work/common/opt/VirtualTesterAPP/shared/temp/*");
    /*直接删除整个文件夹在多线程场景同时下发多个任务时可能会出问题，不在这里删除，单次任务结束时删除*/
    // file_opreation_instance.deleteDir(TEMP_STORAGE_FOLDER, false);
}

void DiagFile::CopyFile(const std::string& sourcePath, const std::string& destPath, const std::string& wildcard)
{

    std::vector<std::string> filesVct;

    if(wildcard.length() <= 0)
    {
        LOGERROR << "wildcard length is 0";
        return;
    }
    
    int ret = file_opreation_instance.Get_all_files_by_wildcard(sourcePath, wildcard, filesVct);
    if(ret < 0)
    {
        LOGERROR << "DiagFile::CopyFile: can not find file name contain " << wildcard.c_str();
        return;
    }
    for (size_t i = 0; i < filesVct.size(); i++)
    {
        file_opreation_instance.copy_file(filesVct[i], destPath);
    }
}

int DiagFile::CompressPackage(std::string resultPath, std::string resultPackageName)
{
    //压缩文件
    int compress_result = 0;
    resultPackageName = resultPath + resultPackageName + ".tar";

    compress_result = file_opreation_instance.tarFolder(resultPath,resultPackageName);
    if(compress_result == 0){
        return 0;
    }
    else{
        return -1;
    }
}

bool DiagFile::ExtractZip(const std::string& sourcePath, const std::string& extractionDirectory)
{
    std::ifstream fin(sourcePath, std::ios::binary);

    try{
        Poco::Zip::Decompress decompress(fin, extractionDirectory); 
        decompress.decompressAllFiles();       
    }
    catch(...){
        LOGERROR << "DiagFile::ExtractZip: extract zip failed.";
        return false;
    }

    fin.close();
    return true;
}

void DiagFile::ExtractFile(const std::vector<uint8_t>& scriptPackageMerge,
                           std::vector<uint8_t>& scriptPackageSign,std::vector<uint8_t>& scriptPackage)
{
    size_t script_package_sign_size = 256;

    for (size_t i = 0; i < scriptPackageMerge.size(); i++)
    {
        if (i < (scriptPackageMerge.size() - script_package_sign_size))
        {
            scriptPackage.push_back(scriptPackageMerge[i]);
        }
        else
        {
            scriptPackageSign.push_back(scriptPackageMerge[i]);
        }
    }
}

void DiagFile::GetCryptoProvider()
{
    // cryptoProvider_sptr = LoadCryptoProvider().Value();
    LOGINFO << "DiagFile::GetCryptoProvider: GetCryptoProvider.";
}

// 转换函数：vec = 输入的uint8_t向量；uppercase = 是否大写（默认小写）
std::string vectorToHexString(const std::vector<uint8_t>& vec, bool uppercase = false) {
    // 16进制字符表（小写/大写）
    const char* hex_chars = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string hex_str;
    hex_str.reserve(vec.size() * 2); // 预分配空间，避免多次扩容（性能关键）
    
    // 遍历每个字节，转换为两位16进制
    for (uint8_t byte : vec) {
        hex_str.push_back(hex_chars[(byte >> 4) & 0x0F]); // 高4位
        hex_str.push_back(hex_chars[byte & 0x0F]);       // 低4位
    }
    return hex_str;
}

bool DiagFile::CompareSha256(std::vector<uint8_t> fileSha256, std::string stringSha256)
{
    LOGINFO << "DiagFile::CompareSha256: stringSha256 is " << stringSha256.c_str();
    LOGINFO << "DiagFile::CompareSha256: fileSha256 is " << vectorToHexString(fileSha256).c_str();
#ifdef MIDWARE_CRYPTO
    std::vector<uint8_t> string_sha256_content;
    std::string byte;
    std::string upper_half_byte;
    std::string lower_half_byte;
    // uint8_t byte;
    if (stringSha256.length() != 64)
    {
        LOGERROR << "DiagFile::CompareSha256: length of sha256 string is not 64.";
        return false;
    }
    for (uint i = 0; i < (stringSha256.length() - 1); i = i + 2)
    {
        upper_half_byte = stringSha256[i];
        lower_half_byte = stringSha256[static_cast<uint>(i + 1)];
        byte = upper_half_byte + lower_half_byte;
        // byte = (uint8_t)stoi(temp,0,16);
        string_sha256_content.push_back((uint8_t)stoi(byte,0,16));
    }
    for (uint i = 0; i < fileSha256.size(); i++)
    {
        if (fileSha256[i] != string_sha256_content[i])
        {
            LOGINFO << "DiagFile::CompareSha256: sha256 value is incorrect.";
            return false;
        }
    }
    LOGINFO << "DiagFile::CompareSha256: sha256 value is correct.";
#endif
    return true;
}

void DiagFile::processPublicKeyforRSA(std::string &publicKey)
{
    int beginpos = static_cast<int>(publicKey.find("-----BEGIN PUBLIC KEY-----"));
    int endpos = static_cast<int>(publicKey.find("-----END PUBLIC KEY-----"));
    LOGINFO << "DiagFile::processPublicKeyforRSA: beginpos " << beginpos << "endpos " << endpos;

    if (endpos < 0)
    {
        publicKey = "-----BEGIN PUBLIC KEY-----\\n" + publicKey;

    }
    if (beginpos < 0)
    {
        publicKey = publicKey + "\\n-----END PUBLIC KEY-----";

    }
}

std::optional<std::string> DiagFile::DecryptFileByRSA(const std::string& scriptPackageSign, const std::string& publicKey)
{
    std::string script_package_sha256 = {};
#if 0
    std::vector<uint8_t> public_key_vec(publicKey.begin(), publicKey.end());
    auto errCode = crypto_service_wrapper.EncDecryptBufferByKey(scriptPackageSign, script_package_sha256, CryptoMWAlgoNid::kMWNID_rsa_2048_pkcs1, public_key_vec, {}, false);
    if (errCode != CryptoMWError::kSuccessed)
    {
        LOGERROR << "DiagFile::DecryptFileByRSA: EncDecryptBufferByKey(DEC) failed!errCode = " << errCode;
        return std::nullopt;
    }
#endif
    return script_package_sha256;
}

std::vector<uint8_t> DiagFile::CalculateFileSha256(std::vector<uint8_t> scriptPackage)
{
    // GetCryptoProvider();
    // std::vector<uint8_t> package_sha256(32, 0);
    std::vector<uint8_t> package_sha256 = {};
#ifdef MIDWARE_CRYPTO
    auto errCode = crypto_service_wrapper.CalHashValueFromData(CryptoMWAlgoNid::kMWNID_sha256, scriptPackage, package_sha256);
    if (errCode != CryptoMWError::kSuccessed)
    {
        LOGERROR << "DiagFile::CalculateFileSha256: CalHashValueFromData(sha256) failed!errCode = " << errCode;
    }
#endif
    return package_sha256;
}

std::vector<uint8_t> DiagFile::CalculateFileMd5(std::vector<uint8_t> file)
{
    std::vector<uint8_t> file_md5(32, 0);
#ifdef AP_CRYPTO
    ReadOnlyMemRegion ReadMem(file.data(), static_cast<int>(file.size()));
    WritableMemRegion WritableMem(file_md5.data(), static_cast<int>(file_md5.size()));

    ara::crypto::cryp::HashFunctionCtx::Uptr hashCtx =
        cryptoProvider_sptr->CreateHashFunctionCtx(ara::crypto::CryptoAlgoNid::kNID_md5).Value();
    hashCtx->Start();
    hashCtx->Update(ReadMem);
    hashCtx->Finish();

    ara::core::Result<size_t> md5_result = hashCtx->GetDigest(WritableMem);
    if (!md5_result){
        LOGERROR << "DiagFile::CalculateFileMd5: failed to compute the md5 value.";
    }
    else{
        LOGINFO << "DiagFile::CalculateFileMd5: compute the md5 value successful.";
    }
#endif
    return file_md5;
}

#ifdef AP_CRYPTO
void DiagFile::initMsgRecoveryPublic(MsgRecoveryPublicCtx &msgRecoveryCtx, const PublicKey &key)
{
    msgRecoveryCtx.SetKey(key);
}
#endif

std::string DiagFile::RandomKey()
{
    std::string key0 = "";
    static bool init = false;
    if (!init)
    {
        init = true;
        srand(static_cast<unsigned>(time(nullptr)));
    }

    key0.clear();
    for (int i = 0; i < 16; i++)
    {
        int temp;
        temp = rand();
        if ((temp % 3 )== 0)
        {
            key0.push_back(static_cast<uint8_t>(temp % 10 + '0'));
        }
        else if ((temp % 3) == 1)
        {
            key0.push_back(static_cast<uint8_t>(temp % 26 + 'a'));
        }
        else
        {
            key0.push_back(static_cast<uint8_t>(temp % 26 + 'A'));
        }
    }
    LOGINFO << "DiagFile::RandomKey create key0 " << key0.c_str();

    return key0;
}

std::vector<uint8_t> DiagFile::CryptFileByAES(std::vector<uint8_t> resultPackage, std::string key1, std::string key0)
{
    // GetCryptoProvider();
    LOGINFO << "DiagFile::CryptFileByAES: file size: " << resultPackage.size();
    LOGINFO << "DiagFile::CryptFileByAES: key1: " << key1.c_str();
    LOGINFO << "DiagFile::CryptFileByAES: key0: " << key0.c_str();
    std::vector<uint8_t> crypt_package{};
#ifdef MIDWARE_CRYPTO
    std::string crypt_package_string{};
    std::vector<uint8_t> symmetric_key_vec;
    // std::vector<uint8_t> iv(16, 0);
    symmetric_key_vec.assign(key1.begin(), key1.end());
    std::string src_data(resultPackage.begin(), resultPackage.end());
    auto errCode = crypto_service_wrapper.EncDecryptBufferByKey(src_data, crypt_package_string, CryptoMWAlgoNid::kMWNID_aes_128_ecb, symmetric_key_vec);
    if (errCode != CryptoMWError::kSuccessed)
    {
        LOGERROR << "DiagFile::CryptFileByAES: EncDecryptBufferByKey(ENC) failed!errCode = " << errCode;
        return {};
    }
    crypt_package.assign(crypt_package_string.begin(), crypt_package_string.end());
#endif
    return crypt_package;
}

std::string DiagFile::ConvertKey(const std::string key0)
{
    std::string key1 = "";
    std::vector<uint8_t> key0_array(16, 0);
    std::vector<uint8_t> key1_array(16, 0);

    LOGINFO << "DiagFile::ConvertKey: key0 " << key0.c_str();
    if (key0.size() != 16)
    {
        LOGINFO << "DiagFile::ConvertKey: Key0 is not 16 bytes,return.";
        return key1;
    }

    for (u_int i = 0; i < key0_array.size(); i++)
    {
        key0_array[i] = (uint8_t)key0[i];
    }
    for (size_t i = 0; i < key0_array.size(); i++)
    {
        if (i == (key0_array.size() - 1))
        {
            key1_array[i] = key0_array[i] ^ key0_array[0];
        }
        else
        {
            key1_array[i] = key0_array[i] ^ key0_array[i + 1];
        }
    }
    for (auto iter : key1_array)
    {
        key1.push_back(iter);
    }

    LOGINFO << "DiagFile::ConvertKey: key1: " << key1.c_str();
    return key1;
}

bool DiagFile::Base64Decode(const std::string& src, std::vector<unsigned char>& out)
{
#ifdef MIDWARE_CRYPTO
    auto base64_result = crypto_service_wrapper.Base64Decode(src, out);
    if (base64_result != CryptoMWError::kSuccessed)
    {
        LOGERROR << "DM_DiagClientInterf::CheckSigalingSign: Base64Decode failed!errCode = " << base64_result;
        return false;
    }
#endif
    return true;
}

std::optional<std::string> DiagFile::DecryptFileByAES(const std::string& public_key_string, std::string key1, std::string key0)
{
    // GetCryptoProvider();
    LOGINFO << "DiagFile::DecryptFileByAES: public_key_string size: " << public_key_string.size();
    LOGINFO << "DiagFile::DecryptFileByAES: key1: " << key1.c_str();
    LOGINFO << "DiagFile::DecryptFileByAES: key0: " << key0.c_str();

    std::string script_package_merge{};
#ifdef MIDWARE_CRYPTO
    std::vector<uint8_t> symmetric_key_vec;
    symmetric_key_vec.assign(key1.begin(), key1.end());
    // 对称解密
    // std::vector<uint8_t> iv(16, 0);
    auto errCode = crypto_service_wrapper.EncDecryptBufferByKey(public_key_string, script_package_merge, CryptoMWAlgoNid::kMWNID_aes_128_ecb, symmetric_key_vec, {}, false);
    if (errCode != CryptoMWError::kSuccessed)
    {
        LOGERROR << "DiagFile::DecryptFileByAES: EncDecryptBufferByKey(DEC) failed!errCode = " << errCode;
        return std::nullopt;
    }
#endif
    return script_package_merge;
}

#if 0
bool DiagFile::VerifyCertificate(std::string certificateStr)
{
    std::vector<uint8_t> CertificateVector;
    CryptoServicesWrapper crypto_service_wrapper;

    for (auto i : certificateStr)
    {
        CertificateVector.push_back(i);
    }

    //获取根证书名
    #if (defined USING_A19G1) || (defined USING_AY5G)|| (defined USING_AY2G)
    std::string cacert_name = crypto_service_wrapper.GetCertName("pre_rsa_ccu_trustca");
    #else
    std::string cacert_name = crypto_service_wrapper.GetCertName("pre_sm2_ccu_trustca");
    #endif
    LOGINFO << "DiagFile::VerifyCertificate: ca cert name: " << cacert_name.c_str();
    //cacert_name =  "pre_sm2_ccu_trustca.pem";

    //验证证书
    int verify_cert_result = 0;
    if (CertificateVector.size() == 0)
    {
        LOGERROR << "DiagFile::VerifyCertificate: no certificate exists,failed!";
        return false;
    }

    verify_cert_result = crypto_service_wrapper.VerifyCertService(CertificateVector, cacert_name, cert_uptr);
    if (verify_cert_result != 0)
    {
        LOGERROR << "DiagFile::VerifyCertificate: verify cert failed,result: " << verify_cert_result;
        return false;
    }
    else
    {
        LOGINFO << "DiagFile::VerifyCertificate: verify certificate successful.";
        return true;
    }
}

bool DiagFile::VerifySign(std::string contextStr, std::string signStr)
{
    std::vector<uint8_t> context, sign;
    CryptoServicesWrapper crypto_service_wrapper;

    LOGINFO << "DiagFile::VerifySign: verifying sign...";

    int verify_sign_result = 0;
    for(auto iter : contextStr){
        context.push_back(iter);
    }
    SignStr2Vector(signStr, sign);

    #if (defined USING_A19G1) || (defined USING_AY5G)|| (defined USING_AY2G)
    verify_sign_result = crypto_service_wrapper.VerifySignByContext(ara::crypto::CryptoAlgoNid::kNID_sha256WithRSAEncryption,
                                                                    context, sign, std::move(cert_uptr));
    #else
    verify_sign_result = crypto_service_wrapper.VerifySignByContext(ara::crypto::CryptoAlgoNid::kNID_sm2_with_SM3,
                                                                    context, sign, std::move(cert_uptr));   
    #endif
    if (verify_sign_result != 0)
    {
        LOGERROR << "DiagFile::VerifySign: verify sign failed,result: " << verify_sign_result;
        return false;
    }
    else
    {
        LOGINFO << "DiagFile::VerifySign: verify sign successful.";
        return true;
    }
}
// #else(autox项目不使用公钥签名，crypto_wrapper没适配VerifyCertChainSign接口)
bool DiagFile::VerifyCertChainSign(std::string certificateStr, std::string contextStr, std::string signStr)
{
    std::vector<uint8_t> CertificateVector;
    CertificateVector.assign(certificateStr.begin(), certificateStr.end());

    //验证证书
    if (CertificateVector.size() == 0)
    {
        LOGERROR << "DiagFile::VerifyCertificate: no certificate exists,failed!";
        return false;
    }

    std::vector<uint8_t> context, sign;

    LOGINFO << "DiagFile::VerifySign: verifying sign...";

    int verify_sign_result = 0;
    for(auto iter : contextStr){
        context.push_back(iter);
    }
    SignStr2Vector(signStr, sign);
    verify_sign_result = crypto_service_wrapper.VerifyCertSignByContext(CryptoMWAlgoNid::kMWNID_rsa_2048_pkcs1_sha256, 
                                                                    CertificateVector, "Slot199/CCUCACert", context, sign);
    // verify_sign_result = 0;
    if (verify_sign_result != 0)
    {
        LOGERROR << "DiagFile::VerifySign: verify sign failed,result: " << verify_sign_result;
        return false;
    }
    else
    {
        LOGINFO << "DiagFile::VerifySign: verify sign successful.";
        return true;
    }
}
#endif

bool DiagFile::VerifySignByContext(const std::vector<uint8_t> &context, const std::vector<uint8_t>& sign, const std::string& public_key)
{
    LOGINFO << "DiagFile::VerifySignByContext: verifying sign...";
    #ifdef MIDWARE_CRYPTO
    std::vector<uint8_t> key_data(public_key.begin(), public_key.end());
    int verify_sign_result = crypto_service_wrapper.VerifySignByContext(CryptoMWAlgoNid::kMWNID_rsa_2048_pkcs1_sha256, context, sign, key_data);
    if (verify_sign_result != 0)
    {
        LOGERROR << "DiagFile::VerifySignByContext: verify sign failed,result: " << verify_sign_result;
        return false;
    }
    else
    {
        LOGINFO << "DiagFile::VerifySignByContext: verify sign successful.";
        return true;
    }
    #endif
    return true;
}

void DiagFile::SignStr2Vector(std::string signStr, std::vector<uint8_t> &signVetvor)
{
    for(unsigned int i = 0; i < signStr.length(); i += 2){
        std::string byteString = signStr.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(),nullptr,16);
        signVetvor.push_back(byte);
        LOGDEBUG << "DiagFile::SignStr2Vector: byte : " << byte;
    }
}

//调用存储中间件解密读取so
#if 0
bool DiagFile::SoDecryptoRead(std::string path1)
{
    Print_Location(LOG_ID, INFO, "read %s start", path1.c_str());

    auto result = OpenFileStorage__NeuSAR_VirtualTesterAPP_PRPort_FS;
    if (!result)
    {
        Print_Location(LOG_ID, ERROR, " %s", "openfs failed");
        return false;
    }
    auto db = std::move(result).Value();
    auto openRst = db->OpenFileReadWrite(SO_FILE_NAME);
    if (!openRst)
    {
        Print_Location(LOG_ID, ERROR, " %s", "OpenFileReadWrite 1st failed");
        auto resetRet = db->ResetFile(SO_FILE_NAME);
        if (!resetRet.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " resetRet libsa.so failed.");
            return false;
        }
        else
        {
            Print_Location(LOG_ID, INFO, "%s", " resetRet libsa.so success.");
        }

        openRst = db->OpenFileReadWrite(SO_FILE_NAME);
        if (!openRst.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " open libsa.so 2nd failed.");
            return false;
        }
    }

    auto db1 = std::move(openRst).Value();
    db1->seek(0, ara::per::BasicOperations::SeekDirection::kEnd);
    long int filesize = db1->tell();
    db1->seek(0);

    char *readch = new char[static_cast<size_t>(filesize)]{0};
    db1->read(ara::core::Span<char>(readch, filesize));
    FILE *fp;

    fp = fopen(path1.c_str(), "wb+");
    if (!fp)
    {
        Print_Location(LOG_ID, ERROR, "open or create  %s failed", path1.c_str());
        delete[] readch;
        return false;
    }
    size_t retVal = fwrite(readch, static_cast<size_t>(filesize), 1, fp);
    fclose(fp);
    Print_Location(LOG_ID, INFO, "fwrite ret %ld , read %s end", retVal, path1.c_str());

    delete[] readch;
    return true;
}
#endif

//调用存储中间件加密写入读取so
#if 0
bool DiagFile::SoCryptoWrite(std::string path1)
{
    // std::string path1 = "/containers/Container06/rootfs/usr/lib/libSA.so"; // 读取加密文件包
    long int filesize = FileSize(path1.c_str());
    if (filesize <= 0 /* condition */)
    {
        Print_Location(LOG_ID, ERROR, " %s", "filesize <=0 ");
        return false;
    }

    Print_Location(LOG_ID, INFO, "filesize = %ld !", filesize);

    FILE *fileFD = fopen(path1.c_str(), "rb");
    char *writeChars1 = new char[static_cast<size_t>(filesize)]{0};

    if (fileFD)
    {
        fread(writeChars1, 1, static_cast<size_t>(filesize), fileFD);
        fclose(fileFD);
    }
    else
    {
        Print_Location(LOG_ID, ERROR, " %s", "read failed");
        fclose(fileFD);
        delete[] writeChars1;
        return false;
    }

    auto result = OpenFileStorage__NeuSAR_VirtualTesterAPP_PRPort_FS;
    if (!result)
    {
        Print_Location(LOG_ID, ERROR, " %s", " openfs failed ");
        delete[] writeChars1;
        return false;
    }
    else
    {
        Print_Location(LOG_ID, INFO, " %s", " openfs success ");
    }
    auto db = std::move(result).Value();

    auto openRst = db->OpenFileReadWrite(SO_FILE_NAME, ara::per::BasicOperations::OpenMode::kTruncate);
    if (!openRst)
    {
        Print_Location(LOG_ID, ERROR, " %s", " OpenFileReadWrite 1st failed");
        auto resetRet = db->ResetFile(SO_FILE_NAME);
        if (!resetRet.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " resetRet libsa.so failed.");
            delete[] writeChars1;
            return false;
        }
        else
        {
            Print_Location(LOG_ID, INFO, "%s", " resetRet libsa.so success.");
        }
        openRst = db->OpenFileReadWrite(SO_FILE_NAME, ara::per::BasicOperations::OpenMode::kTruncate);
        if (!openRst.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " open  libsa.so 2nd failed.");
            delete[] writeChars1;
            return false;
        }
    }
    auto db1 = std::move(openRst).Value();

    // auto writeret = db1->write(ara::core::Span<char>(writeChars1, strlen(writeChars1)));
    auto writeret = db1->write(ara::core::Span<char>(writeChars1, filesize));
    delete[] writeChars1;

    if (writeret!=filesize)
    {
        LOGERROR << "write so failed.");
        return false;
    }
    else
    {
        Print_Location(LOG_ID, INFO, " %s", " write so success");
    }
    db1->flush();
    auto sync_result = db1->fsync();
    if (!sync_result.HasValue()/* condition */)
    {
       Print_Location(LOG_ID, ERROR, "%s", "so Write fsync failed.");
       return false;
    }
    else
    {
        Print_Location(LOG_ID, INFO, "%s", "so Write Success.");
    }

    return true;
}
#endif

//调用存储中间件解密读取公钥
/*
bool DiagFile::PublickeyRead(std::string &Public_key)
{
    auto result = OpenFileStorage__NeuSAR_VirtualTesterAPP_PRPort_FS;
    if (!result)
    {
        Print_Location(LOG_ID, ERROR, "%s", "openfs failed");
        // std::cout << "openfs failed " << std::endl;
        return false;
    }

    auto db = std::move(result).Value();
    auto openRst = db->OpenFileReadWrite(KEY_FILE_NAME);
    if (!openRst)
    {
        Print_Location(LOG_ID, INFO, "%s", "OpenFileReadWrite failed");
        auto resetRet = db->ResetFile(KEY_FILE_NAME);
        if (!resetRet.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " resetRet publickey.txt failed.");
            return false;
        }
        else
        {
            Print_Location(LOG_ID, INFO, "%s", " resetRet publickey.txt success.");
        }
        openRst = db->OpenFileReadWrite(KEY_FILE_NAME);
        if (!openRst.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " open publickey.txt 2nd failed.");
            return false;
        }
    }
    auto db1 = std::move(openRst).Value();
    db1->seek(0, ara::per::BasicOperations::SeekDirection::kEnd);
    auto keysize = db1->tell();
    Print_Location(LOG_ID, INFO, "filesize %d", keysize);

    db1->seek(0);
    char *readch = new char[static_cast<size_t>(keysize) + 1]{0};
    db1->read(ara::core::Span<char>(readch, keysize));
    Print_Location(LOG_ID, INFO, "publickey %s", readch);

    Public_key = readch;
    // cout<<Public_key<<endl;
    delete[] readch;
    return true;
}
*/

bool DiagFile::PublickeyRead(std::string &publickey)
{
    std::string crypto_public_key;
    std::ifstream public_key_file;
    std::stringstream ioss;
    std::string s_temp;
    std::string key;
    char temp;
    int index = 0;

    if(access(PUBLIC_KEY_PATH,0) != 0) {
        LOGERROR << "DiagFile::PublickeyRead: public key file is not exist.";
        return false;
    }
    public_key_file.open(PUBLIC_KEY_PATH, std::ios::binary | std::ios::in);
    if(public_key_file.is_open()){
        while(public_key_file.get(temp)){
            // crypto_public_key.push_back((uint8_t)temp);
            crypto_public_key += temp;
            index++;
        }
        LOGINFO << "DiagFile::PublickeyRead: encrypto public key size: " << index << " bytes";
        public_key_file.close();
    }
    else {
        LOGERROR << "DiagFile::PublickeyRead: can not open public key file.";
        return false;  
    }
    key = DiagFile::ConvertKey(this->aes_key);
    auto publickey_result = DiagFile::DecryptFileByAES(crypto_public_key, key);
    if (!publickey_result.has_value()) 
    {
        LOGERROR << "DiagFile::PublickeyRead: decrypt file by aes failed.";
        return false;
    }
    publickey = publickey_result.value();
    LOGINFO << "DiagFile::PublickeyRead: get public key: " << publickey.c_str();
    return true;
}

//调用存储中间件加密存储公钥
/*
bool DiagFile::PublickeyWrite(std::string publickey)
{
    int keysize = static_cast<int>(strlen(publickey.c_str()));
    if (keysize <= 0)
    {
        return false;
    }

    char *writeChars1 = new char[static_cast<size_t>(keysize + 1)]{0};
    memcpy(writeChars1, publickey.c_str(), strlen(publickey.c_str()));
    auto result = OpenFileStorage__NeuSAR_VirtualTesterAPP_PRPort_FS;
    if (!result)
    {
        Print_Location(LOG_ID, ERROR, "%s", "openfs failed");
        //cout << "openfs failed." << std::endl;
        delete[] writeChars1;
        return false;
    }
    else
    {
        Print_Location(LOG_ID, INFO, "%s", "openfs success");
        //cout << "openfs success." << std::endl;
    }

    auto db = std::move(result).Value();
    auto openRst = db->OpenFileReadWrite(KEY_FILE_NAME, ara::per::BasicOperations::OpenMode::kTruncate);
    if (!openRst)
    {
        Print_Location(LOG_ID, ERROR, "%s", "open publickey.txt 1st failed.");
        auto resetRet = db->ResetFile(KEY_FILE_NAME);
        if (!resetRet.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " resetRet publickey.txt failed.");
            delete[] writeChars1;
            return false;
        }
        else
        {
            Print_Location(LOG_ID, INFO, "%s", " resetRet publickey.txt success.");
        }

        openRst = db->OpenFileReadWrite(KEY_FILE_NAME, ara::per::BasicOperations::OpenMode::kTruncate);
        if (!openRst.HasValue())
        {
            Print_Location(LOG_ID, ERROR, "%s", " open publickey.txt 2nd failed.");
            delete[] writeChars1;
            return false;
        }
    }

    auto db1 = std::move(openRst).Value();
    auto writeret = db1->write(ara::core::Span<char>(writeChars1, static_cast<int>(strlen(writeChars1))));
    delete[] writeChars1;

    if(writeret!=keysize)
    {
        Print_Location(LOG_ID, ERROR, "write key failed writeret = %d .",writeret);
        //cout << "write failed." << writeret << endl;
        return false;
    }
    else
    {
        Print_Location(LOG_ID, INFO, "%s", "write key success.");
        //cout << "write success size." << writeret << endl;
    }
    db1->flush();
    auto sync_result = db1->fsync();
    if (!sync_result.HasValue())
    {
        Print_Location(LOG_ID, ERROR, "%s", "key write fsync failed.");
        //std::cout << "File Write failed " << pos<< std::endl;
        return false;
    }
    else 
    {
        Print_Location(LOG_ID, INFO, "%s", "key Write Success.");
    }
    return true;
}
*/

bool DiagFile::PublickeyWrite(std::string publickey)
{
    std::vector<uint8_t> public_key_byte;
    std::vector<uint8_t> crypto_public_key;
    std::ofstream public_key_file;
    std::string key;
    int index = 0;

    if(publickey.size() == 0){
        LOGERROR << "DiagFile::PublickeyWrite: public key size is 0.";
        return false;
    }
    else{
        LOGINFO << "DiagFile::PublickeyWrite: get public key: " << publickey.c_str();
    }

    for(auto iter : publickey){
        public_key_byte.push_back((uint8_t)iter);
    }
    key = DiagFile::ConvertKey(this->aes_key);
    crypto_public_key = DiagFile::CryptFileByAES(public_key_byte, key);

    public_key_file.open(PUBLIC_KEY_PATH);
    if(public_key_file.is_open()){
        for(auto iter : crypto_public_key){
            public_key_file << iter;
            index++;
        }
        public_key_file.close();
    }
    else{
        LOGERROR << "DiagFile::PublickeyWrite: can not open public key file.";
        return false;
    }

    LOGINFO << "DiagFile::PublickeyWrite: write public key successful,size: " << index << " bytes";
    return true;
}

long int DiagFile::FileSize(const char *fname)
{
    long int filesize = -1;
    struct stat statbuf;
    if (stat(fname, &statbuf) == 0)
    {
        filesize = statbuf.st_size;
    }
    return filesize;
}

long DiagFile::GetFileSize(const std::string& filePath) {
    long size = 0;
    FILE *fp = fopen(filePath.c_str(), "rb");
    if (fp != NULL) {
        fpos_t fpos;
        fgetpos(fp, &fpos);
        size = fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fsetpos(fp, &fpos);
        fclose(fp);
        fp = NULL;
    }
    return size;
}

size_t DiagFile::GetFloderSize(const std::string &folder)
{
    DIR *dir = nullptr;
    size_t total_size = 0;
    struct dirent *file;
    // std::string file_name = "";

    if (folder.length() <= 0) {
        return 0;
    }

    dir = opendir(folder.c_str());
    if (dir == nullptr) {
        return 0;
    }

    while ((file = readdir(dir)) != nullptr) {
        // 过滤"." 和 ".."
        std::string file_name(file->d_name);
        if ((file_name.compare(".") == 0) ||
            (file_name.compare("..") == 0))
            continue;

        // 判断是否是目录
        if (file->d_type == DT_DIR) {
            std::string chdir(folder);
            chdir += "/";
            chdir += file_name;
            size_t ret = GetFloderSize(chdir);
            total_size += ret;
        } else {
            std::string file_path(folder);
            file_path += "/";
            file_path += file->d_name;
            size_t file_size = GetFileSize(file_path);
            total_size += file_size;
        }
    }
 
    closedir(dir);
    return total_size;
}

std::string DiagFile::GetBigDataFormatFileName() 
{
    std::string format_name = "DRST_";
    std::string timestamp = "";
    std::string count_num = "";
    std::string year = "00";
    std::string month = "00";
    std::string day = "00";
    std::string hour = "00";
    std::string min = "00";
    std::string sec = "00";
    std::stringstream ss;
    time_t current_time;

    // 设置宽度为5，填充字符为'0'，以右对齐的方式进行格式化输出
    ss << std::setfill('0') << std::setw(5) << result_file_num;
    count_num = ss.str();
    result_file_num++;
    if(result_file_num > int(99999)){
        result_file_num = 1;
    }

    time(&current_time);
    tm *tm = localtime(&current_time);
    year = std::to_string(1900 + tm->tm_year);
    if((1 + tm->tm_mon) < 10){
        month = "0" + std::to_string(1 + tm->tm_mon);
    }
    else{
        month = std::to_string(1 + tm->tm_mon);
    }
    if(tm->tm_mday < 10){
        day = "0" + std::to_string(tm->tm_mday);
    }
    else{
        day = std::to_string(tm->tm_mday);
    }
    if(tm->tm_hour < 10){
        hour = "0" + std::to_string(tm->tm_hour);
    }
    else{
        hour = std::to_string(tm->tm_hour);
    }
    if(tm->tm_min < 10){
        min = "0" + std::to_string(tm->tm_min);
    }
    else{
        min = std::to_string(tm->tm_min);
    }
    if(tm->tm_sec < 10){
        sec = "0" + std::to_string(tm->tm_sec);
    }
    else{
        sec = std::to_string(tm->tm_sec);
    }
    timestamp = "_" + year + month + day + "_" + hour + min + sec;

    format_name = format_name + count_num + timestamp + ".tar.gz";
    LOGINFO << "DiagFile::GetBigDataFormatFileName: get get big data format file name: " << format_name.c_str();

    return format_name;
}

std::string DiagFile::CompareFileGenTime(const std::string& file1, const std::string& file2)
{
    struct stat file_stat1, file_stat2;
    time_t time1, time2;

    if (stat(file1.c_str(), &file_stat1) == -1) {
        LOGERROR << "DiagFile::CompareFileGenTime: stat" << file1.c_str() << " error";
        return "";
    }
    if (stat(file2.c_str(), &file_stat2) == -1) {
        LOGERROR << "DiagFile::CompareFileGenTime: stat2 " << file2.c_str() << " error";
        return "";
    }

    time1 = file_stat1.st_ctime;
    time2 = file_stat2.st_ctime;

    if(time1 < time2){
        return file1;
    }
    return file2;
}

bool DiagFile::RenameFile(const std::string& oldName, const std::string& newName)
{
    int rename_result = -1;

    if(access(oldName.c_str(),F_OK) != 0){
        LOGERROR << "DiagFile::RenameFile: original file does not exist.";
        return false;
    }
    
    rename_result = rename(oldName.c_str(), newName.c_str());
    if(rename_result == int(0)){
        return true;
    }
    else {
        LOGERROR << "DiagFile::RenameFile: rename failed,rename result: " << rename_result;
        return false;
    }
}

void DiagFile::ProcessFile2BigDataFolder(const std::string script_result_path, const std::string packageName)
{
    // SystemCmd file_opreation_instance;
    std::string source_path = script_result_path + "DiagResult.tar.gz";
    std::string des_path = packageName + "/DiagResult.tar.gz";
    std::string new_result_file_name = "";
    int copy_file_result = -1;
    bool rename_result = false;
    size_t floder_size = 0;
    size_t delete_file_time = 0;

    floder_size = this->GetFloderSize(BIG_DATA_FLODER);
    LOGINFO << "DiagFile::ProcessFile2BigDataFolder: /data/DiagnosticScript/BackupResult/ size: " << floder_size;

    //文件夹大于30M，需要进行文件滚存操作，删除最早生成的文件
    while(this->GetFloderSize(BIG_DATA_FLODER) > size_t(31000000)){
        std::string earliest_file = "";
        std::string file_name = "";
        std::string file_path = "";
        struct dirent* entry;
        DIR* dir;

        delete_file_time++;
        if(delete_file_time > 30){
            LOGERROR << "DiagFile::ProcessFile2BigDataFolder: delete /data/DiagnosticScript/BackupResult/ files too many times,quit.";
            break;
        }
        LOGINFO << "DiagFile::ProcessFile2BigDataFolder: the size of /data/DiagnosticScript/BackupResult/ is over 30m,deleting earliest result file...";

        dir = opendir(BIG_DATA_FLODER);
        if (dir == nullptr) {
            LOGERROR << "DiagFile::ProcessFile2BigDataFolder: can not open /data/DiagnosticScript/BackupResult/";
            return;
        }

        while ((entry = readdir(dir))!= nullptr) {
            file_name = std::string(entry->d_name);
            if (file_name == "." || file_name == "..") {
                continue;
            }

            file_path = BIG_DATA_FLODER + file_name;
            if (earliest_file.empty()) {
                earliest_file = file_path;
            }
            else{
                earliest_file = this->CompareFileGenTime(earliest_file, file_path);
            }
        }
        closedir(dir);

        if(earliest_file.empty()){
            LOGERROR << "DiagFile::ProcessFile2BigDataFolder: can not get earliest file.";
            return;
        } 
        else{
            size_t pos = earliest_file.find_last_of('/');
            LOGINFO << "DiagFile::ProcessFile2BigDataFolder: can not get earliest file: " << earliest_file.substr(pos + 1).c_str() << " , delete.";
        }

        file_opreation_instance.deleteFile(earliest_file);
    }

    //复制文件到大数据文件夹下
    copy_file_result = file_opreation_instance.copy_file(source_path, BIG_DATA_FLODER + packageName);
    if(copy_file_result == 0){
        LOGINFO << "DiagFile::ProcessFile2BigDataFolder: copy result file to big data floder successful.";
    }
    else{
        LOGERROR << "DiagFile::ProcessFile2BigDataFolder: copy result file to big data floder failed.";
        return;
    }

    //重命名在大数据目录下的结果文件
    des_path = BIG_DATA_FLODER + des_path;
    if(access(des_path.c_str(),F_OK) != 0) {
        LOGERROR << "DiagFile::ProcessFile2BigDataFolder: the new result file can not be found under the Big Data folder.";
        return;
    }

    new_result_file_name = this->GetBigDataFormatFileName();
    rename_result = this->RenameFile(des_path, BIG_DATA_FLODER + packageName + "/" + new_result_file_name);
    if(!rename_result){
        LOGERROR << "DiagFile::ProcessFile2BigDataFolder: can not rename /data/DiagnosticScript/BackupResult/DiagResult.tar.gz file.";
        return;
    }

    LOGINFO << "DiagFile::ProcessFile2BigDataFolder: process diag result file to big data folder successful.";
}

std::vector<uint8_t> DiagFile::DecryptCMACKeyFile(const std::vector<uint8_t>& file, bool& decryptResult)
{
    LOGINFO << "DiagFile::DecryptCMACKeyFile: file size: " << file.size();
    LOGINFO << "DiagFile::DecryptCMACKeyFile: decryptResult: " << decryptResult;

    std::vector<uint8_t> file_merge;
#if 0
    StreamCipherCtx::Uptr cipherCtx = nullptr;
    // GetCryptoProvider();
    LOGINFO << "DiagFile::DecryptCMACKeyFile: file size: " << file.size();
    decryptResult = false;

    CryptoServicesWrapper crypto_service_wrapper;
    size_t buffersize;
    size_t restSize = 0, outSize = 0, index = 0, processTimes = 0;
    std::vector<uint8_t> symmetric_key_vec;
    std::vector<uint8_t> iv;
    ReadOnlyMemRegion rbuff4, rbuff3;
    WritableMemRegion wbuff4, wbuff3;

    if (file.size() <= maxblock)
    {
        restSize = file.size();
        processTimes = 0;
        buffersize = maxblock * 2;
    }
    else
    {
        processTimes = (file.size() / maxblock);
        restSize = file.size() % maxblock;
        buffersize = file.size() * 2;
    }

    // // symmetric_key_vec.assign(cmac_file_key.begin(), cmac_file_key.end());
    // for(auto iter : cmac_file_key){
    //     symmetric_key_vec.push_back(iter ^ 0xff);
    // }
    // symmetric_key_vec = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
    // iv.assign(cmac_file_iv.begin(), cmac_file_iv.end());

    for(auto iter : cmac_file_key_h){
        symmetric_key_vec.push_back(iter ^ 0xff);
    }
    for(auto iter : cmac_file_key_l){
        symmetric_key_vec.push_back(iter ^ 0xff);
    }
    iv.assign(cmac_file_iv.begin(), cmac_file_iv.end());
    cipherCtx = crypto_service_wrapper.SymmetricStreamInitKey(symmetric_key_vec, CryptoAlgoNid::kNID_aes_128_cbc, iv, false);

    std::vector<uint8_t> buffer2(buffersize);
    if (cipherCtx == nullptr)
    {
        LOGERROR << "DiagFile::DecryptCMACKeyFile: create stream cipher failed.";
        goto RESULT;
    }

    for (size_t i = 0; i < processTimes; i++)
    {
        LOGINFO << "DiagFile::DecryptCMACKeyFile: out size: " << outSize << " index: " << index;
        if ((restSize == 0) && (processTimes == 1))
        {
            rbuff3 = ara::core::MakeSpan(file.data(), static_cast<int>(file.size()));
            wbuff3 = ara::core::MakeSpan(buffer2); //+32 是因为数据加密后会增大至少16
        }
        else
        {
            rbuff3 = ara::core::MakeSpan(file.data() + i * maxblock, static_cast<int>(maxblock));
            //数据解密后会减少，此处不在额外扩充wbuff大小
            wbuff3 = ara::core::MakeSpan(buffer2.data() + index, static_cast<int>(maxblock));
        }
        ara::core::Result<size_t> szrst3 = cipherCtx->ProcessBytes(wbuff3, rbuff3);
        if (szrst3)
        {
            outSize = (std::move(szrst3)).Value();
            LOGINFO << "DiagFile::DecryptCMACKeyFile: out size: " << outSize << " index: " << index;

            for (size_t j = 0; j < outSize; j++)
            {
                if (((j + 1) % 128) == 0)
                {
                    //std::cout << std::endl;
                }
                file_merge.push_back(buffer2[index + j]);
            }
            index += outSize;
            LOGINFO << "DiagFile::DecryptCMACKeyFile: decrypt cmac key file succeessful.";
        }
        else
        {
            LOGERROR << "DiagFile::DecryptCMACKeyFile: decrypt cmac key failed.";
            goto RESULT;
        }
    }

    if (restSize >= 0)
    {
        rbuff4 = ara::core::MakeSpan(file.data() + processTimes * maxblock, static_cast<int>(restSize));
        wbuff4 = ara::core::MakeSpan(buffer2.data() + index, static_cast<int>(buffer2.size() - index));
        ara::core::Result<size_t> szrst4 = cipherCtx->FinishBytes(wbuff4, rbuff4);
        if (szrst4)
        {
            restSize = (std::move(szrst4)).Value();
            LOGINFO << "DiagFile::DecryptCMACKeyFile: index: " << index << " rest size: " << restSize;
            for (size_t i = 0; i < restSize; i++)
            {
                //std::cout << hex << setfill('0') << setw(2) << (unsigned int)buffer2[index + i];
                // if (((i + 1) % 128) == 0)
                // {
                //     //std::cout << std::endl;
                // }
                file_merge.push_back(buffer2[index + i]);
            }
            // std::cout << std::endl;
            LOGINFO << "DiagFile::DecryptCMACKeyFile: last part decrypt successful.";
        }
        else
        {
            LOGERROR << "DiagFile::DecryptCMACKeyFile: last part decrypt failed.";
            goto RESULT;
        }
    }

    decryptResult = true;

RESULT:
    if (cipherCtx != nullptr)
    {
        cipherCtx.reset();
    }
#endif
    return file_merge;
}

std::vector<uint8_t> DiagFile::CalculateCMACValue(const std::vector<uint8_t>& context, const std::string& key, bool& result)
{
    std::vector<uint8_t> cmac_value(16,0);
    std::vector<uint8_t> key_context;
    std::vector<uint8_t> iv(16,0);
    int generate_result = -1;
    size_t pos = 0;
    std::string cmac_key = key;
    std::string target1 = "0x";
    std::string target2 = ",";
    std::string upper_half_byte;
    std::string lower_half_byte;
    std::string byte;

    result = false;

    while ((pos = cmac_key.find(target1,pos)) != std::string::npos) {
        cmac_key.erase(pos,target1.length());
    }
    for (std::string::iterator it = cmac_key.begin(); it != cmac_key.end(); it++) {
        if (*it == ',') {
            cmac_key.erase(it);
        }
    }
    if(cmac_key.length() != 32) {
        LOGERROR << "DiagFile::CalculateCMACValue: length of cmac key string is not 32,length: " << key.length();
        return cmac_value;
    }
    LOGINFO << "DiagFile::CalculateCMACValue: cmac key string : " << cmac_key.c_str();


    for (uint i = 0; i < (cmac_key.length() - 1); i = i + 2) {
        upper_half_byte = cmac_key[i];
        lower_half_byte = cmac_key[static_cast<uint>(i + 1)];
        byte = upper_half_byte + lower_half_byte;
        key_context.push_back((uint8_t)stoi(byte,0,16));
    }
#if 0
    /* kevin 加密 */
    generate_result = crypto_service_wrapper.GenerateCmacValue(CryptoAlgoNid::kNID_cmac_aes_128_cbc, 
                                                                context, cmac_value, key_context, iv);
#else
    generate_result = 0;
#endif
    if(generate_result == 0) {
        result = true;
        LOGINFO << "DiagFile::CalculateCMACValue: calculate CMAC value successful.";
    }
    else {
        LOGERROR << "DiagFile::CalculateCMACValue: calculate CMAC value failed.";
    }

    return cmac_value;
}
