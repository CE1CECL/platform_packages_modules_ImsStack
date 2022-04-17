/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091208  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "TextParser.h"
#include "base/IMS.h"
#include "IMSCore.h"
#include "media/MediaDescriptor.h"
#include "media/StreamMediaProposal.h"
#include "media/StreamMedia.h"

__IMS_TRACE_TAG_IMS_CORE__;



PUBLIC
StreamMedia::StreamMedia(IN Service *pService_, IN ISDPOAState *piOAState_)
    : Media(pService_, piOAState_)
    , nStreamType(STREAM_TYPE_AUDIO)
    , nQualityLevel(QUALITY_MEDIUM)
{
    SetInitializationDone(IMS_TRUE);
}

PUBLIC VIRTUAL
StreamMedia::~StreamMedia()
{
    //---------------------------------------------------------------------------------------------

    IMS_TRACE_D("Destructor :: StreamMedia", 0, 0, 0);
}

PUBLIC VIRTUAL
IMS_BOOL StreamMedia::CanRead() const
{
    //---------------------------------------------------------------------------------------------

    // TODO::

    return IMS_FALSE;
}

PUBLIC VIRTUAL
IMS_BOOL StreamMedia::Exists() const
{
    //---------------------------------------------------------------------------------------------

    // TODO::

    return IMS_FALSE;
}

PUBLIC VIRTUAL
IMS_SINT32 StreamMedia::GetType() const
{
    //---------------------------------------------------------------------------------------------

    return IMSCore::MEDIA_TYPE_STREAM;
}

PUBLIC
void* StreamMedia::GetReceivingPlayer() const
{
    //---------------------------------------------------------------------------------------------

    return IMS_NULL;
}

PUBLIC
void* StreamMedia::GetSendingPlayer() const
{
    //---------------------------------------------------------------------------------------------

    return IMS_NULL;
}

PUBLIC
IMS_SINT32 StreamMedia::GetStreamType() const
{
    //---------------------------------------------------------------------------------------------

    return nStreamType;
}

PUBLIC
IMS_RESULT StreamMedia::SetPreferredQuality(IN IMS_SINT32 nQuality)
{
    //---------------------------------------------------------------------------------------------

    if (GetState() != STATE_INACTIVE)
    {
        IMS::SetLastError(IMSError::ILLEGAL_STATE);
        return IMS_FAILURE;
    }

    if ((nQuality != QUALITY_LOW)
            && (nQuality != QUALITY_MEDIUM)
            && (nQuality != QUALITY_HIGH))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return IMS_FAILURE;
    }

    nQualityLevel = nQuality;

    return IMS_SUCCESS;
}

PUBLIC
IMS_RESULT StreamMedia::SetSource(IN CONST AString &strSource)
{
    //---------------------------------------------------------------------------------------------

    if (GetState() != STATE_INACTIVE)
    {
        IMS::SetLastError(IMSError::ILLEGAL_STATE);
        return IMS_FAILURE;
    }

    if (!strSource.StartsWith("capture://")
            || !strSource.StartsWith("file://"))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return IMS_FAILURE;
    }

    IMS_SINT32 nIndex = strSource.GetLastIndexOf(TextParser::CHAR_SLASH);
    AString strRealSource = strSource.GetSubStr(nIndex + 1);

    if (strSource.StartsWith("capture"))
    {
        if (strRealSource.EqualsIgnoreCase("audio"))
        {
            nStreamType = STREAM_TYPE_AUDIO;
        }
        else if (strRealSource.EqualsIgnoreCase("video"))
        {
            nStreamType = STREAM_TYPE_VIDEO;
        }
        else if (strRealSource.EqualsIgnoreCase("audio_video"))
        {
            nStreamType = STREAM_TYPE_AUDIO_VIDEO;
        }
        else
        {
            // other device name
        }
    }
    else
    {
        // from file location
    }

    return IMS_SUCCESS;
}

PUBLIC
IMS_RESULT StreamMedia::SetStreamType(IN IMS_SINT32 nType)
{
    //---------------------------------------------------------------------------------------------

    if (GetState() != STATE_INACTIVE)
    {
        IMS::SetLastError(IMSError::ILLEGAL_STATE);
        return IMS_FAILURE;
    }

    if ((nType != STREAM_TYPE_AUDIO)
            && (nType != STREAM_TYPE_VIDEO)
            && (nType != STREAM_TYPE_AUDIO_VIDEO))
    {
        IMS::SetLastError(IMSError::ILLEGAL_ARGUMENT);
        return IMS_FAILURE;
    }

    nStreamType = nType;

    return IMS_SUCCESS;
}

PROTECTED VIRTUAL
MediaProposal* StreamMedia::CreateMediaProposal(IN ISDPOAState *piOAState)
{
    StreamMediaProposal *pMediaProposal = new StreamMediaProposal(piOAState, nStreamType);

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
IMS_BOOL StreamMedia::PreviewInitInstance()
{
    //---------------------------------------------------------------------------------------------

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL StreamMedia::PostInitInstance()
{
    // Set the stream type according to the media descriptors
    IMS_BOOL bVideoPresent = IMS_FALSE;
    IMS_BOOL bAudioPresent = IMS_FALSE;

    //---------------------------------------------------------------------------------------------

    const IMSList<MediaDescriptor*> &objDescriptors = GetMediaDescriptors();

    for (IMS_UINT32 i = 0; i < objDescriptors.GetSize(); ++i)
    {
        const MediaDescriptor *pDescriptor = objDescriptors.GetAt(i);
        SdpMediaParameter *pMediaParam = GetProposalMediaParameter(pDescriptor->GetMid());
        IMS_SINT32 nMediaType = pMediaParam->GetMedia().GetType();

        if (nMediaType == SdpMedia::TYPE_AUDIO)
            bAudioPresent = IMS_TRUE;
        else if (nMediaType == SdpMedia::TYPE_VIDEO)
            bVideoPresent = IMS_TRUE;
    }

    if (bAudioPresent && bVideoPresent)
        nStreamType = STREAM_TYPE_AUDIO_VIDEO;

    if (bVideoPresent)
        nStreamType = STREAM_TYPE_VIDEO;

    // Default value is "AUDIO", so do not check in case of bAudioPresent.

    return IMS_TRUE;
}

PROTECTED VIRTUAL
void StreamMedia::PreviewCleanupMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void StreamMedia::PostCleanupMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void StreamMedia::PreviewRemoveMedia()
{
    //---------------------------------------------------------------------------------------------
}

PROTECTED VIRTUAL
void StreamMedia::PostRemoveMedia()
{
    //---------------------------------------------------------------------------------------------
}
