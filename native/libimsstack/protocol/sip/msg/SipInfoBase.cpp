/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipInfoBase.cpp
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
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

#include "msg/SipInfoBase.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "msg/sip_msgutil.h"
#include "platform/sip_pf_string.h"

extern  SIP_CHAR    gaszSipHdr[][SIP_MAX_HDR_LEN];

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/
SipInfoBase::SipInfoBase(SIP_INT32 eHdrType)
    : SipHeaderBase(eHdrType)
{
}
/******************************************************************************
 * Function name      : SipInfoBase::SipInfoBase
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipInfoBase::SipInfoBase(const SipInfoBase &objHeader)
    : SipHeaderBase(objHeader)
{
}

/******************************************************************************
 * Function name      : SipInfoBase::~SipInfoBase
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipInfoBase::~SipInfoBase()
{
}

/******************************************************************************
 * Function name      : SipInfoBase::EncodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipInfoBase::EncodeHdr
(
    SIP_CHAR     **ppucCurrPos,
    SIP_BOOL     bParams /*Default = SIP_TRUE*/
)
{
    const SIP_CHAR *pValue = GetValue();

    if (pValue == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Empty value", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    **ppucCurrPos = LEFT_ANGLE;
    (*ppucCurrPos)++;

    SipPf_Strcpy(*ppucCurrPos, pValue);
    SipEnc_UpdateCurrPos(ppucCurrPos);

    **ppucCurrPos = RIGHT_ANGLE;
    (*ppucCurrPos)++;

    **ppucCurrPos = '\0';

    return EncodeHeaderParameters(ppucCurrPos, bParams);
}

/******************************************************************************
 * Function name      : SipInfoBase::DecodeHdr
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipInfoBase::DecodeHdr
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    if (uiDecLen == SIP_ZERO)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Empty buffer", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR *pucTemp = SIP_NULL;

    if (sipFindPostDelimiter(pucStartPt, pucEndPt, &pucTemp, LEFT_ANGLE) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    pucStartPt = pucTemp;
    pucTemp = SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTemp, RIGHT_ANGLE) == SIP_FALSE)
    {
        return SIP_FALSE;
    }

    SIP_CHAR *pszValue = sipCreateString(pucStartPt, pucTemp);
    if (SetValue(pszValue) == SIP_FALSE)
    {
        if (pszValue != SIP_NULL) {
            delete[] pszValue;
        }
        return SIP_FALSE;
    }
    delete[] pszValue;

    SIP_INT32 iLen = pucTemp-pucStartPt;
    pucStartPt = pucTemp + SIP_TWO;

    pucStartPt = sipSkipFwLWS(pucStartPt, pucEndPt);
    pucTemp = SIP_NULL;

    if (sipFindPostDelimiter(pucStartPt, pucEndPt, &pucTemp, SIP_SEMI) &&
        ((*pucStartPt) == SIP_SEMI) == SIP_TRUE)
    {
        return DecodeHeaderParameters(pucTemp, pucEndPt, SIP_SEMI);
    }

    if (iLen != (uiDecLen-3))
    {
        return SIP_FALSE;
    }

    return SIP_TRUE;
}
