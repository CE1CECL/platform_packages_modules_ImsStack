/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101010  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ServiceContentProvider.h"
#include "TextParser.h"
#include "conf/ConfigFileBuffer.h"
#include "db/ConfigDB.h"
#include "db/ConfigDBBuffer.h"

__IMS_TRACE_TAG_CONF__;

PUBLIC
ConfigDBBuffer::ConfigDBBuffer(IN const AString &strLocator_, IN const AString &strName_)
    : ConfigBuffer(strLocator_, strName_)
    , strSection(AString::ConstNull())
    , nProperty(ConfigDB::PROPERTY_READONLY)
    , piConfigBuffer(IMS_NULL)
    , piContentCursor(IMS_NULL)
    , piContentTable(IMS_NULL)
    , piContentProvider(IMS_NULL)
{
}

PUBLIC VIRTUAL
ConfigDBBuffer::~ConfigDBBuffer()
{
    if (piConfigBuffer != IMS_NULL)
    {
        piConfigBuffer->Destroy();
        piConfigBuffer = IMS_NULL;
    }

    IMS_DB_DestroyTable(piContentTable);
    IMS_DB_DestroyProvider(piContentProvider);
}

PRIVATE VIRTUAL
void ConfigDBBuffer::Destroy()
{
    delete this;
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::CaptureSection(IN const IMS_CHAR *pszSectName)
{
    if (!CheckValidity(pszSectName, IMS_TRUE))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->CaptureSection(pszSectName);
    }
    else
    {
        strSection = pszSectName;
        strSection.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);
    }

    return IMS_TRUE;
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::CaptureSection(IN const IMS_CHAR *pszSectName, IN IMS_SINT32 nIndex)
{
    if (!CheckValidity(pszSectName, IMS_TRUE))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->CaptureSection(pszSectName, nIndex);
    }
    else
    {
        strSection.Sprintf("%s_%d", pszSectName, nIndex);
        strSection.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);
    }

    return IMS_TRUE;
}

PRIVATE VIRTUAL
void ConfigDBBuffer::ReleaseSection()
{
    if (!CheckValidity("", IMS_TRUE))
    {
        return;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        piConfigBuffer->ReleaseSection();
    }
    else
    {
        strSection = AString::ConstNull();
    }
}

PRIVATE VIRTUAL
IMS_SINT32 ConfigDBBuffer::ReadKeyCount(IN const IMS_CHAR *pszKey) const
{
    if (!CheckValidity(pszKey))
    {
        return (-1);
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->ReadKeyCount(pszKey);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s_count", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->GetInt(strKey);
    }
}

PRIVATE VIRTUAL
const AString& ConfigDBBuffer::ReadValue(IN const IMS_CHAR *pszKey) const
{
    if (!CheckValidity(pszKey))
    {
        return AString::ConstNull();
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->ReadValue(pszKey);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        //4 According to the field type, needs to be enhanced...

        return piContentCursor->GetString(strKey);
    }
}

PRIVATE VIRTUAL
const AString& ConfigDBBuffer::ReadValue(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nIndex) const
{
    if (!CheckValidity(pszKey, IMS_FALSE, nIndex))
    {
        return AString::ConstNull();
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->ReadValue(pszKey, nIndex);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s_%d", strSection.GetStr(), pszKey, nIndex);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->GetString(strKey);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::ReadValueBoolean(IN const IMS_CHAR *pszKey) const
{
    if (!CheckValidity(pszKey))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->ReadValueBoolean(pszKey);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        const AString &strValue = piContentCursor->GetString(strKey);

        // If the value is not "true" in case-insensitively, it returns IMS_FALSE.
        if (strValue.EqualsIgnoreCase(TextParser::STR_SMALL_TRUE))
        {
            return IMS_TRUE;
        }

        return IMS_FALSE;
    }
}

PRIVATE VIRTUAL
IMS_SINT32 ConfigDBBuffer::ReadValueInt(IN const IMS_CHAR *pszKey) const
{
    if (!CheckValidity(pszKey))
    {
        return (-1);
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->ReadValueInt(pszKey);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->GetInt(strKey);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteKeyCount(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nCount)
{
    if (!CheckValidity(pszKey))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->WriteKeyCount(pszKey, nCount);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s_count", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->PutValue(strKey, nCount);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteValue(IN const IMS_CHAR *pszKey, IN const AString &strValue)
{
    if (!CheckValidity(pszKey))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->WriteValue(pszKey, strValue);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->PutValue(strKey, strValue);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteValue(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nIndex,
        IN const AString &strValue)
{
    if (!CheckValidity(pszKey, IMS_FALSE, nIndex))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->WriteValue(pszKey, nIndex, strValue);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s_%d", strSection.GetStr(), pszKey, nIndex);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->PutValue(strKey, strValue);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteValueBoolean(IN const IMS_CHAR *pszKey, IN IMS_BOOL bValue)
{
    if (!CheckValidity(pszKey))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->WriteValueBoolean(pszKey, bValue);
    }
    else
    {
        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->PutValue(strKey, AString(TextParser::BooleanToString(bValue)));
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteValueInt(IN const IMS_CHAR *pszKey, IN IMS_SINT32 nValue)
{
    if (!CheckValidity(pszKey))
    {
        return IMS_FALSE;
    }

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        return piConfigBuffer->WriteValueInt(pszKey, nValue);
    }
    else
    {
        if (nValue < 0)
        {
            return IMS_FALSE;
        }

        AString strKey;

        strKey.Sprintf("%s_%s", strSection.GetStr(), pszKey);
        strKey.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

        return piContentCursor->PutValue(strKey, nValue);
    }
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::WriteToMedium() const
{
    AString strTable = GetName();

    strTable.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        IMS_TRACE_D("No operation (read-only)", 0, 0, 0);
        return IMS_TRUE;
    }

    IMS_SINT32 nId = piContentCursor->GetInt(ConfigDB::FIELD_NAME_ID);
    AString strWhere;
    strWhere.Sprintf("%s='%d'", ConfigDB::FIELD_NAME_ID, (nId >= 0) ? nId : IMS_SLOT_0);

    if (!piContentProvider->UpdateData(strTable, piContentCursor, strWhere))
    {
        IMS_TRACE_E(0, "Updating DB table (%s) failed", strTable.GetStr(), 0, 0);
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PRIVATE VIRTUAL
IMS_BOOL ConfigDBBuffer::Create(IN IMS_SINT32 nId)
{
    // DB name for IMS configuration : GetLocator()
    const AString &strDB = GetLocator();
    AString strTable = GetName();

    strTable.Replace(TextParser::CHAR_DOT, TextParser::CHAR_UNDERSCORE);

    piContentProvider = IMS_DB_CreateProvider(strDB);

    if (piContentProvider == IMS_NULL)
    {
        return IMS_FALSE;
    }

    piContentTable = piContentProvider->CreateTableEx(strTable);

    if (piContentTable == IMS_NULL)
    {
        IMS_TRACE_E(0, "Creating a table (%s) template failed", strTable.GetStr(), 0, 0);

        IMS_DB_DestroyProvider(piContentProvider);
        return IMS_FALSE;
    }

    piContentProvider->AddTable(strTable, piContentTable);

    AString strWhere;
    strWhere.Sprintf("%s='%d'", ConfigDB::FIELD_NAME_ID, nId);

    // Get the first row from the specified table
    piContentCursor = piContentProvider->ManagedQuery(strTable, strWhere);

    if (piContentCursor == IMS_NULL)
    {
        IMS_DB_DestroyTable(piContentTable);
        IMS_DB_DestroyProvider(piContentProvider);
        return IMS_FALSE;
    }

    // Get the property field : Read-Only / Read-Write
    IMS_SINT32 nValue = piContentCursor->GetInt(ConfigDB::FIELD_NAME_PROPERTY);

    if (nValue == 0)
    {
        // Read-Only property
        nProperty = ConfigDB::PROPERTY_READONLY;

        const AString &strConfigData = piContentCursor->GetString(ConfigDB::FIELD_NAME_CONF);

        piConfigBuffer = ConfigFileBuffer::CreateFileBuffer(strConfigData);

        if (piConfigBuffer == IMS_NULL)
        {
            IMS_DB_DestroyTable(piContentTable);
            IMS_DB_DestroyProvider(piContentProvider);
            return IMS_FALSE;
        }
    }
    else
    {
        // Read-Write property
        nProperty = ConfigDB::PROPERTY_READWRITE;
    }

    return IMS_TRUE;
}

PRIVATE
IMS_BOOL ConfigDBBuffer::CheckValidity(IN const IMS_CHAR *pszKey,
        IN IMS_BOOL bSection/* = IMS_FALSE*/, IN IMS_SINT32 nIndex/* = -1*/) const
{
    if (nProperty == ConfigDB::PROPERTY_READONLY)
    {
        if (piConfigBuffer == IMS_NULL)
        {
            IMS_TRACE_E(0, "No config buffer (%s, %d)", _TRACE_S_(pszKey), nIndex, 0);
            return IMS_FALSE;
        }
    }
    else
    {
        if (piContentCursor == IMS_NULL)
        {
            IMS_TRACE_E(0, "No content cursor (%s, %d)", _TRACE_S_(pszKey), nIndex, 0);
            return IMS_FALSE;
        }

        if (!bSection && (strSection.GetLength() == 0))
        {
            IMS_TRACE_E(0, "No captured section (%s, %d)", _TRACE_S_(pszKey), nIndex, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}
