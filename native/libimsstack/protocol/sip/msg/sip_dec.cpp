/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : sip_dec.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           : Saurabh Srivastava
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : July. 27, 2010
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/

#include "msg/sip_msgutil.h"
#include "sip_pf_datatypes.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "msg/SipHeaders.h"
#include "msg/SipMessage.h"
#include "msg/SipHeaderBase.h"
/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define MAX__CONTACT_EXPIRES 4294967295

#define SIP_MAX_HDR_LEN 32


extern SIPHdrAccess* gpObjHdrAccess;


/******************************************************************************
 * Function name      : sipSkipRwWSP
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
    SIP_CHAR*  sipSkipRwWSP
(
 SIP_CHAR            *pucStartPt,
 SIP_CHAR            *pucEndPt
 )
{
    /*NULL validation*/
    if ((pucStartPt == SIP_NULL) || (pucEndPt == SIP_NULL))
    {
        return SIP_NULL;
    }
    while ((pucEndPt > pucStartPt) && IS_WSP(*pucEndPt))
    {
        pucEndPt--;
    }
    return pucEndPt;
}




/******************************************************************************
 * Function name      : sipFindPostDelimiter
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
    SIP_BOOL sipFindPostDelimiter
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        **pucTempLoc,
 SIP_CHAR        ucDelimiter
 )
{
    while (pucStartPt <= pucEndPt)
    {
        if (*pucStartPt == ucDelimiter)
        {
            *pucTempLoc = pucStartPt + SIP_ONE;
            return SIP_TRUE;
        }
        pucStartPt = pucStartPt + SIP_ONE;
    }
    return SIP_FALSE;
}




/*****************************************************************************
 * Function name      : sipGetUriType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
    SIP_INT32 sipGetUriType
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt
 )
{
    SIP_UINT16 usSize = (pucEndPt - pucStartPt) + SIP_ONE;
    if (SipPf_Memcmp(pucStartPt, SIP_SIP, usSize) == 0)
    {
        return SipUri::SCHEME_SIP;
    }
    else if (SipPf_Memcmp(pucStartPt, SIP_SIPS, usSize) == 0)
    {
        return SipUri::SCHEME_SIPS;
    }
    return SipUri::SCHEME_ABS;
}


/*****************************************************************************
 * Function name      : sipGetHdrType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
    SIP_INT32 sipGetHdrType
(
 const SIP_CHAR    *pszHdrName
 )
{
    gpObjHdrAccess = SIPHdrAccess:: GetInstance();
    return gpObjHdrAccess->GetHdrType(pszHdrName);
}

/*****************************************************************************
 * Function name      : CheckAndGetHdrEnumType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 CheckAndGetHdrEnumType
(
SIP_INT32 nType
)
{
    //support EXPIRES_ANY & EXPIRES_DATE
    if ((nType == SipHeaderBase::EXPIRES_ANY) || (nType == SipHeaderBase::EXPIRES_DATE))
    {
        nType = SipHeaderBase::EXPIRES_SEC;
    }//support CONTACT_ANY & CONTACT_WILD
    else if ((nType == SipHeaderBase::CONTACT_ANY) || (nType == SipHeaderBase::CONTACT_WILD))
    {
        nType = SipHeaderBase::CONTACT;
    }//Support for Retry-After Any & Sec header
    else if ((nType == SipHeaderBase::RETRY_AFTER_ANY) ||
             (nType == SipHeaderBase::RETRY_AFTER_DATE))
    {
        nType = SipHeaderBase::RETRY_AFTER_SEC;
    }

    return nType;
}

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/
SIP_BOOL IsValidAddress(SIP_CHAR *pucStartPt, SIP_UINT32 uiDecLen)
{
    SIP_CHAR            *pucTempLoc = SIP_NULL;
    SIP_CHAR            *pucEndPt = pucStartPt +uiDecLen  - SIP_ONE;

    /*Find the Start of header parm*/
    if (sipFindPostDelimiter(pucStartPt, pucEndPt, &pucTempLoc, QMARK) == SIP_FALSE)
    {
        return SIP_TRUE;
    }

    pucStartPt = pucTempLoc;
    pucTempLoc = SIP_NULL;

    if (sipFindPostDelimiter(pucStartPt, pucEndPt, &pucTempLoc, PERCENT) == SIP_FALSE)
    {
        return SIP_TRUE;
    }
    return SIP_FALSE;
}
