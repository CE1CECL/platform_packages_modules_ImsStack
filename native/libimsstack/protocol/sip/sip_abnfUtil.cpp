/*
   Author
   <table>
   date      author                description
   --------  --------------        ----------
   20150218  vijay.nair@           Created
   </table>

   Description

 */
#include "sip_abnfUtil.h"
#include "platform/sip_pf_memory.h"

SIP_CHAR* sipSkipRwLWS(SIP_CHAR *pucStartPt, SIP_CHAR *pucEndPt)
{
    /*NULL validation*/
    if ((pucStartPt == SIP_NULL) || (pucEndPt == SIP_NULL))
    {
        return SIP_NULL;
    }

    SIP_CHAR* pucBuffTemp = pucEndPt;

    while ((pucBuffTemp > pucStartPt) && IS_WSP(*pucBuffTemp))
    {
        pucBuffTemp--;
    }

    SIP_CHAR* pucBuffPrev = pucBuffTemp - SIP_ONE;

    /*Allow one CRLF*/
    if ((pucBuffPrev > pucStartPt) && IS_CRLF(*pucBuffPrev, *pucBuffTemp))
    {
        pucBuffTemp = pucBuffTemp - SIP_TWO;
    }

    while ((pucBuffTemp > pucStartPt) && IS_WSP(*pucBuffTemp))
    {
        pucBuffTemp--;
    }

    return pucBuffTemp;
}

SIP_CHAR* sipCreateString(const SIP_CHAR *pucStartPt, const SIP_CHAR *pucEndPt)
{
    /*Validate the input*/
    if (pucEndPt < pucStartPt)
    {
        return SIP_NULL;
    }
    /*Take one extraa memory for NULL*/
    SIP_UINT16 usSize = (pucEndPt - pucStartPt) + SIP_TWO;
    SIP_CHAR *pszString =  new SIP_CHAR[usSize];
    if (pszString == SIP_NULL)
    {
        return pszString;
    }
    SipPf_Memset(pszString, SIP_NULL, usSize);
    /*Leave the last point as NULL*/
    SipPf_Memcpy(pszString, pucStartPt,(usSize -SIP_ONE));
    return pszString;
}

SIP_CHAR* sipSkipFwLWS(SIP_CHAR *pucStartPt, SIP_CHAR *pucEndPt)
{
    /*NULL validation*/
    /*    LWS = [*WSP   CRLF]   1*WSP  ; linear whitespace  */

    if ((pucStartPt == SIP_NULL) || (pucEndPt == SIP_NULL))
    {
        return SIP_NULL;
    }

    SIP_CHAR* pucBuffTemp = pucStartPt;

    while ((pucBuffTemp <= pucEndPt) && IS_WSP(*pucBuffTemp))
    {
        pucBuffTemp++;
    }

    SIP_CHAR* pucBuffNext = pucBuffTemp + SIP_ONE;

    /*Allow one CRLF*/
    if ((pucBuffNext < pucEndPt) && IS_CRLF(*pucBuffTemp, *pucBuffNext))
    {
        pucBuffTemp = pucBuffTemp + SIP_TWO;
    }

    while ((pucBuffTemp <= pucEndPt) && IS_WSP(*pucBuffTemp))
    {
        pucBuffTemp++;
    }

    return pucBuffTemp;
}

SIP_BOOL sipFindLWS(SIP_CHAR *pucStartPt, SIP_CHAR *pucEndPt, SIP_CHAR **pucTempLoc)
{
    SIP_CHAR        *pucNext1Pt = pucStartPt + SIP_ONE;
    SIP_CHAR        *pucNext2Pt = pucStartPt + SIP_TWO;

    //added for case aplphanum+space+alphanum+CRLF by Ajay Bukan on 17-jan-2013
    while (((pucNext2Pt <= pucEndPt) || (IS_WSP(*pucStartPt))))
    {
        if ((IS_WSP(*pucStartPt) ||
                ((IS_CRLF(*pucStartPt, *pucNext1Pt) && (IS_WSP(*pucNext2Pt))))))
        {
            *pucTempLoc = pucStartPt - SIP_ONE;
            return SIP_TRUE;
        }
        pucStartPt++;
        pucNext1Pt = pucStartPt + SIP_ONE;
        pucNext2Pt = pucStartPt + SIP_TWO;
    }
    return SIP_FALSE;
}

SIP_BOOL sipFindDelimiter(SIP_CHAR *pucStartPt, SIP_CHAR *pucEndPt, SIP_CHAR **pucTempLoc,
                                SIP_CHAR ucDelimiter)
{
    while (pucStartPt <= pucEndPt)
    {
        if (*pucStartPt == ucDelimiter)
        {
            *pucTempLoc = pucStartPt - SIP_ONE;
            return SIP_TRUE;
        }
        pucStartPt = pucStartPt + SIP_ONE;
    }
    return SIP_FALSE;
}

SIP_BOOL sipFindPreDelimiter(SIP_CHAR *pucStartPt, SIP_CHAR *pucEndPt, SIP_CHAR **pucTempLoc,
                                    SIP_CHAR ucDelimiter)
{
    SIP_BOOL      bDQuote = SIP_FALSE;
    SIP_CHAR        *pucPrevPt = pucStartPt;

    while (pucStartPt <= pucEndPt)
    {
        if ((*pucStartPt == DQUOTE) && (*pucPrevPt != BACKSLASH))
        {
            bDQuote = (SIP_BOOL)!bDQuote;
        }

        if (*pucStartPt == ucDelimiter)
        {
            /*for the case of quoted string we have to
              prevent returning in case of " , " and " ; " */
            if (bDQuote == SIP_FALSE)
            {
                *pucTempLoc = pucStartPt - SIP_ONE;
                return SIP_TRUE;
            }
        }
        pucPrevPt = pucStartPt;
        pucStartPt = pucStartPt + SIP_ONE;
    }
    return SIP_FALSE;
}

SIP_CHAR* SkipConsecutiveCRLFs(SIP_CHAR *pucStartPt)
{
    while ((pucStartPt != SIP_NULL && (pucStartPt+1) != SIP_NULL)
        && (IS_CRLF(*pucStartPt, *(pucStartPt+1))))
    {
        pucStartPt = pucStartPt + SIP_TWO;
    }
    return pucStartPt;
}
