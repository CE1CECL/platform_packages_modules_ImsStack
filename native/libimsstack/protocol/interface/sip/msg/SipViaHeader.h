/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipViaHeader.h
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

#ifndef __SIP_VIA_HEADER_H__
#define __SIP_VIA_HEADER_H__

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/
#include "msg/SipHeaderBase.h"


/****************************************************************************
  Macro Definitions
 *****************************************************************************/


/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/

class SipViaHeader : public SipHeaderBase
{
    private:
        /*Sent Protocol*/
        /*Protocol Name*/
        SIP_CHAR    *m_pszProtocolName;
        /*Protocol Version*/
        SIP_CHAR    *m_pszProtocolVer;
        /*Transport*/
        SIP_CHAR    *m_pszTransport;

        /*Sent By*/
        /*Host*/
        SIP_CHAR    *m_pszHost;
        /*Port*/
        SIP_UINT16        m_usPort;

        SIP_INT32     m_eHostType;

        SIP_BOOL DecHostPort
            (
             SIP_CHAR        *pucStartPt,
             SIP_CHAR        *pucEndPt
            );

    public:
        /*constructor*/
        SipViaHeader();
        SipViaHeader(const SipViaHeader &objHeader);
        /*destructor*/
        ~SipViaHeader();

        static    SipHeaderBase*  GetNewObj(SIP_INT32 , SipHeaderBase *);

        SIP_BOOL EncodeHdr
            (
             SIP_CHAR     **ppucCurrPos,
             SIP_BOOL     bParams = SIP_TRUE
            );

        SIP_BOOL DecodeHdr
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32  uiDecLen
            );
        /*set methods*/
        SIP_BOOL SetProtocolName
            (
             const SIP_CHAR    *pkszProtocolNm
            );

        SIP_BOOL SetProtocolVer
            (
             const SIP_CHAR    *pkszProtocolVer
            );

        SIP_BOOL SetTransport
            (
             const SIP_CHAR    *pkszTransport
            );

        SIP_BOOL SetHost
            (
             const SIP_CHAR    *pkszHost
            );
        SIP_BOOL SetPortNum
            (
             SIP_UINT16        usPort
            );

        /*Get methods*/

        inline const SIP_CHAR* GetProtocolName() const
        {
            return m_pszProtocolName;
        }

        inline const SIP_CHAR* GetProtocolVer() const
        {
            return m_pszProtocolVer;
        }

        inline const SIP_CHAR* GetTransport() const
        {
            return m_pszTransport;
        }

        inline const SIP_CHAR* GetHost() const
        {
            return m_pszHost;
        }

        inline SIP_UINT16 GetPort() const
        {
            return m_usPort;
        }

        /* ### TODO */
        const SIP_CHAR *GetBranch() const;

        SIP_BOOL SetRecvdParam
            (
             const SIP_CHAR    *pkszRecvd
            );

        SIP_BOOL SetBranchParam
            (
             const SIP_CHAR    *pkszBranch
            );
        SIP_BOOL IsValidHeader() const;
};

#endif //__SIP_VIA_HEADER_H__
