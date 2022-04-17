/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100503  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "media/IMediaListener.h"
#include "media/IFramedMediaListener.h"
#include "media/MediaDescriptor.h"
#include "media/FramedMediaProposalImpl.h"
#include "media/FramedMediaImpl.h"

__IMS_TRACE_TAG_IMS_CORE__;



PUBLIC
FramedMediaImpl::FramedMediaImpl(IN FramedMedia *pFramedMedia_)
    : piMediaListener(IMS_NULL)
    , piFramedMediaListener(IMS_NULL)
    , pFramedMediaProposal(IMS_NULL)
    , pFramedMedia(pFramedMedia_)
{
    pFramedMedia->SetMediaListener(this);
    pFramedMedia->SetListener(this);
}

PUBLIC VIRTUAL
FramedMediaImpl::~FramedMediaImpl()
{
    if (pFramedMediaProposal != IMS_NULL)
    {
        delete pFramedMediaProposal;
        pFramedMediaProposal = IMS_NULL;
    }
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaImpl::Equals(IN CONST IMedia *piMedia) const
{
    const FramedMediaImpl *pMediaImpl = DYNAMIC_CAST(const FramedMediaImpl*, piMedia);

    //---------------------------------------------------------------------------------------------

    if (pMediaImpl == IMS_NULL)
    {
        return IMS_FALSE;
    }

    return (this == pMediaImpl);
}

PRIVATE VIRTUAL
IMedia* FramedMediaImpl::GetInterface()
{
    //---------------------------------------------------------------------------------------------

    return this;
}

PRIVATE VIRTUAL
Media* FramedMediaImpl::GetMedia() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia;
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaImpl::CanRead() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->CanRead();
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaImpl::CanWrite() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->CanWrite();
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaImpl::Exists() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->Exists();
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaImpl::GetDirection() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetDirection();
}

PRIVATE VIRTUAL
IMSList<IMediaDescriptor*> FramedMediaImpl::GetMediaDescriptors() const
{
    const IMSList<MediaDescriptor*> &objMediaDescriptors = pFramedMedia->GetMediaDescriptors();

    //---------------------------------------------------------------------------------------------
    /*
    if (IMSError::GetLastError() != IMSError::NO_ERROR)
    {
        IMS_TRACE_E(0, "Getting MediaDescriptors failed - %d", IMSError::GetLastError(), 0, 0);
        return IMSList<IMediaDescriptor*>();
    }
    */

    if (objMediaDescriptors.IsEmpty())
    {
        IMS_TRACE_E(0, "No media descriptors in the current media", 0, 0, 0);
        return IMSList<IMediaDescriptor*>();
    }

    IMSList<IMediaDescriptor*> objIMediaDescriptors;

    for (IMS_UINT32 i = 0; i < objMediaDescriptors.GetSize(); ++i)
    {
        objIMediaDescriptors.Append(objMediaDescriptors.GetAt(i));
    }

    return objIMediaDescriptors;
}

PRIVATE VIRTUAL
IMedia* FramedMediaImpl::GetProposal(IN IMS_BOOL bIMSExtension /* = IMS_TRUE */) const
{
    //---------------------------------------------------------------------------------------------

    if ((GetState() != STATE_ACTIVE) || (GetUpdateState() != UPDATE_MODIFIED))
    {
        if (bIMSExtension && (GetState() == STATE_ACTIVE) && (GetUpdateState() == UPDATE_REMOVED))
        {
        }
        else
        {
            return IMS_NULL;
        }
    }

    return pFramedMediaProposal;
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaImpl::GetState() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetState();
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaImpl::GetUpdateState() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetUpdateState();
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaImpl::SetDirection(IN IMS_SINT32 nDirection)
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->SetDirection(nDirection);
}

PRIVATE VIRTUAL
void FramedMediaImpl::SetMediaListener(IN IMediaListener *piListener)
{
    //---------------------------------------------------------------------------------------------

    piMediaListener = piListener;
}

PRIVATE VIRTUAL
IMediaDescriptor* FramedMediaImpl::GetMediaDescriptor() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetMediaDescriptor();
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaImpl::GetType() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetType();
}

PRIVATE VIRTUAL
void FramedMediaImpl::RemoveMediaDescriptor(IN IMS_UINT32 nPosition)
{
    //---------------------------------------------------------------------------------------------

    pFramedMedia->RemoveMediaDescriptor(nPosition);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnMedia_FictitiousMediaCreated(IN Media *pMedia)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    FramedMediaProposal *pMediaProposal
            = DYNAMIC_CAST(FramedMediaProposal*, pFramedMedia->GetProposal());

    if (pMediaProposal == IMS_NULL)
    {
        // Do nothing
        IMS_TRACE_E(0, "NO MEDIA PROPOSAL", 0, 0, 0);
        return;
    }

    if (pFramedMediaProposal != IMS_NULL)
    {
        delete pFramedMediaProposal;
        pFramedMediaProposal = IMS_NULL;
    }

    pFramedMediaProposal = new FramedMediaProposalImpl(pMediaProposal);

    if (pFramedMediaProposal == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO MEMORY", 0, 0, 0);
        return;
    }
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnMedia_FictitiousMediaDestroyed(IN Media *pMedia)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (pFramedMediaProposal != IMS_NULL)
    {
        delete pFramedMediaProposal;
        pFramedMediaProposal = IMS_NULL;
    }
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnMedia_ModeChanged(IN Media *pMedia)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piMediaListener->Media_ModeChanged(this);
}

// IFramedMedia interface
PRIVATE VIRTUAL
void FramedMediaImpl::Cancel(IN CONST AString &strMessageId)
{
    //---------------------------------------------------------------------------------------------

    pFramedMedia->Cancel(strMessageId);
}

PRIVATE VIRTUAL
const AStringArray& FramedMediaImpl::GetAcceptedContentTypes() const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetAcceptedContentTypes();
}

PRIVATE VIRTUAL
const AString& FramedMediaImpl::GetContentType(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetContentType(strMessageId);
}

PRIVATE VIRTUAL
AString FramedMediaImpl::GetHeader(IN CONST AString &strMessageId, IN CONST AString &strName) const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->GetHeader(strMessageId, strName);
}

PRIVATE VIRTUAL
const ByteArray& FramedMediaImpl::ReceiveBytes(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->ReceiveBytes(strMessageId);
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaImpl::ReceiveFile(IN CONST AString &strMessageId, IN CONST AString &strLocator)
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->ReceiveFile(strMessageId, strLocator);
}

PRIVATE VIRTUAL
AString FramedMediaImpl::SendBytes(IN CONST ByteArray &objContent, IN CONST AString &strContentType,
        IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->SendBytes(objContent, strContentType, objHeaders);
}

PRIVATE VIRTUAL
AString FramedMediaImpl::SendFile(IN CONST AString &strLocator, IN CONST AString &strContentType,
        IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->SendFile(strLocator, strContentType, objHeaders);
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaImpl::SetAcceptedContentTypes(IN CONST AStringArray &objAcceptedContentTypes)
{
    //---------------------------------------------------------------------------------------------

    return pFramedMedia->SetAcceptedContentTypes(objAcceptedContentTypes);
}

PRIVATE VIRTUAL
void FramedMediaImpl::SetListener(IN IFramedMediaListener *piListener)
{
    //---------------------------------------------------------------------------------------------

    piFramedMediaListener = piListener;
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_ConnectionError(IN FramedMedia *pMedia)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_ConnectionError(this);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_ContentReceived(IN FramedMedia *pMedia,
        IN CONST AString &strMessageId, IN IMS_SINT32 nSize, IN CONST AString &strFileName)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_ContentReceived(this, strMessageId, nSize, strFileName);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_ContentReceiveFailed(IN FramedMedia *pMedia,
        IN CONST AString &strMessageId)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_ContentReceiveFailed(this, strMessageId);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_DeliveryFailure(IN FramedMedia *pMedia,
        IN CONST AString &strMessageId, IN IMS_SINT32 nStatusCode,
        IN CONST AString &strReasonPhrase)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_DeliveryFailure(this,
            strMessageId, nStatusCode, strReasonPhrase);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_DeliverySuccess(IN FramedMedia *pMedia,
        IN CONST AString &strMessageId)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_DeliverySuccess(this, strMessageId);
}

PRIVATE VIRTUAL
void FramedMediaImpl::OnFramedMedia_TransferProgress(IN FramedMedia *pMedia,
        IN CONST AString &strMessageId, IN IMS_SINT32 nTransferredBytes,
        IN IMS_SINT32 nTotalBytes)
{
    //---------------------------------------------------------------------------------------------

    if (pFramedMedia != pMedia)
    {
        IMS_TRACE_E(0, "MEDIA MISMATCHED", 0, 0, 0);
        return;
    }

    if (piFramedMediaListener == IMS_NULL)
    {
        IMS_TRACE_E(0, "NO LISTENER", 0, 0, 0);
        return;
    }

    piFramedMediaListener->FramedMedia_TransferProgress(this,
            strMessageId, nTransferredBytes, nTotalBytes);
}
