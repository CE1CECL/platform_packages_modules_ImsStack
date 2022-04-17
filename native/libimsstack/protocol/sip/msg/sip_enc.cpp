/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : sip_msg.cpp
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Saurabh Srivastava
 * E-mail id.    : saurabh31.srivastava@
 * Creation date   : July. 27, 2010
 *
 * Edit History     Modification         Description(s)
 *
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Name       0.0a    Initial creation
 *****************************************************************************/

#include "msg/sip_msgutil.h"
#include "sip_pf_datatypes.h"
#include "sip_debug.h"
#include "SipTrace.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "msg/SipHeaders.h"

SIP_CHAR*  SipPercentEncoding::DoPercentDecoding(SIP_CHAR* pcString)
{
    SIP_INT32 iLength = SipPf_Strlen(pcString);

    SIP_CHAR *pcDecodedString = new SIP_CHAR[(iLength + SIP_ONE)];
    SipPf_Memset(pcDecodedString, SIP_NULL, (iLength + SIP_ONE));
    SIP_CHAR *pcReturnString = pcDecodedString;
    SIP_CHAR *pcStringTemp = pcString;
    SIP_BOOL bIsPercentDecoded = SIP_FALSE;
    while (*pcStringTemp != SIP_NULL)
    {
        /*Search for the % char*/
        if ((*pcStringTemp == PERCENT) && (iLength >= SIP_THREE))
        {
            SIP_INT32 iConvertedChar = SIP_ZERO;
            SipPf_Sscanf(pcStringTemp+SIP_ONE,(SIP_CHAR*)"%2X",(SIP_CHAR*)&iConvertedChar);
            SipPf_Sprintf(pcDecodedString,(SIP_CHAR*)"%c",(SIP_CHAR)iConvertedChar);
            iLength = iLength - SIP_THREE;
            pcStringTemp = pcStringTemp + SIP_THREE;
            bIsPercentDecoded = SIP_TRUE;
        }
        else
        {
            *pcDecodedString = *pcStringTemp;
            pcStringTemp++;
            iLength--;
        }
        pcDecodedString = pcDecodedString + SIP_ONE;
    }

    delete[] pcString;

    if (bIsPercentDecoded == SIP_TRUE)
    {
        iLength = SipPf_Strlen(pcReturnString);
        pcDecodedString = new SIP_CHAR[(iLength + SIP_ONE)];
        SipPf_Memset(pcDecodedString, SIP_NULL, (iLength));
        SipPf_Strcpy(pcDecodedString, pcReturnString);
        delete[] pcReturnString;
    }
    else
    {
        pcDecodedString = pcReturnString;
    }

    return pcDecodedString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_UserAndHeader(SIP_CHAR* pcString, SIP_CHAR* pcType)
{
    /**
     * user = 1*( unreserved / escaped / user-unreserved )
     * unreserved = alphanum / mark
     * mark = "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"
     * escaped = "%"   HEXDIG   HEXDIG
     * user-unreserved = "&" / "=" / "+" / "$" / "," / ";" / "?" / "/"
     *
     * header 1*( hnv-unreserved / unreserved / escaped )
     * hnv-unreserved = "[" / "]" / "/" / "?" / ":" / "+" / "$"
     */
    SIP_CHAR* pucCurrPt = pcString;
    SIP_INT32 iLength = SipPf_Strlen(pcString);
    SIP_CHAR *pkucEndPt = pucCurrPt + iLength;

    SIP_CHAR *pcEncodedString = new SIP_CHAR[(3*iLength)];
    SipPf_Memset(pcEncodedString, SIP_NULL, (3*iLength));
    SIP_CHAR *pcReturnString = pcEncodedString;

    while(*(pucCurrPt) != SIP_NULL)
    {
        if (IS_UNRESERVED(*pucCurrPt) ||
            ((SipPf_Strcmp(pcType, (SIP_CHAR *)SIP_USER) == 0) &&
            IS_USER_UNRESERVED(*pucCurrPt)) ||
            ((SipPf_Strcmp(pcType, (SIP_CHAR *)SIP_HEADERS) == 0) &&
             IS_HNV_UNRESERVED(*pucCurrPt)))
        {
            *pcEncodedString = *pucCurrPt;
            pucCurrPt++;
            pcEncodedString++;
        }
        else if ((pucCurrPt+SIP_TWO <= pkucEndPt) &&
                (IS_ESCAPED(*pucCurrPt,*(pucCurrPt+SIP_ONE),*(pucCurrPt+SIP_TWO))))
        {
            *pcEncodedString = *pucCurrPt;
            *(pcEncodedString+SIP_ONE) = *(pucCurrPt+SIP_ONE);
            *(pcEncodedString+SIP_TWO) = *(pucCurrPt+SIP_TWO);
            pucCurrPt+=3;
            pcEncodedString+=3;
        }
        else
        {
            *pcEncodedString = PERCENT;
            pcEncodedString++;
            SipPf_Sprintf(pcEncodedString,(SIP_CHAR *)"%02X",*pucCurrPt);
            SipEnc_UpdateCurrPos(&pcEncodedString);
            pucCurrPt++;
        }
    }
    return pcReturnString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_Password(SIP_CHAR* pcString)
{
    /**
     * password = *( unreserved / escaped / "&" / "=" / "+" / "$"/ "," )
     * * unreserved = alphanum / mark
     * mark = "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"
     * escaped = "%"   HEXDIG   HEXDIG
     */
    SIP_CHAR* pucCurrPt = pcString;
    SIP_INT32 iLength = SipPf_Strlen(pcString);
    SIP_CHAR *pkucEndPt = pucCurrPt + iLength;

    SIP_CHAR *pcEncodedString = new SIP_CHAR[(3*iLength)];
    SipPf_Memset(pcEncodedString, SIP_NULL, (3*iLength));
    SIP_CHAR *pcReturnString = pcEncodedString;

    while(*(pucCurrPt) != SIP_NULL)
    {
        if (IS_UNRESERVED(*pucCurrPt) || IS_AMPERSAND(*pucCurrPt) ||
            IS_EQUALS(*pucCurrPt) || IS_PLUS(*pucCurrPt) ||
            IS_DOLLAR(*pucCurrPt) || IS_COMMA(*pucCurrPt))
        {
            *pcEncodedString = *pucCurrPt;
            pucCurrPt++;
            pcEncodedString++;
        }
        else if ((pucCurrPt+SIP_TWO <= pkucEndPt) &&
                (IS_ESCAPED(*pucCurrPt, *(pucCurrPt+SIP_ONE), *(pucCurrPt+SIP_TWO))))
        {
            *pcEncodedString = *pucCurrPt;
            *(pcEncodedString+SIP_ONE) = *(pucCurrPt+SIP_ONE);
            *(pcEncodedString+SIP_TWO) = *(pucCurrPt+SIP_TWO);
            pucCurrPt+=3;
            pcEncodedString+=3;
        }
        else
        {
            *pcEncodedString = PERCENT;
            pcEncodedString++;
            SipPf_Sprintf(pcEncodedString,(SIP_CHAR *)"%02X",*pucCurrPt);
            SipEnc_UpdateCurrPos(&pcEncodedString);
            pucCurrPt++;
        }
    }
    return pcReturnString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_Host(SIP_CHAR* pcString)
{
    // host = hostname /  IPv4address /  IPv6reference
    // host : 1*(ALPHANUM , "." , ":" , "[" , "]" , "-")
    SIP_CHAR* pucCurrPt = pcString;
    SIP_INT32 iLength = SipPf_Strlen(pcString);

    SIP_CHAR *pcEncodedString = new SIP_CHAR[(3*iLength)];
    SipPf_Memset(pcEncodedString, SIP_NULL, (3*iLength));
    SIP_CHAR *pcReturnString = pcEncodedString;

    while(*(pucCurrPt) != SIP_NULL)
    {
        if (IS_ALPHANUM(*pucCurrPt) || IS_DOT(*pucCurrPt) || IS_COLON(*pucCurrPt) ||
            IS_LSQURE(*pucCurrPt) || IS_RSQURE(*pucCurrPt) || IS_HYPHEN(*pucCurrPt))
        {
            *pcEncodedString = *pucCurrPt;
            pucCurrPt++;
            pcEncodedString++;
        }
        else
        {
            *pcEncodedString = PERCENT;
            pcEncodedString++;
            SipPf_Sprintf(pcEncodedString,(SIP_CHAR *)"%02X",*pucCurrPt);
            SipEnc_UpdateCurrPos(&pcEncodedString);
            pucCurrPt++;
        }
    }
    return pcReturnString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_TokenParam(SIP_CHAR* pcString)
{
    // token = 1*( alphanum / "-" / "." / "!" / "%" / "*" / "_" / "+" / "`" / "'" / "~" )
    SIP_CHAR* pucCurrPt = pcString;
    SIP_INT32 iLength = SipPf_Strlen(pcString);

    SIP_CHAR *pcEncodedString = new SIP_CHAR[(3*iLength)];
    SipPf_Memset(pcEncodedString, SIP_NULL, (3*iLength));
    SIP_CHAR *pcReturnString = pcEncodedString;

    while(*(pucCurrPt) != SIP_NULL)
    {
        if (IS_TOKEN(*pucCurrPt))
        {
            *pcEncodedString = *pucCurrPt;
            pucCurrPt++;
            pcEncodedString++;
        }
        else
        {
            *pcEncodedString = PERCENT;
            pcEncodedString++;
            SipPf_Sprintf(pcEncodedString,(SIP_CHAR *)"%02X",*pucCurrPt);
            SipEnc_UpdateCurrPos(&pcEncodedString);
            pucCurrPt++;
        }
    }
    return pcReturnString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_TtlParam(SIP_CHAR* pcString)
{
    // ttl = 1*3DIGIT     ; 0 to 255
    SIP_CHAR* pucCurrPt = pcString;
    SIP_INT32 iLength = SipPf_Strlen(pcString);

    SIP_CHAR *pcEncodedString = new SIP_CHAR[(3*iLength)];
    SipPf_Memset(pcEncodedString, SIP_NULL, (3*iLength));
    SIP_CHAR *pcReturnString = pcEncodedString;

    while(*(pucCurrPt) != SIP_NULL)
    {
        if (IS_DIGIT(*pucCurrPt))
        {
            *pcEncodedString = *pucCurrPt;
            pucCurrPt++;
            pcEncodedString++;
        }
        else
        {
            *pcEncodedString = PERCENT;
            pcEncodedString++;
            SipPf_Sprintf(pcEncodedString,(SIP_CHAR *)"%02X",*pucCurrPt);
            SipEnc_UpdateCurrPos(&pcEncodedString);
            pucCurrPt++;
        }
    }
    return pcReturnString;
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_MddrParam(SIP_CHAR* pcString)
{
    // "maddr="   host
    return DoPerEnc_Host(pcString);
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_OtherParam(SIP_CHAR* pcString)
{
    // other-param : ( param-unreserved / unreserved /  escaped )
    // param-unreserved = "[" / "]" / "/" / ":" / "&" / "+"/ "$"
    return DoPerEnc_TokenParam(pcString);
}

SIP_CHAR*  SipPercentEncoding::DoPerEnc_Param(SIP_CHAR* pcName, SIP_CHAR* pcValue)
{

    if (SipPf_Stricmp(pcName, SIP_USER_PRM) == 0)
    {
        return DoPerEnc_TokenParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_METHOD) == 0)
    {
        return DoPerEnc_TokenParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_MADDR_PRM) == 0)
    {
        return DoPerEnc_MddrParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_TTL_PRM) == 0)
    {
        return DoPerEnc_TtlParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_TRNSPORT_PRM) == 0)
    {
        return DoPerEnc_TokenParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_LR_PRM) == 0)
    {
        return DoPerEnc_TokenParam(pcValue);
    }
    else if (SipPf_Stricmp(pcName, SIP_HEADERS) == 0)
    {
        return DoPerEnc_UserAndHeader(pcValue, (SIP_CHAR *)SIP_HEADERS);
    }
    return DoPerEnc_OtherParam(pcValue);
}
