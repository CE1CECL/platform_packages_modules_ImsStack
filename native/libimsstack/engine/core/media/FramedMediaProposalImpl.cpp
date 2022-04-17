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
#include "base/IMS.h"
#include "media/MediaDescriptor.h"
#include "media/FramedMediaProposalImpl.h"

__IMS_TRACE_TAG_IMS_CORE__;



PUBLIC
FramedMediaProposalImpl::FramedMediaProposalImpl(IN FramedMediaProposal *pMediaProposal_)
    : pMediaProposal(pMediaProposal_)
{
}

PUBLIC VIRTUAL
FramedMediaProposalImpl::~FramedMediaProposalImpl()
{
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaProposalImpl::CanRead() const
{
    //---------------------------------------------------------------------------------------------

    return IMS_FALSE;
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaProposalImpl::CanWrite() const
{
    //---------------------------------------------------------------------------------------------

    return IMS_FALSE;
}

PRIVATE VIRTUAL
IMS_BOOL FramedMediaProposalImpl::Exists() const
{
    //---------------------------------------------------------------------------------------------

    return IMS_FALSE;
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaProposalImpl::GetDirection() const
{
    //---------------------------------------------------------------------------------------------

    return pMediaProposal->GetDirection();
}

PRIVATE VIRTUAL
IMSList<IMediaDescriptor*> FramedMediaProposalImpl::GetMediaDescriptors() const
{
    const IMSList<MediaDescriptor*> &objMediaDescriptors = pMediaProposal->GetMediaDescriptors();

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
IMedia* FramedMediaProposalImpl::GetProposal(IN IMS_BOOL /* bIMSExtension = IMS_TRUE */) const
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_STATE);

    return IMS_NULL;
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaProposalImpl::GetState() const
{
    //---------------------------------------------------------------------------------------------

    return STATE_PROPOSAL;
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaProposalImpl::GetUpdateState() const
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_STATE);

    return UPDATE_INVALID;
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaProposalImpl::SetDirection(IN IMS_SINT32 nDirection)
{
    //---------------------------------------------------------------------------------------------

    (void) nDirection;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_STATE);

    return IMS_FAILURE;
}

PRIVATE VIRTUAL
void FramedMediaProposalImpl::SetMediaListener(IN IMediaListener *piListener)
{
    //---------------------------------------------------------------------------------------------

    (void) piListener;
}

PRIVATE VIRTUAL
IMediaDescriptor* FramedMediaProposalImpl::GetMediaDescriptor() const
{
    //---------------------------------------------------------------------------------------------

    return pMediaProposal->GetMediaDescriptor();
}

PRIVATE VIRTUAL
IMS_SINT32 FramedMediaProposalImpl::GetType() const
{
    //---------------------------------------------------------------------------------------------

    return pMediaProposal->GetType();
}

PRIVATE VIRTUAL
void FramedMediaProposalImpl::RemoveMediaDescriptor(IN IMS_UINT32 nPosition)
{
    //---------------------------------------------------------------------------------------------

    (void) nPosition;
}

// IFramedMedia interface
PRIVATE VIRTUAL
void FramedMediaProposalImpl::Cancel(IN CONST AString &strMessageId)
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
}

PRIVATE VIRTUAL
const AStringArray& FramedMediaProposalImpl::GetAcceptedContentTypes() const
{
    //---------------------------------------------------------------------------------------------

    return pMediaProposal->GetAcceptedContentTypes();
}

PRIVATE VIRTUAL
const AString& FramedMediaProposalImpl::GetContentType(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);

    return AString::ConstNull();
}


PRIVATE VIRTUAL
AString FramedMediaProposalImpl::GetHeader(IN CONST AString &strMessageId,
        IN CONST AString &strName) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
    (void) strName;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);

    return AString::ConstNull();
}

PRIVATE VIRTUAL
const ByteArray& FramedMediaProposalImpl::ReceiveBytes(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);

    return ByteArray::ConstNull();
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaProposalImpl::ReceiveFile(IN CONST AString &strMessageId,
        IN CONST AString &strLocator)
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
    (void) strLocator;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);

    return IMS_FAILURE;
}

PRIVATE VIRTUAL
AString FramedMediaProposalImpl::SendBytes(IN CONST ByteArray &objContent,
        IN CONST AString &strContentType, IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    (void) objContent;
    (void) strContentType;
    (void) objHeaders;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::INVALID_OPERATION);

    return AString::ConstNull();
}

PRIVATE VIRTUAL
AString FramedMediaProposalImpl::SendFile(IN CONST AString &strLocator,
        IN CONST AString &strContentType, IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    (void) strLocator;
    (void) strContentType;
    (void) objHeaders;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::INVALID_OPERATION);

    return AString::ConstNull();
}

PRIVATE VIRTUAL
IMS_RESULT FramedMediaProposalImpl::SetAcceptedContentTypes(
        IN CONST AStringArray &objAcceptedContentTypes)
{
    //---------------------------------------------------------------------------------------------

    (void) objAcceptedContentTypes;

    IMS_TRACE_E(0, "Operation not allowed in media proposal", 0, 0, 0);

    IMS::SetLastError(IMSError::ILLEGAL_STATE);

    return IMS_FAILURE;
}

PRIVATE VIRTUAL
void FramedMediaProposalImpl::SetListener(IN IFramedMediaListener *piListener)
{
    //---------------------------------------------------------------------------------------------

    (void) piListener;
}
