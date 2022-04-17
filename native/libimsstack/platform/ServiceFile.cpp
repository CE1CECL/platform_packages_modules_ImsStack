/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  YR@                       Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ImsFile.h"
#include "PlatformFactory.h"
#include "ServiceFile.h"

class FileServicePrivate
{
public:
    inline FileServicePrivate()
        : piFileUtil(IMS_NULL)
    {}
    inline ~FileServicePrivate()
    {
        PlatformFactory::DestroyFileUtil(piFileUtil);
    }

private:
    FileServicePrivate(IN const FileServicePrivate& objRHS);
    FileServicePrivate& operator=(IN const FileServicePrivate& objRHS);

public:
    inline IFileUtil* GetFileUtil()
    {
        if (piFileUtil == IMS_NULL)
        {
            piFileUtil = PlatformFactory::CreateFileUtil();
        }

        return piFileUtil;
    }

private:
    IFileUtil *piFileUtil;
};



PRIVATE
FileService::FileService()
    : pPrivate(new FileServicePrivate())
{
}

PRIVATE
FileService::~FileService()
{
    if (pPrivate != IMS_NULL)
    {
        delete pPrivate;
    }
}

PUBLIC
IFile* FileService::CreateFile()
{
    ImsFile *pFile = PlatformFactory::CreateFile();

    IMS_ASSERT(pFile != IMS_NULL);

    return pFile;
}

PUBLIC
void FileService::DestroyFile(IN IFile *&piFile)
{
    ImsFile *pFile = DYNAMIC_CAST(ImsFile*, piFile);

    if (pFile != IMS_NULL)
    {
        delete pFile;
        piFile = IMS_NULL;
    }
}

PUBLIC
IFileUtil* FileService::GetFileUtil()
{
    return pPrivate->GetFileUtil();
}

PUBLIC GLOBAL
FileService* FileService::GetFileService()
{
    static FileService *pFileService = IMS_NULL;

    if (pFileService == IMS_NULL)
    {
        pFileService = new FileService();
    }

    return pFileService;
}
