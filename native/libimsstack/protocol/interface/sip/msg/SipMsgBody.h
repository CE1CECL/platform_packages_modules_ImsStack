/******************************************************************************
 * Project Name     : SIP_RTP
 * Group            : IP-CS [MSG-2]
 * Security         : Confidential
 *****************************************************************************/

/******************************************************************************

 * Filename              : SipMsgBody.h
 * Purpose               :
 * Platform              : Windows OR Android
 * Author(s)               : Saurabh Srivastava
 * E-mail id.            : saurabh31.srivastava@
 * Creation date       : July. 26, 2010
 *
 * Edit History         Modification description(s)
 * Date                Name            Version        Bug-ID        Description
 * ----------        ----------        -------        ------        -------------
 * Month. Date,10        Name                 0.0a            Initial creation
 *****************************************************************************/
#ifndef __SIP_MSG_BODY_H__
#define __SIP_MSG_BODY_H__

/*****************************************************************************
  Header Inclusions
 *****************************************************************************/

#include"sip_pf_datatypes.h"
#include "SipRefBase.h"
#include "msg/SipContentTypeHeader.h"
#include "msg/SipHeaderList.h"

/****************************************************************************
  Macro Definitions
 *****************************************************************************/

/****************************************************************************
  Enum Declaration
 *****************************************************************************/

/****************************************************************************
  Class Declaration Starts
 *****************************************************************************/

class SipMsgBody;
class SipMessageSummary;

class SipMIMEHdrs : public SipRefBase
{
    public:
        enum
        {
            CONTENT_TYPE,
            CONTENT_ENCODING,
            CONTENT_DISPOSITION,
            UNKNOWN,
            END,
            INVALID
        };

    private:
        SipContentTypeHeader *m_pobjContentType;
        SipHeaderBase *m_pobjContentEncoding;
        SipHeaderBase *m_pobjContentDisposition;
        SipHeaderList *m_pobjUnKnownHdrList;
    public:
        SipMIMEHdrs();
        SipMIMEHdrs(const SipMIMEHdrs &objMimeHdr);
        virtual ~SipMIMEHdrs();
        SIP_BOOL EncodeMIMEHdrs
            (
             SIP_CHAR     **ppucCurrPos
            );

        SIP_BOOL DecodeMIMEHdrs
            (
             SIP_CHAR    *pucStartPt,
             SIP_UINT32     uiDecLen
            );

        SipHeaderBase* getNewMIMEHdrObj
            (
             SIP_INT32    eHdrType
            );
        SipHeaderBase* GetContentType();
        SipHeaderBase* GetContentEncoding();
        SipHeaderBase* GetContentDisposition();

        SipHeaderBase* GetUnknownHdr(SIP_UINT32 uiIndex);
        inline SIP_UINT32 GetUnknownHdrCount() const
        {
            return (m_pobjUnKnownHdrList != SIP_NULL) ?
                    m_pobjUnKnownHdrList->GetSize() : SIP_ZERO;
        }

        SIP_BOOL SetMimeHdrs
            (
             SipHeaderBase        *pobjHdr
            );

        SipHeaderBase* getMimeHdrObj(SIP_INT32 eIndex);
};




/*Class for message body list*/
class SipMsgBodyList : public SipRefBase
{

    SipVector<SipMsgBody*> m_objBodyList; //SipMsgBody

    public:
    SipMsgBodyList();
    SipMsgBodyList(const SipMsgBodyList &objMsgBodyList);
    ~SipMsgBodyList();
    SipMsgBody* GetBodyByIndex(SIP_UINT32 index);

    SIP_BOOL AddBody(SipMsgBody* pobjMsgBody);

    inline SIP_UINT32 GetMsgBodyCount() const
    {
        return m_objBodyList.GetSize();
    }

    SIP_UINT32 GetTotalBodyLen(SIP_CHAR* pszBoundary = SIP_NULL);

    SIP_BOOL EncodeBody
        (
         SIP_CHAR     **ppucMsgBuffCurrPos,
         SIP_CHAR     *pszBoundary
        );

    SIP_BOOL DecodeMIMEBody
        (
         SIP_CHAR            *pucStartPt,
         SIP_CHAR             *pucEndPt,
         SIP_CHAR            *pszBoundary
        );
    /*Function for decoding of headers*/
    SIP_BOOL DecodeSingleBody
        (
         SIP_CHAR            *pucStartPt,
         SIP_CHAR             *pucEndPt
        );

    SIP_BOOL DecodeMessageSummaryBody
        (
         SIP_CHAR            *pucStartPt,
         SIP_CHAR             *pucEndPt
        );
};

class SipMsgBody: public SipRefBase
{
    public:
        enum
        {
            SINGLE_BODY,
            MULTI_PART_MIXED_BODY,
            MULTI_PART_ALT_BODY,
            UNKNOWN_BODY,
            MESSAGE_SUMMARY_BODY,
            INVALID_BODY
        };

    private:
        /*Body type*/
        SIP_INT32            m_eBodyType;
        /*Set of Mime Headers*/
        SipMIMEHdrs    *m_pobjMIMEHdrs;
        /* Set for Message Summary*/
        SipMessageSummary *m_pobjMessageSummary;
        /* Msg body buffer*/
        SIP_CHAR            *m_pucBuffer;
        /* Content Buffer Length*/
        SIP_UINT32            m_uiBufLen;
        /*List of self referential objects*/
        SipMsgBodyList        *m_pobjBodyList;// list of SipMsgBody
        /*Flag to disable encoding of MIME header*/
        SIP_BOOL        m_bEncodeMime;

    public:

        /*Constructor*/
        SipMsgBody();
        SipMsgBody(SIP_INT32 eBodyType);

        SipMsgBody(const SipMsgBody &objMsgBody);
        /*Destructor*/
        ~SipMsgBody();

        /*Function for encoding*/

        SIP_BOOL EncodeMIMEHdrs
            (
             SIP_CHAR     **ppucCurrPos
            );

        SIP_BOOL EncodeSingleMsgBody  //RTBD remove this fxn
            (
             SIP_CHAR     **ppucCurrPos
            );

        SIP_BOOL EncodeMIMEMsgBody
            (
             SIP_CHAR     **ppucCurrPos
            );
        SIP_BOOL EncodeMessageSummaryMsgBody
            (
             SIP_CHAR     **ppucCurrPos
            );


        /*Function for decoding*/
        SIP_BOOL DecodeSingleMsgBody     //RTBD remove this fxn
            (
             SIP_CHAR    *pucStartPt,
             SIP_CHAR     *pucEndPt
            );

        SIP_BOOL DecodeMIMEMsgBody     //RTBD remove this fxn
            (
             SIP_CHAR    *pucStartPt,
             SIP_CHAR     *pucEndPt
            );
        SIP_BOOL DecodeMessageSummaryMsgBody
            (
             SIP_CHAR    *pucStartPt,
             SIP_CHAR     *pucEndPt
            );


        inline SIP_BOOL IsMimeEncoding() const
        {
            return m_bEncodeMime;
        }
        /*Set Methods*/
        SIP_BOOL SetMimeHdr
            (
             SipHeaderBase     *pobjHdrBase
            );


        SIP_BOOL SetMsgBuffer
            (
             const SIP_CHAR    *pkszMsgBuffer,
             SIP_UINT32        uiBufLen
            );


        /*Get Methods*/

        inline SIP_INT32 GetBodyType() const
        {
            return m_eBodyType;
        }

        SipContentTypeHeader* GetContentType();

        SipHeaderBase* GetContentEncoding();

        SipHeaderBase* GetContentDisposition();

        SIP_UINT32 GetUnknownHdrCount() const
        {
            return (m_pobjMIMEHdrs != SIP_NULL) ?
                    m_pobjMIMEHdrs->GetUnknownHdrCount() : SIP_ZERO;
        }

        SIP_BOOL IsMessageBodySDP();

        SIP_BOOL GetMsgBuffer
            (
             SIP_CHAR    **ppszMsgBuffer
            );

        inline SIP_VOID GetMsgBuffLen(SIP_UINT32 *puiMsgBufLen) const
        {
            *puiMsgBufLen = m_uiBufLen;
        }

        SipHeaderBase* GetMimeHdr
            (
             SIP_INT32     eHdrType ,
             SIP_UINT32     uiIndex
            );

        inline SIP_CHAR* GetBuffer() const
        { return m_pucBuffer; }
        inline SIP_UINT32 GetBufferLength() const
        { return m_uiBufLen; }
};

class SipSummaryLine;
class SipMessageSummary: public SipRefBase
{
    sipEn_StatusType     m_dStatus;
    SipAddrSpec     *m_pobjAddrSpec;
    SipVector<SipSummaryLine*>   m_objSummaryLineList;    /* List of SummaryLine Class*/
    SipVector<SipNameValue*>  m_objNameValueList; //List of Name Values

    public:
    SipMessageSummary();
    SipMessageSummary(const SipMessageSummary &objMessageSummary);
    virtual ~SipMessageSummary();

    SIP_BOOL EncodeMessageSummary
        (
         SIP_CHAR     **ppucCurrPos
        );

    SIP_BOOL DecodeMessageSummary
        (
         SIP_CHAR    *pucStartPt,
         SIP_CHAR     *pucEndPt
        );
};


class SipSummaryLine : public SipRefBase
{
    SIP_CHAR    *pMedia;        /* Media Type */
    SIP_INT32       newMessages;        /* no of new Messages */
    SIP_INT32       oldMessages;        /* no of old Messages */
    SIP_INT32       newUrgentMessages;    /* no of new Urgent Messages */
    SIP_INT32       oldUrgentMessages;    /* no of old Urgent Messages */

    public:

    SipSummaryLine();
    SipSummaryLine(const SipSummaryLine &objSummaryLine);
    ~SipSummaryLine();
    inline const SIP_CHAR* GetMedia() const
    {
        return pMedia;
    }
    SIP_INT32 GetNewMessages() const
    {
        return newMessages;
    }
    inline SIP_INT32 GetOldMessages() const
    {
        return oldMessages;
    }
    inline SIP_INT32 GetNewUrgentMessages() const
    {
        return newUrgentMessages;
    }
    inline SIP_INT32 GetOldUrgentMessages() const
    {
        return oldUrgentMessages;
    }
    SIP_BOOL DecodeSummaryLine
        (
         SIP_CHAR    *pucStartPt,
         SIP_CHAR     *pucEndPt
        );
};
/****************************************************************************
  Class Declaration Ends
 *****************************************************************************/

#endif //__SIP_MSG_BODY_H__
