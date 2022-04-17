/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_STACK_H_
#define _SIP_STACK_H_

#include "microsip_txnstruct.h"

#include "ByteArray.h"
#include "SIPMethod.h"
#include "SIPParameter.h"
#include "SIPStatusCode.h"

#define SipTimerType en_SipTimerType
#define SipTimerCallback TimeoutFuncPtr

class ISipConfigV;
class SIPProfile;
class SIPTimerValues;
class SIPTxnContextData;
class SIPTxnKey;



namespace SIPStack
{
    // Enumerations
    enum
    {
        /*
         * The message received was a remote retransmission of a message
         * which has already been responded to. The transaction layer resends
         * the previous response again. Application can ignore this message
         * if this return type is given.
         */
        MESSAGE_RETRANSMISSION = 0,

        /*
         * This value is returned when validity checks in the function fails.
         * For example, error was unallocated or an invalid parameter was passed to the function.
         */
        MESSAGE_FAILURE,

        /*
         * The received message was a stray SIP message.
         * For example, a PRACK without an associated RPR, an ACK without a final response,
         * or a final response without having sent a request, etc.
         */
        MESSAGE_STRAY,
        MESSAGE_STRAY_PRACK,

        /*
         * If the message was decoded successfully and is not a remote retransmission,
         * then this value is returned.
         */
        MESSAGE_SUCCESS
    };

    // Options for the selective SIP message encoding
    enum
    {
        OPT_START_LINE = 0x01,
        OPT_HEADER_PART = 0x02,
        OPT_BODY_PART = 0x03,
        OPT_ALL = (OPT_START_LINE | OPT_HEADER_PART | OPT_BODY_PART)
    };

    // SIP message types
    enum
    {
        SIP_MESSAGE_REQUEST = SipMessageRequest,
        SIP_MESSAGE_RESPONSE = SipMessageResponse,
        SIP_MESSAGE_ANY = SipMessageAny
    };

    // SIP transaction API called types
    enum
    {
        SIP_TXN_MSG_SENT = SipTxnMesgSent,
        SIP_TXN_MSG_RECEIVED = SipTxnMesgRecvd
    };

    GLOBAL void Initialize();
    GLOBAL void SetTransactionTimerValues(
            IN CONST SIPProfile *pSIPProfile, IN CONST ISipConfigV *piSipConfigV);

    GLOBAL SipError GetLastError();
    inline IMS_BOOL IsLastErrorNoExist()
    { return (GetLastError() == E_NO_EXIST); }

    template <typename T>
    inline void AddReference(IN_OUT T &pObject)
    { if (pObject != IMS_NULL) (pObject->dRefCount)++; }
    template <typename T>
    inline void RemoveReference(IN_OUT T &pObject)
    { if (pObject != IMS_NULL) (pObject->dRefCount)--; }
    template <typename T>
    inline void Free(IN T &pMemBlock);

    inline IMS_BOOL IsValidHeader(IN CONST SipHeader *pstHeader)
    {
        return ((pstHeader != IMS_NULL)
                && (pstHeader->dType != SipHdrTypeAny)
                && (pstHeader->pHeader != IMS_NULL));
    }
    inline IMS_SINT32 GetAnyHeaderType()
    { return SipHdrTypeAny; }
    inline IMS_SINT32 GetHeaderType(IN CONST SipHeader *pstHeader)
    { return static_cast<IMS_SINT32>(pstHeader->dType); }
    inline void SetHeaderType(IN IMS_SINT32 nType, IN_OUT SipHeader *&pstHeader)
    { pstHeader->dType = static_cast<en_HeaderType>(nType); }

    inline IMS_BOOL IsUriSchemeSIP(IN CONST SipAddrSpec *pstAddrSpec)
    { return pstAddrSpec->dType == SipAddrSipUri; }
    inline IMS_BOOL IsUriSchemeSIPS(IN CONST SipAddrSpec *pstAddrSpec)
    { return pstAddrSpec->dType == SipAddrSipSUri; }
    inline const IMS_CHAR* AddrSpec_GetUser(IN CONST SipAddrSpec *pstAddrSpec)
    { return pstAddrSpec->u.pSipUrl->pUser; }
    inline const IMS_CHAR* AddrSpec_GetPassword(IN CONST SipAddrSpec *pstAddrSpec)
    { return pstAddrSpec->u.pSipUrl->pPassword; }
    inline const IMS_CHAR* AddrSpec_GetHost(IN CONST SipAddrSpec *pstAddrSpec)
    { return pstAddrSpec->u.pSipUrl->pHost; }
    inline IMS_SINT32 AddrSpec_GetPort(IN CONST SipAddrSpec *pstAddrSpec)
    { return (pstAddrSpec->u.pSipUrl->dPort != IMS_NULL) ? *(pstAddrSpec->u.pSipUrl->dPort) : 0; }

    inline IMS_BOOL IsACKTransmissionRequiredForNon2XX()
    { return IMS_TRUE; }

    GLOBAL IMS_BOOL AppendHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL AppendMessageBody(IN SipMsgBody *pstMsgBody, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL AppendUnknownHeader(IN CONST AString &strName, IN CONST AString &strValue,
            IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL PrependHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL PrependUnknownHeader(IN CONST AString &strName, IN CONST AString &strValue,
            IN_OUT SipMessage *&pstMessage);

    GLOBAL IMS_BOOL CheckMandatoryHeaders(IN CONST SipMessage *pstMessage);

    GLOBAL SipHeader* CloneHeader(IN SipHeader *pstHeader);
    GLOBAL SipMessage* CloneMessage(IN SipMessage *pstMessage);
    GLOBAL SipMsgBody* CloneMessageBody(IN SipMsgBody *pstMsgBody);

    GLOBAL SipHeader* CopyHeader(IN SipHeader *pstHeader);
    GLOBAL SipHeader* CopyHeader(IN SipHeader *pstDest, IN SipHeader *pstSrc);

    GLOBAL void CorrectCSeq(IN CONST AString &strMethod, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL CorrectMessageBody(IN_OUT SipMessage *&pstMessage);

    GLOBAL SipHeader* CreateHeader(IN IMS_SINT32 nType);
    GLOBAL SipHeader* CreateHeader(IN IMS_SINT32 nType, IN SipAddrSpec *pstAddrSpec);
    GLOBAL SipMessage* CreateMessage(IN IMS_SINT32 nType);
    GLOBAL SipMsgBody* CreateMessageBody();
    GLOBAL IMS_BOOL CreateMIMEHeader(IN_OUT SipMsgBody *pstMsgBody);
    GLOBAL SipHeader* CreateViaHeader(IN CONST AString &strSentProtocol,
            IN CONST AString &strSentBy, IN CONST AString &strBranch);

    GLOBAL SipAddrSpec* DecodeAddrSpec(IN CONST AString &strAddress);
    GLOBAL SipHeader* DecodeHeader(IN IMS_SINT32 nType, IN CONST AString &strName,
            IN CONST AString &strBody);
    inline SipHeader* DecodeHeader(IN IMS_SINT32 nType, IN CONST AString &strBody)
    { return DecodeHeader(nType, AString::ConstNull(), strBody); }
#if 0
    GLOBAL IMS_SINT32 DecodeMessage(IN CONST SIPTransportBuffer *pBuffer, IN SipOptions stOptions,
            IN_OUT SipEventContext *pstEventContext, OUT SipMessage *&pstMessage,
            OUT SipTxnKey *&pstTxnKey, OUT IMS_SINT8 *&pNextBuffer);
#endif
    GLOBAL IMS_BOOL DecodeMessage(IN CONST IMS_BYTE *pBuffer, IN IMS_SINT32 nBuffLen,
            IN IMS_SINT32 nOptions, OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL DecodeMessageBody(IN SipMessage *pstMessage);

    GLOBAL IMS_BOOL EncodeAddrSpec(IN CONST SipAddrSpec *pstAddrSpec, IN IMS_BOOL bParams,
            OUT AString &strAddrSpec);
    GLOBAL IMS_BOOL EncodeHeaderBody(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AString &strHeaderBody);
    GLOBAL IMS_BOOL EncodeMessage(IN SipMessage *pstMessage, IN IMS_SINT32 nOptions,
            OUT IMS_BYTE *&pBuffer, OUT IMS_SINT32 &nBuffLen);
    GLOBAL IMS_BOOL EncodePartialMessage(IN SipMessage *pstMessage, IN IMS_SINT32 nOptions,
            OUT ByteArray &objMessage);
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN SipHeader *pstHeader);
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN SipAddrSpec *pstAddrSpec);
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN CONST AString &strParams, IN IMS_CHAR cSep);

    GLOBAL void FreeMemBlock(IN void *&pvMemBlock);
    GLOBAL void FreeAddrSpec(IN SipAddrSpec *&pstAddrSpec);
    GLOBAL void FreeHeader(IN SipHeader *pstHeader);
    GLOBAL void FreeHeaderEx(IN SipHeader *&pstHeader);
    GLOBAL void FreeMessage(IN SipMessage *&pstMessage);
    GLOBAL void FreeMessageBody(IN SipMsgBody *&pstMsgBody);

    GLOBAL IMS_CHAR GetCompactHeaderName(IN IMS_SINT32 nType,
            IN CONST AString &strName = AString::ConstNull());
    GLOBAL const IMS_CHAR* GetHeaderName(IN IMS_SINT32 nType,
            IN CONST AString &strName = AString::ConstNull());
    GLOBAL const IMS_CHAR* GetHeaderNameFromType(IN IMS_SINT32 nType);
    GLOBAL IMS_SINT32 GetHeaderTypeFromName(IN CONST AString &strName);

    GLOBAL SipAddrSpec* GetAddrSpec(IN SipHeader *pstHeader);
    GLOBAL SipAddrSpec* GetAddrSpec(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_UINT32 nIndex = 0);
    GLOBAL AString GetChallengeScheme(IN SipHeader *pstHeader);
    GLOBAL IMS_BOOL GetContent(IN SipMsgBody *pstMsgBody, OUT IMS_BYTE *&pContent,
            OUT IMS_SINT32 &nContentLength);
    GLOBAL IMS_UINT32 GetCSeqNumber(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL GetRAckHeader(IN SipMessage *pstMessage, OUT IMS_UINT32 &nResponseNum,
            OUT IMS_UINT32 &nCSeqNum, OUT SIPMethod &objMethod);
    GLOBAL IMS_SINT32 GetDestinationTransport(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL GetEventHeader(IN SipMessage *pstMessage, OUT AString &strEvent,
            OUT AString &strEventId);
    GLOBAL SipHeader* GetHeader(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_UINT32 nIndex = 0);
    GLOBAL AString GetHeaderAsString(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_BOOL bParams = IMS_FALSE, IN IMS_UINT32 nIndex = 0);
    GLOBAL IMS_SINT32 GetHeaderCount(IN SipMessage *pstMessage, IN IMS_SINT32 nType);
    GLOBAL IMS_BOOL GetHostAndPort(IN SipAddrSpec *pstAddrSpec,
            OUT AString &strHost, OUT IMS_UINT32 &nPort);
    GLOBAL IMS_BOOL GetHostNPortFromViaHeader(IN SipMessage *pstMessage,
            OUT AString &strHost, OUT IMS_SINT32 &nPort);
    GLOBAL SIPMethod GetMethod(IN SipMessage *pstMessage);
    GLOBAL SipMsgBody* GetMessageBody(IN SipMessage *pstMessage, IN IMS_SINT32 nIndex = 0);
    GLOBAL IMS_SINT32 GetMessageBodyCount(IN SipMessage *pstMessage);
    GLOBAL AString GetMIMEHeader(IN SipMsgBody *pstMsgBody, IN IMS_SINT32 nType,
            IN IMS_SINT32 nIndex = 0);
    GLOBAL IMS_SINT32 GetMIMEHeaderCount(IN SipMsgBody *pstMsgBody, IN IMS_SINT32 nType);
    GLOBAL AString GetParameter(IN SipAddrSpec *pstAddrSpec, IN CONST AString &strName,
            IN IMS_UINT32 nIndex = 0);
    GLOBAL AString GetParameter(IN SipHeader *pstHeader, IN CONST AString &strName,
            IN IMS_UINT32 nIndex = 0);
    GLOBAL IMS_SINT32 GetParameterCount(IN SipAddrSpec *pstAddrSpec);
    GLOBAL IMS_SINT32 GetParameterCount(IN SipHeader *pstHeader);
    GLOBAL SipAddrSpec* GetRequestUri(IN SipMessage *pstMessage);
    GLOBAL AString GetSentByFromVia(IN SipHeader *pstHeader);
    GLOBAL AString GetSentProtocolFromVia(IN SipHeader *pstHeader);
    GLOBAL AString GetSIPVersion(IN SipMessage *pstMessage);
    GLOBAL IMS_SINT32 GetStatusCode(IN SipMessage *pstMessage);
    GLOBAL SIPStatusCode GetStatusCodeEx(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL GetSubscriptionStateHeader(IN SipMessage *pstMessage,
            OUT AString &strSubsState, OUT IMS_SINT32 *pnExpires = IMS_NULL);
    GLOBAL SipHeader* GetUnknownHeader(IN SipMessage *pstMessage, IN CONST AString &strName,
            IN IMS_UINT32 nIndex = 0);
    GLOBAL AString GetUnknownHeaderName(IN SipHeader *pstHeader);
    GLOBAL AString GetUnknownHeaderBody(IN SipHeader *pstHeader);
    GLOBAL AString GetViaBranchParameter(IN SipMessage *pstMessage);

    GLOBAL IMS_BOOL HasParameter(IN SipHeader *pstHeader, IN CONST AString &strName);
    GLOBAL IMS_BOOL HasParameter(IN SipAddrSpec *pstAddrSpec, IN CONST AString &strName);
    GLOBAL IMS_BOOL HasMIMEMessageBody(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL HasSDPMessageBody(IN SipMessage *pstMessage);

    GLOBAL IMS_BOOL InsertHeader(IN SipHeader *pstHeader, IN IMS_UINT32 nIndex,
            IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL IsCompactHeaderNameSupported(IN IMS_SINT32 nType,
            IN CONST AString &strName = AString::ConstNull());
    GLOBAL IMS_BOOL IsHeaderPresent(IN SipMessage *pstMessage, IN IMS_SINT32 nType);
    GLOBAL IMS_BOOL IsMessageBodySDP(IN SipMsgBody *pstMsgBody);
    GLOBAL IMS_BOOL IsMessageBodyCompressed(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL IsMessageRPR(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL IsOptionRequired(IN SipMessage *pstMessage, IN CONST AString &strOption);
    GLOBAL IMS_BOOL IsOptionSupported(IN SipMessage *pstMessage, IN CONST AString &strOption);
    GLOBAL IMS_BOOL IsRequestMessage(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL IsSingleHeader(IN IMS_SINT32 nType);
    GLOBAL IMS_BOOL IsAddressFormatHeader(IN IMS_SINT32 nType, IN CONST AString &strName);
    GLOBAL IMS_BOOL IsAQUOTRequiredForAddressFormat(
            IN IMS_SINT32 nType, IN CONST AString &strName);
    GLOBAL IMS_BOOL IsUnknownHeader(IN_OUT IMS_SINT32 &nType, IN CONST AString &strName = AString::ConstNull());
    GLOBAL IMS_BOOL IsUriSchemeAllowed(IN SipHeader *pstHeader);
    GLOBAL IMS_BOOL IsUriSchemeSupported(IN SipHeader *pstHeader);

    GLOBAL IMS_BOOL OverwriteHeaders(IN SipMessage *pstSrcMessage,
            IN_OUT SipMessage *&pstDestMessage);

    GLOBAL void ParseHostNPort(IN CONST AString &strHostNPort, OUT AString &strHost,
            OUT IMS_SINT32 &nPort);

    GLOBAL IMS_BOOL RemoveAllMessageBodies(IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL RemoveHeader(IN IMS_SINT32 nType, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL RemoveParameter(IN CONST AString &strName, IN_OUT SipHeader *&pstHeader);
    GLOBAL IMS_BOOL RemoveParameter(IN CONST AString &strName, IN_OUT SipAddrSpec *&pstAddrSpec);
    GLOBAL void RemoveUserAndPassword(IN_OUT SipAddrSpec *&pstAddrSpec);

    GLOBAL IMS_BOOL SetChallengeScheme(IN CONST AString &strScheme, IN_OUT SipHeader *&pstHeader);
    GLOBAL IMS_BOOL SetContent(IN CONST IMS_BYTE *pContent, IN IMS_SINT32 nContentLength,
            IN_OUT SipMsgBody *&pstMsgBody);
    GLOBAL IMS_BOOL SetHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL SetMethod(IN CONST SIPMethod &objMethod, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL SetMIMEHeader(IN IMS_SINT32 nType, IN SipHeader *pstHeader,
            IN_OUT SipMsgBody *&pstMsgBody);
    GLOBAL IMS_BOOL SetMIMEHeader(IN IMS_SINT32 nType, IN CONST AString &strName,
            IN CONST AString &strBody, IN_OUT SipMsgBody *&pstMsgBody);
    GLOBAL IMS_BOOL SetParameter(IN SipHeader* pstHeader,
            IN CONST AString &strName, IN CONST AString &strValue);
    GLOBAL IMS_BOOL SetRequestLine(IN CONST AString &strMethod, IN CONST AString &strURI,
            IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL SetRequestUri(IN SipAddrSpec *pstAddrSpec, IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL SetStatusLine(IN IMS_SINT32 nStatusCode, IN CONST AString &strReasonPhrase,
            IN_OUT SipMessage *&pstMessage);
    GLOBAL IMS_BOOL SetUnknownHeader(IN SipHeader *pstHeader,
            IN CONST AString &strName, IN_OUT SipMessage *&pstMessage);

    GLOBAL IMS_BOOL UncompressMessageBody(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL UpdateSentProtocol(IN SipMessage *pstMessage,
            IN CONST AString &strSentProtocol);

    // APIs for Bad header control
    GLOBAL void DisplayBadHeaders(IN SipMessage *pstMessage);
    GLOBAL IMS_SINT32 GetBadHeaderCount(IN SipMessage *pstMessage);
    GLOBAL IMS_BOOL HasMandatoryHeaders(IN SipMessage *pstMessage);

    // APIs for SIP authentication
    GLOBAL IMS_BOOL GetEntityBody(IN SipMessage *pstMessage, OUT AString &strEntityBody);

    // APIs for SIP transaction layer
    inline SipEventContext* TxnBuffer_GetEventContext(IN SipTxnBuffer *pstTxnBuffer)
    { return pstTxnBuffer->pEventContext; }
    inline IMS_BOOL TxnBuffer_IsClientTransaction(IN SipTxnBuffer *pstTxnBuffer)
    {
        return (pstTxnBuffer->dTxnClass == SipInviteClient)
                || (pstTxnBuffer->dTxnClass == SipNonInviteClient);
    }
    inline const IMS_CHAR* TxnKey_GetViaBranch(IN SipTxnKey *pstTxnKey)
    { return pstTxnKey->pViaBranch; }
    inline const IMS_CHAR* TxnKey_GetCallId(IN SipTxnKey *pstTxnKey)
    { return pstTxnKey->pCallid; }
    inline const IMS_CHAR* TxnKey_GetMethod(IN SipTxnKey *pstTxnKey)
    { return pstTxnKey->pMethod; }
    inline IMS_SINT32 TxnKey_GetStatusCode(IN SipTxnKey *pstTxnKey)
    { return pstTxnKey->dRespCodeNum; }
    inline IMS_BOOL IsSipTxnContextRequired()
    { return IMS_TRUE; }

    GLOBAL IMS_BOOL AbortTransaction(IN SipTxnKey *pstTxnKey, IN SipTxnContext *pstTxnContext);
    GLOBAL SipEventContext* CreateEventContext();
    GLOBAL SipTxnContext* CreateTxnContext();
    GLOBAL SIPTxnKey* CreateTxnKey(IN SipMessage *pstMessage);
    GLOBAL SipTxnKey* CreateTxnKey(IN SipMessage *pstMessage, IN IMS_SINT32 nAPICalled);
    GLOBAL SIPTxnKey* CreateTxnKeyFromKey(IN SipTxnKey *pstTxnKey);
    GLOBAL IMS_BOOL CompareTxnKeys(IN SipTxnKey *pstTxnKey1, IN SipTxnKey *pstTxnKey2);
    GLOBAL IMS_BOOL CompareTxnKeysForAck(IN SipTxnKey *pstTxnKey1, IN SipTxnKey *pstTxnKey2);
    GLOBAL IMS_BOOL CompareTxnKeysForCancel(IN SipTxnKey *pstCancelKey, IN SipTxnKey *pstTxnKey);
    GLOBAL void DestroyEventContext(IN SipEventContext *pstContext);
    GLOBAL void DestroyTxnContext(IN SipTxnContext *pstContext);
    GLOBAL void DisplayTxnKey(IN CONST SipTxnKey *pstTxnKey);
    GLOBAL void FreeTxnKey(IN SipTxnKey *&pstTxnKey);
    GLOBAL void FreeTxnBuffer(IN SipTxnBuffer *&pstTxnBuffer);
    GLOBAL SIPTxnContextData* GetTxnContextData(IN SipEventContext *pstContext);
    GLOBAL IMS_SINT32 HandleTxn(IN SipMessage *pstMessage, IN SipTxnContext *pstTxnContext,
            OUT SipTxnKey *&pstTxnKey);
    GLOBAL IMS_BOOL SendToTxnLayer(IN CONST ByteArray &objBuffer, IN SipMessage *pstMessage,
            IN SipTranspAddr *pstAddr, IN IMS_BYTE byTranspType, IN SipTxnContext *pstTxnContext,
            OUT SipTxnKey *&pstTxnKey);
    GLOBAL void TerminateTransaction(IN SipTxnKey *pstTxnKey);

    // APIs for SIP transaction timer
    GLOBAL const IMS_CHAR* GetTimerTypeAsString(IN SipTimerType enTimerType);
    GLOBAL const IMS_CHAR* GetTimerTypeAsString(IN SipTxnTimeoutData *pstData);
    GLOBAL void InvokeTimerCallback(IN SipTimerCallback pfnCallback,
            IN SipTxnTimeoutData *pstData, IN IMS_PVOID pvExtraParam);
    GLOBAL void SetTimerValues(IN SIPTimerValues *pTV, IN_OUT SipTxnContext *&pstTxnContext);

    // APIs for trace or debugging
    GLOBAL void DisplayUnknownHeaders(IN SipMessage *pstMessage);
    GLOBAL const IMS_CHAR* GetLogString(IN CONST IMS_CHAR *pszInput,
            IN_OUT IMS_CHAR *pszOutput, IN IMS_SINT32 nOutSize /* > 3, excluding null char */,
            IN CONST IMS_CHAR cDelimiter = 0 /* no delimiter */);
} // End of SIP (namespace)

template <typename T>
inline void SIPStack::Free(IN T &pMemBlock)
{ SIPStack::FreeMemBlock(reinterpret_cast<void*&>(pMemBlock)); }

#endif // _SIP_STACK_H_
