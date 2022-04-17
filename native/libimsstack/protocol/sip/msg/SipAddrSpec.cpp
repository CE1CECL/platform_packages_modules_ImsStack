/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipAddrSpec.cpp
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
#include "IPAddress.h"
#include "msg/SipAddrSpec.h"
#include "platform/sip_pf_string.h"
#include "platform/sip_pf_memory.h"
#include "sip_error.h"
#include "SipTrace.h"
#include "sip_debug.h"
#include "msg/SipHeaderBase.h"
#include "msg/SipMessage.h"
#include "msg/sip_msgutil.h"
/****************************************************************************
  Macro Definitions
 *****************************************************************************/
#define SIP_SIP_ENC "sip:"
#define SIP_SIPS_ENC "sips:"
/****************************************************************************
  Class Member Function Implementations
 *****************************************************************************/

/******************************************************************************
 * Function name      : SipUri::SipUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUri::SipUri()
    : m_pszUser(SIP_NULL)
    , m_pszPassword(SIP_NULL)
    , m_pszHost(SIP_NULL)
    , m_usPort(SIP_ZERO)
    , m_eHostType(SipAddrSpec::HOST_NAME)
    , m_pobjUriParamList(SIP_NULL)
    , m_pobjUriHdrParamList(SIP_NULL)
    , m_pobjParameterComponent(SIP_NULL)
{

}

/******************************************************************************
 * Function name      : SipUri::SipUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUri::SipUri(const SipUri& objSipUri)
    : m_pszUser(SipPf_Strdup(objSipUri.m_pszUser))
    , m_pszPassword(SipPf_Strdup(objSipUri.m_pszPassword))
    , m_pszHost(SipPf_Strdup(objSipUri.m_pszHost))
    , m_usPort(objSipUri.m_usPort)
    , m_eHostType(objSipUri.m_eHostType)
    , m_pobjUriParamList(SIP_NULL)
    , m_pobjUriHdrParamList(SIP_NULL)
    , m_pobjParameterComponent(objSipUri.m_pobjParameterComponent)
{
    if (objSipUri.m_pobjUriParamList != SIP_NULL)
    {
        m_pobjUriParamList = new SipParameterList(*(objSipUri.m_pobjUriParamList));
    }

    if (objSipUri.m_pobjUriHdrParamList != SIP_NULL)
    {
        m_pobjUriHdrParamList = new SipParameterList(*(objSipUri.m_pobjUriHdrParamList));
    }
}

/******************************************************************************
 * Function name      : SipUri::~SipUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipUri::~SipUri()
{
    if (m_pszUser != SIP_NULL)
    {
        delete[] m_pszUser;
    }
    if (m_pszPassword != SIP_NULL)
    {
        delete[] m_pszPassword;
    }
    /*Host Port contains host(can be domain name or IP) and port*/
    if (m_pszHost != SIP_NULL)
    {
        delete[] m_pszHost;
    }
    /*List of obj of CSipNameValue*/
    if (m_pobjUriParamList != SIP_NULL)
    {
        m_pobjUriParamList->SipDelete();
    }
    /*List of obj of CSipNameValue*/
    if (m_pobjUriHdrParamList != SIP_NULL)
    {
        m_pobjUriHdrParamList->SipDelete();
    }
}


/******************************************************************************
 * Function name      : SipUri::SetUser
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::SetUser(const SIP_CHAR *pkszUser)
{
    return SetCharVar(pkszUser, m_pszUser);
}

/******************************************************************************
 * Function name      : SipUri::SetPassword
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
 SIP_BOOL SipUri::SetPassword(const SIP_CHAR *pkszPass)
{
    return SetCharVar(pkszPass, m_pszPassword);
}

/******************************************************************************
 * Function name      : SipUri::SetHost
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::SetHost(const SIP_CHAR *pkszHost)
{
    return SetCharVar(pkszHost, m_pszHost);
}

/******************************************************************************
 * Function name      : SipUri::SetParameterComponent
 *
 * Description    :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_VOID SipUri::SetParameterComponent(IParameterComponent *pobjParameterComponent)
{
    m_pobjParameterComponent = pobjParameterComponent;
}

/******************************************************************************
 * Function name      : SipUri::AddUriParam
 *
 * Description       Add only name
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::AddUriParam
(
 const SIP_CHAR     *pszName
 )
{
    if (m_pobjUriParamList == SIP_NULL)
    {
        m_pobjUriParamList = new SipParameterList();
    }
    if (m_pobjUriParamList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "AddUriParam: Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return m_pobjUriParamList->Add(pszName);
}

/******************************************************************************
 * Function name      : SipUri::AddUriParam
 *
 * Description       Add name, val
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::AddUriParam
(
 const SIP_CHAR     *pszName,
 const SIP_CHAR     *pszValue
 )
{
    if (m_pobjUriParamList == SIP_NULL) {
        m_pobjUriParamList = new SipParameterList();
    }
    if (m_pobjUriParamList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "AddUriParam: Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return m_pobjUriParamList->Add(pszName,pszValue);
}

/******************************************************************************
 * Function name      : SipUri::AddUriParam
 *
 * Description       Add name, val, param type
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::AddUriParam
(
 const SIP_CHAR     *pszName,
 const SIP_CHAR     *pszValue,
 SIP_INT32    ePrmType
 )
{
    if (m_pobjUriParamList == SIP_NULL)
    {
        m_pobjUriParamList = new SipParameterList();
    }
    if (m_pobjUriParamList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "AddUriParam: Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return m_pobjUriParamList->Add(pszName,pszValue,ePrmType);
}
/******************************************************************************
 * Function name      : SipUri::AddHeader
 *
 * Description       Add a hdr to uri
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::AddHeader
(
 const SIP_CHAR     *pszName
 )
{
    if (pszName == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "AddHeader: NULL param received",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pobjUriHdrParamList == SIP_NULL)
    {
        m_pobjUriHdrParamList= new SipParameterList();
    }
    if (m_pobjUriHdrParamList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "AddHeader: Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pobjUriHdrParamList->Add(pszName);
    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipUri::AddHeader
 *
 * Description       Add a hdr to uri
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::AddHeader
(
 const SIP_CHAR     *pszName,
 const SIP_CHAR     *pszValue
 )
{
    if (m_pobjUriHdrParamList == SIP_NULL)
    {
        m_pobjUriHdrParamList= new SipParameterList();
    }
    if (m_pobjUriHdrParamList == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "Malloc Failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    m_pobjUriHdrParamList->Add(pszName, pszValue);

    return SIP_TRUE;
}
/******************************************************************************
 * Function name      : SipUri::GetUriType
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList* SipUri::GetUriParamList()
{
    if (m_pobjUriParamList != SIP_NULL) {
        m_pobjUriParamList->increment();
    }
    return m_pobjUriParamList;
}

/******************************************************************************
 * Function name      : SipUri::GetHdrParamList
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipParameterList* SipUri::GetHdrParamList()
{
    if (m_pobjUriHdrParamList != SIP_NULL)
    {
        m_pobjUriHdrParamList->increment();
    }
    return m_pobjUriHdrParamList;
}

/******************************************************************************
 * Function name      : SipUri::RemoveUriParam
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::RemoveUriParam
(
 const SIP_CHAR     *pszName
 )
{
    if (m_pobjUriParamList != SIP_NULL)
    {
        return m_pobjUriParamList->Remove(pszName);
    }
    return SIP_FALSE;
}

/******************************************************************************
 * Function name      : SipUri::RemoveHdrParam
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::RemoveHdrParam
(
 const SIP_CHAR     *pszName
 )
{
    if (m_pobjUriHdrParamList != SIP_NULL)
    {
        return m_pobjUriHdrParamList->Remove(pszName);
    }
    return SIP_FALSE;
}

/********************************************************************************
  Encode a sip uri
 ********************************************************************************/
#define MAX_PORT_LEN 20
SIP_BOOL SipUri::EncodeSipUri
(
 SIP_CHAR   **ppucCurrPos
 )
{
    /* encoding of user info
       userinfo = ( user / telephone-subscriber ) [ ":" password ] "@"  */
    if (m_pszUser != SIP_NULL)
    {
        /*Do Percent Encoding if Required*/
        if ((m_pobjParameterComponent != SIP_NULL) &&
            m_pobjParameterComponent->IsValidComponent(SIP_USER))
        {
            SIP_CHAR *pszTempUser =
                SipPercentEncoding::DoPerEnc_UserAndHeader(m_pszUser, (SIP_CHAR *)SIP_USER);
            SipPf_Strcpy(*ppucCurrPos, pszTempUser);
            delete[] pszTempUser;
        }
        else
        {
            SipPf_Strcpy(*ppucCurrPos, m_pszUser);
        }

        SipEnc_UpdateCurrPos(ppucCurrPos);

        if (m_pszPassword != SIP_NULL)
        {
            /*encode the password*/
            SIP_ENC_COLON(*ppucCurrPos);

            /*Do Percent Encoding if Required*/
            if ((m_pobjParameterComponent != SIP_NULL) &&
                m_pobjParameterComponent->IsValidComponent(SIP_PASSWORD))
            {
                SIP_CHAR *pszTempPassword =
                    SipPercentEncoding::DoPerEnc_Password(m_pszPassword);
                SipPf_Strcpy(*ppucCurrPos, pszTempPassword);
                delete[] pszTempPassword;
            }
            else
            {
                SipPf_Strcpy(*ppucCurrPos, m_pszPassword);
            }

            SipEnc_UpdateCurrPos(ppucCurrPos);
        }

        SIP_ENC_ATTHERATE(*ppucCurrPos);
    }

    /* encoding of host port
       hostport = host [ ":" port ] */
    if (m_pszHost != SIP_NULL)
    {
        /*Check for the IPV6 and IPV4 */
        if (m_eHostType == SipAddrSpec::HOST_IPV6)
        {
            **ppucCurrPos = LEFT_SQUARE;
            (*ppucCurrPos)++;

            SipPf_Strcpy(*ppucCurrPos, m_pszHost);
            SipEnc_UpdateCurrPos(ppucCurrPos);

            **ppucCurrPos = RIGHT_SQUARE;
            (*ppucCurrPos)++;
        }
        /*Do Percent Encoding if Required*/
        else if ((m_pobjParameterComponent != SIP_NULL) &&
            (m_pobjParameterComponent->IsValidComponent(SIP_HOST)))
        {
            SIP_CHAR *pszTempHost = SipPercentEncoding::DoPerEnc_Host(m_pszHost);
            SipPf_Strcpy(*ppucCurrPos, pszTempHost);
            SipEnc_UpdateCurrPos(ppucCurrPos);
            delete[] pszTempHost;
        }
        else
        {
            SipPf_Strcpy(*ppucCurrPos, m_pszHost);
            SipEnc_UpdateCurrPos(ppucCurrPos);
        }

        /*Encoding of Port*/
        if ((m_usPort != SIP_ZERO) &&
            (m_usPort != SIP_UNSPECIFIED_PORT))
        {
            //TODO Percent Encoding
            SIP_CHAR ucTmp[MAX_PORT_LEN];
            memset (ucTmp,0x0,sizeof(ucTmp));
            SipPf_Sprintf(ucTmp,(SIP_CHAR *)"%u",m_usPort);

            SIP_ENC_COLON(*ppucCurrPos);

            SipPf_Strcpy(*ppucCurrPos, ucTmp);
            SipEnc_UpdateCurrPos(ppucCurrPos);
        }
    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                "EncodeSipUri: Host value Missing",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    /*encoding of URI params*/
    if (m_pobjUriParamList != SIP_NULL)
    {
        m_pobjUriParamList->EncodeUriParamList(ppucCurrPos, SIP_SEMI, m_pobjParameterComponent);
    }

    /*encoding of Hdr params*/
    // "?"   header   *( "&"   header )
    if (m_pobjUriHdrParamList != SIP_NULL)
    {
        SipVector<SipNameValue*> &sipList = m_pobjUriHdrParamList->GetList();

        if (sipList.IsEmpty() == SIP_TRUE)
        {
            //ADD LOG
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER,
                    "EncodeSipUri: Empty list",SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        /*Put a semicolon*/
        SIP_ENC_QMARK(*ppucCurrPos);

        SIP_UINT32 usSize = sipList.GetSize();

        SIP_TRACE_NORMAL(ESIPTRACE_MODENCODER, "List Size %d", usSize, SIP_ZERO);

        for (SIP_UINT32 usCount = SIP_ZERO; usCount < usSize ; usCount++)
        {
            SipNameValue* pstParamNamValue = sipList.GetAt(usCount);

            if (pstParamNamValue != SIP_NULL)
            {
                if (usCount != SIP_ZERO)
                {
                    SIP_ENC_AMPERSAND(*ppucCurrPos);
                }

                pstParamNamValue->EncodeFromUriHdrList(ppucCurrPos, m_pobjParameterComponent);
            }
        }
    }

    return SIP_TRUE;
}

/*SipAddrSpec class implementation*/
/******************************************************************************
 * Function name      : SipAddrSpec::SipAddrSpec
 *
 * Description       :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipAddrSpec::SipAddrSpec()
    : m_eUriType(SipUri::SCHEME_SIP)
    , m_pobjSipUri(SIP_NULL)
    , m_pszAbsUri(SIP_NULL)
    , m_pobjParameterComponent(SIP_NULL)
{
}


/******************************************************************************
 * Function name      : SipAddrSpec::SipAddrSpec
 *
 * Description       :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipAddrSpec::SipAddrSpec(const SipAddrSpec &objAddressSpec)
    : m_eUriType(objAddressSpec.m_eUriType)
    , m_pobjSipUri(SIP_NULL)
    , m_pszAbsUri(SipPf_Strdup(objAddressSpec.m_pszAbsUri))
    , m_pobjParameterComponent(objAddressSpec.m_pobjParameterComponent)
{
    if (objAddressSpec.m_pobjSipUri != SIP_NULL)
    {
        m_pobjSipUri = new SipUri(*(objAddressSpec.m_pobjSipUri));
    }
}

/******************************************************************************
 * Function name      : SipAddrSpec::~SipAddrSpec
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SipAddrSpec::~SipAddrSpec()
{
    if (m_pobjSipUri != SIP_NULL)
    {
        m_pobjSipUri->SipDelete();
    }
    if (m_pszAbsUri != SIP_NULL)
    {
        delete[] m_pszAbsUri;
    }
}


/******************************************************************************
 * Function name      : SipAddrSpec::SetAbsUri
 *
 * Description       :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAddrSpec::SetAbsUri(const SIP_CHAR *pszSipUri)
{
    return SetCharVar(pszSipUri, m_pszAbsUri);
}


/******************************************************************************
 * Function name      : SipAddrSpec::EncodeAddrSpec
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAddrSpec::EncodeAddrSpec
(
 SIP_CHAR     **ppucCurrPos
 )const
{
    if (m_pobjSipUri != SIP_NULL)
    {
        // implement this in cpp
        /*encoding of uri name*/
        if (m_eUriType == SipUri::SCHEME_SIP)
        {
            SipPf_Strcpy(*ppucCurrPos, SIP_SIP_ENC);
        }
        else if (m_eUriType == SipUri::SCHEME_SIPS)
        {
            SipPf_Strcpy(*ppucCurrPos, SIP_SIPS_ENC);
        }

        SipEnc_UpdateCurrPos(ppucCurrPos);

        m_pobjSipUri->SetParameterComponent(m_pobjParameterComponent);

        if (m_pobjSipUri->EncodeSipUri(ppucCurrPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Uri Encoding error",SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

    }
    else if (m_pszAbsUri != SIP_NULL)
    {
        SipPf_Strcpy(*ppucCurrPos, m_pszAbsUri);
        SipEnc_UpdateCurrPos(ppucCurrPos);

    }
    else
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No Uri set for encoding",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    return SIP_TRUE;
}

/*SipNameAddr Class implementation*/
SipNameAddr::SipNameAddr()
    : m_pszDispName(SIP_NULL)
    , m_pobjAddrSpec(SIP_NULL)
    , m_pobjParameterComponent(SIP_NULL)
{
}

SipNameAddr::SipNameAddr(const SipNameAddr &objNameAddr)
    : m_pszDispName(SipPf_Strdup(objNameAddr.m_pszDispName))
    , m_pobjAddrSpec(SIP_NULL)
    , m_pobjParameterComponent(objNameAddr.m_pobjParameterComponent)
{
    if (objNameAddr.m_pobjAddrSpec != SIP_NULL)
    {
        m_pobjAddrSpec = new SipAddrSpec(*(objNameAddr.m_pobjAddrSpec));
    }
}

SipNameAddr::~SipNameAddr()
{
    if (m_pszDispName != SIP_NULL)
    {
        delete[] m_pszDispName;
    }
    if (m_pobjAddrSpec != SIP_NULL)
    {
        m_pobjAddrSpec->SipDelete();
    }
}

SipAddrSpec* SipNameAddr::GetAddrSpec()
{
    if (m_pobjAddrSpec != SIP_NULL)
    {
        m_pobjAddrSpec->increment();
    }
    return m_pobjAddrSpec;

}
SIP_BOOL SipNameAddr::SetAddrSpec
(
 SipAddrSpec     *pobjSipAddrSpec
 )
{
    if (m_pobjAddrSpec != SIP_NULL) {
        m_pobjAddrSpec->SipDelete();
    }
    m_pobjAddrSpec = pobjSipAddrSpec;
    return SIP_TRUE;
}

/*set methods*/
SIP_BOOL SipAddrSpec::SetSipUri
(
    SipUri    *pobjSipUri,
    SIP_INT32    eUriType
)
{
    (void)eUriType;

    if (m_pobjSipUri == SIP_NULL)
    {
        m_pobjSipUri = pobjSipUri;
        m_pobjSipUri->increment();

        return SIP_TRUE;
    }
    return SIP_FALSE;
}

SIP_VOID SipAddrSpec::SetParameterComponent(IParameterComponent *pobjParameterComponent)
{
    m_pobjParameterComponent = pobjParameterComponent;
    if (m_pobjSipUri != SIP_NULL) {
        m_pobjSipUri->SetParameterComponent(pobjParameterComponent);
    }
}

SipUri* SipAddrSpec::GetSipUri()
{
    if (m_pobjSipUri != SIP_NULL)
    {
        m_pobjSipUri->increment();
    }
    return m_pobjSipUri;
}
/******************************************************************************
 * Function name      : SipNameAddr::SetName
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameAddr::SetDisplayName(const SIP_CHAR *pkszDispNm)
{
    return SetCharVar(pkszDispNm, m_pszDispName);
}

SIP_VOID SipNameAddr::SetParameterComponent(IParameterComponent *pobjParameterComponent)
{
    m_pobjParameterComponent = pobjParameterComponent;
    if (m_pobjAddrSpec != SIP_NULL) {
        m_pobjAddrSpec->SetParameterComponent(pobjParameterComponent);
    }
}


/******************************************************************************
 * Function name      : SipNameAddr::GetName
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameAddr::EncodeNameAddr
(
 SIP_CHAR     **ppucCurrPos
 )
{
    if (m_pobjAddrSpec == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "No Addr Spec",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_pszDispName != SIP_NULL)
    {
        SipPf_Strcpy(*ppucCurrPos,m_pszDispName);
        SipEnc_UpdateCurrPos(ppucCurrPos);

        // FIX_MESSAGE_ENCODING_OPERATION
        //  Add LWS between the display name and left angle quote ('<').
        //  First, check the display name if it has a double quotation.
        //  If present, just do normal procedure. Else, add the display name and space.
        //  But, we will always add the space after the display name.
        SIP_ENC_SP(*ppucCurrPos);
    }

    SIP_ENC_LAQUOT(*ppucCurrPos);

    m_pobjAddrSpec->SetParameterComponent(m_pobjParameterComponent);

    if (m_pobjAddrSpec->EncodeAddrSpec(ppucCurrPos) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODENCODER, "Addr Spec failed",SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_ENC_RAQUOT(*ppucCurrPos);

    return SIP_TRUE;
}

/******************************************************************************
 * Function name      : SipAddrSpec::DecodeAddrSpec
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipAddrSpec::DecodeAddrSpec
(
 SIP_CHAR            *pucStartPt,
 SIP_UINT32         uiDecLen
 )
{
    /*Validate the input prm*/
    if ((uiDecLen == SIP_ZERO) || (pucStartPt == SIP_NULL))
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalide Input prm", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    SIP_CHAR  *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR  *pucTempPos= SIP_NULL;

    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "No URI scheme", SIP_ZERO, SIP_ZERO);
        m_eUriType = SipUri::SCHEME_ABS;
    }
    else
    {
        m_eUriType = sipGetUriType(pucStartPt, pucTempPos);
    }

    /*CAse of Sip or Sips URI*/
    if ((m_eUriType == SipUri::SCHEME_SIP) || (m_eUriType == SipUri::SCHEME_SIPS))
    {
        /*Set the Start point after COLON*/
        pucStartPt = pucTempPos + SIP_TWO;
        /*Update the length of buffer*/
        uiDecLen = pucEndPt - pucStartPt + SIP_ONE;

        SipUri *pobjSipUri = new SipUri();
        if (pobjSipUri == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipAddrSpec::DecodeAddrSpec: Memory Allocation Failed",
                    SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        /*Set PercentEncoding Value*/
        pobjSipUri->SetParameterComponent(m_pobjParameterComponent);

        /*Decode the sip uri*/
        if (pobjSipUri->DecodeSipUri(pucStartPt,  uiDecLen) == SIP_FALSE)
        {
            pobjSipUri->SipDelete();
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER,
                    "SipAddrSpec::DecodeAddrSpec: Sip Uri Decode Failed",
                    SIP_ZERO,SIP_ZERO);

            return SIP_FALSE;
        }
        m_pobjSipUri = pobjSipUri;
    }
    else
    {
        m_pszAbsUri = sipCreateString(pucStartPt,pucEndPt);
        if (m_pszAbsUri == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Failed", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }

    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipNameAddr::DecodeNameAddr
 *
 * Description   :Function for decoding
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipNameAddr::DecodeNameAddr
(
 SIP_CHAR    *pucStartPt,
 SIP_CHAR     *pucEndPt
 )
{
    SIP_CHAR  *pucTempPre = SIP_NULL;
    SIP_CHAR  *pucTempNext = SIP_NULL;

    if (sipFindActualPos(pucStartPt, pucEndPt, &pucTempPre, &pucTempNext, LEFT_ANGLE) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Left Angle Not Found", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    /*Case of display Name present*/
    if (pucStartPt <= pucTempPre)
    {
        m_pszDispName = sipCreateString(pucStartPt, pucTempPre);
        if (m_pszDispName == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
            return SIP_FALSE;
        }
    }
    /*Now Decode the Addr Spec*/
    m_pobjAddrSpec = new SipAddrSpec();
    if (m_pobjAddrSpec == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation fail", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    m_pobjAddrSpec->SetParameterComponent(m_pobjParameterComponent);

    /*Get the length of address spec*/
    SIP_UINT32 uiDecLen = pucEndPt - pucTempNext + SIP_ONE;

    if (m_pobjAddrSpec->DecodeAddrSpec(pucTempNext, uiDecLen) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Addr Spec decoding failed", SIP_ZERO, SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}


/*****************************************************************************
 * Function name      : SipUri::DecUserInfo
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL SipUri::DecUserInfo
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt
 )
{
    /* check for userinfo = ( user / telephone-subscriber ) [ ":" password ] "@" */

    SIP_CHAR *pucTempPos= SIP_NULL;
    /* Decode password part in userinfo */
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_TRUE)
    {
        SIP_CHAR *pucPasswordStart = pucTempPos + SIP_TWO;
        SIP_CHAR *pucPassword = sipCreateString(pucPasswordStart, pucEndPt);
        if (pucPassword == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        /*Do percentage Encoding*/
        if ((m_pobjParameterComponent != SIP_NULL) &&
            (m_pobjParameterComponent->IsValidComponent(SIP_PASSWORD)))
        {
            m_pszPassword = SipPercentEncoding::DoPercentDecoding(pucPassword);
        }
        else
        {
            m_pszPassword = pucPassword;
        }

        pucEndPt = pucTempPos;
    }
    /* Decode ( user   /   telephone-subscriber ) part in userinfo */
    SIP_CHAR *pszUser = sipCreateString(pucStartPt, pucEndPt);
    if (pszUser== SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }
    /*Do percentage Encoding*/
    if ((m_pobjParameterComponent != SIP_NULL) &&
        (m_pobjParameterComponent->IsValidComponent(SIP_USER)))
    {
        m_pszUser = SipPercentEncoding::DoPercentDecoding(pszUser);
    }
    else
    {
        m_pszUser = pszUser;
    }

    return SIP_TRUE;
}

/*****************************************************************************
 * Function name      : SipUri::DecHostPort
 *
 * Description        :
 *
 * Preconditions      :
 *
 * Side Effects          : none
 *****************************************************************************/
SIP_BOOL SipUri::DecHostPort
(
 SIP_CHAR        *pucStartPt,
 SIP_CHAR        *pucEndPt
 )
{
    /*hostport = host [ ":" port ]
      host = hostname   /   IPv4address   /   IPv6reference
      hostname = *( domainlabel   "." )   toplabel   [ "." ]
      domainlabel = alphanum   /   alphanum   *( alphanum   /   "-" )   alphanum
      toplabel = ALPHA   /   ALPHA   *( alphanum   /   "-" )   alphanum
      IPv4address = 1*3DIGIT   "."   1*3DIGIT   "."   1*3DIGIT   "."   1*3DIGIT
      IPv6reference = "["   IPv6address   "]"
      IPv6address = hexpart   [ ":"   IPv4address ]
      hexpart = hexseq   /   hexseq   "::"   [ hexseq ]   /   "::"   [ hexseq ]
      hexseq = hex4   *( ":"   hex4 )
      hex4 = 1*4HEXDIG
      port = 1*DIGIT */

    SIP_CHAR *pucTempPos = SIP_NULL;

    /* IPV6 is enclosed in between '[' and ']', get start and end point of Ipv6 address*/
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, LEFT_SQUARE) == SIP_TRUE)
    {
        m_eHostType = SipAddrSpec::HOST_IPV6;
        pucStartPt = pucTempPos + SIP_TWO;
        pucTempPos = SIP_NULL;
        if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, RIGHT_SQUARE) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Invalid Host[IPV6]", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
    }
    else if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_FALSE)
    {
        pucTempPos = pucEndPt;
    }
    /* Host : Hostname or IPv4 or IPv6 */
    m_pszHost = sipCreateString(pucStartPt, pucTempPos);
    if (m_pszHost == SIP_NULL)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    if (m_eHostType != SipAddrSpec::HOST_IPV6)
    {
        IPAddress pbjIpAddr;
        if (pbjIpAddr.Parse(AString(m_pszHost)) == SIP_TRUE)
        {
            m_eHostType = SipAddrSpec::HOST_IPV4;
        }
    }

    if ((m_pobjParameterComponent != SIP_NULL) &&
        (m_pobjParameterComponent->IsValidComponent(SIP_HOST)) &&
        (m_eHostType == SipAddrSpec::HOST_NAME))
    {
        m_pszHost =  SipPercentEncoding::DoPercentDecoding(m_pszHost);
    }

    pucStartPt = pucTempPos;
    /* Port number */
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, COLON) == SIP_TRUE)
    {
        pucTempPos = pucTempPos + SIP_TWO;
        SIP_CHAR *pszPort = sipCreateString(pucTempPos, pucEndPt);
        if (pszPort == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        SIP_CHAR  *pszPortTemp = SIP_NULL;

        if ((m_pobjParameterComponent != SIP_NULL) &&
            (m_pobjParameterComponent->IsValidComponent(SIP_PORT)))
        {
            pszPortTemp =  SipPercentEncoding::DoPercentDecoding(pszPort);
        }
        else
        {
            pszPortTemp = pszPort;
        }
        m_usPort = SipPf_Atoi(pszPortTemp);
        delete[] pszPortTemp;
    }
    else
    {
        m_usPort = SIP_UNSPECIFIED_PORT;
    }
    return SIP_TRUE;
}


/******************************************************************************
 * Function name      : SipUri::DecodeSipUri
 *
 * Description     :
 *
 * Preconditions      :
 *
 * Side Effects      : none
 *****************************************************************************/
SIP_BOOL SipUri::DecodeSipUri
(
 SIP_CHAR    *pucStartPt,
 SIP_UINT32  uiDecLen
 )
{
    /* SIP URI : sip(s):user:password@host:port;uri-parameters?headers
     * uri-parameters (parameter-name "=" parameter-value pairs) are separated by semi-colons and
     * headers (hname = hvalue pairs) are separated by Ampersand
     */
    SIP_CHAR  *pucEndPt = pucStartPt + uiDecLen - SIP_ONE;
    SIP_CHAR  *pucTempPos= SIP_NULL;

    /* Decode headers part in SIP URI */
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, QMARK) == SIP_TRUE)
    {
        SIP_CHAR *pucHeaderStart = pucTempPos + SIP_TWO;

        m_pobjUriHdrParamList = new SipParameterList();
        if (m_pobjUriHdrParamList == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        if (m_pobjUriHdrParamList->DecUriHdrSipParameterList(pucHeaderStart, pucEndPt,
            AMPERSEND, m_pobjParameterComponent) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Hdr prm Decode Failed", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        pucEndPt = pucTempPos;
        pucTempPos = SIP_NULL;
    }
    /* Decode uri-parameters part in SIP URI */
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, SIP_SEMI) == SIP_TRUE)
    {
        SIP_CHAR *pucUriParamStart = pucTempPos + SIP_TWO;

        m_pobjUriParamList = new SipParameterList();
        if (m_pobjUriParamList == SIP_NULL)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Memory Allocation Fail", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        if (m_pobjUriParamList->DecUriSipParameterList(pucUriParamStart, pucEndPt,
            SIP_SEMI, m_pobjParameterComponent) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Uri Prm Decode Failed", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }

        pucEndPt = pucTempPos;
        pucTempPos = SIP_NULL;
    }
    /* Decode user:password part in SIP URI */
    if (sipFindPreDelimiter(pucStartPt, pucEndPt, &pucTempPos, ATRATE) == SIP_TRUE)
    {
        if (DecUserInfo(pucStartPt, pucTempPos) == SIP_FALSE)
        {
            SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "User Info Decode Failed", SIP_ZERO,SIP_ZERO);
            return SIP_FALSE;
        }
        pucStartPt = pucTempPos + SIP_TWO;
    }
    /* Decode host:port part in SIP URI */
    if (DecHostPort(pucStartPt, pucEndPt) == SIP_FALSE)
    {
        SIP_DEBUG_WARNING(ESIPTRACE_MODDECODER, "Host port Decode Failed", SIP_ZERO,SIP_ZERO);
        return SIP_FALSE;
    }

    return SIP_TRUE;
}
