/******************************************************************************
 * Project Name   : SIP_RTP
 * Group    : IP-CS [MSG-2]
 * Security   : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename      : SipAuthInfoHeader.h
 * Purpose     :
 * Platform      : Windows OR Android
 * Author(s)     : Saurabh Srivastava
 * E-mail id.    : saurabh31.srivastava@
 * Creation date   : May. 10, 2011
 *
 * Edit History   Modification description(s)
 * Date      Name    Version    Bug-ID    Description
 * ----------    ----------    -------    ------    -------------
 * Month. Date,10    Name       0.0a    Initial creation
 *****************************************************************************/

#ifndef __SIP_AUTH_INFO_HEADER_H__
#define __SIP_AUTH_INFO_HEADER_H__

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

class SipAuthInfoHeader : public SipHeaderBase
{
    private:
        /*Media Disp*/
        SipNameValue*    m_pobjAiInfo;
    public:
        /*constructor*/
        SipAuthInfoHeader();
        SipAuthInfoHeader(const SipAuthInfoHeader &objHeader);

        /*destructor*/
        ~SipAuthInfoHeader();
        static  SipHeaderBase*  GetNewObj(SIP_INT32 , SipHeaderBase *);
        /*virtual methods*/
        /*Function for encoding of headers*/
        SIP_BOOL EncodeHdr
            (
             SIP_CHAR   **ppucCurrPos,
             SIP_BOOL   bParams = SIP_TRUE
            );

        /*Function for decoding of headers*/
        SIP_BOOL DecodeHdr
            (
             SIP_CHAR  *pucStartPt,
             SIP_UINT32  uiDecLen
            );

        const SIP_CHAR* GetAiInfoVal
            (
             SIP_UINT32   usPos = SIP_ZERO
            );
        inline SIP_BOOL IsValidHeader() const
        {
            return (m_pobjAiInfo != SIP_NULL) ? SIP_TRUE : SIP_FALSE;
        }
};
#endif //__SIP_AUTH_INFO_HEADER_H__
