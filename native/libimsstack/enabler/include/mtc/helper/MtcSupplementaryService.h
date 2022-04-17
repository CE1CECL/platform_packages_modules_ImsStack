#ifndef MTC_SUPPLEMENTARY_SERVICE_H_
#define MTC_SUPPLEMENTARY_SERVICE_H_

#include "IMSMap.h"
#include "MtcDef.h"

class ISession;
class IMessage;
class ISIPHeader;
class SIPAddress;
class MtcConfigurationProxy;

class MtcSupplementaryService final
{
public:
    explicit MtcSupplementaryService(IN MtcConfigurationProxy& objConfigurationProxy,
            IN IMSMap<IMS_UINT32, SuppService*> objSuppServices =
                    IMSMap<IMS_UINT32, SuppService*>());
    ~MtcSupplementaryService();
    MtcSupplementaryService(const MtcSupplementaryService&) = delete;
    MtcSupplementaryService& operator=(const MtcSupplementaryService&) = delete;

    void UpdateService(IN const IMSMap<IMS_UINT32, SuppService*>& objSuppServices);
    IMS_BOOL UpdateService(IN ISession* piSession, IN IMessage* piMessage);

    IMS_BOOL UpdateCallerID(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCnap(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCNAPEX(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateMMC(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateGTT(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCDIV_CAUSE(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCDIV_HISTORY(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCW(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateUSSD(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateVM(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateAnswerHold(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateMCID(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateDualNumber(IN ISession* piSession, IN IMessage* piMessage);
    IMS_BOOL UpdateCallingNumVerification(IN ISession* piSession, IN IMessage* piMessage);
    void Add(IN SuppService* pService);
    void Delete(IN IMS_UINT32 eType);
    void DeleteAll();
    const SuppService* Get(IN IMS_UINT32 eType);
    IMSMap<IMS_UINT32, SuppService*>& GetAll();

private:
    ISIPHeader* GetHistoryInfoHeader(IN IMessage* piMessage);
    IMS_BOOL GetCDIVCause(IN const SIPAddress* pAddress, OUT IMS_SINT32& nCause);
    IMS_BOOL GetCDIVTarget(IN const SIPAddress* pAddress, OUT AString &strTarget);
    IMS_SINT32 ConvertCDIVCause(IN IMS_SINT32 nCause);
    IMS_UINT32 GetCallingNumVerificationResult(IN AString& strValue);
    IMS_BOOL IsIncomingUSSDCall(IN IMessage* piMessage);
    IMS_SINT32 GetCNVHeaderType(IN IMessage* piMessage);
    void LoadConfig();

private:
    IMSMap<IMS_UINT32, SuppService*> m_objSuppService;
    MtcConfigurationProxy& m_objConfigurationProxy;
    IMS_SINT32 m_nCnapType;

    static const IMS_CHAR STR_VERSTAT[];
    static const IMS_CHAR STR_VERSTAT_TN_VALIDATION_PASSED[];
    static const IMS_CHAR STR_VERSTAT_TN_VALIDATION_FAILED[];
};

#endif
