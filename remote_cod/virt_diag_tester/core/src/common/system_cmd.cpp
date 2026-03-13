#include <algorithm>

#include "common/system_cmd.h"
#include "common/log.h"
// tarFile  -- like /home/user/test.tar
//参数srcDir为要打包压缩的某个目录，tarFile 为要打包的文件名(.tar 结尾) 生成的文件最后未.tar.gz

int SystemCmd::comparss_dir(char *srcDir, char *tarFile, bool isFile)
{
    //printf("\n2136  %s,%s \n", srcDir, tarFile);
    TAR *pTar;
    char extractTo[FILE_PATH_LEN] = {0};
    char desFile[FILE_PATH_LEN] = {0};
    char szBuf[10000] = {0};
    ssize_t read_len;
    FILE *tarFd;
    gzFile gzFd;
    memset(desFile, 0, sizeof(desFile));
    sprintf(desFile, "%s.gz", tarFile);
    if (isFile){
        //sprintf(extractTo, "%s", ".");
    }
    else{
        //sprintf(extractTo, "%s", srcDir);
    }

    if (tar_open(&pTar, tarFile, NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU) < 0){
        LOGWARN << "SystemCmd::comparss_dir: tar_open: " << tarFile << " error:" << strerror(errno);
        return -1;
    }
    if (tar_append_tree(pTar, srcDir, extractTo) < 0){
        tar_close(pTar);
        LOGWARN << "SystemCmd::comparss_dir: tar_append_tree: error:" << strerror(errno);
        return -1;
    }
    tar_close(pTar);

    tarFd = fopen(tarFile, "rb");
    if (tarFd == nullptr){
        LOGWARN << "SystemCmd::comparss_dir: fopen: " <<  tarFile << "error: " << strerror(errno); 
        return -1;
    }
    gzFd = gzopen(desFile, "wb");
    if (gzFd == nullptr){
        fclose(tarFd);
        remove(tarFile);
        LOGWARN << "SystemCmd::comparss_dir: gzopen: " <<  tarFile << "error: " << strerror(errno); 
        return -1;
    }
    while ((read_len = fread(szBuf, 1, 10000, tarFd)) > 0){
        gzwrite(gzFd, szBuf, static_cast<size_t>(read_len));
    }
    gzclose(gzFd);
    fclose(tarFd);
    remove(tarFile);
    return 0;
}

// 解压缩和压缩过程相反，先解压，再解包
// uncomparss to srcDir
int SystemCmd::uncomparss_dir(char *srcDir, char *tarFile)
{
    if ((srcDir == nullptr) || (tarFile == nullptr))
    {
        LOGWARN << "SystemCmd::uncomparss_dir: error,input value is nullptr.";
        return -1;
    }
    
    gzFile gzFd;
    char szTmpFile[FILE_PATH_LEN] = {0};
    FILE *fp;
    int nReadLen = 0;
    char szBuf[10000] = {0};
    TAR *pTar;
    if (strstr(tarFile, "tar.gz") == nullptr){
        LOGWARN << "SystemCmd::uncomparss_dir: file: " << tarFile << "is not end with .tar.gz"; 
        return -1;
    }
    if (access(tarFile, F_OK) < 0){
        LOGWARN << "SystemCmd::uncomparss_dir: not find file: " << tarFile; 
        return -1;
    }
    gzFd = gzopen(tarFile, "rb");
    if (gzFd == nullptr){
        LOGWARN << "SystemCmd::uncomparss_dir: gzopen file: " << tarFile << "err: " << strerror(errno); 
        return -1;
    }
    memset(szTmpFile, 0, sizeof(szTmpFile));
    // memcpy(szTmpFile, tarFile, strlen(tarFile) - 3); // remove .gz
    if(tarFile==nullptr)
    {
        LOGINFO << "SystemCmd::uncomparss_dir: " << tarFile << "tarFile is nullptr";
        return -1;
    }
    (void)strncpy(&szTmpFile[0], static_cast<const char*>(tarFile), 1024-1);
    if(strlen(szTmpFile)>3)
    {
        szTmpFile[strlen(szTmpFile)-1]=(char)0;
        szTmpFile[strlen(szTmpFile)-2]=(char)0;
        szTmpFile[strlen(szTmpFile)-3]=(char)0;
    }

    fp = fopen(szTmpFile, "wb");
    if (fp == nullptr){
        gzclose(gzFd);
        //printf("open file[%s] err[%s]", szTmpFile, strerror(errno));
        LOGWARN << "SystemCmd::uncomparss_dir: open file: " << szTmpFile << " err " << strerror(errno);
        return -1;
    }
    while ((nReadLen = gzread(gzFd, szBuf, 10000)) > 0){
        fwrite(szBuf, static_cast<size_t>(nReadLen), 1, fp);
    }

    gzclose(gzFd);
    fclose(fp);
    if (tar_open(&pTar, szTmpFile, nullptr, O_RDONLY, 0644, TAR_GNU) < 0){
        unlink(szTmpFile);
        //printf("tar_open[%s] error[%s]", szTmpFile, strerror(errno));
        LOGWARN << "SystemCmd::uncomparss_dir: ar_open: " << szTmpFile << " error: " << strerror(errno); 
        return -1;
    }
    if (tar_extract_all(pTar, srcDir) < 0){
        tar_close(pTar);
        unlink(szTmpFile);
        //printf("tar_extract_all error[%s]", strerror(errno));
        LOGWARN << "SystemCmd::uncomparss_dir: tar_extract_all error: " << strerror(errno);
        return -1;
    }
    tar_close(pTar);
    unlink(szTmpFile);
    return 0;
}

int SystemCmd::IsDir(std::string path)
{
    if (path.empty()){
        return 0;
    }
    struct stat st;
    if (0 != stat(path.c_str(), &st)){
        return 0;
    }
    if (S_ISDIR(st.st_mode)){
        return 1;
    }
    else{
        return 0;
    }
}

int SystemCmd::CopyFile(std::string sourcePath, std::string destPath)
{
    int len = 0;
    FILE *pIn = nullptr;
    FILE *pOut = nullptr;
    char buff[BUFFER_LENGTH] = {0};
     pIn = fopen(sourcePath.c_str(), "r");
    if (pIn == nullptr){
        LOGWARN << "Open File " << sourcePath.c_str() << "Failed...";
        return -1;
    }
    else{
        printf("\nOpen source %s suc\n", sourcePath.c_str());
    }
    pOut = fopen(destPath.c_str(), "w");
    if (pOut == nullptr){
        LOGWARN << "Create Dest File " << destPath.c_str() << "Failed...";
        fclose(pIn);
        return -1;
    }
    else{
        //printf("\nOpen dest %s suc\n", destPath.c_str());
        LOGINFO << "open dest " << destPath.c_str() << " successful.";
    }
    while ((len = fread(buff, sizeof(char), sizeof(buff), pIn)) > 0){
        fwrite(buff, sizeof(char), static_cast<size_t> (len), pOut);
    }
    fclose(pOut);
    fclose(pIn);
    return 0;
}

int SystemCmd::CopyFolder(std::string sourcePath, std::string destPath)
{
    struct dirent *filename = nullptr;
    DIR *destpathdir = opendir(destPath.c_str());
    if (destpathdir == nullptr){
        if (mkdir(destPath.c_str(), 0777)){
            LOGWARN << "SystemCmd::CopyFolder: Create Dir Failed";
              if (destpathdir /* condition */)
            {
                closedir(destpathdir);
            }
            return -1;
        }
        else{
            LOGINFO << "SystemCmd::CopyFolder: Creaat Dir " << destPath.c_str() << " Successfl.";
        }
    }
    if (destpathdir)
    {
        closedir(destpathdir);
    }
    std::string path = sourcePath;
    if (sourcePath.back() != '/'){
        sourcePath += "/";
    }
    if (destPath.back() != '/'){
        destPath += "/";
    }
    DIR *dp = opendir(path.c_str());
    filename = readdir(dp);
    while (filename!=nullptr){
        std::string fileSourceFath = sourcePath;
        std::string fileDestPath = destPath;
        fileSourceFath += filename->d_name;
        fileDestPath += filename->d_name;
        if (IsDir(fileSourceFath.c_str())){
            if (strncmp(filename->d_name, ".", 1) && strncmp(filename->d_name, "..", 2)){
                CopyFolder(fileSourceFath, fileDestPath);
            }
        }
        else{
            CopyFile(fileSourceFath, fileDestPath);
            LOGINFO << "Copy From " <<  fileSourceFath.c_str() << "To " << fileDestPath.c_str() << " Successful.";
        }
    filename=readdir(dp);
    }
     if (dp)
    {
       closedir(dp);
    }
    return 0;
}

int SystemCmd::copy_file(std::string sourcefile, std::string destPath)
{
    LOGINFO << "SystemCmd::copy_file: sourcefile: " << sourcefile.c_str() << "destPath: " << destPath.c_str();

    DIR *dest = opendir(destPath.c_str());
    if (dest==nullptr){
        if (mkdir(destPath.c_str(), 0777)){
            printf("Create Dir Failed...\n");
            if (dest){
                closedir(dest);
            }

            return -1;
        }
        else{
            printf("Creaat Dir %s Successed...\n", destPath.c_str());
        }
    }
    if (dest){
        closedir(dest);
    }
    auto pos = sourcefile.find_last_of("/");
    std::string FileName = sourcefile.substr(pos + 1);
    if (destPath.back() != '/'){
        destPath += "/";
    }
    std::string destfile = destPath + FileName;

    if (CopyFile(sourcefile, destfile) != 0){
        LOGWARN << "SystemCmd::copy_file: Copy From " << sourcefile.c_str() << "To " << destfile.c_str() << " failed.";
        return -1;
    }
    else{
        LOGINFO << "SystemCmd::copy_file: Copy From " << sourcefile.c_str() << "To " << destfile.c_str() << " Successful.";
    }

    return 0;
}

int SystemCmd::copy_dir(std::string sourcePath, std::string destPath)
{
    DIR *source = opendir(sourcePath.c_str());
    DIR *destination = opendir(destPath.c_str());
    if (!source){
        //printf("Source Dir Path Is Not Existed\n");
        
        return -1;
    }
    if (!destination){
        //printf("Destnation Dir Path Is Not Existed\n");
    }
    CopyFolder(sourcePath, destPath);

    closedir(source);
    closedir(destination);
    return 0;
}

int SystemCmd::copyAllOfFolder(std::string sourceFloder, std::string destPath)
{
    std::vector<std::string> content_name;
    std::string file_name = "";
    DIR* dir;
    struct dirent* dir_index;
    int copy_result = -1;

    if (sourceFloder.back() != '/'){
        sourceFloder += "/";
    }
    if (destPath.back() != '/'){
        destPath += "/";
    }

    dir = opendir(sourceFloder.c_str());
   
    while((dir_index = readdir(dir))!= nullptr){
        file_name = dir_index->d_name;
        if((strcmp(file_name.c_str(),".") != 0) && ( strcmp(file_name.c_str(),"..") != 0)){
            content_name.push_back((sourceFloder + file_name));
            //cout << file_name << endl;
        }
    }
    if (dir)
    {
        closedir(dir);
    }
    if(strcmp(file_name.c_str(),"") == 0){
        //cout << "There are no members in the folder." << endl;
        return -1;
    }

    for(auto iter : content_name){
        if(IsDir(iter)){
           copy_result = copy_dir(iter,destPath);
           if(copy_result == -1){
                return -1; 
           }
        }
        else{
            copy_result = copy_file(iter,destPath);
            if(copy_result == -1){
                return -1; 
            }
        }
    }

    return 0;
}

std::vector<std::string> SystemCmd::getfiles(std::string strCurrentDir, std::vector<std::string> &strVecFile, std::vector<std::string> &strVecDir)
{
    std::vector<std::string> vFiles; // 存放文件名们
    vFiles.clear();
    DIR *dir;
    struct dirent *pDir;
    struct stat st;
    dir = opendir(strCurrentDir.c_str());
    if(dir == nullptr){
        return vFiles;
    }

    strVecDir.push_back(strCurrentDir);
    while((pDir = readdir(dir))!= nullptr){
        if((strcmp(pDir->d_name,".")==0) || (strcmp(pDir->d_name,"..")==0)){
            continue;
        }
        else if(pDir->d_type == 8){ // 文件
            //vFiles.push_back(strCurrentDir + "/" + pDir->d_name);
            strVecFile.push_back(strCurrentDir + "/" + pDir->d_name);
        }
        else if(pDir->d_type == 10){
            continue;
        }
        else if(pDir->d_type == 4){ // 子目录
            std::string strNextdir = strCurrentDir + "/" + pDir->d_name;
            std::vector<std::string> ss = getfiles(strNextdir,strVecFile,strVecDir);
            strVecFile.insert(strVecFile.end(),ss.begin(),ss.end()); // 组合到一起。
        }
        else{
            // 当有的时候linux 不认识的文件类型是，需要手动调用stat来查看是否是文件类型。
            // 其实应该所有的判断都用stat来，我这里只不过是懒得弄了。
            std::string AbsolutePath = strCurrentDir + "/" + pDir->d_name;
            if(stat(AbsolutePath.c_str(),&st)!=-1){// 读取成功
                if((st.st_mode&S_IFMT)==S_IFREG) {//普通文件
                    strVecFile.push_back(strCurrentDir + "/" + pDir->d_name);
                }
            }
            else{
                //文件类型是0 的。这些得用stat显示查看。
                //printf("name : %s , type :%d \n", pDir->d_name, pDir->d_type);
                LOGINFO << "SystemCmd::getfiles: name : " <<  pDir->d_name << "type : " << pDir->d_type;
            }
        }
    }
    closedir(dir);
    return vFiles;
}

int SystemCmd::DeleteDir(std::string path)
{
    DIR *dir;
    struct dirent *pDir;

    //文件夹不存在默认删除成功
    //  if(access(path.c_str(),F_OK) <0);
    //  {
    //     //  return 0;
    //  }
    dir = opendir(path.c_str());
    if(dir== nullptr){
        //cout << "open dir Faild" << path.c_str()<<endl;
        return(-1);
    }

    int i=0;
    int ret=0;
    pDir = readdir(dir);
    while(pDir!= nullptr){
        if((strcmp(pDir->d_name,".")==0) || (strcmp(pDir->d_name,"..")==0)){
            i++;
        }
        else if(pDir->d_type == 8){ // 文件
            i++;
        }
        else if(pDir->d_type == 10){
            i++;
        }
        else if(pDir->d_type == 4){ // 子目录
            std::string nextPath=path+ "/" +pDir->d_name;
            int back=DeleteDir(nextPath);
            if(back==-1){
                i++;
            }
        }
        else{
            i++;
        }
    pDir = readdir(dir);
    }
    if (dir)
    {
       closedir(dir);
    }
    
    if(i==2){
        ret=rmdir(path.c_str());
        if(ret<0){
            ret=-1;
            //cout<<"delelte dir:"<<path<<" fail!"<<endl;
        }
    }
    return ret;
}
int SystemCmd::deleteDir(std::string path, bool clear_all=true)
{
    int ret = 0;
    std::vector<std::string> vecFile;
    std::vector<std::string> vecDir;
    vecDir.clear();
    vecFile.clear();

    std::vector<std::string> vecStr=getfiles(path,vecFile,vecDir);
    LOGINFO << "SystemCmd::deleteDir: path: " << path.c_str();
    if(vecFile.size())
    {
        for(auto n:vecFile)
        {
            ret=unlink(n.c_str());
            if(ret<0){
                LOGWARN << "SystemCmd::deleteDir: unlink ret: " << ret << " delete file" << n.c_str() << " failed!";
                return ret;
            }
        }
    }
    sort(vecDir.begin(),vecDir.end());

    if(clear_all){
        if(vecDir.size()){
            ret=DeleteDir(path);
        }
    }
    else{
        struct stat st;
        for(int i=vecDir.size()-1;i>0;i--){
            if(stat(vecDir[i].c_str(),&st)!=-1) {// 文件有状态
                if((st.st_mode&S_IFMT)==S_IFDIR) {//文件夹                
            ret=rmdir(vecDir[i].c_str());
                    if(ret <0){
                        ret=-1;
                        LOGWARN << "dir : " << vecDir[i].c_str() << " can not delete!";
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

int SystemCmd::deleteFile(std::string file)
{
    int ret=0;
    struct stat st;
    if(stat(file.c_str(),&st)!=-1){//读取成功
        if((st.st_mode&S_IFMT)==S_IFREG){//普通文件
            unlink(file.c_str());
        }
    }
    else{
        //文件类型是0 的。这些得用stat显示查看。
        LOGWARN << "SystemCmd::deleteFile: not normal file,can not delete.";
        ret=-1;
    }
    return ret;
}

int SystemCmd::getNextName(std::string path, std::vector<std::string> &strVecFile, std::vector<std::string> &strVecDir)
{
    for(auto iter=strVecFile.begin();iter!=strVecFile.end();){
        if(strncmp(path.c_str(),iter->c_str(),path.size())==0){    
            (*iter)=iter->substr(path.size()+1,(*iter).size());
        }
        iter++;
    }
    for(auto iter=strVecDir.begin();iter!=strVecDir.end();){
        if(strncmp(path.c_str(),iter->c_str(),path.size())==0){
            if(path.size()!=iter->size()){
                (*iter)=iter->substr(path.size()+1,(*iter).size());
            }
            else{
                iter=strVecDir.erase(iter);
                continue;
            }
        }
        iter++;
    }

    return 0;
}

//path：输入进去的文件路径， 
//tartarName ：DiagResult.tar
/*
int SystemCmd::tarFile(std::string path, std::string tarName)
{
    int ret;
    std::vector<std::string> vecFile;
    std::vector<std::string> vecDir;

    char desFile[FILE_PATH_LEN] = {0};
    char szBuf[10000] = {0};
    ssize_t read_len;
    FILE *tarFd;
    gzFile gzFd;
    vecDir.clear();
    vecFile.clear();
    
    memset(desFile, 0, sizeof(desFile));
    sprintf(desFile, "%s.gz", tarName.c_str());

    std::vector<std::string> vecStr=getfiles(path,vecFile,vecDir);
    ret=chdir(path.c_str());
    //cout<<"chdir ="<<ret<<endl;

    ret=getNextName(path,vecFile,vecDir);

    int iret = -1;
    TAR* ptar_handle = NULL;
    const char* ptar_fname = "tartest.tar";

    iret = tar_open(&ptar_handle, ptar_fname, NULL,  O_WRONLY | O_CREAT,  0644,  TAR_GNU);
    if (-1 == iret){
        //printf("tar_open failed, reason: \n");
        Print_Location(LOG_ID, ERROR, "%s", "tar_open failed.");
        return(-1);
    }
    for(auto n: vecDir){
        //cout<<"dir tar="<<tar_append_file(ptar_handle, n.c_str(),  n.c_str())<<endl;
    }
    for(auto n: vecFile){
        //cout<<"file tar="<<tar_append_file(ptar_handle, n.c_str(),  n.c_str())<<endl;
    }
    tar_append_eof(ptar_handle);
    tar_close(ptar_handle);
    tarFd = fopen(ptar_fname, "rb");
    if (tarFd == NULL){
        //printf("\n 2161 fopen[%s] error[%s]", ptar_fname, strerror(errno));
        Print_Location(LOG_ID, ERROR, "fopen[%s] error[%s]", ptar_fname, strerror(errno));
        return -1;
    }
    gzFd = gzopen(desFile, "wb");
    if (gzFd == NULL){
        fclose(tarFd);
        remove(ptar_fname);
        //fprintf(stderr, "gzopen error\n");
        // /printf("\n 2170 gzopen[%s] error[%s]", ptar_fname, strerror(errno));
        Print_Location(LOG_ID, ERROR, "gzopen[%s] error[%s]", ptar_fname, strerror(errno));
        return -1;
    }
    while ((read_len = fread(szBuf, 1, 10000, tarFd)) > 0){
        gzwrite(gzFd, szBuf, read_len);
    }
    gzclose(gzFd);
    fclose(tarFd);
    // remove(ptar_fname);
    unlink(ptar_fname);
    return 0;
}
*/

int SystemCmd::tarFile(std::string path, std::string tarName)
{
    int ret;
    std::vector<std::string> vecFile;
    std::vector<std::string> vecDir;

    char desFile[FILE_PATH_LEN] = {0};
    char szBuf[10000] = {0};
    ssize_t read_len;
    FILE *tarFd;
    gzFile gzFd;
    vecDir.clear();
    vecFile.clear();
    
    memset(desFile, 0, sizeof(desFile));
    sprintf(desFile, "%s.gz", tarName.c_str());

    std::vector<std::string> vecStr=getfiles(path,vecFile,vecDir);
    ret=chdir(path.c_str());
    if(ret<0)
    {
        LOGWARN << "SystemCmd::tarFile: chdir fail,path= " << path.c_str();
        return -1;
    }

    ret=getNextName(path,vecFile,vecDir);

    int iret = -1;
    TAR* ptar_handle = nullptr;
    const char* ptar_fname = "tartest.tar";

    iret = tar_open(&ptar_handle, ptar_fname, nullptr,  O_WRONLY | O_CREAT,  0755,  TAR_GNU);
    if (-1 == iret)
    {
        LOGWARN << "SystemCmd::tarFile: tar_open failed, reason: ";
        return(-1);
    }
    for(auto n: vecDir)
    {
        if(tar_append_file(ptar_handle, n.c_str(),  n.c_str()) < 0)
        {
            LOGWARN << "SystemCmd::tarFile: tar append dir= " << n.c_str() << " ,fail!";
        }
    }
    for(auto n: vecFile)
    {
        if(tar_append_file(ptar_handle, n.c_str(),  n.c_str()) < 0)
        {
            LOGWARN << "SystemCmd::tarFile: tar append file= " << n.c_str() << ",fail!";
        }
    }
    tar_append_eof(ptar_handle);
    tar_close(ptar_handle);
    tarFd = fopen(ptar_fname, "rb");
    if (tarFd == nullptr)
    {
        LOGWARN << "SystemCmd::tarFile: fopen: " << ptar_fname << ", error: " << strerror(errno);
        return -1;
    }
    gzFd = gzopen(desFile, "wb");
    if (gzFd == nullptr)
    {
        fclose(tarFd);
        remove(ptar_fname);
        fprintf(stderr, "gzopen error\n");
        LOGWARN << "SystemCmd::tarFile: fopen2: " << ptar_fname << ", error2: " << strerror(errno);
        return -1;
    }
    while ((read_len = fread(szBuf, 1, 10000, tarFd)) > 0)
    {
        gzwrite(gzFd, szBuf,  static_cast<size_t>(read_len));
    }
    gzclose(gzFd);
    fclose(tarFd);
    // remove(ptar_fname);
    unlink(ptar_fname);
    return 0;
}

int SystemCmd::tarFolder(std::string folderPath, std::string tarPath)
{
    char *current = nullptr;
    char *current2 = nullptr;
    int tar_result = 0;
    int change_result = 0;
    
    current = getcwd(current, 0);
    if (current == nullptr){
        LOGWARN << "SystemCmd::tarFolder: can not get cwd.";
        // free(current);
        // free(current2);
        return -1;
    }
    else{
        LOGINFO << "SystemCmd::tarFolder: get cwd: " << current;
    }

    tar_result = tarFile(folderPath,tarPath);
    LOGINFO << "SystemCmd::tarFolder: compress folder result: " << tar_result;
    if(tar_result == -1){
        free(current);
        // free(current2);
        return -1;
    }

    change_result = chdir(current);
    if(change_result == -1){
        LOGWARN << "SystemCmd::tarFolder: can not chdir.";
        free(current);
        // free(current2);
        return -1;
    }
    else{
        current2 = getcwd(current2, 0);
        if (current2 == NULL){
            LOGWARN << "SystemCmd::tarFolder: can not get cwd.";
            free(current);
            // free(current2);
            return -1;
        }
        else{
            LOGINFO << "SystemCmd::tarFolder: get cwd: " << current2;
        }
    }

    free(current);
    free(current2);
    return 0;
}

int SystemCmd::Get_all_files_by_wildcard(std::string path, const std::string& wildcard,std::vector<std::string> &files)
{
    int ret = 0;
    DIR *dp;
    struct dirent *dirp;
    if (path.back() != '/'){
        path += "/";
    }
    LOGINFO << "SystemCmd::Get_all_files_by_wildcard: path is" << path.c_str() << "wildcard is " << wildcard.c_str();

    if ((dp = opendir(path.c_str())) == NULL){
        LOGWARN << "SystemCmd ::Get_all_files_by_wildcard: can not open " << path.c_str();
        ret = -1;
        return ret;
    }

    std::regex reg_obj(wildcard.c_str(), std::regex::icase);
    while ((dirp = readdir(dp)) != NULL){
        if (dirp->d_type == 8) // 4 means catalog; 8 means file; 0 means unknown
        {
            LOGINFO << "SystemCmd::Get_all_files_by_wildcard: name: " << dirp->d_name << " , type: " << dirp->d_type;
            if (regex_match(dirp->d_name, reg_obj)){
               
                std::string all_path = path + dirp->d_name;
                files.push_back(all_path);
            }
        }
    }
    closedir(dp);
    return ret;
}