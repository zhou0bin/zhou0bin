/*****************************************************************************
* @file     file_adapter.h
* @brief    
* @author   kevin
* @date     2025/4/23
*****************************************************************************/
#ifndef VD_FILE_ADAPTER_H_
#define VD_FILE_ADAPTER_H_

#include <string.h>
#include <stdio.h>

#include "common/log.h"

#ifdef HTTP_WRAPPER
#include "http_wrapper/file_operate_interface.h"
#include "http_wrapper/httpinterface.h"
using namespace middleware::vcproxyservicewapper;

namespace asf {
namespace vdi {
/*kevin http*/

class FileAdapter : public FileOperateInterface
{
private:
    /* data */
    std::string _file_path;
    FILE * _fp;
public:
    FileAdapter(/* args */)
    {
         _fp = nullptr;
    };
    ~FileAdapter() noexcept
    {
         _fp = nullptr;
    };

void SetFilePath(const std::string path){
    _file_path = path;
}

/**
 * @brief 打开文件
 * @param  mode             打开文件方式，可参照"C标准"
 * @return true
 * @return false
 */

bool FopenImpl(char *mode) override {
    if(_fp == nullptr){
        _fp = fopen(_file_path.c_str(),mode);
        LOGINFO << "FopenImpl: _file_path " << _file_path.c_str();
        if(_fp == nullptr){
             LOGERROR << "fopen result NULL.";
            return false;
        }
        return true;
    } else {
        LOGERROR << "_fp is not NULL.";
    }            
    return false;
}

/**
 * @brief 关闭文件
 * @return int              0 成功 -1失败
 */
int CloseImpl() override {
    int ret = -1;
    if(_fp != nullptr ){
        ret = fclose(_fp);
        _fp = nullptr;
    }
    return ret;
}

/**
 * @brief 文件写入
 * @param  ptr              指向被写入原素数组的指针
 * @param  size             被写入元素大小，以字节为单位
 * @param  nMember          元素的个数，每个元素大小为size字节
 * @return size_t           返回写入原素大小
 */
size_t FwriteImpl(void *data, size_t size, size_t nMember) override {
    size_t ret = -1;

    if(_fp != nullptr){
        ret = fwrite(data, size, nMember, _fp);
        fflush(_fp);
    } else {
        FopenImpl((char*)"a+");
        if(_fp != nullptr){
            ret = fwrite(data, size, nMember, _fp);
            fflush(_fp);
        }
        CloseImpl();
    }
    return ret;
}

/**
 * @brief  文件读取
 * @param  data             指向读取数组的首个元素的指针
 * @param  size             每个元素的大小,以字节为单位
 * @param  nMember          读取元素的个数
 * @return size_t           返回读取原素大小
 */
size_t FreadImpl(void *data, size_t size, size_t nMember) override {
    size_t ret = -1;
    LOGINFO << "FreadImpl: FreadImpl";
    if(_fp != nullptr){
        ret = fread(data, size, nMember, _fp);
    } else {
        FopenImpl((char*)"r");
        if(_fp != nullptr){
            ret = fread(data, size, nMember, _fp);
        }
        CloseImpl();
    }
    return ret;
}
/**
 * @brief 设置文件指针的流位置
 * @param  off              为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param  whenceT          设定从文件哪里开始偏移 0：文件开头  1：当前位置  2 文件结尾
 * @return size_t           成功返回0，失败返回非0值，可以用perror()输出的错误
 */
int FseekImpl(long off, int whenceT) override {
    LOGINFO << "FseekImpl: FseekImpl";
    int ret = -1;
    if(_fp != nullptr){
        ret =  fseek(_fp,off,whenceT);
    } else {
        FopenImpl((char*)"r");
        if(_fp != nullptr){
            ret =  fseek(_fp,off,whenceT);
        }
        CloseImpl();
    }
    return ret;
}
/**
 * @brief 获取本地文件大小
 * @return long             文件大小
 */
long GetLocaFileLenthImpl() override{
    LOGINFO << "GetLocaFileLenthImpl.";
    long length = 0;
    fpos_t fpos;
    if(_fp != nullptr){
        fgetpos(_fp,& fpos);
        length = fseek(_fp,0L,SEEK_END);
        length = ftell(_fp);
        fsetpos(_fp,&fpos);
    } else {
        FopenImpl((char*)"r");
        if(_fp != nullptr){
            fgetpos(_fp,& fpos);
            length = fseek(_fp,0L,SEEK_END);
            length = ftell(_fp);
            fsetpos(_fp,&fpos);
        }
        CloseImpl();
    }
    return length;
}
/**
 * @brief 上传/下载进度回调方法
 * @param  type        类型  1： 下载  2：上传
 * @param  progress    上传/下载进度
 */
void ProgressHandler(int type, int progress) override
{
    LOGINFO << "ProgressHandler: type: " << type << "progress: " << progress;
    if(type == 1){
        // std::cout << "[FileAdapter::ProgressHandler]type:" << type << ", download" << " progress : " << progress << std::endl; 
    } else if(type == 2){
        // std::cout << "[FileAdapter::ProgressHandler]type:" << type << ", upload" << " progress : " << progress << std::endl;
    } else{
        // std::cout << "[FileAdapter::ProgressHandler]unknow type:" << type << std::endl;
    }
}

};

} // namespace vdi
} // namespace asf

#endif

#endif // VD_FILE_ADAPTER_H_
