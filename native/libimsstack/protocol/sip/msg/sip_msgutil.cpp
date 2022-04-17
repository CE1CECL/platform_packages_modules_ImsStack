/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : sip_msgutil.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)             : Vijay Nair
 * E-mail id.            : vijay.nair@
 * Creation date         : Jan.7, 2015
 *
 * Edit History             Modification                         Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/
#include "sip_debug.h"
#include "platform/sip_pf_memory.h"
#include "platform/sip_pf_string.h"
#include "msg/sip_msgutil.h"
#include "msg/SipMessage.h"
#include <ctype.h>

SIPHdrAccess* SIPHdrAccess::s_pInstance = SIP_NULL;

SIP_CHAR    gaszSipHdr[][SIP_MAX_HDR_LEN] =
{
    "Allow",//0
    "Allow-Events",
    "Authorization",
    "Call-ID",
    "Contact",
    "Contact",
    "Contact",
    "Content-Disposition",
    "Content-Encoding",
    "Content-Length",
    "Content-Type",//10
    "CSeq",

    "Event",
    "Expires",
    "Expires",
    "Expires",
    "Accept",
    "Min-Expires",
    "From",
    "Max-Forwards",
    "MIME-Version",//20
    "Privacy",
    "P-Preferred-Identity",
    "P-Asserted-Identity",

    "Min-SE",
    "Path",
    "P-Associated-URI",
    "P-Called-Party-ID",
    "P-Visited-Network-ID",
    "P-Charging-Function-Addresses",
    "P-Access-Network-Info",//30
    "P-Charging-Vector",
    "Service-Route",
    "History-Info",

    "Request-Disposition",
    "Accept-Contact",
    "Reject-Contact",
    "Join",
    "SIP-If-Match",
    "SIP-ETag",
    "Proxy-Authenticate",//40
    "Proxy-Authorization",
    "RAck",
    "Record-Route",

    "Referred-By",
    "Refer-To",
    "Replaces",
    "Require",
    "Route",
    "RSeq",
    "Security-Client",//50
    "Security-Verify",
    "Security-Server",
    "Session-Expires",

    "Subscription-State",
    "Supported",
    "Timestamp",
    "To",
    "Unsupported",
    "Via",
    "Warning",//60
    "WWW-Authenticate",
    "Unknown",
    "Retry-After",
    "Retry-After",
    "Retry-After",
    "P-Early-Media",

    "Resource-Priority",
    "Accept-Resource-Priority",
    "Date",
    "Accept-Encoding",//70
    "Accept-Language",
    "Alert-Info",
    "Answer-Mode",
    "Authentication-Info",
    "Call-Info",
    "Content-Language",

    "Error-Info",
    "Flow-Timer",
    "Identity",
    "Identity-Info",//80
    "In-Reply-To",
    "Organization",
    "P-Answer-State",
    "Permission-Missing",
    "P-Media-Authorization",
    "P-Profile-Key",

    "P-Refused-URI-List",
    "Priority",
    "Priv-Answer-Mode",
    "Proxy-Require",//90
    "P-Served-User",
    "P-User-Database",
    "Reason",
    "Refer-Sub",
    "Reply-To",
    "Response-Key",

    "Server",
    "Subject",
    "Suppress-If-Match",
    "Target-Dialog",//100
    "Trigger-Consent",
    "User-Agent",

    "Feature-Caps",
    "Geolocation",
    "Geolocation-Error",
    "Geolocation-Routing",
    "Info-Package",
    "Max-Breadth",
    "P-Asserted-Service",
    "Policy-Contact",//110

    "Policy-ID",
    "P-Preferred-Service",
    "Recv-Info",
    "Session-ID",
    "UNKNOWN",  //115
};

const SIP_CHAR*    gaszSipContentHdr[SIP_CONTENT_HDRS_LEN] =
{
    "Content-Type",           /*CONTENT_TYPE*/
    "Content-Disposition",    /*CONTENT_DISPOSITION*/
    "Content-Encoding",       /*CONTENT_TRANSFER_ENCODING*/
    "Content-ID",             /*CONTENT_ID*/
    "Content-Description"     /*CONTENT_DESCRIPTION*/
};


const SIP_CHAR    gaszSipHdrCompact[21] = "abcdefijklmnorstuvxy";
const SIP_INT16 gaszSipHdrCompactEnum[20] = {
    SipHeaderBase::ACCEPT_CONTACT,
    SipHeaderBase::REFERRED_BY,
    SipHeaderBase::CONTENT_TYPE,
    SipHeaderBase::REQUEST_DISPOSITION,
    SipHeaderBase::CONTENT_ENCODING,
    SipHeaderBase::FROM,
    SipHeaderBase::CALL_ID,
    SipHeaderBase::REJECT_CONTACT,
    SipHeaderBase::SUPPORTED,
    SipHeaderBase::CONTENT_LENGTH,
    SipHeaderBase::CONTACT,
    SipHeaderBase::IDENTITY_INFO,
    SipHeaderBase::EVENT,
    SipHeaderBase::REFER_TO,
    SipHeaderBase::SUBJECT,
    SipHeaderBase::TO,
    SipHeaderBase::ALLOW_EVENTS,
    SipHeaderBase::VIA,
    SipHeaderBase::SESSION_EXPIRES,
    SipHeaderBase::IDENTITY
};



SIPHdrAccess *gpObjHdrAccess = SIP_NULL;

/*****************************************************************************
 * Function name      : SetCharVar
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL SetCharVar(const SIP_CHAR *pcSource,SIP_CHAR* &cDestination)
{
    if (pcSource == SIP_NULL)
    {
        return SIP_FALSE;
    }

    if (cDestination != SIP_NULL)
    {
        delete[] cDestination;
    }

    cDestination = SipPf_Strdup(pcSource);
    if (cDestination == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,"Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}


/*****************************************************************************
 * Function name      : HasSpace
 *
 * Description        : Returns SUCCESS if the character string has Space
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL HasSpace(const SIP_CHAR* pcValue)
{
    while (*pcValue)
    {
        if (*pcValue == ' ')
        {
            return SIP_TRUE;
        }
        pcValue++;
    }
    return SIP_FALSE;
}

/*****************************************************************************
 * Function name      : sipGetMsgType
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 sipGetMsgType(SIP_CHAR *pucStartPoint)
{
    return (SipPf_Strncmp(SIP_SIPVER, pucStartPoint, SIP_FOUR) == 0) ?
        SipMessage::RESP_TYPE : SipMessage::REQ_TYPE;
}


/*****************************************************************************
 * Function name      : sipFindTerminatingCRLF
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
    SIP_BOOL  sipFindTerminatingCRLF
(
 SIP_CHAR        *pucStartPoint,
 SIP_CHAR        *pucEndPoint,
 SIP_CHAR        **pucLocation,
 SIP_BOOL        *pbHdrEnd
 )
{
    /* To check two consecutive CRLF */
    SIP_UINT32 uiStrlen = pucEndPoint - pucStartPoint;
    if (uiStrlen < SIP_THREE)
    {
        return SIP_FALSE;
    }
    SIP_CHAR *pucPrevPoint2 = pucStartPoint;
    SIP_CHAR *pucPrevPoint1 = pucStartPoint + SIP_ONE;
    pucStartPoint = pucStartPoint + SIP_TWO;
    /*Check for CRLF*/
    SIP_CHAR *pucNextPoint = pucStartPoint + SIP_ONE;

    while (pucNextPoint <= pucEndPoint)
    {
        if (IS_CRLF(*pucPrevPoint2, *pucPrevPoint1) &&
            (IS_WSP(*pucStartPoint) == SIP_FALSE))
        {
            *pucLocation = pucPrevPoint2 - SIP_ONE;
            if (IS_CRLF(*pucStartPoint, *pucNextPoint))
            {
                *pbHdrEnd = SIP_TRUE;
            }
            return SIP_TRUE;
        }
        pucPrevPoint2 = pucPrevPoint1;
        pucPrevPoint1 = pucStartPoint;
        pucStartPoint++;
        pucNextPoint = pucStartPoint + SIP_ONE;
    }
    /*Check For one CRLF at Extream End*/
    if (IS_CRLF(*pucPrevPoint2, *pucPrevPoint1) &&
        (IS_WSP(*pucStartPoint) == SIP_FALSE))
    {
        *pucLocation = pucPrevPoint2 - SIP_ONE;
        return SIP_TRUE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : sipFindActualPos
 *
 * Description     : this Api will find the delimiter and Remove LWS form
 *                    both the Side
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
    SIP_BOOL sipFindActualPos
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt,
 SIP_CHAR        **pucTempPre,
 SIP_CHAR        **pucTempNext,
 SIP_CHAR        ucDelimiter
 )
{
    SIP_BOOL      bDQuote = SIP_FALSE;
    SIP_CHAR        *pucPrevPt = pucStartPt;
    SIP_CHAR        *pucStartTemp = pucStartPt;

    while (pucStartPt <= pucEndPt)
    {
        /*Preventing the case of Feature Prm and quoted text*/
        if ((*pucStartPt == DQUOTE) && (*pucPrevPt != BACKSLASH))
        {
            bDQuote = (SIP_BOOL)!bDQuote;
        }
        if (*pucStartPt == ucDelimiter)
        {
            /*for the case of quoted string we have to
              prevent returning in case of " ," and " ;" */
            if (bDQuote == SIP_FALSE)
            {
                *pucTempPre = pucStartPt - SIP_ONE;
                *pucTempNext = pucStartPt + SIP_ONE;

                /*remove the LWS from Back*/
                if (ucDelimiter != RIGHT_ANGLE)
                {
                    *pucTempPre = sipSkipRwLWS(pucStartTemp, *pucTempPre);
                }
                /*remove the LWS from start*/
                if (ucDelimiter != LEFT_ANGLE)
                {
                    *pucTempNext = sipSkipFwLWS(*pucTempNext, pucEndPt);
                }

                /*Remove the LWS form both the side to get the actual pos*/
                return SIP_TRUE;
            }
        }
        pucPrevPt = pucStartPt;
        pucStartPt = pucStartPt + SIP_ONE;
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name  : SipEnc_UpdateCurrPos
 * Description     :  This api will update the current position of the sip msg
 *****************************************************************************/
SIP_VOID SipEnc_UpdateCurrPos
(
 SIP_CHAR **ppucMsgBuffer //in -out param
 )
{
    while (**ppucMsgBuffer != '\0')
    {
        (*ppucMsgBuffer)++;
    }
}


/*****************************************************************************
 * Function name      : SIPHdrAccess :: SIPHdrAccess()
 *
 * Description        : Constructor to Singleton Class SIPHdrAccess.
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/

SIPHdrAccess :: SIPHdrAccess()
{
    for (SIP_INT32 usHdrLenIndex = SIP_ZERO; usHdrLenIndex < SIP_MAX_HDR_LEN; usHdrLenIndex++)
    {
        SIP_INT32 usNoOfHdr = SIP_ZERO;
        objHdrLenRecord[usHdrLenIndex].NoOfEntries = SIP_ZERO;
        objHdrLenRecord[usHdrLenIndex].Hdrlen = usHdrLenIndex;

        for (SIP_INT32 usHdrIndex = SIP_ZERO; usHdrIndex < SipHeaderBase::TYPE_END; usHdrIndex++)
        {
            if (SipPf_Strlen(gaszSipHdr[usHdrIndex]) == usHdrLenIndex)
            {
                objHdrLenRecord[usHdrLenIndex].NoOfEntries++;
                objHdrLenRecord[usHdrLenIndex].objHeaders[usNoOfHdr].HdrType = usHdrIndex;

                SipPf_Memset(objHdrLenRecord[usHdrLenIndex].objHeaders[usNoOfHdr].HdrName, 0,
                    SIP_MAX_HDR_LEN);

                SipPf_Strncpy(objHdrLenRecord[usHdrLenIndex].objHeaders[usNoOfHdr].HdrName,
                        gaszSipHdr[usHdrIndex], SipPf_Strlen(gaszSipHdr[usHdrIndex]));
                usNoOfHdr++;
            }
        }
    }
}


SIPHdrAccess* SIPHdrAccess :: GetInstance()
{
     if (s_pInstance == SIP_NULL) {
         s_pInstance = new SIPHdrAccess();
     }
     return s_pInstance;
}

void SIPHdrAccess::DestroyInstance()
{
    if(s_pInstance != SIP_NULL) {
        delete s_pInstance;
    }
    s_pInstance = SIP_NULL;
}

/*****************************************************************************
 * Function name      : SIPHdrAccess :: GetHdrType
 *
 * Description        : returns Enum corresponding to the received Header
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_INT32 SIPHdrAccess :: GetHdrType(const SIP_CHAR *pRcvdHdrName)
{
    if ( pRcvdHdrName == SIP_NULL) {
        return SipHeaderBase::TYPE_INVALID;
    }

    SIP_INT32 uslen = SipPf_Strlen(pRcvdHdrName);
    if ( uslen >= SIP_MAX_HDR_LEN) {
        return SipHeaderBase::UNKNOWN;
    } else if (uslen == SIP_ONE) {
        return GetHdrTypeCompact(pRcvdHdrName[0]);
    }

    /*Content header are separately parsed based Content headers array gaszSipContentHdr
      and treated as known headers */
    if (SipPf_Strnicmp(pRcvdHdrName, (SIP_CHAR *)"Content", SIP_SEVEN) == SIP_ZERO)
    {
        SIP_BOOL isContHdrFound = SIP_FALSE;
        for (SIP_INT32 usNContHdr = SIP_ZERO; usNContHdr < SIP_CONTENT_HDRS_LEN; usNContHdr++) {
              if (SipPf_Stricmp(gaszSipContentHdr[usNContHdr], pRcvdHdrName) == SIP_ZERO) {
                  isContHdrFound = SIP_TRUE;
                  break;
              }
        }
        //Other Content headers treated as unknown headers like Content-Length.
        if (isContHdrFound == SIP_FALSE) {
            return SipHeaderBase::UNKNOWN;
        }
    } // Conversion for Expires / Retry-After Headers
    else if (SipPf_Strnicmp(pRcvdHdrName, (SIP_CHAR *)"Expires", SIP_SEVEN) == SIP_ZERO)
    {
        return SipHeaderBase::EXPIRES_SEC;
    }
    else if (SipPf_Strnicmp(pRcvdHdrName, (SIP_CHAR *)"Retry-After", SIP_11) == SIP_ZERO)
    {
        return SipHeaderBase::RETRY_AFTER_SEC;
    }
    else if (SipPf_Strnicmp(pRcvdHdrName, (SIP_CHAR *)"Feature-Caps", SIP_12) == SIP_ZERO)
    {
       return SipHeaderBase::UNKNOWN;
    }

    for ( SIP_INT32 usNoOfHdr = SIP_ZERO; usNoOfHdr < objHdrLenRecord[uslen].NoOfEntries ;
            usNoOfHdr++)
    {
        if (SipPf_Stricmp(objHdrLenRecord[uslen].objHeaders[usNoOfHdr].HdrName,
                pRcvdHdrName) == SIP_ZERO)
        {
            return objHdrLenRecord[uslen].objHeaders[usNoOfHdr].HdrType;
        }
    }
    return SipHeaderBase::UNKNOWN;
}

SIP_INT32 SIPHdrAccess :: GetHdrTypeCompact( SIP_CHAR RcvdHdrName)
{
    SIP_CHAR lowHdrName = tolower(RcvdHdrName);

    /*Content-Length (l) header to be considered as unknown header to synch with Engine.*/
    /*Other content headers which has compact form (type - c & encoding - e) are known headers in engine*/
    if (lowHdrName == 'l') {
        return SipHeaderBase::UNKNOWN;
    }

    SIP_CHAR *ptemp = (SIP_CHAR *)gaszSipHdrCompact;
    for ( SIP_INT32 i=0; (*ptemp != '\0'); i++)
    {
        if ( *ptemp == lowHdrName)
            return gaszSipHdrCompactEnum[i];
        ptemp++;
    }
    return SipHeaderBase::UNKNOWN;
}
