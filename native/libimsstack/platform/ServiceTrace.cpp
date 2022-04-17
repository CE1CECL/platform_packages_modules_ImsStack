/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090831  yhrhee@                   Created
    </table>

    Description
     IMS Trace Service
*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ImsTrace.h"
#include "PlatformFactory.h"

#define __IMS_TRACE_TAG_DEF__(ENUM,NAME,MODULE)           { NAME, IMS_TRACE_MODULE_##MODULE },

LOCAL const IMSTraceTag gstIMSTraceTag[IMS_TRACE_TAG_MAX + 1] =
{
    { __IMS_TRACE_DEFAULT_NAME__, IMS_TRACE_MODULE_DEFAULT },

#include "ITraceTagDef.h"

    { __IMS_TRACE_DEFAULT_NAME__, IMS_TRACE_MODULE_DEFAULT }
};

#undef __IMS_TRACE_TAG_DEF__


class TraceServicePrivate
{
public:
    inline TraceServicePrivate()
        : pTrace(IMS_NULL)
    {}
    inline ~TraceServicePrivate()
    {
        if (pTrace != IMS_NULL)
        {
            delete pTrace;
        }
    }

private:
    TraceServicePrivate(IN const TraceServicePrivate& objRHS);
    TraceServicePrivate& operator=(IN const TraceServicePrivate& objRHS);

public:
    inline ImsTrace* GetTrace()
    {
        if (pTrace == IMS_NULL)
        {
            pTrace = PlatformFactory::CreateTrace();
        }

        return pTrace;
    }

private:
    ImsTrace *pTrace;
};



PRIVATE GLOBAL
IMS_SINT32 TraceService::sLoggableForDebug = -1;

PRIVATE
TraceService::TraceService()
    : pPrivate(new TraceServicePrivate())
{
    for (IMS_SINT32 i = 0; i < IMS_TRACE_TAG_MAX + 1; ++i)
    {
        TRACE_TAG[i] = const_cast<IMSTraceTag*>(&gstIMSTraceTag[i]);
    }

    sLoggableForDebug = ImsTrace::IsLoggable(ITrace::CAT_D);
}

PRIVATE
TraceService::~TraceService()
{
    if (pPrivate != IMS_NULL)
    {
        delete pPrivate;
    }
}

PUBLIC
const IMS_CHAR* TraceService::GetFileName(IN const IMS_CHAR *pszFileName) const
{
    ImsTrace *pTrace = pPrivate->GetTrace();

    if (pTrace == IMS_NULL)
    {
        return "__NULL__";
    }

    return pTrace->GetFileName(pszFileName);
}

PUBLIC
const IMS_CHAR* TraceService::GetFileName(IN_OUT IMS_CHAR *pszOutFileName,
        IN const IMS_CHAR *pszFileName) const
{
    ImsTrace *pTrace = pPrivate->GetTrace();

    if (pTrace == IMS_NULL)
    {
        return "__NULL__";
    }

    return pTrace->GetFileName(pszOutFileName, pszFileName);
}

PUBLIC
ITrace* TraceService::GetTrace()
{
    return pPrivate->GetTrace();
}

PUBLIC
const IMSTraceTag& TraceService::GetTraceTag(IN IMS_SINT32 nTAG) const
{
    if ((nTAG < 0) || (nTAG > IMS_TRACE_TAG_MAX))
    {
        return (*TRACE_TAG[IMS_TRACE_TAG_MAX]);
    }

    return (*TRACE_TAG[nTAG]);
}

PUBLIC
void TraceService::SetOption(IN IMS_UINT32 nOption, IN IMS_UINT32 nModule)
{
    ImsTrace *pTrace = pPrivate->GetTrace();

    if (pTrace == IMS_NULL)
    {
        return;
    }

    pTrace->SetOption(nOption, nModule);
}

PUBLIC
void TraceService::PrintPrivacyLog(IN IMS_SINT32 nCategory, IN const IMS_CHAR *pszTag,
        IN IMS_UINT32 nModule, IN const IMS_CHAR *pszFormat,
        IN const IMS_CHAR *pszFileName,IN IMS_UINT32 nLine,
        IN const IMS_CHAR *A1, IN const IMS_CHAR *A2, IN const IMS_CHAR *A3)
{
    ImsTrace *pTrace = pPrivate->GetTrace();

    if (pTrace == IMS_NULL)
    {
        return;
    }

    IMS_CHAR _buffer_A1[512 + 7] = {0,};
    IMS_CHAR _buffer_A2[512 + 7] = {0,};
    IMS_CHAR _buffer_A3[512 + 7] = {0,};
    pTrace->OutP(nCategory, pszTag, nModule, pszFormat, pszFileName, nLine
                   , pTrace->EncryptPrivacyLog(_buffer_A1, A1)
                   , pTrace->EncryptPrivacyLog(_buffer_A2, A2)
                   , pTrace->EncryptPrivacyLog(_buffer_A3, A3));
}

PUBLIC GLOBAL
TraceService* TraceService::GetTraceService()
{
    static TraceService *pTraceService = IMS_NULL;

    if (pTraceService == IMS_NULL)
    {
        pTraceService = new TraceService();
    }

    return pTraceService;
}

GLOBAL
void TraceService_Assert(IN const IMS_CHAR *pszCondition, IN const IMS_CHAR *pszModule,
        IN IMS_UINT16 nLine)
{
    TraceService::GetTraceService()->GetTrace()->Out(ITrace::CAT_E, "ASSERT",
            IMS_TRACE_MODULE_DEFAULT,  "(%s) FAILED AT (%s, %d)", pszCondition, pszModule, nLine);
}

GLOBAL
const IMSTraceTag& TraceService_GetTraceTag(IN IMS_SINT32 nTAG)
{
    return TraceService::GetTraceService()->GetTraceTag(nTAG);
}
