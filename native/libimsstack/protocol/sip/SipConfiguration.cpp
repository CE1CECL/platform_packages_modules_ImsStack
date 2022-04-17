/*****************************************************************************
 * Project Name  : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/*****************************************************************************
 * Filename    : lsdp_uf_trace.c
 * Purpose    :  debug Functions
 * Platform    : Windows OR Android
 * Author(s)    : Seema
 * E-mail id.    : seema.lijo@
 * Creation date   : april 6,2010
 *
 * Edit History     Modification       Description(s)
 *
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * april 6,10    Seema      0.0a    --- Initial creation

 *****************************************************************************/
/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_memory.h"
#include "platform/sip_pf_string.h"

#include "SipTrace.h"
#include "sip_error.h"

#include "SipConfiguration.h"

// 3GPP-based timer intervals
#define DEFAULT_T1      2000
#define DEFAULT_T2      16000

SipConfiguration* SipConfiguration::pSipConfig = SIP_NULL;

SipConfiguration::SipConfiguration()
{
    bPANIHeaderForACK = SIP_FALSE;

    /* Normal form, Single Line and loose parsing */
    m_usParseStyle = ~ESIPMSGOPT_ENCMULTILINE;
    m_usParseStyle &= ~ESIPMSGOPT_ENCSHORTFORM;
    m_usParseStyle &= ~ESIPMSGOPT_DECSTRICT;

    m_usT1                  = DEFAULT_T1;
    m_usT2                  = DEFAULT_T2;
    m_usT4                  = DEFAULT_T2 + 1000;
    m_usTimerB              = 64*m_usT1;
    m_usTimerC              = 180000;
    m_usTimerCr             = 180000;
    m_usTimerD_T3           = 64*m_usT1;
    m_usTimerF_T3           = 64*m_usT1;
    m_usTimerH              = 64*m_usT1;
    m_usTimerI_T4           = m_usT4;
    m_usTimerJ_T3           = 64*m_usT1;
    m_usTimerK_T4           = m_usT4;
}

SipConfiguration::~SipConfiguration()
{
}

SIP_VOID SipConfiguration::SetMultiLineEncoding(SIP_BOOL bEnableMultiLine)
{
    if (bEnableMultiLine == SIP_TRUE)
    {
        m_usParseStyle = m_usParseStyle | ESIPMSGOPT_ENCMULTILINE;
    }
    else
    {
        m_usParseStyle = m_usParseStyle & ESIPMSGOPT_ENCMULTILINE;
    }
}

SIP_VOID SipConfiguration::SetShortFormEncoding(SIP_BOOL bEncInShortForm)
{
    if (bEncInShortForm == SIP_TRUE)
    {
        m_usParseStyle = m_usParseStyle | ESIPMSGOPT_ENCSHORTFORM;
    }
    else
    {
        m_usParseStyle = m_usParseStyle & ESIPMSGOPT_ENCSHORTFORM;
    }
}

SIP_VOID SipConfiguration::SetDecodeStrictness(SIP_BOOL bEnableStrictDecode)
{
    if (bEnableStrictDecode == SIP_TRUE)
    {
        m_usParseStyle = m_usParseStyle & ESIPMSGOPT_DECSTRICT;
    }
    else
    {
        m_usParseStyle = m_usParseStyle & ~ESIPMSGOPT_DECSTRICT;
    }
}

SipConfiguration* SipConfiguration::GetInstance()
{
    if (pSipConfig == SIP_NULL)
    {
        pSipConfig = new SipConfiguration();
    }

    return pSipConfig;
}
