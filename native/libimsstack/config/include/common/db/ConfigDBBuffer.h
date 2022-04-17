/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101010  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _CONFIG_DB_BUFFER_H_
#define _CONFIG_DB_BUFFER_H_

#include "ConfigBuffer.h"

class IContentCursor;
class IContentTable;
class IContentProvider;

class ConfigDBBuffer
    : public ConfigBuffer
{
public:
    ConfigDBBuffer(IN const AString &strLocator_, IN const AString &strName_);
    virtual ~ConfigDBBuffer();

private:
    ConfigDBBuffer(IN const ConfigDBBuffer &objRHS);
    ConfigDBBuffer& operator=(IN const ConfigDBBuffer &objRHS);

private:
    // IConfigBuffer interface
    virtual void Destroy();

    virtual IMS_BOOL CaptureSection(IN const IMS_CHAR *pszSectName);
    virtual IMS_BOOL CaptureSection(IN const IMS_CHAR *pszSectName, IN IMS_SINT32 nIndex);
    virtual void ReleaseSection();

    virtual IMS_SINT32 ReadKeyCount(IN const IMS_CHAR *pszKey) const;
    virtual const AString& ReadValue(IN const IMS_CHAR *pszKey) const;
    virtual const AString& ReadValue(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nIndex) const;
    virtual IMS_BOOL ReadValueBoolean(IN const IMS_CHAR *pszKey) const;
    virtual IMS_SINT32 ReadValueInt(IN const IMS_CHAR *pszKey) const;

    virtual IMS_BOOL WriteKeyCount(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nCount);
    virtual IMS_BOOL WriteValue(IN const IMS_CHAR *pszKey, IN const AString &strValue);
    virtual IMS_BOOL WriteValue(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nIndex,
            IN const AString &strValue);
    virtual IMS_BOOL WriteValueBoolean(IN const IMS_CHAR *pszKey, IN IMS_BOOL bValue);
    virtual IMS_BOOL WriteValueInt(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nValue);

    virtual IMS_BOOL WriteToMedium() const;

    // ConfigBuffer class
    virtual IMS_BOOL Create(IN IMS_SINT32 nId);

    IMS_BOOL CheckValidity(IN const IMS_CHAR *pszKey,
            IN IMS_BOOL bSection = IMS_FALSE, IN IMS_SINT32 nIndex = -1) const;

private:
    // Current working section
    AString strSection;
    // Property
    IMS_SINT32 nProperty;

    IConfigBuffer *piConfigBuffer;
    IContentCursor *piContentCursor;
    IContentTable *piContentTable;
    IContentProvider *piContentProvider;
};

#endif // _CONFIG_DB_BUFFER_H_
