/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100518  eunwoo.cho@               Created
    20101010  hwangoo.park@             Modified the ContentProvider's initialization procedure
    </table>

    Description

*/

#include "ServiceMemory.h"

#include "ImsContentTable.h"
#include "ImsContentCursor.h"
#include "ImsContentProvider.h"

#include "PlatformApi.h"
#include "PlatformFactory.h"
#include "ServiceContentProvider.h"

PUBLIC
ContentProviderService::ContentProviderService()
{
}

PUBLIC
ContentProviderService::~ContentProviderService()
{
}

PUBLIC
IContentTable* ContentProviderService::CreateContentTable()
{
    ImsContentTable *pContentTable = PlatformFactory::CreateContentTable();

    IMS_ASSERT(pContentTable != IMS_NULL);

    return pContentTable;
}

PUBLIC
void ContentProviderService::DestroyContentTable(IN IContentTable *&piTable)
{
    ImsContentTable *pTable = DYNAMIC_CAST(ImsContentTable*, piTable);

    if (pTable != IMS_NULL)
    {
        delete pTable;
        piTable = IMS_NULL;
    }
}

PUBLIC
IContentCursor* ContentProviderService::CreateContentCursor()
{
    ImsContentCursor *pContentCursor = PlatformFactory::CreateContentCursor();

    IMS_ASSERT(pContentCursor != IMS_NULL);

    return pContentCursor;
}

PUBLIC
void ContentProviderService::DestroyContentCursor(IN IContentCursor *&piCursor )
{
    ImsContentCursor *pCursor = DYNAMIC_CAST(ImsContentCursor*, piCursor);

    if (pCursor != IMS_NULL)
    {
        delete pCursor;
        piCursor = IMS_NULL;
    }
}

PUBLIC
IContentProvider* ContentProviderService::CreateContentProvider(IN const AString& strDB)
{
    ImsContentProvider *pContentProvider= PlatformFactory::CreateContentProvider();

    if (pContentProvider == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (!pContentProvider->Initialize(strDB))
    {
        delete pContentProvider;
        return IMS_NULL;
    }

    return pContentProvider;
}

PUBLIC
void ContentProviderService::DestroyContentProvider(IN IContentProvider *&piProvider)
{
    ImsContentProvider *pProvider = DYNAMIC_CAST(ImsContentProvider*, piProvider);

    if (pProvider != IMS_NULL)
    {
        delete pProvider;
        piProvider = IMS_NULL;
    }
}

PUBLIC GLOBAL
ContentProviderService* ContentProviderService::GetContentProviderService()
{
    static ContentProviderService *pContentProviderService = IMS_NULL;

    if (pContentProviderService == IMS_NULL)
    {
        pContentProviderService = new ContentProviderService();
    }

    return pContentProviderService;
}

PUBLIC GLOBAL
IMS_BOOL ContentProviderService::CheckIfDatabaseExists(IN const AString &strDB)
{
    return PlatformApi::CheckIfDatabaseExists(strDB);
}
