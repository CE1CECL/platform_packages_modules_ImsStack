#include "msg/SipIntegerHeader.h"
#include "msg/sip_msgutil.h"
#include "platform/sip_pf_string.h"
#include "sip_debug.h"
#include "sip_error.h"
#include "SipTrace.h"

#define MAX_LEN 20

SipIntegerHeader::SipIntegerHeader(SIP_INT32 iHeaderType)
    : SipHeaderBase(iHeaderType)
{
}

SipIntegerHeader::SipIntegerHeader(const SipIntegerHeader& objHeader)
    : SipHeaderBase(objHeader)
{
}

SipIntegerHeader::~SipIntegerHeader()
{
}

SIP_BOOL SipIntegerHeader::SetValueInt(const SIP_UINT32 nValue)
{
    if (nValue > MAX_MAXFD)
    {
        if (GetHdrType() == SipHeaderBase::MAX_FORWARDS)
        {
            return SIP_FALSE;
        }
    }

    SIP_CHAR szValue[MAX_LEN];
    SipPf_Sprintf(szValue, (SIP_CHAR*)"%u", nValue);
    return SetValue(szValue);
}

SIP_UINT32 SipIntegerHeader::GetValueInt() const
{
    return SipPf_Atoi(GetValue());
}

SIP_BOOL SipIntegerHeader::EncodeHdr(SIP_CHAR **ppucCurrPos,SIP_BOOL bParams)
{
    const SIP_CHAR *pValue = GetValue();

    if ((pValue == SIP_NULL) && (GetHdrType() == SipHeaderBase::CONTENT_LENGTH))
    {
        SetValue("0");
    }
    return SipHeaderBase::EncodeHdr(ppucCurrPos, bParams);
}

SIP_BOOL SipIntegerHeader::DecodeHdr(SIP_CHAR    *pucStartPt,SIP_UINT32  uiDecLen)
{
    if (SipHeaderBase::DecodeHdr(pucStartPt,uiDecLen) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    const SIP_CHAR *pValue = GetValue();

    if ((pValue != SIP_NULL) && (SipPf_Atoi_IsZero(pValue) == SIP_FALSE))
    {
        SIP_UINT32 uiValue = SipPf_Atoi(pValue);
        SIP_INT32 iHeaderType = GetHdrType();
        if ((iHeaderType == SipHeaderBase::MAX_FORWARDS) && (uiValue > MAX_MAXFD))
        {
            return SIP_FALSE;
        }
        else if ((iHeaderType == SipHeaderBase::EXPIRES_SEC) && (uiValue > MAX_EXPIRES))
        {
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}

SipHeaderBase* SipIntegerHeader::GetNewObj(SIP_INT32 eHdr, SipHeaderBase* pHeader)
{
    if (pHeader != SIP_NULL) {
        return new SipIntegerHeader(*reinterpret_cast<SipIntegerHeader*>(pHeader));
    }
    return new SipIntegerHeader(eHdr);
}
