/******************************************************************************
 * Project Name    : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename        : lsdp_pf_debug.c
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

 *****************************************************************************/
/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "sip_pf_datatypes.h"
#include "platform/sip_pf_memory.h"
#include "platform/sip_pf_string.h"

#include "sip_debug.h"
#include "SipUtil.h"


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


/******************************************************************************
 * Function name    : SIP_DEBUG_LOG
 * Description    : This function logs the errors
 *                :
 *
 * Return type    : None
 *
 * Argument      :
 *    [IN]        : pszFilename[IN] - Filename
 *    [IN]        : iLine[IN]       - Line number in file.
 eError[IN]    - Error
 pcFormat[IN]     - Format String
 *
 * Side Effects    :
 * NOTE             :
 ******************************************************************************/
    void SIP_DEBUG_LOG
(
 SIP_UINT32       nCategory,
 SIP_CHAR         *pszFilename,
 SIP_INT32         iLine,
 SIP_CHAR         *pcFormat,...
 )
{
    SipUtil* pObjUtil = SipUtil_GetInstance();

    if (pObjUtil == SIP_NULL)
    {
        return;
    }

    SIP_CHAR cTemp[DEBUG_MSG_MAX_SIZE + 1] = {SIP_ZERO};

    va_list args;
    va_start(args, pcFormat);
    vsnprintf(cTemp, DEBUG_MSG_MAX_SIZE, pcFormat, args);
    va_end(args);

    SIP_CHAR* pcFileName = SipPf_Strdup(pszFilename);

    const SIP_CHAR* pcTemp = (pcFileName != SIP_NULL) ?\
            SipPf_StripFileName(pcFileName) : "xxx";

    SIP_CHAR cFrmtString[DEBUG_MSG_MAX_SIZE + 1] = {SIP_ZERO};
    SipPf_Snprintf(cFrmtString, DEBUG_MSG_MAX_SIZE, "[%s:%d] %s", pcTemp, iLine, cTemp);

    pObjUtil->GetLogger()->DumpLog(nCategory, SIP_NULL, iLine, cFrmtString);

    if (pcFileName != SIP_NULL)
    {
        delete[] pcFileName;
    }
}

/****************************************************************************
  Function Implementation [ENDS]
 *****************************************************************************/
