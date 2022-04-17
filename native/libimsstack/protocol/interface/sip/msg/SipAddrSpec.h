/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipAddrSpec.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : Month. Date,10
 *
 * Edit History         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/
#ifndef __SIP_ADDR_SPEC_H__
#define __SIP_ADDR_SPEC_H__

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/

#include "sip_pf_datatypes.h"
#include "SipRefBase.h"
#include "msg/SipParameterList.h"
#include "msg/IParameterComponent.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/


/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/
class SipUri: public SipRefBase
{
    public:
        /*Enumeration for URI Scheme*/
        enum
        {
            SCHEME_INVALID = SIP_INVALID,
            SCHEME_SIP,
            SCHEME_SIPS,
            SCHEME_ABS
        };

    private:
        /*User Info contains username and pwd*/
        SIP_CHAR    *m_pszUser;
        SIP_CHAR    *m_pszPassword;
        /*Host Port contains host(can be domain name or IP) and port*/
        SIP_CHAR    *m_pszHost;
        SIP_UINT16    m_usPort;
        SIP_INT32     m_eHostType;
        SipParameterList        *m_pobjUriParamList;
        // for storing each header in
        // "?"   header   *( "&"   header )
        //each node consists of a SipNameValue obj for one header
        SipParameterList        *m_pobjUriHdrParamList;
        //For Percent Encoding and Decoding
        IParameterComponent *m_pobjParameterComponent;

        SIP_BOOL DecUserInfo
            (
             SIP_CHAR        *pucStartPt,
             SIP_CHAR        *pucEndPt
            );

        SIP_BOOL DecHostPort
            (
             SIP_CHAR        *pucStartPt,
             SIP_CHAR        *pucEndPt
            );

    public:
        /*constructor*/
        SipUri();
        SipUri(const SipUri &objSipUri);
        /*destructor*/
        ~SipUri();

        SIP_BOOL DecodeSipUri
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32  uiDecLen
            );

        /*Set Methods*/
        SIP_BOOL SetUser
            (
             const SIP_CHAR    *pkszUser
            );

        SIP_BOOL SetPassword
            (
             const SIP_CHAR    *pkszPass
            );

        SIP_BOOL SetHost
            (
             const SIP_CHAR    *pkszHost
            );

        inline SIP_VOID SetPortNum(SIP_UINT16 usPort)
        {
            m_usPort = usPort;
        }
        /*Get methods*/
        inline const SIP_CHAR* GetUser() const
        {
            return m_pszUser;
        }

        inline const SIP_CHAR* GetPassword() const
        {
            return m_pszPassword;
        }

        inline const SIP_CHAR* GetHost() const
        {
            return m_pszHost;
        }

        inline SIP_UINT16 GetPort() const
        {
            return m_usPort;
        }

        SipParameterList* GetUriParamList();

        inline SIP_UINT32 GetUriParamCount() const
        {
            return (m_pobjUriParamList != SIP_NULL) ? m_pobjUriParamList->GetCount() : SIP_ZERO;
        }
        SipParameterList* GetHdrParamList();

        inline SIP_UINT32 GetHdrParamCount() const
        {
            return (m_pobjUriHdrParamList != SIP_NULL) ?
                    m_pobjUriHdrParamList->GetCount() : SIP_ZERO;
        }

        /*Add only name*/
        SIP_BOOL AddUriParam
            (
             const SIP_CHAR     *pszName
            );

        SIP_BOOL AddUriParam
            (
             const SIP_CHAR     *pszName,
             const SIP_CHAR     *pszValue
            );

        //This api may not be required .. re-check
        SIP_BOOL AddUriParam
            (
             const SIP_CHAR     *pszName,
             const SIP_CHAR     *pszValue,
             SIP_INT32    ePrmType
            );
        SIP_BOOL AddHeader
            (
             const SIP_CHAR     *pszName
            );
        SIP_BOOL AddHeader
            (
             const SIP_CHAR     *pszName,
             const SIP_CHAR     *pszValue
            );

        SIP_BOOL EncodeSipUri
            (
             SIP_CHAR     **ppucCurrPos
            );
        SIP_BOOL DecodeSipUri
            (
             SIP_CHAR     **ppucCurrPos
            );

        SIP_BOOL RemoveUriParam
            (
             const SIP_CHAR     *pszName
            );

        SIP_BOOL RemoveHdrParam
            (
             const SIP_CHAR     *pszName
            );
        SIP_VOID SetParameterComponent(IParameterComponent *pobjParameterComponent);
};


class SipAddrSpec: public SipRefBase
{
    public:
        /*Enumeration for host type*/
        enum
        {
            HOST_NAME,
            HOST_IPV4,
            HOST_IPV6,
            HOST_INVALID = SIP_INVALID
        };

    private:
        SIP_INT32    m_eUriType;

    protected:
        SipUri            *m_pobjSipUri;
        SIP_CHAR            *m_pszAbsUri;

        IParameterComponent *m_pobjParameterComponent;

    public:
        SipAddrSpec();

        SipAddrSpec(const SipAddrSpec &objAddressSpec);
        ~SipAddrSpec();
        /*Function for encoding*/
        SIP_BOOL EncodeAddrSpec
            (
             SIP_CHAR     **ppucCurrPos
            )const;

        /*Function for decoding*/
        SIP_BOOL DecodeAddrSpec
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32  uiDecLen
            );
        /*function for getting the header type*/
        inline SIP_INT32 GetUriScheme() const
        {
            return m_eUriType;
        }

        /*set methods*/
        SIP_BOOL SetSipUri
            (
             SipUri    *pobjSipUri,
             SIP_INT32    eUriType
            );

        SipUri* GetSipUri();

        SIP_BOOL SetAbsUri(const SIP_CHAR *pszSipUri);

        inline const SIP_CHAR* GetAbsUri() const
        {
            return m_pszAbsUri;
        }

        SIP_VOID SetParameterComponent(IParameterComponent *pobjParameterComponent);
        /*get methods*/

        /* Get methods as reference */
        inline const SipUri* GetSipUriAsRef() const
        { return m_pobjSipUri; }
};

class SipNameAddr: public SipRefBase
{
    public:
        SIP_CHAR *m_pszDispName;
        SipAddrSpec* m_pobjAddrSpec;
        IParameterComponent *m_pobjParameterComponent;


    public:
        SipNameAddr();
        SipNameAddr(const SipNameAddr &objNameAddr);
        virtual ~SipNameAddr();

        SIP_BOOL SetAddrSpec
            (
             SipAddrSpec     *pobjSipAddrSpec
            );
        SIP_BOOL EncodeNameAddr
            (
             SIP_CHAR     **ppucCurrPos
            );

        /*Function for decoding*/
        SIP_BOOL DecodeNameAddr
            (
             SIP_CHAR    *pucStartPt,
             SIP_CHAR     *pucEndPt
            );

        SipAddrSpec* GetAddrSpec();

        /*set methods*/
        SIP_BOOL SetDisplayName
            (
             const SIP_CHAR    *pkszDispNm
            );

        SIP_VOID SetParameterComponent(IParameterComponent *pobjParameterComponent);
        /*Get Display Name*/
        inline const SIP_CHAR* GetDisplayName() const
        {
            return m_pszDispName;
        }

};

/****************************************************************************
  Class Declaration Ends
 *****************************************************************************/

#endif //__SIP_ADDR_SPEC_H__
