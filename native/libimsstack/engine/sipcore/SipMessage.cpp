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
#include "SipPrivate.h"
#include "SipMessage.h"

__IMS_TRACE_TAG_SIP__;

namespace sipcore
{

PUBLIC
SipMessage::SipMessage(IN IMS_SINT32 nType_ /* = ISipMessage::TYPE_REQUEST */) :
        nType(nType_),
        strRequestURI(AString::ConstNull()),
        bBodyPartParsed(IMS_FALSE),
        pstMessage(IMS_NULL)
{
    //---------------------------------------------------------------------------------------------

    pstMessage = SipStack::CreateMessage(nType);
}

PUBLIC
SipMessage::SipMessage(IN ::SipMessage* pstMessage_) :
        nType(TYPE_ANY),
        strRequestURI(AString::ConstNull()),
        bBodyPartParsed(IMS_FALSE),
        pstMessage(pstMessage_)
{
    Init(IMS_FALSE);
}

PUBLIC
SipMessage::SipMessage(IN ::SipMessage* pstMessage_, IN IMS_BOOL bMessageClone) :
        nType(TYPE_ANY),
        strRequestURI(AString::ConstNull()),
        bBodyPartParsed(IMS_FALSE),
        pstMessage(pstMessage_)
{
    //---------------------------------------------------------------------------------------------

    Init(bMessageClone);
}

PUBLIC VIRTUAL SipMessage::~SipMessage()
{
    //---------------------------------------------------------------------------------------------

    SipStack::FreeMessage(pstMessage);

    if (!objBodyParts.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
        {
            SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

            if (pBodyPart != IMS_NULL)
                delete pBodyPart;
        }

        objBodyParts.Clear();
    }

#ifdef __IMS_SIP_DEBUG__
    IMS_TRACE_D("Destructor :: SipMessage", 0, 0, 0);
#endif
}

/*

Remarks

*/
PUBLIC
SipMessage& SipMessage::operator=(IN CONST SipMessage& objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        SipStack::FreeMessage(pstMessage);

        nType = objRHS.nType;
        objMethod = objRHS.objMethod;
        strRequestURI = objRHS.strRequestURI;
        objStatusCode = objRHS.objStatusCode;
        objUnknownHeaders = objRHS.objUnknownHeaders;
        bBodyPartParsed = objRHS.bBodyPartParsed;

        if (!objBodyParts.IsEmpty())
        {
            for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
            {
                SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

                if (pBodyPart != IMS_NULL)
                    delete pBodyPart;
            }

            objBodyParts.Clear();
        }

        for (IMS_UINT32 i = 0; i < objRHS.objBodyParts.GetSize(); ++i)
        {
            const SipMessageBodyPart* pBodyPart = objRHS.objBodyParts.GetAt(i);
            SipMessageBodyPart* pNewBodyPart =
                    DYNAMIC_CAST(SipMessageBodyPart*, pBodyPart->Clone());

            if (pNewBodyPart != IMS_NULL)
            {
                objBodyParts.Append(pNewBodyPart);
            }
        }

        pstMessage = objRHS.pstMessage;

        if (pstMessage != IMS_NULL)
        {
            SipStack::AddReference(pstMessage);
        }
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipMessage::Destroy()
{
    //---------------------------------------------------------------------------------------------

    delete this;
}

/*

Remarks

*/
PUBLIC VIRTUAL ISipMessage* SipMessage::Clone() const
{
    SipMessage* pNewMessage = new SipMessage(nType);

    //---------------------------------------------------------------------------------------------

    if (pNewMessage == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_NULL;
    }

    pNewMessage->objMethod = objMethod;
    pNewMessage->strRequestURI = strRequestURI;
    pNewMessage->objStatusCode = objStatusCode;
    pNewMessage->objUnknownHeaders = objUnknownHeaders;
    pNewMessage->bBodyPartParsed = bBodyPartParsed;

    for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
    {
        const SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);
        SipMessageBodyPart* pNewBodyPart = DYNAMIC_CAST(SipMessageBodyPart*, pBodyPart->Clone());

        if (pNewBodyPart == IMS_NULL)
        {
            delete pNewMessage;

            SipPrivate::SetLastError(SipError::NO_MEMORY);
            return IMS_NULL;
        }

        if (!pNewMessage->objBodyParts.Append(pNewBodyPart))
        {
            delete pNewBodyPart;
            delete pNewMessage;

            SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
            return IMS_NULL;
        }
    }

    if (pstMessage != IMS_NULL)
    {
        SipStack::FreeMessage(pNewMessage->pstMessage);
        pNewMessage->pstMessage = SipStack::CloneMessage(pstMessage);

        if (pNewMessage->pstMessage == IMS_NULL)
        {
            delete pNewMessage;

            SipPrivate::SetLastError(SipError::NO_MEMORY);
            return IMS_NULL;
        }
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);
    return pNewMessage;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipMessage::AddHeader(IN IMS_SINT32 nType, IN CONST AString& strValue,
        IN CONST AString& strName /* = AString::ConstNull() */)
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.AddHeader(strName, strValue);
    }
    else
    {
        SipHeaderBase* pstHeader = SipStack::DecodeHeader(nType, strName, strValue);

        if (pstHeader == IMS_NULL)
        {
            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        if (!SipStack::AppendHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);

            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        SipStack::FreeHeaderEx(pstHeader);

        return IMS_SUCCESS;
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_UINT32 SipMessage::GetCSeqNumber() const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::GetCSeqNumber(pstMessage);
}

/*

Remarks

*/
PUBLIC VIRTUAL AString SipMessage::GetHeader(IN IMS_SINT32 nType, IN IMS_SINT32 nIndex /* = 0 */,
        IN CONST AString& strName /* = AString::ConstNull() */) const
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.GetHeader(strName, nIndex);
    }
    else
    {
        if (SipStack::GetHeaderCount(pstMessage, nType) == 0)
        {
            return AString::ConstNull();
        }

        SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, nType, nIndex);
        AString strHeader;

        SipStack::EncodeHeaderBody(pstHeader, IMS_TRUE, strHeader);

        SipStack::FreeHeaderEx(pstHeader);

        return strHeader;
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_SINT32 SipMessage::GetHeaderCount(
        IN IMS_SINT32 nType, IN CONST AString& strName /* = AString::ConstNull() */) const
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (pstMessage == IMS_NULL)
        return 0;

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.GetHeaderCount(strName);
    }
    else
    {
        return SipStack::GetHeaderCount(pstMessage, nType);
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL IMSList<AString> SipMessage::GetHeaders(
        IN IMS_SINT32 nType, IN CONST AString& strName /* = AString::ConstNull() */) const
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.GetHeaders(strName);
    }
    else
    {
        IMSList<AString> objHeaders;
        IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstMessage, nType);
        AString strHeader;

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, nType, i);

            if (SipStack::IsValidHeader(pstHeader))
            {
                SipStack::EncodeHeaderBody(pstHeader, IMS_TRUE, strHeader);

                if (!objHeaders.Append(strHeader))
                {
                    SipStack::FreeHeaderEx(pstHeader);
                    return IMSList<AString>();  // ???, throw exception
                }
            }

            SipStack::FreeHeaderEx(pstHeader);
        }

        return objHeaders;
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipMessage::PrependHeader(IN IMS_SINT32 nType, IN CONST AString& strValue,
        IN CONST AString& strName /* = AString::ConstNull() */)
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.PrependHeader(strName, strValue);
    }
    else
    {
        SipHeaderBase* pstHeader = SipStack::DecodeHeader(nType, strName, strValue);

        if (pstHeader == IMS_NULL)
        {
            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        if (!SipStack::PrependHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);

            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        SipStack::FreeHeaderEx(pstHeader);

        return IMS_SUCCESS;
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipMessage::RemoveHeader(
        IN IMS_SINT32 nType, IN CONST AString& strName /* = AString::ConstNull() */)
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
        objUnknownHeaders.RemoveHeader(strName);
    else
        SipStack::RemoveHeader(nType, pstMessage);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipMessage::SetHeader(IN IMS_SINT32 nType, IN CONST AString& strValue,
        IN CONST AString& strName /* = AString::ConstNull() */)
{
    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.SetHeader(strName, strValue);
    }
    else
    {
        SipHeaderBase* pstHeader = SipStack::DecodeHeader(nType, strName, strValue);

        if (pstHeader == IMS_NULL)
        {
            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        if (!SipStack::SetHeader(pstHeader, pstMessage))
        {
            SipStack::FreeHeaderEx(pstHeader);

            SipPrivate::SetLastError(SipError::GENERAL_ERROR);
            return IMS_FAILURE;
        }

        SipStack::FreeHeaderEx(pstHeader);

        return IMS_SUCCESS;
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL ISipMessageBodyPart* SipMessage::CreateBodyPart()
{
    SipMessageBodyPart* pBodyPart = new SipMessageBodyPart(IMS_FALSE);

    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (pBodyPart == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_NULL;
    }

    if (!objBodyParts.Append(pBodyPart))
    {
        delete pBodyPart;

        SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
        return IMS_NULL;
    }

    return pBodyPart;
}

/*

Remarks

*/
PUBLIC VIRTUAL ISipMessageBodyPart* SipMessage::CreateSdpBodyPart()
{
    SipMessageBodyPart* pBodyPart = new SipMessageBodyPart(IMS_TRUE);

    //---------------------------------------------------------------------------------------------

    SipPrivate::SetLastError(SipError::NO_ERROR);

    if (pBodyPart == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::NO_MEMORY);
        return IMS_NULL;
    }

    if (!objBodyParts.Prepend(pBodyPart))
    {
        delete pBodyPart;

        SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
        return IMS_NULL;
    }

    return pBodyPart;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMSList<ISipMessageBodyPart*> SipMessage::GetBodyParts() const
{
    //---------------------------------------------------------------------------------------------

    if (objBodyParts.IsEmpty())
    {
        return IMSList<ISipMessageBodyPart*>();
    }

    IMSList<ISipMessageBodyPart*> objSIPBodyParts;

    for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
    {
        SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

        if (!pBodyPart->IsSdpBodyPart())
        {
            objSIPBodyParts.Append(pBodyPart);
        }
    }

    return objSIPBodyParts;
}

/*

Remarks

*/
PUBLIC VIRTUAL ISipMessageBodyPart* SipMessage::GetSdpBodyPart() const
{
    //---------------------------------------------------------------------------------------------

    if (objBodyParts.IsEmpty())
        return IMS_NULL;

    for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
    {
        SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

        if (pBodyPart->IsSdpBodyPart())
        {
            return pBodyPart;
        }
    }

    return IMS_NULL;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMSList<ISipMessageBodyPart*> SipMessage::GetSdpBodyParts() const
{
    //---------------------------------------------------------------------------------------------

    if (objBodyParts.IsEmpty())
    {
        return IMSList<ISipMessageBodyPart*>();
    }

    IMSList<ISipMessageBodyPart*> objSIPBodyParts;

    for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
    {
        SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

        if (pBodyPart->IsSdpBodyPart())
        {
            objSIPBodyParts.Append(pBodyPart);
        }
    }

    return objSIPBodyParts;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_RESULT SipMessage::CopyHeadersAndBodyParts(IN CONST ISipMessage* piSIPMsg)
{
    const SipMessage* pSIPMsg = DYNAMIC_CAST(const SipMessage*, piSIPMsg);

    //---------------------------------------------------------------------------------------------

    if (pSIPMsg == IMS_NULL)
    {
        SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
        return IMS_FAILURE;
    }

    // Overwrites any known headers
    if (!SipStack::OverwriteHeaders(pSIPMsg->pstMessage, pstMessage))
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FAILURE;
    }

    // Overwrites any unknown headers
    if (!objUnknownHeaders.OverwriteHeaders(pSIPMsg->objUnknownHeaders))
    {
        SipPrivate::SetLastError(SipError::GENERAL_ERROR);
        return IMS_FAILURE;
    }

    // Copy any message body parts if present
    if (pSIPMsg->objBodyParts.IsEmpty())
    {
        SipPrivate::SetLastError(SipError::NO_ERROR);
        return IMS_SUCCESS;
    }

    for (IMS_UINT32 i = 0; i < pSIPMsg->objBodyParts.GetSize(); ++i)
    {
        const SipMessageBodyPart* pBodyPart = pSIPMsg->objBodyParts.GetAt(i);
        SipMessageBodyPart* pNewBodyPart = new SipMessageBodyPart(pBodyPart->IsSdpBodyPart());

        if (pNewBodyPart == IMS_NULL)
        {
            return IMS_FAILURE;
        }

        (*pNewBodyPart) = (*pBodyPart);

        if (!objBodyParts.Append(pNewBodyPart))
        {
            delete pNewBodyPart;

            SipPrivate::SetLastError(SipError::LIST_OPERATION_FAILED);
            return IMS_FAILURE;
        }
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);
    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipMessage::IsHeaderPresent(
        IN IMS_SINT32 nType, IN CONST AString& strName /* = AString::ConstNull() */) const
{
    //---------------------------------------------------------------------------------------------

    if (SipStack::IsUnknownHeader(nType, strName))
    {
        return objUnknownHeaders.IsHeaderPresent(strName);
    }

    return SipStack::IsHeaderPresent(pstMessage, nType);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipMessage::IsMessageRpr() const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::IsMessageRpr(pstMessage);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipMessage::IsOptionRequired(IN CONST AString& strOption) const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::IsOptionRequired(pstMessage, strOption);
}

/*

Remarks

*/
PUBLIC VIRTUAL IMS_BOOL SipMessage::IsOptionSupported(IN CONST AString& strOption) const
{
    //---------------------------------------------------------------------------------------------

    return SipStack::IsOptionSupported(pstMessage, strOption);
}

/*

Remarks

*/
PUBLIC VIRTUAL void SipMessage::RemoveBodyParts()
{
    //---------------------------------------------------------------------------------------------

    if (!objBodyParts.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
        {
            SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

            if (pBodyPart != IMS_NULL)
                delete pBodyPart;
        }

        objBodyParts.Clear();

        SipStack::RemoveAllMessageBodies(pstMessage);
    }
}

/*

Remarks

*/
PUBLIC VIRTUAL ByteArray SipMessage::ToByteArray(IN IMS_SINT32 nOptions /* = OPT_ALL */) const
{
    ByteArray objMessage;

    //---------------------------------------------------------------------------------------------

    if (!SipStack::EncodePartialMessage(pstMessage, nOptions, objMessage))
    {
        return ByteArray::ConstNull();
    }

    return objMessage;
}

/*

Remarks

*/
PUBLIC
SipMessageBodyPart* SipMessage::GetBodyPart() const
{
    //---------------------------------------------------------------------------------------------

    if (objBodyParts.IsEmpty())
        return IMS_NULL;

    return objBodyParts.GetAt(0);
}

/*

Remarks

*/
PUBLIC
IMS_RESULT SipMessage::SetRequestUri(IN CONST AString& strURI)
{
    //---------------------------------------------------------------------------------------------

    if (strURI.IsNULL() || strURI.IsEmpty())
    {
        // Remove Request-URI if set previously
        strRequestURI = AString::ConstNull();
    }
    else
    {
        SipAddrSpec* pstAddrSpec = SipStack::DecodeAddrSpec(strURI);

        if (pstAddrSpec == IMS_NULL)
        {
            SipPrivate::SetLastError(SipError::ILLEGAL_ARGUMENT);
            return IMS_FAILURE;
        }

        SipStack::EncodeAddrSpec(pstAddrSpec, IMS_TRUE, strRequestURI);

        SipStack::FreeAddrSpec(pstAddrSpec);
    }

    SipPrivate::SetLastError(SipError::NO_ERROR);

    return IMS_SUCCESS;
}

/*

Remarks

*/
PUBLIC
void SipMessage::UpdateRequestUri()
{
    SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstMessage);

    //---------------------------------------------------------------------------------------------

    if (pstAddrSpec == IMS_NULL)
    {
        // Remove Request-URI if set previously
        strRequestURI = AString::ConstNull();
    }
    else
    {
        SipStack::EncodeAddrSpec(pstAddrSpec, IMS_TRUE, strRequestURI);
        SipStack::FreeAddrSpec(pstAddrSpec);
    }
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipMessage::CreateBodyParts()
{
    //---------------------------------------------------------------------------------------------

    if (bBodyPartParsed)
        return IMS_TRUE;

    if (objBodyParts.IsEmpty())
    {
        bBodyPartParsed = IMS_TRUE;
        return IMS_TRUE;
    }

    // If message does not include a MIME body, do not parse the message body
    if (!SipStack::HasMimeMessageBody(pstMessage))
    {
        bBodyPartParsed = IMS_TRUE;

        if (SipStack::IsMessageBodyCompressed(pstMessage))
        {
            if (!SipStack::UncompressMessageBody(pstMessage))
            {
                IMS_TRACE_E(0, "Uncompressing SIP message body failed", 0, 0, 0);
                return IMS_FALSE;
            }

            // Clear the previous message body parts ...
            while (!objBodyParts.IsEmpty())
            {
                SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(0);

                delete pBodyPart;

                objBodyParts.RemoveAt(0);
            }

            if (!ExtractBodyParts())
            {
                return IMS_FALSE;
            }
        }

        return IMS_TRUE;
    }

    if (!SipStack::DecodeMessageBody(pstMessage))
    {
        IMS_TRACE_E(0, "Parsing SIP message body failed", 0, 0, 0);
        return IMS_FALSE;
    }

    // Clear the previous message body parts ...
    while (!objBodyParts.IsEmpty())
    {
        SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(0);

        delete pBodyPart;

        objBodyParts.RemoveAt(0);
    }

    if (!ExtractBodyParts())
    {
        return IMS_FALSE;
    }

    bBodyPartParsed = IMS_TRUE;

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipMessage::FormMessage()
{
    //---------------------------------------------------------------------------------------------

    if (pstMessage == IMS_NULL)
        return IMS_FALSE;

    /*
     * Set properties
     *    In case of CSeq header, it will set by the SIP transaction with a correct value.
     */
    if (nType == TYPE_REQUEST)
    {
        if (!SipStack::SetRequestLine(objMethod.ToString(), strRequestURI, pstMessage))
        {
            return IMS_FALSE;
        }
    }
    else
    {
        if (!SipStack::SetStatusLine(
                    objStatusCode.ToInt(), objStatusCode.GetReasonPhrase(), pstMessage))
        {
            return IMS_FALSE;
        }
    }

    // Set unknown headers
    if (objUnknownHeaders.GetCount() > 0)
    {
        AString strName;
        AString strValue;
        IMS_BOOL bCompactForm =
                ((SipPrivate::GetEncodingOptions() & SipPrivate::OPT_E_SHORTFORM) != 0);

        for (IMS_SINT32 i = objUnknownHeaders.GetCount() - 1; i >= 0; --i)
        {
            strName = objUnknownHeaders.GetHeaderName(i, bCompactForm);
            strValue = objUnknownHeaders.GetHeaderBodys(i);

            (void)SipStack::PrependUnknownHeader(strName, strValue, pstMessage);
        }
    }

    // Set message body parts
    if (!objBodyParts.IsEmpty())
    {
        for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
        {
            SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

            if (pBodyPart == IMS_NULL)
                continue;

            if (!pBodyPart->FormMessageBody())
            {
                IMS_TRACE_E(0, "Forming a message body failed", 0, 0, 0);
                continue;  // throw exception
            }

            if (!SipStack::AppendMessageBody(pBodyPart->GetMessageBody(), pstMessage))
            {
                IMS_TRACE_E(0, "Appending a message body failed", 0, 0, 0);
                continue;  // throw exception
            }
        }

        // Do more things related to the message body handling
        //  : Content-Type / Content-Length and so on .
        if (!SipStack::CorrectMessageBody(pstMessage))
        {
            IMS_TRACE_E(0, "Correcting SIP message body failed", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipMessage::FormMessageOnChallenge()
{
    //---------------------------------------------------------------------------------------------

    // FIXME: need to improve the logic

    while (SipStack::GetHeaderCount(pstMessage, ISipHeader::UNKNOWN) > 0)
    {
        SipStack::RemoveHeader(ISipHeader::UNKNOWN, pstMessage);
    }

    // Set unknown headers
    if (objUnknownHeaders.GetCount() > 0)
    {
        AString strName;
        AString strValue;
        IMS_BOOL bCompactForm =
                ((SipPrivate::GetEncodingOptions() & SipPrivate::OPT_E_SHORTFORM) != 0);

        for (IMS_SINT32 i = 0; i < objUnknownHeaders.GetCount(); ++i)
        {
            strName = objUnknownHeaders.GetHeaderName(i, bCompactForm);
            strValue = objUnknownHeaders.GetHeaderBodys(i);

            (void)SipStack::PrependUnknownHeader(strName, strValue, pstMessage);
        }
    }

    // Set message body parts
    if (!objBodyParts.IsEmpty())
    {
        // FIXME: need to improve the logic
        // Message body is formed in the re-submitted request
        if (SipStack::GetMessageBodyCount(pstMessage) == 0)
        {
            for (IMS_UINT32 i = 0; i < objBodyParts.GetSize(); ++i)
            {
                SipMessageBodyPart* pBodyPart = objBodyParts.GetAt(i);

                if (pBodyPart == IMS_NULL)
                    continue;

                if (!pBodyPart->FormMessageBody())
                {
                    IMS_TRACE_E(0, "Forming a message body failed", 0, 0, 0);
                    continue;  // throw exception
                }

                if (!SipStack::AppendMessageBody(pBodyPart->GetMessageBody(), pstMessage))
                {
                    IMS_TRACE_E(0, "Appending a message body failed", 0, 0, 0);
                    continue;  // throw exception
                }
            }
        }

        // Do more things related to the message body handling
        //  : Content-Type / Content-Length and so on .
        if (!SipStack::CorrectMessageBody(pstMessage))
        {
            IMS_TRACE_E(0, "Correcting SIP message body failed", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipMessage::FormMessageOnRetransmission()
{
    // Set message body parts
    if (!objBodyParts.IsEmpty())
    {
        // Do more things related to the message body handling
        //  : Content-Type / Content-Length and so on .
        if (!SipStack::CorrectMessageBody(pstMessage))
        {
            IMS_TRACE_E(0, "Correcting SIP message body failed", 0, 0, 0);
            return IMS_FALSE;
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC GLOBAL SipMessage* SipMessage::CreateMessage(IN CONST ByteArray& objMessage)
{
    ::SipMessage* pstMessage = IMS_NULL;

    //---------------------------------------------------------------------------------------------

    if (!SipStack::DecodeMessage(objMessage.GetData(), objMessage.GetLength(),
                SipPrivate::OPTIONS_D_PARTIAL, pstMessage))
    {
        return IMS_NULL;
    }

    SipMessage* pSIPMsg = new SipMessage(pstMessage);

    if (pSIPMsg == IMS_NULL)
    {
        SipStack::FreeMessage(pstMessage);
        return IMS_NULL;
    }

    SipStack::FreeMessage(pstMessage);

    if (!pSIPMsg->CreateBodyParts())
    {
        delete pSIPMsg;
        return IMS_NULL;
    }

    return pSIPMsg;
}

/*

Remarks

*/
PRIVATE
void SipMessage::Init(IN IMS_BOOL bMessageClone)
{
    //---------------------------------------------------------------------------------------------

    if (pstMessage != IMS_NULL)
    {
        if (bMessageClone)
        {
            pstMessage = SipStack::CloneMessage(pstMessage);
        }
        else
        {
            SipStack::AddReference(pstMessage);
        }
    }

    if (pstMessage != IMS_NULL)
    {
        if (SipStack::IsRequestMessage(pstMessage))
        {
            nType = TYPE_REQUEST;
        }
        else
        {
            nType = TYPE_RESPONSE;
        }

        ExtractProperties();
        ExtractUnknownHeaders();
        ExtractBodyParts();
    }
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipMessage::ExtractBodyParts()
{
    //---------------------------------------------------------------------------------------------

    if (pstMessage == IMS_NULL)
        return IMS_FALSE;

    IMS_SINT32 nBodyCount = SipStack::GetMessageBodyCount(pstMessage);

    if (nBodyCount == 0)
        return IMS_TRUE;

    SipMessageBodyPart* pBodyPart = IMS_NULL;

    if (nBodyCount == 1)
    {
        SipMsgBody* pstMsgBody = SipStack::GetMessageBody(pstMessage, 0);

        if (SipStack::HasSdpMessageBody(pstMessage))
            pBodyPart = new SipMessageBodyPart(pstMsgBody, IMS_TRUE);
        else
            pBodyPart = new SipMessageBodyPart(pstMsgBody, IMS_FALSE);

        if (pBodyPart == IMS_NULL)
        {
            SipStack::FreeMessageBody(pstMsgBody);
            return IMS_FALSE;
        }

        SipStack::FreeMessageBody(pstMsgBody);

        // Set "Content-" headers
        SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::CONTENT_TYPE);
        pBodyPart->SetHeader(pstHeader, ISipMessageBodyPart::CONTENT_TYPE);

        SipStack::FreeHeaderEx(pstHeader);

        if (!objBodyParts.Append(pBodyPart))
        {
            delete pBodyPart;
            return IMS_FALSE;
        }
    }
    else
    {
        for (IMS_SINT32 i = 0; i < nBodyCount; ++i)
        {
            SipMsgBody* pstMsgBody = SipStack::GetMessageBody(pstMessage, i);

            if (pstMsgBody == IMS_NULL)
            {
                return IMS_FALSE;
            }

            if (SipStack::IsMessageBodySdp(pstMsgBody))
                pBodyPart = new SipMessageBodyPart(pstMsgBody, IMS_TRUE);
            else
                pBodyPart = new SipMessageBodyPart(pstMsgBody, IMS_FALSE);

            if (pBodyPart == IMS_NULL)
            {
                SipStack::FreeMessageBody(pstMsgBody);
                return IMS_FALSE;
            }

            SipStack::FreeMessageBody(pstMsgBody);

            if (!objBodyParts.Append(pBodyPart))
            {
                delete pBodyPart;
                return IMS_FALSE;
            }
        }
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipMessage::ExtractProperties()
{
    //---------------------------------------------------------------------------------------------

    if ((pstMessage == IMS_NULL) || (nType == TYPE_ANY))
        return IMS_FALSE;

    objMethod = SipStack::GetMethod(pstMessage);

    if (nType == TYPE_REQUEST)
    {
        SipAddrSpec* pstAddrSpec = SipStack::GetRequestUri(pstMessage);

        SipStack::EncodeAddrSpec(pstAddrSpec, IMS_TRUE, strRequestURI);
        SipStack::FreeAddrSpec(pstAddrSpec);
    }
    else
    {
        objStatusCode = SipStack::GetStatusCodeEx(pstMessage);
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PRIVATE
IMS_BOOL SipMessage::ExtractUnknownHeaders()
{
    //---------------------------------------------------------------------------------------------

    if (pstMessage == IMS_NULL)
    {
        return IMS_FALSE;
    }
    else
    {
        objUnknownHeaders.Clear();

        IMS_SINT32 nHCount = SipStack::GetHeaderCount(pstMessage, ISipHeader::UNKNOWN);

        if (nHCount == 0)
        {
            return IMS_TRUE;
        }

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            SipHeaderBase* pstHeader = SipStack::GetHeader(pstMessage, ISipHeader::UNKNOWN, i);

            objUnknownHeaders.AddHeader(SipStack::GetUnknownHeaderName(pstHeader),
                    SipStack::GetUnknownHeaderBody(pstHeader));

            SipStack::FreeHeaderEx(pstHeader);
        }

        return IMS_TRUE;
    }
}

}  // namespace sipcore
