/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipWarningHeader.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           :
 * E-mail id.            : giridhar.a@
 * Creation date       : July. 27,2010
 *
 * Edit HisAlertry         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Giridhar               0.0a            Initial creation
 *****************************************************************************/

#ifndef __SIP_WARNING_HEADER_H__
#define __SIP_WARNING_HEADER_H__

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

class SipWarningHeader : public SipHeaderBase
{
    private:
        /*warn-code*/
        SIP_UINT32        m_uiWarnCode;

        /*Protocol Version*/
        SIP_CHAR        *m_pszWarnAgent;

        /*Transport*/
        SIP_CHAR        *m_pszWarnText;

    public:
        /*constructor*/
        SipWarningHeader();

        SipWarningHeader(const SipWarningHeader &objHeader);

        /*destructor*/
        ~SipWarningHeader();

        static    SipHeaderBase*  GetNewObj(SIP_INT32 , SipHeaderBase *);
        /*virtual methods*/
        /*Function for encoding of headers*/
        SIP_BOOL EncodeHdr
            (
             SIP_CHAR     **ppucCurrPos,
             SIP_BOOL     bParams = SIP_TRUE
            );

        /*Function for decoding of headers*/
        SIP_BOOL DecodeHdr
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32  uiDecLen
            );
        /*Sets the visited nw name*/
        inline SIP_VOID SetWarnCode(SIP_UINT32 uiWarnCode)
        {
            m_uiWarnCode = uiWarnCode;
        }

        /*Gets the visited nw name*/
        inline SIP_UINT32 GetWarnCode() const
        {
            return m_uiWarnCode;
        }

        /*Sets the visited nw name*/
        SIP_BOOL SetWarnAgent
            (
             const SIP_CHAR    *pucWarnAgent
            );

        /*Gets the visited nw name*/
        inline SIP_CHAR* GetWarnAgent() const
        {
            return m_pszWarnAgent;
        }

        /*Sets the visited nw name*/
        SIP_BOOL SetWarnText
            (
             const SIP_CHAR    *pucWarnText
            );

        /*Gets the visited nw name*/
        inline const SIP_CHAR* GetWarnText() const
        {
            return m_pszWarnText;
        }

        SIP_BOOL IsValidHeader() const;
};
#endif //__SIP_WARNING_HEADER_H__
