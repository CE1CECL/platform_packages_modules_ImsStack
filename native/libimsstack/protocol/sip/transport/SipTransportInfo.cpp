/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename          : sip_filename.cpp
 * Purpose           :
 * Platform          : Windows OR Android
 * Author(s)         :
 * E-mail id.        : name@
 * Creation date        : Month. Date,10
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

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

#include "sip_pf_datatypes.h"
#include "msg/sip_comdef.h"
#include "SipConfiguration.h"
#include "transport/SipTransportInfo.h"
#include "SipTrace.h"

SipTransportInfo::SipTransportInfo(SipTransportParameter *pobjTranspParam,
                SipTransportBuffer *pobjTransSipBuffer)
                :m_cNumTimeReqSent(SIP_ZERO), m_pobjActualDestParam (SIP_NULL),
                m_pobjTranspParam (SIP_NULL), m_pobjSentBuffer (SIP_NULL),
                m_pobjSentSipMsg (SIP_NULL), m_bExceedMTU (SIP_FALSE)
{
    if (pobjTranspParam == SIP_NULL){
        return;
    }
    m_pobjTranspParam = new SipTransportParameter(pobjTranspParam);
    m_pobjSentBuffer = pobjTransSipBuffer;
}


SipTransportInfo::~SipTransportInfo()
{
    if (m_pobjActualDestParam != SIP_NULL)
    {
        delete m_pobjActualDestParam;
        m_pobjActualDestParam = SIP_NULL;
    }

    if (m_pobjTranspParam != SIP_NULL)
    {
        delete m_pobjTranspParam;
    }

    if (m_pobjSentBuffer != SIP_NULL)
    {
        delete m_pobjSentBuffer;
    }

    if (m_pobjSentSipMsg != SIP_NULL)
    {
        delete m_pobjSentSipMsg;
    }
}


SipTransportParameter* SipTransportInfo::GetMsgSentTranspParam()
{
    return m_pobjActualDestParam;
}

SIP_BOOL SipTransportInfo::SetMsgSentTranspParam(SipTransportParameter* pobjTranspParam)
{
    m_pobjActualDestParam = pobjTranspParam;
    return SIP_TRUE;
}

SipTransportBuffer* SipTransportInfo::GetTranspSipBuffer()
{
    return m_pobjSentBuffer;
}

SIP_BOOL SipTransportInfo::SetExceedMTUFlag(SIP_BOOL bFlag)
{
    m_bExceedMTU = bFlag;
    return SIP_TRUE;
}

SIP_BOOL SipTransportInfo::IsExceedMTU()
{
    return m_bExceedMTU;
}

SipMessage* SipTransportInfo::GetSentSipMsg()
{
    return m_pobjSentSipMsg;
}

SIP_VOID SipTransportInfo::SetSentSipMsg(SipMessage* _pObjSipMsg)
{
    if (m_pobjSentSipMsg != SIP_NULL)
    {
        m_pobjSentSipMsg->SipDelete();
        m_pobjSentSipMsg = SIP_NULL;
    }
    m_pobjSentSipMsg = _pObjSipMsg;
}
