/******************************************************************************
 * Project Name    : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename        : SipTrace.cpp
 * Purpose        :  debug Functions
 * Platform        : Windows OR Android
 * Author(s)        : Seema
 * E-mail id.        : seema.lijo@
 * Creation date     : april 6,2010
 *
 * Edit History             Modification                     Description(s)
 *
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * april 6,10          Seema              0.0a            ---   Initial creation

 ******************************************************************************/
/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_memory.h"
#include "platform/sip_pf_string.h"

#include "SipTrace.h"
#include "SipUtil.h"
#include "sip_error.h"

/****************************************************************************
  Global Variables
 *****************************************************************************/
SipTrace    *gpObjTrace = SIP_NULL;

/****************************************************************************
  Function Implementation [STARTS]
 *****************************************************************************/

/******************************************************************************
 * Function name    : SipTrace
 * Description    :
 *                :
 *
 * Return type    : None
 *
 * Argument      :
 *                :

 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
SipTrace::SipTrace()
{
    for (SIP_UINT16 usIndex = SIP_ZERO; usIndex < ESIPTRACE_MODEND; usIndex++)
    {
        m_ausModTrace[usIndex] =
            (ESIPTRACE_TYPEMESSAGE | ESIPTRACE_TYPENORMAL | ESIPTRACE_TYPEALL);
    }

    return;
}
/******************************************************************************
 * Function name    : ~SipTrace
 * Description    :
 *                :
 *
 * Return type    : None
 *
 * Argument      :
 *                :

 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
SipTrace::~SipTrace()
{
}

/******************************************************************************
 * Function name    : EnableTrace
 * Description    : This function will enable the given trace type t for the module.
 *                :
 *
 * Return type    : SIP_BOOL
 SIP_TRUE if setting trace type is
 successful
 *
 * Argument      :
 *    [IN]        : eModule[IN]   - Module to trace
 *    [IN]        : ulTraceType[IN] - Type of trace to be enabled.
 pusError[OUT]    - Error if any
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    SIP_BOOL SipTrace::EnableTrace
(
 SipEn_TraceModules    eModule,
 SIP_UINT32        ulTraceType
 )
{
    if (eModule >= ESIPTRACE_MODEND)
    {
        return SIP_FALSE;
    }

    m_ausModTrace[eModule] = ulTraceType;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name    : EnableTrace
 * Description    : This function will enable the given trace type for all the module.
 *                :
 *
 * Return type    : SIP_BOOL
 SIP_TRUE if setting trace type is
 successful
 *
 * Argument      :
 *    [IN]        : ulTraceType[IN] - Type of trace to be enabled.
 pusError[OUT]    - Error if any
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    SIP_BOOL SipTrace::EnableTrace
(
 SIP_UINT32        ulTraceType
 )
{

    for (SIP_UINT16 usIndex = SIP_ZERO; usIndex < ESIPTRACE_MODEND; usIndex++)
    {
        m_ausModTrace[usIndex] = ulTraceType;
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name    : DisableTrace
 * Description    : This function will disable the given trace type for the module.
 *                :
 *
 * Return type    : SIP_BOOL
 SIP_TRUE if setting trace type is
 successful
 *
 * Argument      :
 *    [IN]        : eModule[IN]   - Module
 *    [IN]        : ulTraceType[IN] - Type of trace to be disabled.
 pusError[OUT]    - Error if any
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    SIP_BOOL SipTrace::DisableTrace
(
 SipEn_TraceModules    eModule,
 SIP_UINT32        ulTraceType
 )
{
    if (eModule >= ESIPTRACE_MODEND)
    {
        return SIP_FALSE;
    }

    m_ausModTrace[eModule] = m_ausModTrace[eModule] & ~ulTraceType;
    return SIP_TRUE;
}

/******************************************************************************
 * Function name    : DisableTrace
 * Description    : This function will disable the given trace type for all the module.
 *                :
 *
 * Return type    : SIP_BOOL
 SIP_TRUE if setting trace type is
 successful
 *
 * Argument      :
 *    [IN]        : ulTraceType[IN] - Type of trace to be enabled.
 pusError[OUT]    - Error if any
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    SIP_BOOL SipTrace::DisableTrace
(
 SIP_UINT32        ulTraceType
 )
{

    for (SIP_UINT16 usIndex = SIP_ZERO; usIndex < ESIPTRACE_MODEND; usIndex++)
    {
        m_ausModTrace[usIndex] = m_ausModTrace[usIndex] & ~ulTraceType;
    }
    return SIP_TRUE;
}

/******************************************************************************
 * Function name    : IsTraceEnable
 * Description    : This function check if for the given module particulr trace type is enable or not
 *                :
 *
 * Return type    : SIP_BOOL
 SIP_TRUE if trace type is enable
 *
 * Argument      :
 *    [IN]        : eModule[IN]   - Module
 *    [IN]        : ulTraceType[IN] - Type of trace to.
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    SIP_BOOL SipTrace::IsTraceEnable
(
 SipEn_TraceModules     eModule,
 SipEn_TraceTypes        eTraceType
 )
{
    if (eModule >= ESIPTRACE_MODEND)
    {
        return SIP_FALSE;
    }

    if (m_ausModTrace[eModule] & eTraceType)
    {
        return SIP_TRUE;
    }

    return SIP_FALSE;
}


/******************************************************************************
 * Function name    : SIP_TRACE_LOG
 * Description    : This function logs the trace in case module is set for tracing.
 *                :
 *
 * Return type    : SIP_BOOL

 SIP_TRUE if logging of trace is successful
 *
 * Argument      :
 *    [IN]        : eModule[IN] : Module traced
 eTraceType[IN] - Trace type
 *    [IN]        : pszFilename[IN] - File traced.
 iLine[IN]    - Line number in file traced
 pcFormat[IN] - Format String
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    void SIP_TRACE_LOG
(
 SIP_UINT32       nCategory,
 SIP_CHAR             *pszFilename,
 SIP_INT32             iLine,
 SIP_CHAR             *pcFormat,...)
{
    SipUtil* pObjUtil = SipUtil_GetInstance();
    SipTrace* pObjTrace = SipTrace_GetInstance();

    if ((pObjTrace == SIP_NULL) || (pObjUtil == SIP_NULL))
    {
        return;
    }

    va_list args;
    va_start(args, pcFormat);
    SIP_CHAR cTemp[SIP_TRACE_MAX_SIZE + 1]= {SIP_ZERO};
    vsnprintf(cTemp, SIP_TRACE_MAX_SIZE, pcFormat, args);
    va_end(args);

    SIP_CHAR* pcFileName = SipPf_Strdup(pszFilename);
    const SIP_CHAR* pcTemp = (pcFileName != SIP_NULL) ?\
            SipPf_StripFileName(pcFileName) : "xxx";

    SIP_CHAR cFrmtString[SIP_TRACE_MAX_SIZE + 1] = {SIP_ZERO};
    SipPf_Snprintf(cFrmtString, SIP_TRACE_MAX_SIZE, "[%s:%d] %s", pcTemp, iLine, cTemp);

    pObjUtil->GetLogger()->DumpLog(nCategory, SIP_NULL, iLine, cFrmtString);

    if (pcFileName != SIP_NULL)
    {
        delete[] pcFileName;
    }
}

/****************************************************************************
  Local Function Implementation [ENDS]
 *****************************************************************************/
/****************************************************************************
  Global Function Implementation [START]
 *****************************************************************************/
void SipTrace_Construct()
{
    SipTrace *pObj = gpObjTrace;

    if (pObj)
    {
        return;
    }

    pObj = new SipTrace();
    gpObjTrace = pObj;
}

void SipTrace_Destruct()
{
    SipTrace *pObj = gpObjTrace;

    if (pObj == SIP_NULL)
    {
        return;
    }

    delete pObj;
    gpObjTrace = SIP_NULL;
}

SipTrace* SipTrace_GetInstance()
{
    SipTrace *pObj = gpObjTrace;
    return pObj;
}

/****************************************************************************
  global Function Implementation [ENDS]
 *****************************************************************************/
