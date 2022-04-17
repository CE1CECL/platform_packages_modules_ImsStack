/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipRAcKHeader.h
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

#ifndef __SIP_RACK_HEADER_H__
#define __SIP_RACK_HEADER_H__

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

class SipRAcKHeader :public SipHeaderBase
{
    private:

        /*Response Num*/
        SIP_UINT32        m_uiResponseNum;

        /*CSeq Num*/
        SIP_UINT32        m_uiCSeqNum;

        /*Method*/
        SIP_CHAR    *m_pszMethod;


    public:
        /*constructor*/
        SipRAcKHeader();
        SipRAcKHeader(const SipRAcKHeader &objHeader);
        /*destructor*/
        ~SipRAcKHeader();
        static SipHeaderBase* GetNewObj(SIP_INT32,SipHeaderBase*);

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
        SIP_BOOL SetMethod
            (
             const SIP_CHAR    *pkszMethod
            );
        inline SIP_VOID SetResponseNum(SIP_UINT32 uiResponseNum)
        {
            m_uiResponseNum = uiResponseNum;
        }

        inline SIP_VOID SetCSeqNum(SIP_UINT32 uiCSeqNum)
        {
            m_uiCSeqNum = uiCSeqNum;
        }
        /*Get methods*/

        inline const SIP_CHAR* GetMethod() const
        {
            return m_pszMethod;
        }

        inline SIP_UINT32 GetResponseNum() const
        {
            return m_uiResponseNum;
        }

        inline SIP_UINT32 GetCSeqNum() const
        {
            return m_uiCSeqNum;
        }
        inline SIP_BOOL IsValidHeader() const
        {
            return (m_pszMethod == SIP_NULL) ? SIP_FALSE : SIP_TRUE;
        }

};

#endif //__SIP_RACK_HEADER_H__
