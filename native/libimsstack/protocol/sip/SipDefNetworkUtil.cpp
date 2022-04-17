/***********************************************************
 * Project Name : MSG2_SIP_RTP
 * Group        : MSG2
 * Security     : Confidential
 ***********************************************************/

/**********************************************************
 * Filename          : sip_IDefTimerUtil.cpp
 * Purpose           : Default implementation of timer service
 * Platform          : (E.g. Windows 2000)
 * Author(s)         :
 * E-mail id.        :
 * Creation date      : Jul 31 , 2010
 *
 * Edit History             Modification                     Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------

 *****************************************************************************
 *****************************************************************************
 Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_string.h"
#include "SipDefNetworkUtil.h"
#include "INetSocket.h"
/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Global Extern Variables
 *****************************************************************************/

/****************************************************************************
  Global Variables
 *****************************************************************************/

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

SipDefNetworkUtil::SipDefNetworkUtil()
{
}
SipDefNetworkUtil::~SipDefNetworkUtil()
{
}

SIP_BOOL SipDefNetworkUtil::SendToNetwork
(
 SipTransportBuffer     *pobjTranspSipBuffer,
 SipTransportParameter            *pobjFinalTranspParam,
 ISipUserData            *pobjUserData
 )
{
    (void)pobjTranspSipBuffer;
    (void)pobjFinalTranspParam;
    (void)pobjUserData;
    return SIP_TRUE;
}

SIP_BOOL SipDefNetworkUtil::CheckTCPConnection
(
 SipTransportParameter            *pobjTransportParam,
 ISipUserData            *pobjUserData
 )
{
    (void)pobjTransportParam;
    (void)pobjUserData;

    return SIP_TRUE;
}
SIP_BOOL SipDefNetworkUtil::AbortTransmission
(
 SipTransportParameter     *pobjTranspParam,
 ISipUserData        *pobjUserData
 )
{
    (void)pobjTranspParam;
    (void)pobjUserData;

    return SIP_TRUE;
}
/****************************************************************************
  Function Implementation [ENDS]
 *****************************************************************************/
