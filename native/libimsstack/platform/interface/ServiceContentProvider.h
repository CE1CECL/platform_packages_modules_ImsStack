/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100517  eunwoo.cho@               Created
    20101010  hwangoo.park@             Modified the ContentProvider's initialization procedure
    </table>

    Description

*/

#ifndef _SERVICE_CONTENT_PROVIDER_H_
#define _SERVICE_CONTENT_PROVIDER_H_

#include "IContentTable.h"
#include "IContentCursor.h"
#include "IContentProvider.h"

class ContentProviderService
{
private:
    ContentProviderService();
    ~ContentProviderService();

public:
    IContentTable* CreateContentTable();
    void DestroyContentTable(IN IContentTable *&piTable);

    IContentCursor* CreateContentCursor();
    void DestroyContentCursor(IN IContentCursor *&piCursor);

    IContentProvider* CreateContentProvider(IN const AString& strDB);
    void DestroyContentProvider(IN IContentProvider *&piProvider);

    static ContentProviderService* GetContentProviderService();

    static IMS_BOOL CheckIfDatabaseExists(IN const AString &strDB);
};

#define IMS_DB_CreateTable() \
        ContentProviderService::GetContentProviderService()->CreateContentTable()

#define IMS_DB_CreateCursor() \
        ContentProviderService::GetContentProviderService()->CreateContentCursor()

#define IMS_DB_CreateProvider(NAME) \
        ContentProviderService::GetContentProviderService()->CreateContentProvider(NAME)

#define IMS_DB_DestroyTable(TABLE) \
        ContentProviderService::GetContentProviderService()->DestroyContentTable(TABLE)

#define IMS_DB_DestroyCursor(CURSOR) \
        ContentProviderService::GetContentProviderService()->DestroyContentCursor(CURSOR)

#define IMS_DB_DestroyProvider(PROVIDER) \
        ContentProviderService::GetContentProviderService()->DestroyContentProvider(PROVIDER)

#define IMS_DB_Exists(NAME) \
        ContentProviderService::CheckIfDatabaseExists(NAME)

#endif // _SERVICE_CONTENT_PROVIDER_H_
