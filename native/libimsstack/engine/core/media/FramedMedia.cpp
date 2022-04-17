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
#include "TextParser.h"
#include "base/IMS.h"
#include "IMSCore.h"
#include "media/IOnFramedMediaListener.h"
#include "media/MediaDescriptor.h"
#include "media/FramedMediaProposal.h"
#include "media/FramedMedia.h"

__IMS_TRACE_TAG_IMS_CORE__;



PUBLIC
FramedMedia::FramedMedia(IN Service *pService_, IN ISDPOAState *piOAState_)
    : Media(pService_, piOAState_)
    , piListener(IMS_NULL)
{
    SetInitializationDone(IMS_TRUE);
}

PUBLIC VIRTUAL
FramedMedia::~FramedMedia()
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_D("Destructor :: FramedMedia", 0, 0, 0);
}

PUBLIC VIRTUAL
IMS_BOOL FramedMedia::CanRead() const
{
    //---------------------------------------------------------------------------------------------

    // TODO::

    return IMS_FALSE;
}

PUBLIC VIRTUAL
IMS_BOOL FramedMedia::Exists() const
{
    //---------------------------------------------------------------------------------------------

    // TODO::

    return IMS_FALSE;
}

PUBLIC VIRTUAL
IMS_SINT32 FramedMedia::GetType() const
{
    //---------------------------------------------------------------------------------------------

    return IMSCore::MEDIA_TYPE_FRAMED;
}

PUBLIC
void FramedMedia::Cancel(IN CONST AString &strMessageId)
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
}

PUBLIC
const AStringArray& FramedMedia::GetAcceptedContentTypes() const
{
    //---------------------------------------------------------------------------------------------

    return objAcceptedContentTypes;
}

PUBLIC
const AString& FramedMedia::GetContentType(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;

    return AString::ConstNull();
}

PUBLIC
AString FramedMedia::GetHeader(IN CONST AString &strMessageId, IN CONST AString &strName) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
    (void) strName;

    return AString::ConstNull();
}

PUBLIC
const ByteArray& FramedMedia::ReceiveBytes(IN CONST AString &strMessageId) const
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;

    if (!CanRead())
    {
        IMS::SetLastError(IMSError::INVALID_OPERATION);
        return ByteArray::ConstNull();
    }

    IMS::SetLastError(IMSError::NO_ERROR);

    return ByteArray::ConstNull();
}

PUBLIC
IMS_RESULT FramedMedia::ReceiveFile(IN CONST AString &strMessageId, IN CONST AString &strLocator)
{
    //---------------------------------------------------------------------------------------------

    (void) strMessageId;
    (void) strLocator;

    if (!CanRead())
    {
        IMS::SetLastError(IMSError::INVALID_OPERATION);
        return IMS_FAILURE;
    }

    IMS::SetLastError(IMSError::NO_ERROR);

    return IMS_FAILURE;
}

PUBLIC
AString FramedMedia::SendBytes(IN CONST ByteArray &objContent, IN CONST AString &strContentType,
        IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    if (objContent.IsNULL())
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (strContentType.IsNULL())
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (!TextParser::IsValidMediaType(strContentType))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (!objAcceptedContentTypes.Contains(strContentType, IMS_FALSE))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (!CanWrite())
    {
        IMS::SetLastError(IMSError::INVALID_OPERATION);
        return AString::ConstNull();
    }

    (void) objHeaders;

    IMS::SetLastError(IMSError::NO_ERROR);

    return AString::ConstNull();
}

PUBLIC
AString FramedMedia::SendFile(IN CONST AString &strLocator, IN CONST AString &strContentType,
        IN CONST IMSMap<AString, AStringArray> &objHeaders)
{
    //---------------------------------------------------------------------------------------------

    if (strContentType.IsNULL())
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (!TextParser::IsValidMediaType(strContentType))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    if (!objAcceptedContentTypes.Contains(strContentType, IMS_FALSE))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return AString::ConstNull();
    }

    (void) strLocator;
    (void) objHeaders;

    IMS::SetLastError(IMSError::NO_ERROR);

    return AString::ConstNull();
}

PUBLIC
IMS_RESULT FramedMedia::SetAcceptedContentTypes(IN CONST AStringArray &objAcceptedContentTypes)
{
    //---------------------------------------------------------------------------------------------

    if (GetState() != STATE_INACTIVE)
    {
        IMS::SetLastError(IMSError::ILLEGAL_STATE);
        return IMS_FAILURE;
    }

    for (IMS_SINT32 i = 0; i < objAcceptedContentTypes.GetCount(); ++i)
    {
        const AString &strContentType = objAcceptedContentTypes.GetElementAt(i);

        if (!TextParser::IsValidMediaType(strContentType))
        {
            IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
            return IMS_FAILURE;
        }
    }

    // Update the attribute of this media descriptor
    MediaDescriptor *pDescriptor = GetMediaDescriptor();

    if (pDescriptor != IMS_NULL)
    {
        AString strAcceptTypes;

        if (objAcceptedContentTypes.IsEmpty())
        {
            strAcceptTypes = TextParser::CHAR_ASTERISK;
        }
        else
        {
            strAcceptTypes = objAcceptedContentTypes.GetElementAt(0);

            for (IMS_SINT32 i = 1; i < objAcceptedContentTypes.GetCount(); ++i)
            {
                strAcceptTypes += TextParser::CHAR_SP;
                strAcceptTypes += objAcceptedContentTypes.GetElementAt(i);
            }
        }

        SdpMediaParameter *pParameter = GetProposalMediaParameter(pDescriptor->GetMid());

        pParameter->RemoveAttribute(SdpAttribute::ACCEPT_TYPES);

        SdpAttribute objAttribute;
        objAttribute.SetValue(SdpAttribute::ACCEPT_TYPES, strAcceptTypes);

        if (!pParameter->AddAttribute(objAttribute))
        {
            IMS_TRACE_E(0, "Adding 'accept-types' attribute failed", 0, 0, 0);
            return IMS_FAILURE;
        }
    }

    this->objAcceptedContentTypes = objAcceptedContentTypes;

    IMS::SetLastError(IMSError::NO_ERROR);

    return IMS_SUCCESS;
}

PUBLIC
void FramedMedia::SetListener(IN IOnFramedMediaListener *piListener)
{
    //---------------------------------------------------------------------------------------------

    this->piListener = piListener;
}

PROTECTED VIRTUAL
MediaProposal* FramedMedia::CreateMediaProposal(IN ISDPOAState *piOAState)
{
    FramedMediaProposal *pMediaProposal
            = new FramedMediaProposal(piOAState, objAcceptedContentTypes);

    //---------------------------------------------------------------------------------------------

    if (pMediaProposal == IMS_NULL)
    {
        return IMS_NULL;
    }

    if (!pMediaProposal->CreateDescriptor(GetMediaDescriptors()))
    {
        delete pMediaProposal;
        return IMS_NULL;
    }

    return pMediaProposal;
}

PROTECTED VIRTUAL
IMS_BOOL FramedMedia::PreviewInitInstance()
{
    //---------------------------------------------------------------------------------------------

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL FramedMedia::PostInitInstance()
{
    //---------------------------------------------------------------------------------------------

    // Update the managed items
    MediaDescriptor *pDescriptor = GetMediaDescriptor();

    if (pDescriptor == IMS_NULL)
    {
        // do nothing ...
        return IMS_TRUE;
    }

    SdpMediaParameter *pParameter = GetProposalMediaParameter(pDescriptor->GetMid());
    const SdpAttribute *pAttribute = pParameter->GetAttribute(SdpAttribute::ACCEPT_TYPES);

    if (pAttribute != IMS_NULL)
    {
        const AString &strAcceptTypes = pAttribute->GetAttributeValue();

        objAcceptedContentTypes = strAcceptTypes.Split(TextParser::CHAR_SP);
    }

    return IMS_TRUE;
}

PROTECTED VIRTUAL
void FramedMedia::PreviewCleanupMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void FramedMedia::PostCleanupMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void FramedMedia::PreviewRemoveMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void FramedMedia::PostRemoveMedia()
{
    //---------------------------------------------------------------------------------------------
}
