/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "SipPrivate.h"
#include "ISipErrorListener.h"
#include "ISipClientConnectionListener.h"
#include "SipConnectionNotifierImpl.h"
#include "SipDialogImpl.h"
#include "SipClientConnection.h"
#include "SipClientConnectionImpl.h"

__IMS_TRACE_TAG_SIP__;

PUBLIC
SipClientConnectionImpl::SipClientConnectionImpl(IN SipClientConnection* pSCC_) :
        piErrorListener(IMS_NULL),
        piListener(IMS_NULL),
        pDialogImpl(IMS_NULL),
        pSCC(pSCC_)
{
    pSCC->SetErrorListener(this);
    pSCC->SetListener(this);
}

PUBLIC VIRTUAL SipClientConnectionImpl::~SipClientConnectionImpl()
{
    //---------------------------------------------------------------------------------------------

    if (pDialogImpl != IMS_NULL)
    {
        pDialogImpl->Destroy();
    }

    if (pSCC != IMS_NULL)
    {
        pSCC->SetErrorListener(IMS_NULL);
        pSCC->SetListener(IMS_NULL);
        pSCC->Close();
    }
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipClientConnectionImpl::InitDialogRequest()
{
    //---------------------------------------------------------------------------------------------

    if (pSCC == IMS_NULL)
    {
        IMS_TRACE_E(0, "SCC is null", 0, 0, 0);
        return IMS_FAILURE;
    }

    if (pDialogImpl == IMS_NULL)
    {
        SipDialog* pDialog = pSCC->GetDialog();

        if (pDialog == IMS_NULL)
        {
            IMS_TRACE_E(0, "Dialog is null", 0, 0, 0);
            return IMS_FAILURE;
        }

        pDialogImpl = new SipDialogImpl(new SipDialog(*pDialog));

        if (pDialogImpl == IMS_NULL)
        {
            IMS_TRACE_E(0, "Creating SipDialogImpl failed", 0, 0, 0);
            return IMS_FAILURE;
        }
    }

    return IMS_SUCCESS;
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::Close()
{
    //---------------------------------------------------------------------------------------------

    if (pDialogImpl != IMS_NULL)
    {
        pDialogImpl->Destroy();
        pDialogImpl = IMS_NULL;
    }

    pSCC->SetErrorListener(IMS_NULL);
    pSCC->SetListener(IMS_NULL);
    pSCC->Close();
    pSCC = IMS_NULL;

    delete this;
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::AddHeader(
        IN CONST AString& strName, IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->AddHeader(strName, strValue);
}

/*

Remarks

*/
PRIVATE VIRTUAL ISipDialog* SipClientConnectionImpl::GetDialog() const
{
    //---------------------------------------------------------------------------------------------

    return pDialogImpl;
}

/*

Remarks

*/
PRIVATE VIRTUAL AString SipClientConnectionImpl::GetHeader(
        IN CONST AString& strName, IN IMS_SINT32 nIndex /* = 0 */)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetHeader(strName, nIndex);
}

/*

Remarks

*/
PRIVATE VIRTUAL IMSList<AString> SipClientConnectionImpl::GetHeaders(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetHeaders(strName);
}

/*

Remarks

*/
PRIVATE VIRTUAL const SipMethod& SipClientConnectionImpl::GetMethod() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetMethod();
}

/*

Remarks

*/
PRIVATE VIRTUAL const AString& SipClientConnectionImpl::GetReasonPhrase() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetReasonPhrase();
}

/*

Remarks

*/
PRIVATE VIRTUAL const AString& SipClientConnectionImpl::GetRequestUri() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetRequestUri();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_SINT32 SipClientConnectionImpl::GetStatusCode() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetStatusCode();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::RemoveHeader(IN CONST AString& strName)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->RemoveHeader(strName);
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::Send()
{
    IMS_RESULT nResult = pSCC->Send();

    // in-dialog & dialogUsage used
    if (nResult == IMS_SUCCESS)
    {
        SipDialog* pDialog = (pDialogImpl != IMS_NULL) ? pDialogImpl->GetDialog() : IMS_NULL;
        SipDialog* pSccDialog = pSCC->GetDialog();

        if ((pSccDialog != IMS_NULL) && (pDialog != IMS_NULL) &&
                (pDialog->GetState() == SipDialog::STATE_CONFIRMED))
        {
            const SipMethod& objMethod = GetMethod();

            if (objMethod.Equals(SipMethod::SUBSCRIBE) || objMethod.Equals(SipMethod::REFER))
            {
                *pDialog = *pSccDialog;
            }
        }
    }

    return nResult;
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetErrorListener(IN ISipErrorListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    piErrorListener = piListener;
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetHeader(
        IN CONST AString& strName, IN CONST AString& strValue)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetHeader(strName, strValue);
}

/*

Remarks

*/
PRIVATE VIRTUAL const ByteArray& SipClientConnectionImpl::GetContent() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetContent();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetContent(IN CONST ByteArray& objContent)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetContent(objContent);
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_SINT32 SipClientConnectionImpl::GetHeaderCount(IN CONST AString& strName) const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetHeaderCount(strName);
}

/*

Remarks

*/
PRIVATE VIRTUAL ISipMessage* SipClientConnectionImpl::GetMessage() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetMessage();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_SINT32 SipClientConnectionImpl::GetSlotId() const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetSlotId();
}

/*

Remarks
 MULTI_REG_SIP_PROFILE
*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetSipProfile(IN SipProfile* pProfile)
{
    //---------------------------------------------------------------------------------------------

    pSCC->SetSipProfile(pProfile);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetTransactionTimerValues(
        IN CONST SipTimerValues& objTV)
{
    //---------------------------------------------------------------------------------------------

    pSCC->SetTransactionTimerValues(objTV);
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::InitAck()
{
    //---------------------------------------------------------------------------------------------

    return pSCC->InitAck();
}

/*

Remarks

*/
PRIVATE VIRTUAL ISipClientConnection* SipClientConnectionImpl::InitCancel()
{
    //---------------------------------------------------------------------------------------------

    // 3 To-Tag removal needs to be handled by the user because the re-INVITE may be cancelled
    // 3 Session implementation has the responsibility of the to-tag removal.

    SipClientConnection* pCANCEL = pSCC->InitCancel();

    if (pCANCEL == IMS_NULL)
    {
        return IMS_NULL;
    }

    SipClientConnectionImpl* pCANCELImpl = new SipClientConnectionImpl(pCANCEL);

    if (pCANCELImpl == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::CONNECTION_NOT_FOUND);
        return IMS_NULL;
    }

    return pCANCELImpl;
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::InitRequest(
        IN CONST AString& strMethod, IN ISipConnectionNotifier* piSCN)
{
    SipConnectionNotifierImpl* pSCNImpl = DYNAMIC_CAST(SipConnectionNotifierImpl*, piSCN);
    SipConnectionNotifier* pSCN = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (pSCNImpl != IMS_NULL)
    {
        pSCN = pSCNImpl->GetConnectionNotifier();
    }

    if (pSCC->InitRequest(strMethod, pSCN) != IMS_SUCCESS)
    {
        return IMS_FAILURE;
    }

    SipDialog* pDialog = pSCC->GetDialog();

    if (pDialog != IMS_NULL)
    {
        pDialogImpl = new SipDialogImpl(new SipDialog(*pDialog));

        if (pDialogImpl == IMS_NULL)
        {
            IMS_TRACE_E(0, "Allocating DialogImpl failed", 0, 0, 0);
        }
    }

    return IMS_SUCCESS;
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::Receive(IN IMS_SLONG /* nTimeout = 0 */)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->Receive();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetCredentials(
        IN IMSList<Credential>& objCredentials)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetCredentials(objCredentials);
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetCredentials(
        IN CONST Credential& objCredential)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetCredentials(objCredential);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetListener(
        IN ISipClientConnectionListener* piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetRequestUri(IN CONST AString& strURI)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetRequestUri(strURI);
}

/*

Remarks

*/
PRIVATE VIRTUAL ISipGenericChallenge* SipClientConnectionImpl::GetAuthenticationChallenge(
        IN IMS_SINT32 nIndex /* = 0 */) const
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GetAuthenticationChallenge(nIndex);
}

/*

Remarks

*/
PRIVATE VIRTUAL ISipAckPackage* SipClientConnectionImpl::GrabAck()
{
    //---------------------------------------------------------------------------------------------

    return pSCC->GrabAck();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::InitResubmissionRequest()
{
    //---------------------------------------------------------------------------------------------

    return pSCC->InitResubmissionRequest();
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::RemoveAllChallenges()
{
    //---------------------------------------------------------------------------------------------

    pSCC->RemoveAllChallenges();
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::RemoveAllCredentials()
{
    //---------------------------------------------------------------------------------------------

    pSCC->RemoveAllCredentials();
}

/*

Remarks

*/
PRIVATE VIRTUAL IMS_RESULT SipClientConnectionImpl::SetAuthenticationChallenge(
        IN ISipGenericChallenge* piChallenge)
{
    //---------------------------------------------------------------------------------------------

    return pSCC->SetAuthenticationChallenge(piChallenge);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetExtensionTokenForViaBranch(
        IN CONST AString& strToken)
{
    //---------------------------------------------------------------------------------------------

    pSCC->SetExtensionTokenForViaBranch(strToken);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetImplicitRouteHeader(
        IN CONST AString& strRouteHeader)
{
    //---------------------------------------------------------------------------------------------

    pSCC->SetImplicitRouteHeader(strRouteHeader);
}

/*

Remarks
 RFC5626_FLOW_CONTROL, MULTI_REG_TRANSPORT
*/
PRIVATE VIRTUAL void SipClientConnectionImpl::SetTransportTuple(IN CONST IPAddress& objIPA,
        IN IMS_SINT32 nPortS, IN IMS_SINT32 nPortC, IN IMS_SINT32 nPortFC /* = 0xFFFF */,
        IN IMS_SINT32 nTransportExt /* = 0 (ANY) */)
{
    //---------------------------------------------------------------------------------------------

    pSCC->SetTransportTuple(objIPA, nPortS, nPortC, nPortFC, nTransportExt);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::OnError_NotifyError(
        IN SipConnection* pSC, IN IMS_SINT32 nCode, IN CONST AString& strMessage)
{
    //---------------------------------------------------------------------------------------------

    if (pSCC != pSC)
    {
        IMS_TRACE_E(0, "SCC MISMATCHED", 0, 0, 0);
        return;
    }

    if (piErrorListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piErrorListener->Error_NotifyError(this, nCode, strMessage);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::OnClientConnection_NotifyResponse(
        IN SipClientConnection* pSCC)
{
    //---------------------------------------------------------------------------------------------

    if (this->pSCC != pSCC)
    {
        IMS_TRACE_E(0, "SCC MISMATCHED", 0, 0, 0);
        return;
    }

    if (piListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piListener->ClientConnection_NotifyResponse(this);
}

/*

Remarks

*/
PRIVATE VIRTUAL void SipClientConnectionImpl::OnClientConnection_NotifyForkedResponse(
        IN SipClientConnection* pSCC, IN SipClientConnection* pForkedSCC)
{
    //---------------------------------------------------------------------------------------------

    if (this->pSCC != pSCC)
    {
        IMS_TRACE_E(0, "SCC MISMATCHED", 0, 0, 0);
        return;
    }

    if (piListener == IMS_NULL)
    {
        pForkedSCC->Close();

        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    SipClientConnectionImpl* pSCCImpl = new SipClientConnectionImpl(pForkedSCC);

    if (pSCCImpl == IMS_NULL)
    {
        pForkedSCC->Close();
        return;
    }

    SipDialog* pDialog = pForkedSCC->GetDialog();

    if (pDialog != IMS_NULL)
    {
        pSCCImpl->pDialogImpl = new SipDialogImpl(new SipDialog(*pDialog));

        if (pSCCImpl->pDialogImpl == IMS_NULL)
        {
            IMS_TRACE_E(0, "Allocating DialogImpl failed", 0, 0, 0);
        }
    }

    piListener->ClientConnection_NotifyResponse(this, pSCCImpl);
}
