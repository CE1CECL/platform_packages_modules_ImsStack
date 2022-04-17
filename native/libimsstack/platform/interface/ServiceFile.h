/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090831  yhrhee@                   Created
    </table>

    Description
    IMS File Service
*/

#ifndef _SERVICE_IMS_FILE_H_
#define _SERVICE_IMS_FILE_H_

#include "IFile.h"

class FileServicePrivate;

class FileService
{
private:
    FileService();
    ~FileService();

    FileService(IN const FileService& objRHS);
    FileService& operator=(IN const FileService& objRHS);

public:
    IFile* CreateFile();
    void DestroyFile(IN IFile *&piFile);
    IFileUtil* GetFileUtil();

    static FileService* GetFileService();

private:
    FileServicePrivate *pPrivate;
};

//-------------------------------------------------------------------------------------------------

#define IMS_FILE_Create() \
        FileService::GetFileService()->CreateFile()

#define IMS_FILE_Destroy(piFile) \
        FileService::GetFileService()->DestroyFile(piFile)

#define IMS_FILE_GetName(path) \
        FileService::GetFileService()->GetFileUtil()->GetName(path)

#define IMS_FILE_GetExtension(path) \
        FileService::GetFileService()->GetFileUtil()->GetExtension(path)

#define IMS_FILE_Exist(path) \
        FileService::GetFileService()->GetFileUtil()->Exist(path)

#define IMS_FILE_Rename(oldPath, newPath) \
        FileService::GetFileService()->GetFileUtil()->Rename(oldPath, newPath)

#define IMS_FILE_Delete(path) \
        FileService::GetFileService()->GetFileUtil()->Delete(path)

#define IMS_FILE_GetSeparator() \
        FileService::GetFileService()->GetFileUtil()->GetFileSeparator()

#endif // _SERVICE_IMS_FILE_H_
