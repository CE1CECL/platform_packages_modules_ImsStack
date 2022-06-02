/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_CONNECTION_H_
#define _SIP_CONNECTION_H_

#include "Connection.h"
#include "ISipTransactionStateListener.h"
#include "ISipTransportErrorListener.h"
#include "SipMessage.h"
#include "SipDialog.h"
#include "SipTimerValues.h"

class SipProfile;
class IOnSipErrorListener;

class SipConnection :
        public Connection,
        public ISipTransactionStateListener,
        public ISipTransportErrorListener
{
protected:
    SipConnection();
    virtual ~SipConnection();

private:
    SipConnection(IN CONST SipConnection& objRHS);
    SipConnection& operator=(IN CONST SipConnection& objRHS);

public:
    // IConnection interface
    virtual void Close();

    // ISipConnection interface
    virtual IMS_RESULT AddHeader(IN CONST AString& strName, IN CONST AString& strValue);
    virtual SipDialog* GetDialog() const;
    virtual AString GetHeader(IN CONST AString& strName, IN IMS_SINT32 nIndex = 0);
    virtual IMSList<AString> GetHeaders(IN CONST AString& strName);
    virtual const SipMethod& GetMethod() const;
    virtual const AString& GetReasonPhrase() const;
    virtual const AString& GetRequestUri() const;
    virtual IMS_SINT32 GetStatusCode() const;
    virtual IMS_RESULT RemoveHeader(IN CONST AString& strName);
    virtual IMS_RESULT Send();
    virtual void SetErrorListener(IN IOnSipErrorListener* piListener);
    virtual IMS_RESULT SetHeader(IN CONST AString& strName, IN CONST AString& strValue);
    virtual const ByteArray& GetContent() const;
    virtual IMS_RESULT SetContent(IN CONST ByteArray& objContent);
    // IMS extensions
    virtual IMS_SINT32 GetHeaderCount(IN CONST AString& strName) const;
    inline virtual ISipMessage* GetMessage() const { return pMessage; }
    // MULTI_REG_SIP_PROFILE
    virtual void SetSipProfile(IN SipProfile* pProfile);
    void SetTransactionTimerValues(IN CONST SipTimerValues& objTV);

protected:
    // ISipTransactionStateListener interface
    virtual void TransactionState_TimerExpired();

    // ISipTransportErrorListener interface
    virtual void TransportError_NotifyError(IN IMS_SINT32 nCode, IN CONST AString& strMessage);

    // SIP_TRANSPORT_ERROR_REPORT_ON_TXN
    virtual IMS_BOOL IsTransportErrorReportRequired(
            IN IMS_SINT32 nCode, IN CONST AString& strMessage) const;

    SipTimerValues* GetTransactionTimerValues() const;
    void InitMessage(IN sipcore::SipMessage* pMessage_ = IMS_NULL,
            IN IMS_SINT32 nType_ = sipcore::SipMessage::TYPE_REQUEST);
    void NotifyError(IN IMS_SINT32 nCode, IN CONST AString& strMessage);

private:
    static IMS_BOOL IsCommaSeparatedListHeader(IN IMS_SINT32 nHType, IN CONST AString& strHName);
    static IMS_BOOL IsInaccessibleHeader(IN IMS_SINT32 nHType, IN CONST AString& strHName);
    static IMS_BOOL IsReadOnlyHeader(IN IMS_SINT32 nHType, IN CONST AString& strHName);

protected:
    IOnSipErrorListener* piErrorListener;

    SipDialog* pDialog;
    sipcore::SipMessage* pMessage;

    // SIP transaction timer values
    SipTimerValues* pTV;
};

#endif  // _SIP_CONNECTION_H_
