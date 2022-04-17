#include "ServiceTrace.h"
#include "ServiceThread.h"
#include "SIPAddress.h"
#include "SIPHeaderName.h"
#include "SIPParameter.h"
#include "SIPParsingHelper.h"
#include "IMessage.h"
#include "IuMtcService.h"
#include "helper/MtcSupplementaryService.h"
#include "utility/MessageUtil.h"
#include "ussi/UssiConstants.h"
#include "configuration/ConfigDef.h"
#include "MtcDef.h"
#include "ISIPHeader.h"
#include "define/MtcStringDef.h"
#include "IMtcContext.h"
#include "configuration/MtcConfigurationProxy.h"

__IMS_TRACE_TAG_COM_MTC__;

PRIVATE GLOBAL
const IMS_CHAR MtcSupplementaryService::STR_VERSTAT[] = "verstat";
const IMS_CHAR MtcSupplementaryService::STR_VERSTAT_TN_VALIDATION_PASSED[] = "TN-Validation-Passed";
const IMS_CHAR MtcSupplementaryService::STR_VERSTAT_TN_VALIDATION_FAILED[] = "TN-Validation-Failed";

PUBLIC
MtcSupplementaryService::MtcSupplementaryService(IN MtcConfigurationProxy& objConfigurationProxy,
        IN IMSMap<IMS_UINT32, SuppService*> objSuppServices) :
        m_objSuppService(objSuppServices),
        m_objConfigurationProxy(objConfigurationProxy),
        m_nCnapType(CNAP_SCHEME_PAID_FROM)
{
    IMS_TRACE_MEM("mtc", "mtc_M : MtcSupplementaryService[%" PFLS_u "][%" PFLS_x "]",
            sizeof(MtcSupplementaryService), this, 0);
    LoadConfig();
}

PUBLIC
MtcSupplementaryService::~MtcSupplementaryService()
{
    IMS_TRACE_MEM("mtc", "mtc_F : MtcSupplementaryService[%" PFLS_u "][%" PFLS_x "]",
            sizeof(MtcSupplementaryService), this, 0);

    DeleteAll();
}

PUBLIC
void MtcSupplementaryService::UpdateService(
        IN const IMSMap<IMS_UINT32, SuppService*>& objSuppServices)
{
    IMS_UINT32 nInServiceSize = objSuppServices.GetSize();
    IMS_TRACE_I("MtcSupplementaryService : ServiceNum[%d] InServiceNum[%d]",
            m_objSuppService.GetSize(), nInServiceSize, 0);

    for (IMS_UINT32 i = 0; i < nInServiceSize; i++)
    {
        const IMS_UINT32 nType = objSuppServices.GetKeyAt(i);
        IMS_SLONG nIndex = m_objSuppService.GetIndexOfKey(nType);

        if (nIndex >= 0)
        {
           delete m_objSuppService.GetValueAt(nIndex);
        }

        m_objSuppService.Add(nType, objSuppServices.GetValueAt(i));
    }
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateService(IN ISession* piSession, IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    bUpdate |= UpdateCallerID(piSession, piMessage);
    bUpdate |= UpdateCnap(piSession, piMessage);
    bUpdate |= UpdateCNAPEX(piSession, piMessage);
    bUpdate |= UpdateMMC(piSession, piMessage);
    bUpdate |= UpdateGTT(piSession, piMessage);
    bUpdate |= UpdateCDIV_CAUSE(piSession, piMessage);
    bUpdate |= UpdateCDIV_HISTORY(piSession, piMessage);
    bUpdate |= UpdateCW(piSession, piMessage);
    bUpdate |= UpdateUSSD(piSession, piMessage);
    bUpdate |= UpdateVM(piSession, piMessage);
    bUpdate |= UpdateAnswerHold(piSession, piMessage);
    bUpdate |= UpdateMCID(piSession, piMessage);
    bUpdate |= UpdateDualNumber(piSession, piMessage);
    bUpdate |= UpdateCallingNumVerification(piSession, piMessage);

    IMS_TRACE_I("UpdateService : [%s]", PS_BOOL(bUpdate), 0, 0);
    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCallerID(IN ISession* /*piSession*/,
        IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateCallerID", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCnap(IN ISession* /*piSession*/, IN IMessage* piMessage)
{
    AString strDisplayName;

    if (m_nCnapType == CNAP_SCHEME_PAID_FROM || m_nCnapType == CNAP_SCHEME_PAID)
    {
        MessageUtil::GetDisplayName(piMessage, ISIPHeader::P_ASSERTED_IDENTITY, strDisplayName);
    }

    if (strDisplayName.GetLength() <= 0 &&
            (m_nCnapType == CNAP_SCHEME_PAID_FROM || m_nCnapType == CNAP_SCHEME_FROM))
    {
        MessageUtil::GetDisplayName(piMessage, ISIPHeader::FROM, strDisplayName);
    }

    if (strDisplayName.GetLength() <= 0)
    {
        return IMS_FALSE;
    }

    SuppService* pSuppService = new SuppService();
    pSuppService->nType = SUPP_TYPE_CNAP;
    pSuppService->aStrValue = strDisplayName;
    m_objSuppService.Add(pSuppService->nType, pSuppService);

    return IMS_TRUE;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCNAPEX(IN ISession* /*piSession*/,
        IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateCNAPEX", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateMMC(IN ISession* /*piSession*/, IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;
    IMS_BOOL bUseMMC = m_objConfigurationProxy.Is(Feature::USE_MMC_SUPPLEMENTARY_SERVICE);

    if (bUseMMC)
    {

    }

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateMMC", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateGTT(IN ISession* /*piSession*/, IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateGTT", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCDIV_CAUSE(IN ISession* /*piSession*/,
        IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    ISIPHeader* piHeader = GetHistoryInfoHeader(piMessage);

    if (piHeader == IMS_NULL)
    {
        return bUpdate;
    }

    IMS_SINT32 nCause;

    if (GetCDIVCause(piHeader->GetSIPAddress(), nCause))
    {
        SuppService* pSuppService = new SuppService();

        pSuppService->nType = SUPP_TYPE_CDIV_CAUSE;
        pSuppService->nValue = ConvertCDIVCause(nCause);

        m_objSuppService.Add(pSuppService->nType, pSuppService);
        bUpdate = IMS_TRUE;
    }

    piHeader->Destroy();

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateCDIV_CAUSE", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCDIV_HISTORY(IN ISession* /*piSession*/,
        IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    ISIPHeader* piHeader = GetHistoryInfoHeader(piMessage);

    if (piHeader != IMS_NULL)
    {
        AString strTarget;

        if (GetCDIVTarget(piHeader->GetSIPAddress(), strTarget))
        {
            SuppService* pSuppService = new SuppService();

            pSuppService->nType = SUPP_TYPE_CDIV_HISTORY;
            pSuppService->aStrValue = strTarget;

            m_objSuppService.Add(pSuppService->nType, pSuppService);
            bUpdate = IMS_TRUE;
        }

        piHeader->Destroy();
    }

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateCDIV_HISTORY", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCW(IN ISession* /*piSession*/, IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (MessageUtil::IsHeaderPresent(piMessage, ISIPHeader::UNKNOWN,
            SIPHeaderName::ALERT_INFO) == IMS_TRUE)
    {
        IMS_TRACE_D("Alert Info header is present, Display the toast", 0, 0, 0);

        SuppService* pSuppService = new SuppService();
        pSuppService->nType = SUPP_TYPE_CW;
        pSuppService->bValue = IMS_TRUE;

        m_objSuppService.Add(pSuppService->nType, pSuppService);
        bUpdate = IMS_TRUE;
    }

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateCW", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateUSSD(IN ISession* /*piSession*/, IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IsIncomingUSSDCall(piMessage);

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateUSSD", 0, 0, 0);
        SuppService* pSuppService = new SuppService();

        pSuppService->nType = SUPP_TYPE_USSD;
        pSuppService->aStrValue = "true";

        m_objSuppService.Add(pSuppService->nType, pSuppService);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateVM(IN ISession* /*piSession*/, IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateVM", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateAnswerHold(IN ISession* /*piSession*/,
        IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateAnswerHold", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateMCID(IN ISession* /*piSession*/, IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;
    IMS_BOOL bUseMCID = m_objConfigurationProxy.Is(Feature::USE_MCID_SUPPLEMENTARY_SERVICE);

    if (bUseMCID)
    {

    }

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateMCID", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateDualNumber(IN ISession* /*piSession*/,
        IN IMessage* /*piMessage*/)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    if (bUpdate)
    {
        IMS_TRACE_I("UpdateDualNumber", 0, 0, 0);
    }

    return bUpdate;
}

PUBLIC
IMS_BOOL MtcSupplementaryService::UpdateCallingNumVerification(IN ISession* /*piSession*/,
        IN IMessage* piMessage)
{
    IMS_BOOL bUpdate = IMS_FALSE;

    IMS_SINT32 nHeaderType = GetCNVHeaderType(piMessage);

    AString strValue;
    MessageUtil::GetParameterValueFromUri(piMessage, STR_VERSTAT, nHeaderType, strValue,
            AString::ConstNull());

    SuppService* pSuppService = new SuppService();

    pSuppService->nType = SUPP_TYPE_CALLING_NUM_VERIFICATION;
    pSuppService->nValue = GetCallingNumVerificationResult(strValue);

    bUpdate = IMS_TRUE;

    m_objSuppService.Add(pSuppService->nType, pSuppService);
    IMS_TRACE_D("UpdateCallingNumVerification : verstat[%d]", pSuppService->nValue, 0, 0);

    return bUpdate;
}

PUBLIC
void MtcSupplementaryService::Add(IN SuppService* pService)
{
    if (pService == IMS_NULL)
    {
        IMS_TRACE_E(0, "pSession is NULL", 0, 0, 0);
        return;
    }

    IMS_SLONG nIndex = m_objSuppService.GetIndexOfKey(pService->nType);

    if (nIndex >= 0)
    {
        SuppService* pSuppService = m_objSuppService.GetValueAt(nIndex);
        delete pSuppService;
    }

    m_objSuppService.Add(pService->nType, pService);

    IMS_TRACE_I("Add : size[%d] Type[%d]", m_objSuppService.GetSize(), pService->nType, 0);
}

PUBLIC
void MtcSupplementaryService::Delete(IN IMS_UINT32 nType)
{
    IMS_SLONG nIndex = m_objSuppService.GetIndexOfKey(nType);

    if (nIndex >= 0)
    {
        SuppService* pSuppService = m_objSuppService.GetValueAt(nIndex);
        delete pSuppService;
        m_objSuppService.RemoveAt(nIndex);
        IMS_TRACE_I("Delete : size[%d] Type[%d]", m_objSuppService.GetSize(), nType, 0);
        return;
    }

    IMS_TRACE_I("Delete : NoT Matched Size[%d]", m_objSuppService.GetSize(), 0, 0);
}

PUBLIC
void MtcSupplementaryService::DeleteAll()
{
    IMS_UINT32 nSize = m_objSuppService.GetSize();

    IMS_TRACE_I("DeleteAll : Size[%d]", nSize, 0, 0);

    for (IMS_UINT32 index = 0; index < nSize; index++)
    {
        SuppService* pService = m_objSuppService.GetValueAt(index);
        delete pService;
    }

    m_objSuppService.Clear();
}

PUBLIC
const SuppService* MtcSupplementaryService::Get(IN IMS_UINT32 eType)
{
    IMS_SLONG nIndex = m_objSuppService.GetIndexOfKey(eType);

    if (nIndex >= 0)
    {
        SuppService* pSuppService = m_objSuppService.GetValueAt(nIndex);
        return pSuppService;
    }

    IMS_TRACE_I("Get : NoT Matched Size[%d]", m_objSuppService.GetSize(), 0, 0);
    return IMS_NULL;
}

PUBLIC
IMSMap<IMS_UINT32, SuppService*>& MtcSupplementaryService::GetAll()
{
    return m_objSuppService;
}

PRIVATE
ISIPHeader* MtcSupplementaryService::GetHistoryInfoHeader(IN IMessage* piMessage)
{
    IMSList<AString> lstHistoryInfos;
    MessageUtil::GetHeaders(piMessage, ISIPHeader::HISTORY_INFO, lstHistoryInfos);
    if (lstHistoryInfos.IsEmpty())
    {
        return IMS_NULL;
    }

    const AString& strHistoryInfo = lstHistoryInfos.GetAt(lstHistoryInfos.GetSize() - 1);
    if (strHistoryInfo.GetLength() == 0)
    {
        return IMS_NULL;
    }

    return SIPParsingHelper::CreateHeader(SIPHeaderName::HISTORY_INFO, strHistoryInfo);
}

PRIVATE
IMS_BOOL MtcSupplementaryService::GetCDIVCause(IN const SIPAddress* pAddress,
        OUT IMS_SINT32& nCause)
{
    if (pAddress != IMS_NULL)
    {
        const SIPParameter* pSIPParameter = pAddress->GetParameter("cause");

        if (pSIPParameter != IMS_NULL)
        {
            const AString strCause = pSIPParameter->GetValue();

            if (strCause.GetLength() > 0)
            {
                IMS_TRACE_D("GetCDIVCause : Cause=%s", strCause.GetStr(), 0, 0);

                nCause = strCause.ToInt32();
                return IMS_TRUE;
            }
        }
    }

    return IMS_FALSE;
}

PRIVATE
IMS_BOOL MtcSupplementaryService::GetCDIVTarget(IN const SIPAddress* pAddress,
        OUT AString& strTarget)
{
    if (pAddress == IMS_NULL)
    {
        IMS_TRACE_E(0, "GetCDIVTarget : pAddress is NULL", 0, 0, 0);
        return IMS_FALSE;
    }

    if (pAddress->IsSchemeSIP() || pAddress->IsSchemeSIPS())
    {
        strTarget = pAddress->GetUser();
    }
    else if (pAddress->IsSchemeTEL())
    {
        strTarget = pAddress->GetHost();
    }
    else
    {
        IMS_TRACE_I("GetCDIVTarget : Getting the target failed", 0, 0, 0);
        return IMS_FALSE;
    }

    IMS_TRACE_D("GetCDIVTarget : Target=%s", strTarget.GetStr(), 0, 0);
    return IMS_TRUE;
}

PRIVATE
IMS_SINT32 MtcSupplementaryService::ConvertCDIVCause(IN IMS_SINT32 nCause)
{
    IMS_SINT32 nCDIVCause;

    switch (nCause)
    {
        case 302:
            nCDIVCause = IuMtcService::CDIVCAUSE_UNCONDITION;
            break;
        case 404:
            nCDIVCause = IuMtcService::CDIVCAUSE_NOT_LOGGED_IN;
            break;
        case 408:
            nCDIVCause = IuMtcService::CDIVCAUSE_NO_REPLY;
            break;
        case 480:
            nCDIVCause = IuMtcService::CDIVCAUSE_DEFLECTION;
            break;
        case 486:
            nCDIVCause = IuMtcService::CDIVCAUSE_BUSY;
            break;
        case 487:
            nCDIVCause = IuMtcService::CDIVCAUSE_DEFLECTION_ALERTING;
            break;
        case 503:
            nCDIVCause = IuMtcService::CDIVCAUSE_NOT_REACHABLE;
            break;

        default:
            nCDIVCause = IuMtcService::CDIVCAUSE_NONE;
            break;
    }

    return nCDIVCause;
}

PRIVATE
IMS_UINT32 MtcSupplementaryService::GetCallingNumVerificationResult(IN AString& strValue)
{
    IMS_UINT32 nVerstatResult = CALLING_NUM_VERSTAT_NONE;

    if (strValue.GetLength() > 0)
    {
        if (strValue.EqualsIgnoreCase(STR_VERSTAT_TN_VALIDATION_PASSED))
        {
            nVerstatResult = CALLING_NUM_VERSTAT_VERIFIED;
        }
        else if (strValue.EqualsIgnoreCase(STR_VERSTAT_TN_VALIDATION_FAILED))
        {
            nVerstatResult = CALLING_NUM_VERSTAT_NOT_VERIFIED;
        }
    }

    IMS_TRACE_D("GetCallingNumVerificationResult : result is [%d]", nVerstatResult, 0, 0);
    return nVerstatResult;
}

PRIVATE
IMS_BOOL MtcSupplementaryService::IsIncomingUSSDCall(IN IMessage* piMessage)
{
    if (piMessage == IMS_NULL)
    {
        IMS_TRACE_D("IsIncomingUSSDCall : piMessage is null..", 0, 0, 0);
        return IMS_FALSE;
    }

    IMSList<AString> objAcceptHeaders;
    MessageUtil::GetHeaders(piMessage, ISIPHeader::ACCEPT, objAcceptHeaders);
    for (IMS_UINT32 index = 0; index < objAcceptHeaders.GetSize(); index++)
    {
        AString strAcceptHeader = objAcceptHeaders.GetAt(index);

        if (strAcceptHeader.Equals(USSDConstants::HEADER_APPLICATION_USSDXML))
        {
            IMS_TRACE_D("IsIncomingUSSDCall : Accept header has ussd+xml", 0, 0, 0);
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

PRIVATE
IMS_SINT32 MtcSupplementaryService::GetCNVHeaderType(IN IMessage* piMessage)
{
    if (m_nCnapType == CNAP_SCHEME_PAID ||
            (m_nCnapType == CNAP_SCHEME_PAID_FROM &&
            MessageUtil::IsHeaderPresent(piMessage, ISIPHeader::P_ASSERTED_IDENTITY)))
    {
        IMS_TRACE_D("GetCNVHeaderType - P_ASSERTED_IDENTITY", 0, 0, 0);
        return ISIPHeader::P_ASSERTED_IDENTITY;
    }

    IMS_TRACE_D("GetCNVHeaderType - FROM", 0, 0, 0);
    return ISIPHeader::FROM;
}

PRIVATE
void MtcSupplementaryService::LoadConfig()
{
    IMS_BOOL bOipFrom = m_objConfigurationProxy.Is(Feature::OIP_SOURCE_FROM_HEADER);

    if (bOipFrom)
    {
        m_nCnapType = CNAP_SCHEME_FROM;
    }

    IMS_TRACE_I("LoadConfig : Done", 0, 0, 0);
}
