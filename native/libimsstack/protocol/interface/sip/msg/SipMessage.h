/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipMessage.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)           : Saurabh Srivastava
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : Month. Date,10
 *
 * Edit History         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/

#ifndef __SIP_MESSAGE_H__
#define __SIP_MESSAGE_H__

#include "msg/SipHeaders.h"
#include "msg/SipMsgBody.h"
#include "msg/SipRequestLine.h"
#include "SipConfiguration.h"
#ifdef SIP_BADMESSAGE_PARSING
#include "msg/SipBadHeader.h"
#endif

/**
  This class represents one SIP message. The stack user can create a suitable message by setting
  the headers in SipMessage. Each header that has to be added should be set using
  SipMessage::SetHdr(). To send a request, the request line along with the mandatory headers must
  be set. SipStackManager::SendMsg() should be used send the message

  To send a response, the request line along with the mandatory headers must be set.
  SipStackManager::SendMsg() should be used send the message

  The object can be either of request type or response type.

  After setting any of the headers, the header being set can be freed using SipDelete(). The stack
  implements reference counting. When ever, the stack stores any header object it will increment
  the reference count.

 */

class SipMessage: public SipRefBase
{
    public:
        enum
        {
            METHOD_INVITE = 0,
            METHOD_ACK,
            METHOD_OPTIONS,
            METHOD_BYE,
            METHOD_CANCEL,
            METHOD_REGISTER,
            METHOD_INFO,
            METHOD_PRACK,
            METHOD_SUBSCRIBE,
            METHOD_NOTIFY,
            METHOD_UPDATE,
            METHOD_MESSAGE,
            METHOD_REFER,
            METHOD_PUBLISH,
            METHOD_END,
            METHOD_INVALID
        };

        enum
        {
            MSG_INCOMPLETE = 0,
            MSG_COMPLETE,
            MSG_WRONG
        };

        enum
        {
            REQ_TYPE = 0,
            RESP_TYPE,
            TYPE_INVALID
        };

        enum
        {
            MANDATORY_HDR_NONE = 0x0000,
            MANDATORY_HDR_FROM = 0x0001,
            MANDATORY_HDR_TO = 0x0002,
            MANDATORY_HDR_CALL_ID = 0x0004,
            MANDATORY_HDR_CSEQ = 0x0008,
            MANDATORY_HDR_VIA = 0x0010,
            MANDATORY_HDR_MASK = 0x001F
        };

    private:
        /* eSipMsgType will tell the type of the message    */
        SIP_INT32        m_eSipMsgType;

        /*Request Line*/
        SipRequestLine        *m_pObjReqLine;

        /*Status Line*/
        SipStatusLine        *m_pObjStatusLine;

        /*common(Request & Response Both)headers*/
        SipHeaders            *m_objHdrs;

        /* List of SipMsgBody*/
        SipMsgBodyList        *m_pObjMsgBodyList;

#ifdef SIP_BADMESSAGE_PARSING
        SIP_INT32   mbitMask;
        SipHeaderList             *m_pobjBadHdrList;
#endif
        SIP_BOOL UpdateMessage(SIP_UINT32 uiMsgOptions);

    public:
        SipMessage();
        ~SipMessage();
        SipMessage(SIP_INT32    eSipMsgType);
        SipMessage(const SipMessage &objSipMsg);
        SIP_BOOL EncodeMsg
            (
             SIP_CHAR        **ppucSipMsgBuffer, /* in-out parameter*/
             SIP_UINT32        *puiSipMsgLength, /* in-out parameter*/
             SIP_UINT32         uiMsgOptions
            );

        SIP_BOOL DecCompleteMsg
            (
             SIP_CHAR            * pucMsgBuff,
             SIP_UINT32            uiMsgBuffLen
            );

        SIP_BOOL DecodeMsg
            (
             SIP_CHAR * pucStartPt,
             SIP_UINT32     uiMsgBuffLen
            );

        SIP_BOOL DecMultiPartBody
            (
                SIP_CHAR            * pucBuffStart,
                SIP_CHAR            * pucBuffEnd,
                SIP_UINT32            uiMsgBuffLen
            );

        SipMsgBodyList *GetMsgBodyList(){
            if (m_pObjMsgBodyList)
            {
                m_pObjMsgBodyList->increment();
            }
            return m_pObjMsgBodyList;
        }
        SipMsgBody* GetMessageBody(SIP_INT32 nIndex);

        SIP_BOOL SetRequestline(SipRequestLine* pobjReqLine);
        SIP_BOOL SetHeader
            (
             SipHeaderBase* pobjHdr
            );
        SIP_BOOL AppendHeader
           (
            SipHeaderBase* pobjHdr
           );
        SIP_BOOL InsertHeader
           (
            SipHeaderBase* pobjHdr,
            SIP_UINT32 nIndex
           );
        SIP_BOOL SetMessageBody(SipMsgBody* pObj);
        SIP_BOOL SetMessageType(SIP_INT32 eMsgType);
        /* ### TODO */
        inline SipHeaders* GetMsgHdrs() {
            return (m_objHdrs);
        }
        inline SIP_INT32 GetMsgType() const {
            return m_eSipMsgType;
        }
        inline SipRequestLine* GetReqLine() {
            if (m_pObjReqLine != SIP_NULL) {
                m_pObjReqLine->increment();
            }
            return m_pObjReqLine;
        }
        inline SipStatusLine* GetStatusLine() {
            if (m_pObjStatusLine != SIP_NULL) {
                m_pObjStatusLine->increment();
            }
            return m_pObjStatusLine;
        }
        SIP_INT32 GetMethodType();
        const SIP_CHAR* GetMethod(); /* Based on Req and Resp*/
        SipHeaderBase* GetHdrObj(SIP_INT32 eHdrType);
        SipUnknownHeader* GetUnknownHdrObj(const SIP_CHAR* strHdrName);
        SipUnknownHeader* GetUnknownHdrObj(SIP_INT32 eType);
        SIP_BOOL HasHeader(SIP_INT32 eHdrType) const;
        SipHeaderList* GetHdrList(SIP_INT32 eHdrType);
        SIP_VOID SetContentLengthHdr(SIP_UINT16 usLen, SIP_UINT32 uiMsgOptions);
        SIP_BOOL SetHdrList(SipHeaderList* pstObjHdrList);
        SIP_BOOL SetMsgBodyList(SipMsgBodyList *pobjMsgBodyList);
        inline SIP_UINT16 GetMsgBodyCount() const
        {
            return (m_pObjMsgBodyList != SIP_NULL) ?
                    m_pObjMsgBodyList->GetMsgBodyCount() : SIP_ZERO;
        }
        SIP_BOOL AppendMessageBody(SipMsgBody *pObjMsgBody);
        SipMsgBody* GetMsgBody(SIP_UINT32 uiPos);
        SIP_BOOL RemoveHdr(SIP_INT32 eHdrType);
        SIP_BOOL SetStatusLine(SipStatusLine *pobjStatusLine);
        SIP_BOOL IsReqLineExists();
        SIP_BOOL IsStatusLineExists();
        SIP_BOOL HasMIMEMessageBody();
        SIP_BOOL HasSDPMessageBody();
#ifdef SIP_BADMESSAGE_PARSING
        SIP_BOOL HasMandatoryHdrs();
        SIP_INT32 GetBadHeaderCount() const;
        SipHeaderList* GetBadHdrs();
        SIP_VOID DeleteBadHdrList();
#endif

        void AdjustContentLengthHdr();

        // Utility methods
        SIP_UINT16 GetStatusCode() const;

        SipHeaderBase* GetHeader(SIP_INT32 nType, SIP_UINT32 nIndex);
        SIP_INT32 GetHeaderCount(SIP_INT32 nType);
};
#endif //__SIP_MESSAGE_H__
