#include "IMessage.h"
#include "IMSList.h"
#include "ISIPHeader.h"
#include "utility/MessageUtil.h"
#include "call/extension/MtcExtension.h"
#include "call/extension/MtcExtensionSet.h"
#include "SIPHeaderName.h"
#include "SIPMethod.h"
#include "ServiceTrace.h"

__IMS_TRACE_TAG_COM_MTC__;

PUBLIC
MtcExtension::MtcExtension(IN const AString& strOptionTag) :
        m_strOptionTag(strOptionTag),
        m_bRequiredOnRemote(IMS_FALSE),
        m_bSupportedOnRemote(IMS_FALSE)
{
}

PUBLIC
MtcExtension::MtcExtension(IN const MtcExtension& objRhs) :
        m_strOptionTag(objRhs.m_strOptionTag),
        m_bRequiredOnRemote(objRhs.m_bRequiredOnRemote),
        m_bSupportedOnRemote(objRhs.m_bSupportedOnRemote)
{
}

PUBLIC VIRTUAL
MtcExtension::~MtcExtension()
{
}

PUBLIC VIRTUAL
IMtcExtension* MtcExtension::Clone() const
{
    return new MtcExtension(*this);
}

PUBLIC VIRTUAL
IMS_BOOL MtcExtension::IsAvailableOnRemote() const
{
    return m_bRequiredOnRemote || m_bSupportedOnRemote;
}

PUBLIC VIRTUAL
const AString& MtcExtension::GetOptionTag() const
{
    return m_strOptionTag;
}

PUBLIC VIRTUAL
void MtcExtension::FormatRequest(
        IN IMS_UINT32 /* eMethod */, IN_OUT IMessage& /* objRequest */)
{
}

PUBLIC VIRTUAL
void MtcExtension::FormatResponse(
        IN IMS_UINT32 /* eMethod */, IN_OUT IMessage& /* objResponse */)
{
}

PUBLIC VIRTUAL
void MtcExtension::HandleRequest(IN IMS_UINT32 eMethod, IN const IMessage& objRequest)
{
    if (eMethod == IMessage::SESSION_PRACK)
    {
        return;
    }

    if (GetOptionTag().Equals(MtcExtensionSet::OPTION_TAG_PRECONDITION))
    {
        if (eMethod != IMessage::SESSION_START && eMethod != IMessage::SESSION_EARLY_UPDATE)
        {
            return;
        }
        else if (eMethod == IMessage::SESSION_EARLY_UPDATE && !MessageUtil::HasSdp(&objRequest))
        {
            IMS_TRACE_D("HandleRequest : don't check precondition feature without SDP.", 0, 0, 0);
            return;
        }
    }

    m_bRequiredOnRemote = MessageUtil::HasValue(
            &objRequest, GetOptionTag(), ISIPHeader::REQUIRE);
    m_bSupportedOnRemote = MessageUtil::HasValue(
            &objRequest, GetOptionTag(), ISIPHeader::SUPPORTED);
}

PUBLIC VIRTUAL
void MtcExtension::HandleResponse(
        IN IMS_UINT32 eMethod, IN const IMessage& objResponse)
{
    if (eMethod == IMessage::SESSION_PRACK)
    {
        return;
    }

    if (GetOptionTag().Equals(MtcExtensionSet::OPTION_TAG_PRECONDITION))
    {
        if (eMethod != IMessage::SESSION_START)
        {
            return;
        }
        else if (!MessageUtil::HasSdp(&objResponse))
        {
            IMS_TRACE_D("HandleResponse : don't check precondition feature without SDP.", 0, 0, 0);
            return;
        }
    }

    m_bRequiredOnRemote = MessageUtil::HasValue(
            &objResponse, GetOptionTag(), ISIPHeader::REQUIRE);
    m_bSupportedOnRemote = MessageUtil::HasValue(
            &objResponse, GetOptionTag(), ISIPHeader::SUPPORTED);
}
