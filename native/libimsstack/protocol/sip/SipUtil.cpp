/******************************************************************************
 * Project Name    : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename        : SipUtil.cpp
 * Purpose        :  utility Functions
 * Platform        : Windows OR Android
 * Author(s)        : Seema
 * E-mail id.        : seema.lijo@
 * Creation date     : may 4,2010
 *
 * Edit History             Modification                     Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * may 4,2010         Seema          0.0a        ---            Initial creation
 * July,21,2010        Giridhar        0.0b        ---            Coverted to cpp

 ****************************************************************************/
/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_string.h"

#include "SipUtil.h"
#include "sip_error.h"

#include "SipDefTimerUtil.h"
#include "SipDefLoggerUtil.h"
#include "SipDefNetworkUtil.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Global Extern Variables
 *****************************************************************************/

/****************************************************************************
  Global Variables
 *****************************************************************************/
static SipUtil *gpObjUtil = SIP_NULL;

/****************************************************************************
  Local Function Declaration [Static Functions]
 *****************************************************************************/

/****************************************************************************
  Local Function Implementation [STARTS]
 *****************************************************************************/


/****************************************************************************
  Local Function Implementation [ENDS]
 *****************************************************************************/

/****************************************************************************
  Function Implementation [STARTS]
 *****************************************************************************/
    SipUtil::SipUtil()
:m_pObjTxnListener(SIP_NULL)
{
    /* Create Default In-Build Services */
    m_pObjLoggerUtil = new SipDefLoggerUtil();
    m_pObjTimerUtil = new SipDefTimerUtil();
    m_pObjNetworkUtil = new SipDefNetworkUtil();
}

SipUtil::~SipUtil()
{
    if (m_pObjTxnListener != SIP_NULL)
    {
        delete m_pObjTxnListener;
    }

    if (m_pObjNetworkUtil != SIP_NULL)
    {
        delete m_pObjNetworkUtil;
    }

    if (m_pObjTimerUtil != SIP_NULL)
    {
        delete m_pObjTimerUtil;
    }

    if (m_pObjLoggerUtil != SIP_NULL)
    {
        delete m_pObjLoggerUtil;
    }

}

SIP_VOID SipUtil::RegisterTimer(ISipTimerUtil *pObjTimerUtil)
{
    if (m_pObjTimerUtil != SIP_NULL)
    {
        delete m_pObjTimerUtil;
    }

    m_pObjTimerUtil = pObjTimerUtil;
}

SIP_VOID SipUtil::RegisterLogger(ISipLoggerUtil* pObjLoggerUtil)
{
    if (m_pObjLoggerUtil != SIP_NULL)
    {
        delete m_pObjLoggerUtil;
    }

    m_pObjLoggerUtil = pObjLoggerUtil;
}
SIP_VOID SipUtil::RegisterNetwork(ISipNetworkUtil* pObjNwUtil)
{
    if (m_pObjNetworkUtil != SIP_NULL)
    {
        delete m_pObjNetworkUtil;
    }

    m_pObjNetworkUtil = pObjNwUtil;
}

SIP_VOID SipUtil::RegisterTxnListener(ISipTxnListener *pobjTxnListener)
{
    if (m_pObjTxnListener != SIP_NULL)
    {
        delete m_pObjTxnListener;
    }

    m_pObjTxnListener = pobjTxnListener;
}

ISipTimerUtil* SipUtil::GetTimer()
{
    return m_pObjTimerUtil;
}

ISipLoggerUtil* SipUtil::GetLogger()
{
    return m_pObjLoggerUtil;
}
ISipNetworkUtil* SipUtil::GetNetwork()
{
    return m_pObjNetworkUtil;
}

ISipTxnListener* SipUtil::GetTxnListener()
{
    return m_pObjTxnListener;
}

/****************************************************************************
  Function Implementation [ENDS]
 ****************************************************************************/
/****************************************************************************
  Global Function Implementation [START]
 *****************************************************************************/
SIP_VOID SipUtil_Construct()
{
    SipUtil *pObj = gpObjUtil;

    if (pObj)
    {
        return;
    }

    pObj = new SipUtil();
    gpObjUtil = pObj;
}

SIP_VOID SipUtil_Destruct()
{
    SipUtil *pObj = gpObjUtil;

    if (pObj == SIP_NULL)
    {
        return;
    }

    delete pObj;
    gpObjUtil = SIP_NULL;
}

SipUtil* SipUtil_GetInstance()
{
    SipUtil *pObj = gpObjUtil;
    return pObj;
}

/****************************************************************************
  global Function Implementation [ENDS]
 *****************************************************************************/
