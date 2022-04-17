/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090831  yhrhee@                   Adjust file location and class re-organize
                                        for layered approach
    </table>

    Description
*/

#ifndef _INTERFACE_IMS_FILE_H_
#define _INTERFACE_IMS_FILE_H_

#include "AString.h"

// Open mode
enum FILE_OPEN_ENTYPE
{
    FILE_OPEN_READWRITE =    0,
    FILE_OPEN_READONLY = 1,
    FILE_OPEN_WRITEONLY = 2
};

// Seek mode
enum FILE_SEEK_ENTYPE
{
    // Begin of the file
    FILE_SEEK_BEGIN = 0,
    // Current position of the file
    FILE_SEEK_CURRENT,
    // End of the file
    FILE_SEEK_END
};

class IFile
{
public:
    /*
     It creates or opens a file.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strName                 File name
    eMode                   Desired mode to create or open a file
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success to create or open a file
    IMS_FALSE               It indicates failure to create or open a file
    </table>
    */
    virtual IMS_BOOL Open(IN const AString &strName, IN FILE_OPEN_ENTYPE eMode) = 0;

    /*
     Closes the handle of a created or open file.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual void Close() = 0;

    /*
     Moves the file pointer of an open file from the offset, nPosition.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    nOffset                 Number of bytes to move the file pointer
    eFrom                   Start point for the file pointer move
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    New file pointer        It indicates success
    INVALID_VALUE           It indicates failure
    </table>
    */
    virtual IMS_UINT32 Seek(IN IMS_UINT32 nOffset, IN FILE_SEEK_ENTYPE eFrom) const = 0;

    /*
     Reads data from a file, starting at the position indicated by the file pointer.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pBuffer                 Pointer to the buffer that receives the date read from the file
    nNumberOfBytesToRead    Number of bytes to be read from the file
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    Nonzero                 It indicates success
    0                       It indicates failure
    </table>
    */
    virtual IMS_UINT32 Read(OUT void *pBuffer, IN IMS_UINT32 nNumberOfBytesToRead) = 0;

    /*
     Writes data to a file, starting at the position indicated by the file pointer.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pBuffer                 Pointer to the buffer containing the date to write the file
    nNumberOfBytesToWrite   Number of bytes to be written to the file
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    Nonzero                 It indicates success
    0                       It indicates failure
    </table>
    */
    virtual IMS_UINT32 Write(IN void *pBuffer, IN IMS_UINT32 nNumberOfBytesToWrite) = 0;

    /*
     Retrieves the size, in bytes, of the specified file.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------

    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    Size of file
    </table>
    */
    virtual IMS_UINT32 GetSize() const = 0;

    /*
     Retrieves the current file pointer.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------

    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    Current file pointer
    </table>
    */
    virtual IMS_UINT32 GetPos() const = 0;

public:
    // Invalid value: Seek(), GetPos()
    enum { INVALID_VALUE = 0xFFFFFFFF };
};



class IFileUtil
{
public:
    /*
     Changes the file mode to share the file to another user / module.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFileName             File name to be changed
    nMode                   File mode
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates the operation succeeds
    IMS_FALSE               It indicates the operation fails
    </table>
    */
    virtual IMS_BOOL ChangeMode(IN const AString &strFileName, IN IMS_SINT32 nMode) const = 0;

    /*
     Changes the file owner.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFileName             File name to be changed
    nUID                    User id of the file
    nGID                    Group id of the file
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates the operation succeeds
    IMS_FALSE               It indicates the operation fails
    </table>
    */
    virtual IMS_BOOL ChangeOwner(IN const AString &strFileName,
            IN IMS_SINT32 nUID, IN IMS_SINT32 nGID) const = 0;

    /*
     Checks if the specified file exists.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFileName             File name to be checked
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates the specified file exists
    IMS_FALSE               It indicates the specified file does not exist
    </table>
    */
    virtual IMS_BOOL Exist(IN const AString &strFileName) const = 0;

    /*
     Renames the existing file name to new file name.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strOldName              Existing file name
    strNewName              File name to be renamed
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success
    IMS_FALSE               It indicates failure
    </table>
    */
    virtual IMS_BOOL Rename(IN const AString &strOldName, IN const AString &strNewName) const = 0;

    /*
     Deletes an existing file from a file system.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFileName             File name to be deleted
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success
    IMS_FALSE               It indicates failure
    </table>
    */
    virtual IMS_BOOL Delete(IN const AString &strFileName) const = 0;

    /*
     Extracts the file name without the directory path & file extension from the specified
    file name.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFilePath             File name with full path info.
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    File name only
    </table>
    */
    virtual AString GetName(IN const AString &strFilePath) const = 0;

    /*
     Extracts the file extension only from the specified file name.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strFileName             File name
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    File extension only
    </table>
    */
    virtual AString GetExtension(IN const AString &strFileName) const = 0;

    /*
     Returns the directory separator of the file system.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    File separator
    </table>
    */
    virtual const IMS_CHAR* GetFileSeparator() const = 0;

    /*
    Create an directory to file system.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPathName             Directory name to be create
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success
    IMS_FALSE               It indicates failure
    </table>
    */
    virtual IMS_BOOL MakeDir(IN const AString &strPathName, IN IMS_SINT32 nMode) const = 0;

    /*
    Create directory with parents to file system.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPathName             Directory name to be create
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success to create folder or already exist
    IMS_FALSE               It indicates failure to create any folder
    </table>
    */
    virtual IMS_BOOL MakeDirs(IN const AString &strPathName, IN IMS_SINT32 nMode) const = 0;

    /*
    Deletes an existing empty directory from a file system.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPathName             Directory name to be deleted
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success
    IMS_FALSE               It indicates failure
    </table>
    */
    virtual void RemoveDir(IN const AString &strPathName) = 0;

    /*
     Checks if the specified directory exists.

    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPathName             Directory name to be checked
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>
    */
    virtual IMS_BOOL ExistDir(IN const AString &strPathName) const = 0;

    /*
    Get External Storage Path.

    Remarks


    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    External Storage Path
    </table>
    */
    virtual AString GetExternalStoragePath() const = 0;

    /*
     Delete all files that exists under the given path.
     Especially the strFileType is provied, only files that its extension is same
     with given strFileType are deleted.

    Remarks


    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPathName             Directory path
    strFileType             The file extension that should be deleted
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_TRUE                It indicates success
    IMS_FALSE               It indicates failure
    </table>
    */
    virtual IMS_BOOL DeleteAllFiles(IN const AString &strPathName,
            IN const AString &strFileType = AString::ConstNull()) = 0;

    virtual IMSList<AString> GetAllFiles(IN const AString &strPathName,
            IN const AString &strFileType = AString::ConstNull()) = 0;

    virtual IMS_SLONG GetLastModifiedTime(IN const AString &strPathName) = 0;

public:
    // UID & GID for access permission
    // In this time, only supports: root/system for user/group id
    enum
    {
        UID_NONE = (-1), // no change
        UID_ROOT = 1,
        UID_SYSTEM = 2
    };

    enum
    {
        GID_NONE = (-1), // no change
        GID_ROOT = 1,
        GID_SYSTEM = 2
    };

    // Mode bits for access permission
    enum
    {
        // file mode: user, read-only, write-only, executable-only, all
        MODE_USER_RO = 0x0001,
        MODE_USER_WO = 0x0002,
        MODE_USER_XO = 0x0004,
        MODE_USER_RWX = 0x0007,

        // file mode: group, read-only, write-only, executable-only, all
        MODE_GROUP_RO = 0x0010,
        MODE_GROUP_WO = 0x0020,
        MODE_GROUP_XO = 0x0040,
        MODE_GROUP_RWX = 0x0070,

        // file mode: other, read-only, write-only, executable-only, all
        MODE_OTHER_RO = 0x0100,
        MODE_OTHER_WO = 0x0200,
        MODE_OTHER_XO = 0x0400,
        MODE_OTHER_RWX = 0x0700
    };
};

#endif // _INTERFACE_IMS_FILE_H_
