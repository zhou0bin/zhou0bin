#ifndef VD_SYSTEM_CMD_H_
#define VD_SYSTEM_CMD_H_

#include <stdint.h>  // included for  uint8_t ..etc
#include <stdbool.h> // included for  bool
#include <stddef.h>  // included for NULL definition
#include <string.h>  // included for memset/memcpy definition
#include <stdio.h>   // included for printf definition
#include <stdlib.h>  // exit function
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <net/if.h>

#include <iostream>
#include <thread>
#include <fstream>
#include <sys/stat.h>
#include <vector>

#include <dirent.h>

#include <zlib.h>
#include <libtar.h>
#include <fcntl.h>
#include <regex>

#define FILE_PATH_LEN 1024
#define BUFFER_LENGTH 8192

class SystemCmd
{
public:
    SystemCmd() = default;
    
    ~SystemCmd() = default;

    int comparss_dir(char *srcDir, char *tarFile, bool isFile);
    int uncomparss_dir(char *srcDir, char *tarFile);
    int copy_file(std::string sourcefile, std::string destPath);
    int copy_dir(std::string sourcePath, std::string destPath);

    int deleteDir(std::string path, bool clear_all);
    int DeleteDir(std::string path);
    std::vector<std::string> getfiles(std::string strCurrentDir, std::vector<std::string> &strVecFile, std::vector<std::string> &strVecDir);

    int copyAllOfFolder(std::string sourceFloder, std::string destPath);
    int deleteFile(std::string file);
    int tarFolder(std::string folderPath, std::string tarPath);
    int tarFile(std::string path, std::string tarName);
    int getNextName(std::string path, std::vector<std::string> &strVecFile, std::vector<std::string> &strVecDir);
    int Get_all_files_by_wildcard(std::string path, const std::string& wildcard, std::vector<std::string> &files);

private:
    int CopyFolder(std::string, std::string);
    int CopyFile(std::string, std::string);
    int IsDir(std::string);
};

#endif // VD_SYSTEM_CMD_H_