/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090819  YR@                       Created
    </table>

    Description

*/

#ifndef _SERVICE_IMS_MUTEX_H_
#define _SERVICE_IMS_MUTEX_H_

#include "AString.h"
#include "IMutex.h"

class MutexService
{
private:
    MutexService();
    ~MutexService();

public:
    IMutex* CreateMutex(IN const AString &strName = AString::ConstNull());
    void DestroyMutex(IN IMutex *&piMutex);

    static MutexService* GetMutexService();
};

#endif // _SERVICE_IMS_MUTEX_H_
