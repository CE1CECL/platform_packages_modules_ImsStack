/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceUtil.h"
#include "IMSStrLib.h"
#include "AStringBuffer.h"
#include "SystemConfigManager.h"
#include "stack_headers.h"
#include "SIPPrivate.h"
#include "SIPConfigProxy.h"
#include "SIPMessageBuffer.h"
#include "SIPHeaderName.h"
#include "SIPStackPL.h"
#include "SIPHeader.h"
#include "SIPMessageBodyPart.h"
#include "SIPTxnContextData.h"
#include "SIPTxnKey.h"
#include "SIPUtil.h"
#include "SIPTimerValues.h"
#include "SIPStack.h"

__IMS_TRACE_TAG_SIP__;



extern "C" SipBool sip_captureTxnInformn(SipMessage*, SIP_S8bit*, SIP_U32bit, SipTxnContext*,
SipTranspAddr*, SIP_S8bit, SipTxnInformn*);
extern "C" SipBool sip_checkMandatoryTxnHeaders(SipMessage*, en_SipTxnType);
extern "C" SipBool sip_isTxnAnRPR(SipTxnInformn*);
extern "C" SipBool sip_getTxnClass(SipTxnInformn*, en_SipTxnAPICalled, en_SipTxnClass*);
extern "C" SipBool sip_txn_sendBuffer(SipTxnInformn*, SipError*);
extern "C" SipBool sip_updateRPRTxnForMesgSent(SipTxnInformn*, SipError*);
extern "C" SipBool sip_updateInvClTxnForMesgSent(SipTxnInformn*, SipError*);
extern "C" SipBool sip_updateNonInvClTxnForMesgSent(SipTxnInformn*, SipError*);
extern "C" SipBool sip_updateInvSrvTxnForMesgSent(SipTxnInformn*, SipError*);
extern "C" SipBool sip_updateNonInvSrvTxnForMesgSent(SipTxnInformn*, SipError*);

extern void SIPStackTxnLayer_Initialize();

namespace SIPStack
{

    // SIP stack last error storage -- starts

    LOCAL SipError genError;

    /*

    Remarks

    */
    LOCAL inline SipError* SIPStackError()
    { return &genError; }

    /*

    Remarks

    */
    LOCAL inline void SIPStackError(IN SipError enError_)
    { genError = enError_; }

    // SIP stack last error storage -- ends

    // Wrapper APIs for SIP stack list structure -- starts

    /*

    Remarks

    */
    template <typename T>
    LOCAL inline SipBool List_Append(IN SipList *pstList, IN T pData, OUT SipError *penError)
    {
        return sip_listAppend(pstList, reinterpret_cast<SIP_Pvoid>(pData), penError);
    }

    /*

    Remarks

    */
    LOCAL inline SipBool List_DeleteAll(IN SipList *pstList, OUT SipError *penError)
    {
        return sip_listDeleteAll(pstList, penError);
    }

    /*

    Remarks

    */
    LOCAL inline SipBool List_DeleteAt(IN SipList *pstList, IN IMS_UINT32 nIndex,
            OUT SipError *penError)
    {
        return sip_listDeleteAt(pstList, nIndex, penError);
    }

    /*

    Remarks

    */
    template <typename T>
    LOCAL inline SipBool List_Prepend(IN SipList *pstList, IN T pData, OUT SipError *penError)
    {
        return sip_listPrepend(pstList, reinterpret_cast<SIP_Pvoid>(pData), penError);
    }

    /*

    Remarks

    */
    template <typename T>
    LOCAL inline SipBool List_GetAt(IN SipList *pstList, IN IMS_UINT32 nIndex, IN T *ppData,
            OUT SipError *penError)
    {
        return sip_listGetAt(pstList, nIndex, reinterpret_cast<SIP_Pvoid*>(ppData), penError);
    }

    /*

    Remarks

    */
    template <typename T>
    LOCAL inline SipBool List_InsertAt(IN SipList *pstList, IN IMS_UINT32 nIndex, IN T pData,
            OUT SipError *penError)
    {
        return sip_listInsertAt(pstList, nIndex, reinterpret_cast<SIP_Pvoid>(pData), penError);
    }

    /*

    Remarks

    */
    template <typename T>
    LOCAL inline SipBool List_SetAt(IN SipList *pstList, IN IMS_UINT32 nIndex, IN T pData,
            OUT SipError *penError)
    {
        return sip_listSetAt(pstList, nIndex, reinterpret_cast<SIP_Pvoid>(pData), penError);
    }

    /*

    Remarks

    */
    LOCAL inline SipBool List_SizeOf(IN SipList *pstList, IN IMS_UINT32 *pnSize,
            OUT SipError *penError)
    {
        return sip_listSizeOf(pstList, pnSize, penError);
    }

    // Wrapper APIs for SIP stack list structure -- ends

    /*

    Remarks

    */
    LOCAL void FormParameterList(IN SipList *pstList, IN CONST AString &strSep,
            IN IMS_SINT32 nLeadingSep, OUT AStringBuffer &objStringBuffer)
    {
        SipParam* pstParam = IMS_NULL;

        IMS_UINT32 nCount = 0;
        IMS_UINT32 nIndex = 0;
        IMS_UINT32 nValueCount = 0;
        IMS_UINT32 nValueIndex = 0;
        IMS_CHAR *pszValue = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        (void) List_SizeOf(pstList, &nCount, SIPStackError());

        while (nIndex < nCount)
        {
            pstParam = IMS_NULL;
            nValueCount = 0;
            nValueIndex = 0;

            (void) List_GetAt(pstList, nIndex, &pstParam, SIPStackError());

            if ((nIndex != 0) || (nLeadingSep != 0))
                objStringBuffer += strSep;

            objStringBuffer += pstParam->pName;

            (void) List_SizeOf(&(pstParam->slValue), &nValueCount, SIPStackError());

            if (nValueCount >= 1)
            {
                (void) List_GetAt(&(pstParam->slValue), nValueIndex, &pszValue, SIPStackError());

                if (pszValue != IMS_NULL)
                {
                    objStringBuffer += TextParser::CHAR_EQUAL;

                    while (nValueIndex < nValueCount)
                    {
                        if (nValueIndex > 0)
                        {
                            objStringBuffer += TextParser::CHAR_COMMA;
                        }

                        (void) List_GetAt(&(pstParam->slValue),
                                nValueIndex, &pszValue, SIPStackError());
                        objStringBuffer += pszValue;
                        nValueIndex++;
                    }
                }
            }

            nIndex++;
        }
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormAddrSpec(IN CONST SipAddrSpec *pstAddrSpec, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        //-----------------------------------------------------------------------------------------

        switch (pstAddrSpec->dType)
        {
        case SipAddrSipUri:
        case SipAddrSipSUri:
            {
                SipUrl* pstURL = pstAddrSpec->u.pSipUrl;

                if (pstAddrSpec->dType == SipAddrSipUri)
                    objStringBuffer += SIP::STR_SIP;
                else
                    objStringBuffer += SIP::STR_SIPS;

                objStringBuffer += TextParser::CHAR_COLON;

                objStringBuffer += pstURL->pUser;

                if (pstURL->pPassword != IMS_NULL)
                {
                    objStringBuffer += TextParser::CHAR_COLON;
                    objStringBuffer += pstURL->pPassword;
                }

                if (pstURL->pUser != IMS_NULL)
                    objStringBuffer += TextParser::CHAR_AT;

                objStringBuffer += pstURL->pHost;

                if (pstURL->dPort != IMS_NULL)
                {
                    objStringBuffer += TextParser::CHAR_COLON;
                    objStringBuffer += *(pstURL->dPort);
                }

                if (bParams)
                {
                    FormParameterList(&(pstURL->slParam),
                            AString(&TextParser::CHAR_SEMICOLON,1), 1, objStringBuffer);
                }

                if (pstURL->pHeader != IMS_NULL)
                {
                    objStringBuffer += TextParser::CHAR_QUESTIONMARK;
                    objStringBuffer += pstURL->pHeader;
                }
            }
            break;

        default:
            objStringBuffer += pstAddrSpec->u.pUri;
            break;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormAuthFamily(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

#ifdef SIP_INCREMENTAL_PARSING
        objStringBuffer += pSCH->pBuffer;
#else
        if (pSCH->pStr1 != IMS_NULL)
        {
            objStringBuffer += pSCH->pStr1;

            if (!AString::ConstEmpty().Equals(pSCH->pStr1))
                objStringBuffer += TextParser::CHAR_SP;
        }

        if (bParams == IMS_TRUE)
        {
            FormParameterList(&(pSCH->slParam),
                    AString(&TextParser::CHAR_COMMA, 1), 0, objStringBuffer);
        }
#endif // SIP_INCREMENTAL_PARSING

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormFromFamily(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        IMS_BOOL bUseAQUOT = IMS_FALSE;
        IMS_BOOL bCheckAQUOT = IMS_FALSE;

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        switch (pstHeader->dType)
        {
        // name-addr format : (including "Reply-To" header)
        case SipHdrTypeRoute:
        case SipHdrTypeRecordRoute:
#ifdef SIP_3GPP
        case SipHdrTypePath:
        case SipHdrTypePCalledPartyId:
        case SipHdrTypePAssociatedUri:
        case SipHdrTypeServiceRoute:
        case SipHdrTypeHistoryInfo:
#endif
            bUseAQUOT = IMS_TRUE;
            break;

        case SipHdrTypeFrom:
        case SipHdrTypeTo:
#ifdef SIP_REFER
        case SipHdrTypeReferTo:
        case SipHdrTypeReferredBy:
#endif
        case SipHdrTypeContactAny:
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
            bCheckAQUOT = IMS_TRUE;
            break;
#ifdef SIP_PRIVACY
        case SipHdrTypePPreferredId:
        case SipHdrTypePAssertId:
            // name-addr / addr-spec only used,
            // but insert AQUOT according to the context
            // (application does not concern the detailed information...)
            bCheckAQUOT = IMS_TRUE;
            break;
#endif
        default:
            break;
        }

#ifdef SIP_INCREMENTAL_PARSING
        objStringBuffer += pSCH->pBuffer;
#else
        if ((pstHeader->dType == SipHdrTypeContactAny)
                || (pstHeader->dType == SipHdrTypeContactNormal)
                || (pstHeader->dType == SipHdrTypeContactWildCard))
        {
            if (pSCH->dIntVar1 == SipContactWildCard)
            {
                objStringBuffer += TextParser::CHAR_ASTERISK;
                return IMS_TRUE;
            }
        }

        /*
         * We come here if this contact header is not a "*"
         * Fill display name if present.
         */
        if (pSCH->pStr1 != IMS_NULL)
        {
            if (!AString::ConstEmpty().Equals(pSCH->pStr1))
            {
                bUseAQUOT = IMS_TRUE;

                objStringBuffer += pSCH->pStr1;
                /*
                 * If URI is a type of name-addr and is not a quoted-string,
                 * it MUST include a least one WSP(SP/HTAB).
                 * But, we will not check the double quote in this case.
                 * SP is always inserted after setting display name.
                 */
                objStringBuffer += TextParser::CHAR_SP;
            }
        }

        // Checks uri-parameter
        if ((pSCH->pAddrSpec != IMS_NULL) && (!bUseAQUOT) && (bCheckAQUOT))
        {
            // Check if uri parameter exists or not
            // If it exists, the forming buffer will insert LAQUOT('<') & RAQUOT('>')
            if (GetParameterCount(pSCH->pAddrSpec) > 0)
            {
                bUseAQUOT = IMS_TRUE;
            }
        }

        // Checks header parameters
        if ((bParams) && (!bUseAQUOT) && (bCheckAQUOT))
        {
            // Check if header parameter exists or not
            // If it exists, the forming buffer will insert LAQUOT('<') & RAQUOT('>')
            if (GetParameterCount(const_cast<SipHeader*>(pstHeader)) > 0)
            {
                bUseAQUOT = IMS_TRUE;
            }
        }

        // Check if ';' is included in the unknown URI scheme (tel, im, ...)
        if (!bUseAQUOT && (pSCH->pAddrSpec != IMS_NULL))
        {
            if ((pSCH->pAddrSpec->dType != SipAddrSipUri)
                    && (pSCH->pAddrSpec->dType != SipAddrSipSUri)
                    && (pSCH->pAddrSpec->u.pUri != IMS_NULL))
            {
                if (IMS_StrChr(pSCH->pAddrSpec->u.pUri, TextParser::CHAR_SEMICOLON) != IMS_NULL)
                {
                    bUseAQUOT = IMS_TRUE;
                }
            }
        }

        if (bUseAQUOT)
        {
            objStringBuffer += TextParser::CHAR_LAQUOT;
        }

        if (pSCH->pAddrSpec != IMS_NULL)
        {
            FormAddrSpec(pSCH->pAddrSpec, IMS_TRUE, objStringBuffer);
        }

        if (bUseAQUOT)
        {
            objStringBuffer += TextParser::CHAR_RAQUOT;
        }

        if (bParams == IMS_TRUE)
        {
            FormParameterList(&(pSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON,1), 1, objStringBuffer);
        }
#endif // SIP_INCREMENTAL_PARSING

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormIntegerFamily(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

#ifdef SIP_INCREMENTAL_PARSING
        objStringBuffer += pSCH->pBuffer;
#else
        AString strNumber;

        objStringBuffer += strNumber.SetNumber(pSCH->dIntVar1);

        if ((bParams == IMS_TRUE)
                && ((pstHeader->dType == SipHdrTypeMinSE)
                    || (pstHeader->dType == SipHdrTypeSessionExpires)))
        {
            FormParameterList(&(pSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON, 1), 1, objStringBuffer);
        }
#endif // SIP_INCREMENTAL_PARSING

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormStringFamily(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

#ifdef SIP_INCREMENTAL_PARSING
        objStringBuffer += pSCH->pBuffer;
#else
        if (pSCH->pStr1 != IMS_NULL)
        {
            objStringBuffer += pSCH->pStr1;
        }

        if (bParams == IMS_TRUE)
        {
            FormParameterList(&(pSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON, 1), 1, objStringBuffer);
        }
#endif // SIP_INCREMENTAL_PARSING

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormTimeFamily(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

#ifdef SIP_INCREMENTAL_PARSING
        objStringBuffer += pSCH->pBuffer;
#else
#ifdef SIP_DATE
        if (pSCH->dIntVar1 == SipExpSeconds)
        {
            AString strNumber;

            objStringBuffer += strNumber.SetNumber(pSCH->u.dSec);
        }
        else
        {
            IMS_CHAR acBuffer[32] = {0, };
            IMS_CHAR *pszBuffer = &(acBuffer[0]);

            if (sip_formDateStruct(&pszBuffer, pSCH->u.pDate, SIPStackError()) == SipFail)
            {
                // throw exception
            }

            objStringBuffer += pszBuffer;
        }

        if ((pstHeader->dType == SipHdrTypeRetryAfterDate)
                || (pstHeader->dType == SipHdrTypeRetryAfterSec)
                || (pstHeader->dType == SipHdrTypeRetryAfterAny))
        {
            if (pSCH->pStr1 != IMS_NULL)
            {
                objStringBuffer += TextParser::CHAR_SP;
                objStringBuffer += pSCH->pStr1;
            }
        }
#else
        AString strNumber;

        objStringBuffer += strNumber.SetNumber(pSCH->dIntVar1);

        if (pstHeader->dType == SipHdrTypeRetryAfter)
        {
            if (pSCH->pStr1 != IMS_NULL)
            {
                objStringBuffer += TextParser::CHAR_SP;
                objStringBuffer += pSCH->pStr1;
            }
        }
#endif // SIP_DATE
        if (bParams == IMS_TRUE)
        {
            FormParameterList(&(pSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON, 1), 1, objStringBuffer);
        }
#endif // SIP_INCREMENTAL_PARSING

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormHeaderBody(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AStringBuffer &objStringBuffer)
    {
        IMS_BOOL bEncodingResult = IMS_TRUE;

        //-----------------------------------------------------------------------------------------

        // Check if the header is valid
        if (!IsValidHeader(pstHeader))
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        switch (pstHeader->dType)
        {
        case SipHdrTypeFrom:
        case SipHdrTypeTo:
        case SipHdrTypeRoute:
        case SipHdrTypeRecordRoute:
#ifdef SIP_3GPP
        case SipHdrTypePath:
        case SipHdrTypeServiceRoute:
        case SipHdrTypePAssociatedUri:
        case SipHdrTypePCalledPartyId:
        case SipHdrTypeHistoryInfo:
#endif
#ifdef SIP_PRIVACY
        case SipHdrTypePPreferredId:
        case SipHdrTypePAssertId:
#endif
#ifdef SIP_REFER
        case SipHdrTypeReferredBy:
        case SipHdrTypeReferTo:
#endif
            bEncodingResult = FormFromFamily(pstHeader, bParams, objStringBuffer);
            break;

        case SipHdrTypeContactAny:
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
            bEncodingResult = FormFromFamily(pstHeader, bParams, objStringBuffer);
            break;

#ifdef SIP_AUTHENTICATE
        case SipHdrTypeAuthorization:
        case SipHdrTypeWwwAuthenticate:
        case SipHdrTypeProxyauthorization:
        case SipHdrTypeProxyAuthenticate:
            bEncodingResult = FormAuthFamily(pstHeader, bParams, objStringBuffer);
            break;
#endif
        case SipHdrTypeAllow:
        case SipHdrTypeCallId:
        case SipHdrTypeContentType:
        case SipHdrTypeContentDisposition:
        case SipHdrTypeContentEncoding:
        case SipHdrTypeMimeVersion:
        case SipHdrTypeRequire:
        case SipHdrTypeSupported:
        case SipHdrTypeUnsupported:
#ifdef SIP_3GPP
        case SipHdrTypeRequestDisposition:
        case SipHdrTypeIfMatch:
        case SipHdrTypeETag:
        case SipHdrTypePVisitedNetworkId:
        case SipHdrTypePcfAddr:
        case SipHdrTypeAcceptContact:
        case SipHdrTypeRejectContact:
        case SipHdrTypePanInfo:
        case SipHdrTypeJoin:
        case SipHdrTypePcVector:
#endif
#ifdef SIP_IMPP
        case SipHdrTypeAllowEvents:
        case SipHdrTypeEvent:
        case SipHdrTypeSubscriptionState:
#endif
#ifdef SIP_REPLACES
        case SipHdrTypeReplaces:
#endif
#ifdef SIP_SECURITY
        case SipHdrTypeSecurityClient:
        case SipHdrTypeSecurityServer:
        case SipHdrTypeSecurityVerify:
#endif
#ifdef SIP_PEMEDIA
        case SipHdrTypePEarlyMedia:
#endif
            bEncodingResult = FormStringFamily(pstHeader, bParams, objStringBuffer);
            break;

        case SipHdrTypeContentLength:
#ifdef SIP_RPR
        case SipHdrTypeRSeq:
#endif
        case SipHdrTypeMaxforwards:
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeMinSE:
        case SipHdrTypeSessionExpires:
#endif
        case SipHdrTypeMinExpires:
            bEncodingResult = FormIntegerFamily(pstHeader, bParams, objStringBuffer);
            break;

#ifdef SIP_DATE
        case SipHdrTypeRetryAfterDate:
        case SipHdrTypeRetryAfterSec:
        case SipHdrTypeRetryAfterAny:
#else
        case SipHdrTypeRetryAfter:
#endif
#ifdef SIP_DATE
        case SipHdrTypeExpiresDate:
        case SipHdrTypeExpiresSec:
        case SipHdrTypeExpiresAny:
#else
        case SipHdrTypeExpires:
#endif
            bEncodingResult = FormTimeFamily(pstHeader, bParams, objStringBuffer);
            break;

        case SipHdrTypeUnknown:
            objStringBuffer += ((static_cast<SipCommonHeader*>(pstHeader->pHeader))->pStr2);
            break;

        case SipHdrTypeAccept:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            // Fill media type
            objStringBuffer += pstSCH->pStr1;

            // Fill accept parameters (q-value) if present
            objStringBuffer += pstSCH->pStr2;
#endif // SIP_INCREMENTAL_PARSING
        }
        break;

        case SipHdrTypeCseq:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);
            AString strCSeqNumber;

            // Fill the command sequence number
            objStringBuffer += strCSeqNumber.SetNumber(pstSCH->dIntVar1);
            // Append SP
            objStringBuffer += TextParser::CHAR_SP;
            // Append SIP method
            objStringBuffer += pstSCH->pStr1;
        }
        break;

#ifdef SIP_TIMESTAMP
        case SipHdrTypeTimestamp:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            objStringBuffer += pstSCH->pStr1;

            if (pstSCH->pStr2 != IMS_NULL)
                objStringBuffer += TextParser::CHAR_SP;

            objStringBuffer += pstSCH->pStr2;
#endif // SIP_INCREMENTAL_PARSING
        }
        break;
#endif // SIP_TIMESTAMP

        case SipHdrTypeVia:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            // Fill sent protocol & SP
            objStringBuffer += pstSCH->pStr1;
            objStringBuffer += TextParser::CHAR_SP;
            // Fill sent-by
            objStringBuffer += pstSCH->pStr2;

            // Fill header parameters
            FormParameterList(&(pstSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON,1), 1, objStringBuffer);
#endif // SIP_INCREMENTAL_PARSING
        }
        break;

#ifdef SIP_RPR
        case SipHdrTypeRAck:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            AString strNumber;

            // Fill response number
            objStringBuffer += strNumber.SetNumber(pstSCH->dIntVar1);
            objStringBuffer += TextParser::CHAR_SP;

            // Fill CSeq number
            objStringBuffer += strNumber.SetNumber(pstSCH->dIntVar2);
            objStringBuffer += TextParser::CHAR_SP;

            // Fill SIP method
            objStringBuffer += pstSCH->pStr1;
#endif // SIP_INCREMENTAL_PARSING
        }
        break;
#endif // SIP_RPR

#ifdef SIP_WARNING
        case SipHdrTypeWarning:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            AString strNumber;

            // Fill warning code & SP
            objStringBuffer += strNumber.SetNumber(pstSCH->dIntVar1);
            objStringBuffer += TextParser::CHAR_SP;

            // Fill Agent & SP
            objStringBuffer += pstSCH->pStr1;
            objStringBuffer += TextParser::CHAR_SP;

            // Fill text
            objStringBuffer += pstSCH->pStr2;
#endif // SIP_INCREMENTAL_PARSING
        }
        break;
#endif // SIP_WARNING

#ifdef SIP_PRIVACY
        case SipHdrTypePrivacy:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            FormParameterList(&(pstSCH->slParam),
                    AString(&TextParser::CHAR_SEMICOLON,1), 0, objStringBuffer);
#endif // SIP_INCREMENTAL_PARSING
        }
        break;
#endif // SIP_PRIVACY

#ifdef SIP_CONGEST
        case SipHdrTypeAcceptRsrcPriority:
        case SipHdrTypeRsrcPriority:
        {
            SipCommonHeader *pstSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

#ifdef SIP_INCREMENTAL_PARSING
            objStringBuffer += pstSCH->pBuffer;
#else
            // Fill namespace
            objStringBuffer += pstSCH->pStr1;
            objStringBuffer += TextParser::CHAR_DOT;

            // Fill priority value
            objStringBuffer += pstSCH->pStr2;
#endif // SIP_INCREMENTAL_PARSING
        }
        break;
#endif // SIP_CONGEST

        default:
            bEncodingResult = IMS_FALSE;
            break;
    }

        return bEncodingResult;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormHeader(IN SipHeader *pstHeader, IN IMS_BOOL bCompactForm,
            OUT AStringBuffer &objStringBuffer)
    {
        AString strName;

        //-----------------------------------------------------------------------------------------

        // Add the header name field
        if (pstHeader->dType == SipHdrTypeUnknown)
        {
            strName = GetUnknownHeaderName(pstHeader);
        }

        if (bCompactForm)
        {
            const IMS_CHAR cCompactName = GetCompactHeaderName(pstHeader->dType, strName);

            if (cCompactName != '\0')
                objStringBuffer += cCompactName;
            else
                objStringBuffer += strName;
        }
        else
        {
            const IMS_CHAR* pszName = GetHeaderName(pstHeader->dType, strName);

            if (pszName == IMS_NULL)
            {
                return IMS_FALSE;
            }

            objStringBuffer += pszName;
        }

        // Adds a colon
        objStringBuffer += TextParser::CHAR_COLON;
        objStringBuffer += TextParser::CHAR_SP;

        // Add the header body field
        (void) FormHeaderBody(pstHeader, IMS_TRUE, objStringBuffer);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL FormEachHeader(IN SipMessage *pstMessage, IN IMS_UINT32 nHeaderOrder,
            IN_OUT IMS_SINT32 *pnHeaderCounter, OUT AStringBuffer &objStringBuffer)
    {
        en_HeaderType enType = SipHdrTypeAny;
        en_HeaderForm enFormRule = SipFormShort;
        IMS_UINT32 nHeaderCount = 0;

        //-----------------------------------------------------------------------------------------

        (void) sip_getHeaderCountFromHeaderLine(pstMessage,
                nHeaderOrder, &nHeaderCount, SIPStackError());
        (void) sip_getHeaderTypeAtHeaderLine(pstMessage,
                nHeaderOrder, &enType, SIPStackError());
        (void) sip_getHeaderFormFromHeaderLine(pstMessage,
                nHeaderOrder, &enFormRule, SIPStackError());

        IMS_UINT32 nInitialIndex = pnHeaderCounter[enType];

        // Update the header counter
        pnHeaderCounter[enType] += nHeaderCount;

        IMS_BOOL bCompactForm = (enFormRule == SipFormShort) ? IMS_TRUE : IMS_FALSE;
        SipHeader *pstHeader = GetHeader(pstMessage, enType, nInitialIndex);

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        (void) FormHeader(pstHeader, bCompactForm, objStringBuffer);

        FreeHeaderEx(pstHeader);

        nHeaderCount = nInitialIndex + nHeaderCount;

        for (IMS_UINT32 i = nInitialIndex + 1; i < nHeaderCount; ++i)
        {
            objStringBuffer += TextParser::CHAR_COMMA;

            pstHeader = GetHeader(pstMessage, enType, i);

            if (!IsValidHeader(pstHeader))
            {
                FreeHeaderEx(pstHeader);
                return IMS_FALSE;
            }

            (void) FormHeaderBody(pstHeader, IMS_TRUE, objStringBuffer);

            FreeHeaderEx(pstHeader);
        }

        objStringBuffer += TextParser::CHAR_CR;
        objStringBuffer += TextParser::CHAR_LF;

        return IMS_TRUE;
    }

#ifdef SIP_MIME_PARSING
    LOCAL IMS_BOOL FormMIMEHeader(IN SipMimeHeader *pstMIMEHeader,
            OUT AStringBuffer &objStringBuffer)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMIMEHeader != IMS_NULL)
        {
            // Content-Type header
            if (pstMIMEHeader->pContentType != IMS_NULL)
            {
                objStringBuffer += SIPHeaderName::CONTENT_TYPE;
                objStringBuffer += TextParser::CHAR_COLON;
                objStringBuffer += TextParser::CHAR_SP;

                if (pstMIMEHeader->pContentType->pStr1 != IMS_NULL)
                {
                    objStringBuffer += pstMIMEHeader->pContentType->pStr1;
                }

                FormParameterList(&(pstMIMEHeader->pContentType->slParam),
                        AString(&TextParser::CHAR_SEMICOLON, 1), 1, objStringBuffer);

                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            // Content-ID header
            if (pstMIMEHeader->pContentId != IMS_NULL)
            {
                objStringBuffer += SIPHeaderName::CONTENT_ID;
                objStringBuffer += TextParser::CHAR_COLON;
                objStringBuffer += TextParser::CHAR_SP;

                objStringBuffer += pstMIMEHeader->pContentId;

                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            // Content-Description header
            if (pstMIMEHeader->pContentDescription != IMS_NULL)
            {
                objStringBuffer += SIPHeaderName::CONTENT_DESCRIPTION;
                objStringBuffer += TextParser::CHAR_COLON;
                objStringBuffer += TextParser::CHAR_SP;

                objStringBuffer += pstMIMEHeader->pContentDescription;

                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            // Content-Disposition header
            if (pstMIMEHeader->pContentDisposition != IMS_NULL)
            {
                objStringBuffer += SIPHeaderName::CONTENT_DISPOSITION;
                objStringBuffer += TextParser::CHAR_COLON;
                objStringBuffer += TextParser::CHAR_SP;

                if (pstMIMEHeader->pContentDisposition->pStr1 != IMS_NULL)
                {
                    objStringBuffer += pstMIMEHeader->pContentDisposition->pStr1;
                }

                FormParameterList(&(pstMIMEHeader->pContentDisposition->slParam),
                        AString(&TextParser::CHAR_SEMICOLON, 1), 1, objStringBuffer);

                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            // Content-Transfer-Encoding header
            if (pstMIMEHeader->pContentTransEncoding != IMS_NULL)
            {
                objStringBuffer += SIPHeaderName::CONTENT_TRANSFER_ENCODING;
                objStringBuffer += TextParser::CHAR_COLON;
                objStringBuffer += TextParser::CHAR_SP;

                objStringBuffer += pstMIMEHeader->pContentTransEncoding;

                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            // Unknonw MIME headers
            IMS_UINT32 nHeaderCount = 0;

            List_SizeOf(&(pstMIMEHeader->slAdditionalMimeHeaders), &nHeaderCount, SIPStackError());

            for (IMS_UINT32 i = 0; i < nHeaderCount; ++i)
            {
                IMS_CHAR *pszUnknownHeader = IMS_NULL;

                List_GetAt(&(pstMIMEHeader->slAdditionalMimeHeaders),
                        i, &pszUnknownHeader, SIPStackError());

                if (pszUnknownHeader != IMS_NULL)
                {
                    objStringBuffer += pszUnknownHeader;
                    objStringBuffer += TextParser::CHAR_CR;
                    objStringBuffer += TextParser::CHAR_LF;
                }
            }
        }

        return IMS_TRUE;
    }
#endif // SIP_MIME_PARSING

    LOCAL IMS_BOOL FormMessageBody(IN SipMsgBody *pstMsgBody, IN IMS_BOOL bMIMEHeader,
            OUT AStringBuffer &objStringBuffer)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody->dType != SipUnknownBody)
        {
            return IMS_FALSE;
        }

#ifdef SIP_MIME_PARSING
        if (bMIMEHeader && (pstMsgBody->pMimeHeader != IMS_NULL))
        {
            if (!FormMIMEHeader(pstMsgBody->pMimeHeader, objStringBuffer))
            {
                return IMS_FALSE;
            }

            objStringBuffer += TextParser::CHAR_CR;
            objStringBuffer += TextParser::CHAR_LF;
        }
#endif // SIP_MIME_PARSING

        AString strBuffer(pstMsgBody->u.pUnknownMessage->pBuffer,
                pstMsgBody->u.pUnknownMessage->dLength);

        objStringBuffer += strBuffer;

        return IMS_TRUE;
    }

    LOCAL IMS_BOOL FormMessageBodys(IN SipMessage *pstMessage, IN SipHeader *pstContentType,
            OUT AStringBuffer &objStringBuffer)
    {
        //-----------------------------------------------------------------------------------------

        if (!IsValidHeader(pstContentType))
        {
            return IMS_TRUE;
        }

        IMS_UINT32 nBodyCount = 0;

        List_SizeOf(&(pstMessage->slMessageBody), &nBodyCount, SIPStackError());

        if (nBodyCount == 0)
        {
            return IMS_TRUE;
        }

        IMS_CHAR *pszType = IMS_NULL;

        (void) sip_getMediaTypeFromContentTypeHdr(pstContentType, &pszType, SIPStackError());

        if (pszType == IMS_NULL)
        {
            return IMS_FALSE;
        }

        AString strSeparator;

#ifdef SIP_MIME_PARSING
        // Check if the media type is multipart MIME type and find a separator ...
        if (IMS_StrNICmp(pszType, SIP::STR_MULTIPART, 9) == 0)
        {
            // Find a boundary parameter from Content-Type header
            AString strBoundaryValue = GetParameter(pstContentType, SIP::STR_BOUNDARY);

            // if DQUOT is present, strip it from the original string
            strBoundaryValue = TextParser::TrimDQUOT(strBoundaryValue);

            strSeparator.Append("--");
            strSeparator.Append(strBoundaryValue);

            // Check if the multipart message has been read into the unknown body.
            // There will be one message body in the list and its type will be SipUnknownBody.
            // Check if the body begins with the boundary.
            if (nBodyCount == 1)
            {
                SipMsgBody *pstMsgBody = IMS_NULL;

                (void) List_GetAt(&(pstMessage->slMessageBody), 0, &pstMsgBody, SIPStackError());

                if (pstMsgBody->dType == SipUnknownBody)
                {
                    if (IMS_StrNCmp(pstMsgBody->u.pUnknownMessage->pBuffer,
                            strSeparator.GetStr(), strSeparator.GetLength()) == 0)
                    {
                        strSeparator = AString::ConstNull();
                    }
                }
            }
        }
#endif // SIP_MIME_PARSING

        for (IMS_UINT32 i = 0; i < nBodyCount; ++i)
        {
            if (!strSeparator.IsNULL())
            {
                if (i != 0)
                {
                    objStringBuffer += TextParser::CHAR_CR;
                    objStringBuffer += TextParser::CHAR_LF;
                }

                objStringBuffer += strSeparator;
                objStringBuffer += TextParser::CHAR_CR;
                objStringBuffer += TextParser::CHAR_LF;
            }

            SipMsgBody *pstMsgBody = IMS_NULL;

            (void) List_GetAt(&(pstMessage->slMessageBody), i, &pstMsgBody, SIPStackError());

            if (pstMsgBody != IMS_NULL)
            {
#ifdef SIP_MIME_PARSING
                if (pstMsgBody->pMimeHeader == IMS_NULL)
#endif // SIP_MIME_PARSING
                {
                    if (!strSeparator.IsNULL())
                    {
                        objStringBuffer += TextParser::CHAR_CR;
                        objStringBuffer += TextParser::CHAR_LF;
                    }
                }

                if (!FormMessageBody(pstMsgBody, (nBodyCount > 1), objStringBuffer))
                {
                    return IMS_FALSE;
                }
            }
        }

        if (!strSeparator.IsNULL())
        {
            objStringBuffer += TextParser::CHAR_CR;
            objStringBuffer += TextParser::CHAR_LF;

            objStringBuffer += strSeparator;
            objStringBuffer += "--";

            objStringBuffer += TextParser::CHAR_CR;
            objStringBuffer += TextParser::CHAR_LF;
        }

        return IMS_TRUE;
    }

#if 0
    /*

    Remarks

    */
    LOCAL IMS_BOOL ExtractMIMEHeaders(IN CONST SipMsgBody *pstMsgBody,
            IN_OUT SIPMessageBodyPart *pBodyPart)
    {
#ifdef SIP_MIME_PARSING
        SipHeader        stHeader = { SipHdrTypeAny, IMS_NULL };
        SipHeader        *pstHeader;
        SipMimeHeader    *pstMIMEHeader = pstMsgBody->pMimeHeader;

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstMIMEHeader == IMS_NULL)
        {
            return IMS_TRUE;
        }

        pstHeader = &stHeader;

        // Getting Content-Type header
        if (sip_bcpt_getContentTypeFromMimeHdr(pstMIMEHeader,
                &pstHeader, SIPStackError()) == SipSuccess)
        {
            pBodyPart->SetHeader(pstHeader);
            FreeHeader(pstHeader);
        }

        // Getting Content-Disposition header
        if (sip_bcpt_getContentDispositionFromMimeHdr(pstMIMEHeader,
                &pstHeader, SIPStackError()) == SipSuccess)
        {
            pBodyPart->SetHeader(pstHeader);
            FreeHeader(pstHeader);
        }

        // Set Content-Transfer-Encoding header
        if (pstMIMEHeader->pContentTransEncoding != IMS_NULL)
        {
            AString strValue(pstMIMEHeader->pContentTransEncoding);

            pBodyPart->SetHeader(SIPMessageBodyPart::CONTENT_TRANSFER_ENCODING, strValue);
        }

        // Set Content-ID header
        if (pstMIMEHeader->pContentId != IMS_NULL)
        {
            AString strValue(pstMIMEHeader->pContentId);

            pBodyPart->SetHeader(SIPMessageBodyPart::CONTENT_ID, strValue);
        }

        // Set Content-Description header
        if (pstMIMEHeader->pContentDescription != IMS_NULL)
        {
            AString strValue(pstMIMEHeader->pContentDescription);

            pBodyPart->SetHeader(SIPMessageBodyPart::CONTENT_DESCRIPTION, strValue);
        }

        // TODO:: Additional MIME header processing

        return IMS_TRUE;
#else
        SIPStackError(E_NO_ERROR);
        return IMS_TRUE;
#endif // SIP_MIME_PARSING
    }

    /*

    Remarks

    */
    LOCAL IMS_BOOL ExtractAdditionalBodys(IN CONST SipMsgBody *pstMsgBody,
            IN IMSList<SIPMessageBodyPart*> &objMessageBodys)
    {
        (void) pstMsgBody;
        (void) objMessageBodys;

        return IMS_TRUE;
    }
#endif

    /*

    Remarks

    */
    LOCAL IMS_BOOL GetParameter(IN SipHeader *pstHeader, IN CONST AString &strName,
            OUT SipParam *&pstParam)
    {
        SipParam *pstTmpParam;
        SipCommonHeader *pstSCHdr = static_cast<SipCommonHeader*>(pstHeader->pHeader);

        IMS_UINT32 nParamCount = 0;

        //-----------------------------------------------------------------------------------------

        List_SizeOf(&(pstSCHdr->slParam), &nParamCount, SIPStackError());

        if (nParamCount == 0)
        {
            SIPStackError(E_NO_EXIST);
            return IMS_FALSE;
        }

        for (IMS_UINT32 i = 0; i < nParamCount; ++i)
        {
            pstTmpParam = IMS_NULL;

            if (List_GetAt(&(pstSCHdr->slParam), i, &pstTmpParam, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            if (strName.EqualsIgnoreCase(pstTmpParam->pName))
            {
                pstParam = pstTmpParam;
                AddReference(pstParam);
                return IMS_TRUE;
            }
        }

        SIPStackError(E_NO_EXIST);

        return IMS_FALSE;
    }

#ifdef SIP_MIME_PARSING
    /*

    Remarks

    */
    LOCAL IMS_BOOL FindBoundary(IN CONST AString &strBoundary, IN IMS_CHAR *pEnd,
            IN_OUT IMS_CHAR *&pCurrentPos)
    {
        // Iterate through the buffer looking for "--" followed by boundary value.
        // Anything before the boundary value is preamble text; ignore it.
        IMS_BOOL bBoundaryValueFound = IMS_FALSE;

        //-----------------------------------------------------------------------------------------

        while ((pCurrentPos + strBoundary.GetLength() + 1) <= pEnd)
        {
            if ((*pCurrentPos == TextParser::CHAR_HYPHEN)
                    && (*(pCurrentPos + 1) == TextParser::CHAR_HYPHEN))
            {
                IMS_CHAR *pTmpVal = pCurrentPos + 2;

                bBoundaryValueFound = IMS_TRUE;

                for (IMS_SINT32 i = 0; i < strBoundary.GetLength(); ++i)
                {
                    if (*pTmpVal != strBoundary[i])
                    {
                        bBoundaryValueFound = IMS_FALSE;
                        break;
                    }

                    pTmpVal++;
                }

                if (bBoundaryValueFound)
                {
                    break;
                }
                else
                {
                    pCurrentPos++;
                }
            }
            else
            {
                pCurrentPos++;
            }
        }

        return bBoundaryValueFound;
    }

    /*

    Remarks
     WE ASSUME THAT THE INCREMENTAL PARSING IS NOT SUPPORTED ...

    */
    LOCAL IMS_BOOL ParseMIMEBody(IN IMS_CHAR *pStart, IN IMS_CHAR *pEnd,
            IN SipOptions *pstOptions, IN_OUT SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        SIPStackError(E_PARSER_ERROR);

        if (pStart >= pEnd)
        {
            return IMS_FALSE;
        }

        // Retrieve the boundary parameter value contained in the Content-Type header
        SipHeader *pstContentType = GetHeader(pstMessage, SipHdrTypeContentType);

        if (!IsValidHeader(pstContentType))
        {
            FreeHeaderEx(pstContentType);
            return IMS_FALSE;
        }

        IMS_CHAR *pszTmpBoundary = IMS_NULL;

        (void) sip_getParamForNameFromCommonHdr(
                pstContentType, (SIP_S8bit*)SIP::STR_BOUNDARY, &pszTmpBoundary, SIPStackError());

        if (pszTmpBoundary == IMS_NULL)
        {
            FreeHeaderEx(pstContentType);

            // Boundary parameter value is necessary for parsing MIME body. Raise parser error.
            IMS_TRACE_E(0, "Can't retrieve boundary parameter value", 0, 0, 0);
            return IMS_FALSE;
        }

        IMS_UINT32 nBoundarySize = 0;

        if (pszTmpBoundary[0] == TextParser::CHAR_DQUOT)
        {
            // except for DQUOT (2)
            nBoundarySize = SIP_STRLEN(pszTmpBoundary) - 2;
            pszTmpBoundary++;
        }
        else
        {
            nBoundarySize = SIP_STRLEN(pszTmpBoundary);
        }

        IMS_CHAR *pCurrentPos = pStart;
        AString strBoundary(pszTmpBoundary, nBoundarySize);

        FreeHeaderEx(pstContentType);

        if (!FindBoundary(strBoundary, pEnd, pCurrentPos))
        {
            IMS_TRACE_E(0, "Error in parsing MIME body: no boundary value found", 0, 0, 0);
            return IMS_FALSE;
        }

        // pCurrentPos will now be pointing to the last character of the dash-boundary.
        IMS_CHAR *pToken = IMS_NULL;
        IMS_CHAR *pStartOfBody = IMS_NULL;
        IMS_CHAR *pEndOfBody = IMS_NULL;

        SipMimeHeader *pstMIMEHeader = IMS_NULL;

        while (IMS_TRUE)
        {
            pToken = sip_tokenize(pCurrentPos, pEnd, '\r');

            if (pToken == IMS_NULL)
            {
                IMS_TRACE_E(0, "Error in parsing MIME body: No CRLF at the end of boundary line",
                        0, 0, 0);
                return IMS_FALSE;
            }

            if (((pToken + 1) > pEnd) || (*(pToken + 1) != '\n'))
            {
                IMS_TRACE_E(0, "Error in parsing MIME body: No CRLF at the end of boundary line",
                        0, 0, 0);
                return IMS_FALSE;
            }

            // pCurrentPos was pointing to the last character of dash-boundary;
            // Now update it to point to first character after boundary line.
            pCurrentPos = pToken + 2;

            if (pCurrentPos > pEnd)
            {
                IMS_TRACE_E(0, "Error in parsing MIME body: Incomplete MIME body", 0, 0, 0);
                return IMS_FALSE;
            }

            if ((*pCurrentPos) != '\r')
            {
                IMS_CHAR *pStartOfEntityBody = IMS_NULL;

                // Body headers are present ...
                if (sip_initSipMimeHeader(&pstMIMEHeader, SIPStackError()) == SipFail)
                {
                    return IMS_FALSE;
                }

                // Parse the MIME header from the MIME body ...
                if (sip_parseSipBuffer(pCurrentPos, &pstMessage, pstOptions,
                        (pEnd - pCurrentPos + 1), &pStartOfEntityBody, IMS_NULL,
                        pstMIMEHeader, SIPStackError()) == SipFail)
                {
                    IMS_TRACE_E(0, "Error (%d) in parsing MIME headers", GetLastError(), 0, 0);
                    goto EXIT_ParseMIMEBody;
                }

                // The above function will return the start position of the body part
                // of the entity.
                // Updates pCurrentPos to point to this.
                pCurrentPos = pStartOfEntityBody;

                // Retrieve the Content-Type if any parsed for the body-part to
                // which these headers pertain.
                // -> All bodies will be handled as an unknown body parts ...
            }
            else
            {
                // No body headers; Move to the beginning of the body part.
                pCurrentPos += 2;
            }

            // pCurrentPos now points to the start of the body part.
            pStartOfBody = pCurrentPos;

            if (!FindBoundary(strBoundary, pEnd, pCurrentPos))
            {
                IMS_TRACE_E(0, "Incomplete MIME body: no close delimiter", 0, 0, 0);
                goto EXIT_ParseMIMEBody;
            }

            if ((*(pCurrentPos - 1) != '\n')
                    || (*(pCurrentPos - 2) != '\r')
                    || ((pCurrentPos + nBoundarySize + 3) > pEnd))
            {
                IMS_TRACE_E(0, "Error in parsing MIME body: boundary delimiter does not occur "
                        "at the beginning of a line, or incomplete delimiter line", 0, 0, 0);
                goto EXIT_ParseMIMEBody;
            }

            pCurrentPos += (nBoundarySize + 1);

            // The CRLF at the beginning of the delimiter line is not conceptually a part
            // of the body part itself (refer to RFC 2046), so calculate body size accordingly.
            pEndOfBody = pCurrentPos - (nBoundarySize + 4);

            // Check if the current position is an end of MIME body or not
            if ((*(pCurrentPos + 1) == '-') && (*(pCurrentPos + 2) == '-'))
            {
                // Close-Delimiter found; This is the last body part.
                if (sip_parseSingleMimeEntity(0, SipUnknownBody, pStartOfBody, pEndOfBody,
                        pstMessage, IMS_NULL, IMS_NULL, pstOptions,
                        pstMIMEHeader, SIPStackError()) == SipFail)
                {
                    IMS_TRACE_E(0, "Error in parsing MIME body: setting a body part failed",
                            0, 0, 0);
                    goto EXIT_ParseMIMEBody;
                }

                // Reset the MIME header
                pstMIMEHeader = IMS_NULL;
                break;
            }
            else
            {
                // Not a Close-Delimiter; Another body part present; repeat the above processing

                if (sip_parseSingleMimeEntity(0, SipUnknownBody, pStartOfBody, pEndOfBody,
                        pstMessage, IMS_NULL, IMS_NULL, pstOptions,
                        pstMIMEHeader, SIPStackError()) == SipFail)
                {
                    IMS_TRACE_E(0, "Error in parsing MIME body: setting a body part failed",
                            0, 0, 0);
                    goto EXIT_ParseMIMEBody;
                }

                pCurrentPos--;

                // Reset the MIME header
                pstMIMEHeader = IMS_NULL;
            }
        }

        SIPStackError(E_NO_ERROR);

        return IMS_TRUE;

    EXIT_ParseMIMEBody:

        sip_bcpt_freeSipMimeHeader(pstMIMEHeader);
        return IMS_FALSE;
    }
#endif // SIP_MIME_PARSING

    /*

    Remarks

    */
    GLOBAL void Initialize()
    {
        IMS_SINT32 nSlotId = SystemConfigManager::GetInstance()->GetActiveSlotId();
        const ISipConfigV *piSipConfigV = SIPConfigProxy::GetSipConfigV(nSlotId);

        //-----------------------------------------------------------------------------------------

        // For transaction layer handling
        // Initialize the retransmission initial timer values, timeout timer values,
        // and wait timer values.
        // Below timer values are based on 3GPP; Consider IETF timer values, later.
        glbTimerC = (180000);
        glbTimerCr = (180000);

        if (piSipConfigV == IMS_NULL)
        {
            glbT1 = SIPConfigProxy::GetTimerValueT1(nSlotId);
            glbT2 = SIPConfigProxy::GetTimerValueT2(nSlotId);
            glbTimerB = (64 * glbT1);
            glbTimerD_T3 = (64 * glbT1);
            glbTimerF_T3 = (64 * glbT1);
            glbTimerH = (64 * glbT1);
            glbTimerI_T4 = (glbT2 + 1000);
            glbTimerJ_T3 = (64 * glbT1);
            glbTimerK_T4 = (glbT2 + 1000);
        }
        else
        {
            SetTransactionTimerValues(IMS_NULL, piSipConfigV);
        }

        // For non-transaction layer handling
        // Used in the function, sip_sendMessage().
        // Keeps the number of maximum retransmission on the value given by SIP stack.
        SIP_T1 = glbT1;
        SIP_T2 = glbT2;

        // Initialize the transaction aggregation
        //objTxnAggregate.InitHashTable(SIPPrivate::MAX_TRANSACTION);
        //objTxnAggregate.SetListener(this);

        // Register SIP transaction layer's callback functions
        SIPStackTxnLayer_Initialize();
    }

    /*

    Remarks

    */
    GLOBAL void SetTransactionTimerValues(
            IN CONST SIPProfile *pSIPProfile, IN CONST ISipConfigV *piSipConfigV)
    {
        // For transaction layer handling
        // Initialize the retransmission initial timer values, timeout timer values,
        // and wait timer values.
        // Below timer values are based on 3GPP; Consider IETF timer values, later.
        glbT1 = SIPConfigProxy::GetTimerValueT1(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbT2 = SIPConfigProxy::GetTimerValueT2(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerB = SIPConfigProxy::GetTimerValueTB(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerD_T3 = SIPConfigProxy::GetTimerValueTD(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerF_T3 = SIPConfigProxy::GetTimerValueTF(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerH = SIPConfigProxy::GetTimerValueTH(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerI_T4 = SIPConfigProxy::GetTimerValueTI(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerJ_T3 = SIPConfigProxy::GetTimerValueTJ(IMS_SLOT_0, pSIPProfile, piSipConfigV);
        glbTimerK_T4 = SIPConfigProxy::GetTimerValueTK(IMS_SLOT_0, pSIPProfile, piSipConfigV);

        // For non-transaction layer handling
        // Used in the function, sip_sendMessage().
        // Keeps the number of maximum retransmission on the value given by SIP stack.
        SIP_T1 = glbT1;
        SIP_T2 = glbT2;
    }

    /*

    Remarks

    */
    GLOBAL SipError GetLastError()
    {
        //-----------------------------------------------------------------------------------------

        return genError;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL AppendHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (IsSingleHeader(pstHeader->dType))
        {
            if (sip_setHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }
        else
        {
            IMS_SINT32 nHCount = GetHeaderCount(pstMessage, pstHeader->dType);

            if (sip_insertHeaderAtIndex(pstMessage,
                    pstHeader, nHCount, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL AppendMessageBody(IN SipMsgBody *pstMsgBody, IN_OUT SipMessage *&pstMessage)
    {
        IMS_UINT32 nCount = 0;

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        (void) sip_getMsgBodyCount(pstMessage, &nCount, SIPStackError());

        if (sip_insertMsgBodyAtIndex(pstMessage, pstMsgBody, nCount, SIPStackError()) == SipFail)
        {
            IMS_TRACE_E(0, "Inserting a message body failed - error=%d", GetLastError(), 0, 0);
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL AppendUnknownHeader(IN CONST AString &strName, IN CONST AString &strValue,
            IN_OUT SipMessage *&pstMessage)
    {
        SipHeader *pstHeader = DecodeHeader(SipHdrTypeUnknown, strName, strValue);

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (!AppendHeader(pstHeader, pstMessage))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        FreeHeaderEx(pstHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL PrependHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (IsSingleHeader(pstHeader->dType))
        {
            if (sip_setHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }
        else
        {
            if (sip_insertHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL PrependUnknownHeader(IN CONST AString &strName, IN CONST AString &strValue,
            IN_OUT SipMessage *&pstMessage)
    {
        SipHeader *pstHeader = DecodeHeader(SipHdrTypeUnknown, strName, strValue);

        //---------------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (!PrependHeader(pstHeader, pstMessage))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        FreeHeaderEx(pstHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CheckMandatoryHeaders(IN CONST SipMessage *pstMessage)
    {
        IMS_UINT32 nCount = 0;

        //---------------------------------------------------------------------------------------------

        // If the message is a request, then check if it is PRACK.
#ifdef SIP_RPR
        if (pstMessage->dType == SipMessageRequest)
        {
            if ((AString::Compare(pstMessage->u.pRequest->pRequestLine->pStr1,
                    SIPMethod::ToName(SIPMethod::PRACK)) == 0)
                    && (pstMessage->u.pRequest->pRequestHdr->pRackHdr == SIP_NULL))
            {
                IMS_TRACE_E(0, "RAck header field is missing in PRACK request message", 0, 0, 0);
                return IMS_FALSE;
            }
        }
#endif

        /*
         * Check for the following mandatory headers :
         *    From / To / Call-ID / CSeq / Via
         * We don't check for Max-Forwards even though it is mandated by RFC 3261
         * in order to parse some of the tortue test messages.
         */

        (void) List_SizeOf(&(pstMessage->pGeneralHdr->slViaHdr), &nCount, SIPStackError());

        if ((pstMessage->pGeneralHdr->pCallidHdr == SIP_NULL)
                || (pstMessage->pGeneralHdr->pCseqHdr == SIP_NULL)
                || (pstMessage->pGeneralHdr->pFromHeader == SIP_NULL)
                || (pstMessage->pGeneralHdr->pToHdr == SIP_NULL)
                || (nCount == 0))
        {
            IMS_TRACE_E(0, "Mandatory header field is missing", 0, 0, 0);
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CloneHeader(IN SipHeader *pstHeader)
    {
        SipHeader *pstNewHeader = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        pstNewHeader = CreateHeader(pstHeader->dType);

        if (pstNewHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        if (sip_cloneSipHeader(pstNewHeader, pstHeader, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstNewHeader);
            return IMS_NULL;
        }

        return pstNewHeader;
    }

    /*

    Remarks

    */
    GLOBAL SipMessage* CloneMessage(IN SipMessage *pstMessage)
    {
        SipMessage *pstNewMessage = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        if (sip_initSipMessage(&pstNewMessage, pstMessage->dType, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if (sip_cloneSipMessage(pstNewMessage, pstMessage, SIPStackError()) == SipFail)
        {
            FreeMessage(pstNewMessage);
            return IMS_NULL;
        }

        return pstNewMessage;
    }

    /*

    Remarks

    */
    GLOBAL SipMsgBody* CloneMessageBody(IN SipMsgBody *pstMsgBody)
    {
        SipMsgBody *pstNewMsgBody = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        if (sip_initSipMsgBody(&pstNewMsgBody, pstMsgBody->dType, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if (sip_cloneSipMsgBody(pstNewMsgBody, pstMsgBody, SIPStackError()) == SipFail)
        {
            FreeMessageBody(pstNewMsgBody);
            return IMS_NULL;
        }

        return pstNewMsgBody;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CopyHeader(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        SipHeader *pstNewHeader = CreateHeader(pstHeader->dType);

        if (pstNewHeader == IMS_NULL)
            return IMS_NULL;

        pstNewHeader->pHeader = pstHeader->pHeader;

        SipCommonHeader *pstCommonHdr = static_cast<SipCommonHeader*>(pstNewHeader->pHeader);

        if (pstCommonHdr != IMS_NULL)
        {
            AddReference(pstCommonHdr);
        }
        else
        {
            FreeHeaderEx(pstNewHeader);
            return IMS_NULL;
        }

        return pstNewHeader;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CopyHeader(IN SipHeader *pstDest, IN SipHeader *pstSrc)
    {
        //-----------------------------------------------------------------------------------------

        if (pstSrc == IMS_NULL)
        {
            return pstDest;
        }

        if (pstDest == IMS_NULL)
        {
            return CopyHeader(pstSrc);
        }

        pstDest->dType = pstSrc->dType;
        pstDest->pHeader = pstSrc->pHeader;

        SipCommonHeader *pstCommonHdr = static_cast<SipCommonHeader*>(pstDest->pHeader);

        if (pstCommonHdr != IMS_NULL)
        {
            AddReference(pstCommonHdr);
        }

        return pstDest;
    }

    /*

    Remarks

    */
    GLOBAL void CorrectCSeq(IN CONST AString &strMethod, IN_OUT SipMessage *&pstMessage)
    {
        IMS_CHAR *pszMethod = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        // Correct the CSeq's method if it's different from the Request-URI method.
        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeCseq);

        if (sip_getMethodFromCseqHdr(pstHeader, &pszMethod, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return;
        }

        if (!strMethod.Equals(pszMethod))
        {
            pszMethod = strMethod.Duplicate();

            if (sip_setMethodInCseqHdr(pstHeader, pszMethod, SIPStackError()) == SipFail)
            {
                Free(pszMethod);
                FreeHeaderEx(pstHeader);
                return;
            }
        }

        FreeHeaderEx(pstHeader);
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CorrectMessageBody(IN_OUT SipMessage *&pstMessage)
    {
        SipHeader stHeader;

        IMS_UINT32 nMBCount = 0;

        //-----------------------------------------------------------------------------------------

        (void) sip_getMsgBodyCount(pstMessage, &nMBCount, SIPStackError());

        if (nMBCount == 0)
        {
            IMS_TRACE_D("No message body", 0, 0, 0);
            return IMS_FALSE;
        }

        if (nMBCount == 1)
        {
            // If the Content-Type header does not exist,
            // then insert the Content-Type header into SIP message.
            SipMsgBody *pstMsgBody = IMS_NULL;

            (void) List_GetAt(&(pstMessage->slMessageBody), 0, &pstMsgBody, SIPStackError());

            if (sip_getHeader(pstMessage,
                    SipHdrTypeContentType, &stHeader, SIPStackError()) == SipFail)
            {
                if (GetLastError() == E_NO_EXIST)
                {
#ifdef SIP_MIME_PARSING
                    if ((pstMsgBody != IMS_NULL) && (pstMsgBody->pMimeHeader != IMS_NULL))
                    {
                        if (pstMsgBody->pMimeHeader->pContentType != IMS_NULL)
                        {
                            stHeader.dType = SipHdrTypeContentType;
                            stHeader.pHeader = pstMsgBody->pMimeHeader->pContentType;

                            if (sip_setHeader(pstMessage, &stHeader, SIPStackError()) == SipFail)
                            {
                                return IMS_FALSE;
                            }
                        }
                        else
                        {
                            return IMS_FALSE;
                        }

                        // Content-Disposition header
                        if (pstMsgBody->pMimeHeader->pContentDisposition != IMS_NULL)
                        {
                            stHeader.dType = SipHdrTypeContentDisposition;
                            stHeader.pHeader = pstMsgBody->pMimeHeader->pContentDisposition;

                            if (!AppendHeader(&stHeader, pstMessage))
                            {
                                return IMS_FALSE;
                            }
                        }

                        // Content-Transfer-Encoding header
                        if (pstMsgBody->pMimeHeader->pContentTransEncoding != IMS_NULL)
                        {
                            SipHeader *pstHeader = DecodeHeader(SipHdrTypeUnknown,
                                    SIPHeaderName::CONTENT_TRANSFER_ENCODING,
                                    pstMsgBody->pMimeHeader->pContentTransEncoding);

                            if (!AppendHeader(pstHeader, pstMessage))
                            {
                                FreeHeaderEx(pstHeader);
                                return IMS_FALSE;
                            }

                            FreeHeaderEx(pstHeader);
                        }

                        // Content-ID header
                        if (pstMsgBody->pMimeHeader->pContentId != IMS_NULL)
                        {
                            SipHeader *pstHeader = DecodeHeader(SipHdrTypeUnknown,
                                    SIPHeaderName::CONTENT_ID,
                                    pstMsgBody->pMimeHeader->pContentId);

                            if (!AppendHeader(pstHeader, pstMessage))
                            {
                                FreeHeaderEx(pstHeader);
                                return IMS_FALSE;
                            }

                            FreeHeaderEx(pstHeader);
                        }

                        // Content-Description header
                        if (pstMsgBody->pMimeHeader->pContentDescription != IMS_NULL)
                        {
                            SipHeader *pstHeader = DecodeHeader(SipHdrTypeUnknown,
                                    SIPHeaderName::CONTENT_DESCRIPTION,
                                    pstMsgBody->pMimeHeader->pContentDescription);

                            if (!AppendHeader(pstHeader, pstMessage))
                            {
                                FreeHeaderEx(pstHeader);
                                return IMS_FALSE;
                            }

                            FreeHeaderEx(pstHeader);
                        }

                        // Additional MIME headers
                        IMS_UINT32 nCount = 0;

                        (void) List_SizeOf(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                                &nCount, SIPStackError());

                        if (nCount > 0)
                        {
                            AString strHeader;
                            AString strHName;
                            AString strHBody;

                            for (IMS_UINT32 i = 0; i < nCount; ++i)
                            {
                                IMS_CHAR *pszHeader = IMS_NULL;

                                List_GetAt(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                                        i, &pszHeader, SIPStackError());

                                strHeader = pszHeader;

                                IMS_SINT32 nIndex = strHeader.GetIndexOf(TextParser::CHAR_COLON);

                                if (nIndex == AString::NPOS)
                                {
                                    continue;
                                }

                                strHName = strHeader.GetSubStr(0, nIndex).Trim();
                                strHBody = strHeader.GetSubStr(nIndex + 1).Trim();

                                IMS_SINT32 nHType = SipHdrTypeUnknown;

                                if (strHName.EqualsIgnoreCase(SIPHeaderName::CONTENT_LENGTH)
                                        || strHName.Equals(SIPHeaderName::CF_CONTENT_LENGTH))
                                {
                                    nHType = SipHdrTypeContentLength;
                                }
                                else if (strHName.EqualsIgnoreCase(SIPHeaderName::CONTENT_ENCODING)
                                        || strHName.Equals(SIPHeaderName::CF_CONTENT_ENCODING))
                                {
                                    nHType = SipHdrTypeContentEncoding;
                                }

                                SipHeader *pstHeader = DecodeHeader(nHType, strHName, strHBody);

                                if (!AppendHeader(pstHeader, pstMessage))
                                {
                                    FreeHeaderEx(pstHeader);
                                    return IMS_FALSE;
                                }

                                FreeHeaderEx(pstHeader);
                            }
                        }
                    }
#endif // SIP_MIME_PARSING
                }
            }
            else
            {
                FreeHeader(&stHeader);
            }

            // Remove the Content MIME headers
            if ((pstMsgBody != IMS_NULL) && (pstMsgBody->pMimeHeader != IMS_NULL))
            {
                sip_bcpt_freeSipMimeHeader(pstMsgBody->pMimeHeader);
                pstMsgBody->pMimeHeader = IMS_NULL;
            }
        }
        else
        {
            // Sets the boundary parameter if not present
            SipHeader stHeader;

            if (sip_getHeader(pstMessage,
                    SipHdrTypeContentType, &stHeader, SIPStackError()) == SipFail)
            {
                // In case of the multipart MIME body,
                // "multipart/mixed" media type will be set as a default.
                if (GetLastError() == E_NO_EXIST)
                {
                    SipHeader *pstHeader
                            = DecodeHeader(SipHdrTypeContentType, SIP::STR_MULTIPART_MIXED);

                    if (pstHeader == IMS_NULL)
                    {
                        return IMS_FALSE;
                    }

                    // Insert a boundary parameter
                    AString strBoundary = SIPUtil::GenerateBoundary();

                    if (!SetParameter(pstHeader, SIP::STR_BOUNDARY, strBoundary))
                    {
                        FreeHeaderEx(pstHeader);
                        return IMS_FALSE;
                    }

                    if (sip_setHeader(pstMessage, pstHeader, SIPStackError()) == SipFail)
                    {
                        FreeHeaderEx(pstHeader);
                        return IMS_FALSE;
                    }

                    FreeHeaderEx(pstHeader);
                    return IMS_TRUE;
                }

                return IMS_FALSE;
            }

            SipParam *pstParam = IMS_NULL;
            AString strName(SIP::STR_BOUNDARY);

            if (!GetParameter(&stHeader, strName, pstParam))
            {
                if (GetLastError() == E_NO_EXIST)
                {
                    // Insert a boundary parameter
                    AString strBoundary = SIPUtil::GenerateBoundary();

                    if (!SetParameter(&stHeader, strName, strBoundary))
                    {
                        FreeHeader(&stHeader);
                        return IMS_FALSE;
                    }
                }
            }
            else
            {
                sip_freeSipParam(pstParam);
            }

            FreeHeader(&stHeader);
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CreateHeader(IN IMS_SINT32 nType)
    {
        SipHeader *pstHeader = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (sip_initSipHeader(&pstHeader,
                static_cast<en_HeaderType>(nType), SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if ((nType == SipHdrTypeAny)
            && (pstHeader != IMS_NULL))
        {
            // NULL initialization
            pstHeader->pHeader = IMS_NULL;
        }

        return pstHeader;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CreateHeader(IN IMS_SINT32 nType, IN SipAddrSpec *pstAddrSpec)
    {
        SipHeader *pstNewHeader = CreateHeader(nType);

        //-----------------------------------------------------------------------------------------

        if (pstNewHeader == IMS_NULL)
        {
            return IMS_NULL;
        }

        if (sip_setAddrSpecInCommonHdr(pstNewHeader, pstAddrSpec, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstNewHeader);
            return IMS_NULL;
        }

        return pstNewHeader;
    }

    /*

    Remarks

    */
    GLOBAL SipMessage* CreateMessage(IN IMS_SINT32 nType)
    {
        SipMessage *pstMessage;

        //-----------------------------------------------------------------------------------------

        if (sip_initSipMessage(&pstMessage,
                static_cast<en_SipMessageType>(nType), SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if (pstMessage->dType == SipMessageRequest)
        {
            SipReqLine *pstReqLine;
            IMS_CHAR *pszVersion;

            if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
            {
                FreeMessage(pstMessage);
                return IMS_NULL;
            }

            pszVersion = sip_strdup(SIP::STR_SIP_VERSION, 0);

            if (sip_setVersionInReqLine(pstReqLine, pszVersion, SIPStackError()) == SipFail)
            {
                Free(pszVersion);
                sip_freeSipReqLine(pstReqLine);
                FreeMessage(pstMessage);
                return IMS_NULL;
            }

            sip_freeSipReqLine(pstReqLine);
        }
        else
        {
            SipStatusLine *pstStatusLine;
            IMS_CHAR *pszVersion;

            if (sip_getStatusLine(pstMessage, &pstStatusLine, SIPStackError()) == SipFail)
            {
                FreeMessage(pstMessage);
                return IMS_NULL;
            }

            pszVersion = sip_strdup(SIP::STR_SIP_VERSION, 0);

            if (sip_setVersionInStatusLine(pstStatusLine, pszVersion, SIPStackError()) == SipFail)
            {
                Free(pszVersion);
                sip_freeSipStatusLine(pstStatusLine);
                FreeMessage(pstMessage);
                return IMS_NULL;
            }

            sip_freeSipStatusLine(pstStatusLine);
        }

        return pstMessage;
    }

    /*

    Remarks

    */
    GLOBAL SipMsgBody* CreateMessageBody()
    {
        SipMsgBody *pstMsgBody;

        //-----------------------------------------------------------------------------------------

        if (sip_initSipMsgBody(&pstMsgBody, SipUnknownBody, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

#ifdef SIP_MIME_PARSING
        if (sip_initSipMimeHeader(&(pstMsgBody->pMimeHeader), SIPStackError()) == SipFail)
        {
            sip_freeSipMsgBody(pstMsgBody);
            return IMS_NULL;
        }
#endif

        if (sip_initSipUnknownMessage(
                &(pstMsgBody->u.pUnknownMessage), SIPStackError()) == SipFail)
        {
            sip_freeSipMsgBody(pstMsgBody);
            return IMS_NULL;
        }

        return pstMsgBody;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CreateMIMEHeader(IN_OUT SipMsgBody *pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

#ifdef SIP_MIME_PARSING
        SIPStackError(E_NO_ERROR);

        if (pstMsgBody->pMimeHeader != IMS_NULL)
        {
            return IMS_TRUE;
        }

        if (sip_initSipMimeHeader(&(pstMsgBody->pMimeHeader), SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
#else
        SIPStackError(E_NO_EXIST);

        return IMS_FALSE;
#endif
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* CreateViaHeader(IN CONST AString &strSentProtocol,
            IN CONST AString &strSentBy, IN CONST AString &strBranch)
    {
        SipHeader *pstHeader = CreateHeader(SipHdrTypeVia);

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return IMS_NULL;
        }

        IMS_CHAR *pszTmpVal = strSentProtocol.Duplicate();

        if (sip_setSentProtocolInViaHdr(pstHeader, pszTmpVal, SIPStackError()) == SipFail)
        {
            Free(pszTmpVal);
            FreeHeaderEx(pstHeader);
            return IMS_NULL;
        }

        pszTmpVal = strSentBy.Duplicate();

        if (sip_setSentByInViaHdr(pstHeader, pszTmpVal, SIPStackError()) == SipFail)
        {
            Free(pszTmpVal);
            FreeHeaderEx(pstHeader);
            return IMS_NULL;
        }

        if (!SetParameter(pstHeader, AString(SIP::STR_BRANCH), strBranch))
        {
            FreeHeaderEx(pstHeader);
            return IMS_NULL;
        }

        return pstHeader;
    }

    /*

    Remarks

    */
    GLOBAL SipAddrSpec* DecodeAddrSpec(IN CONST AString &strAddress)
    {
        SipAddrSpec *pstAddrSpec = IMS_NULL;
        AString strAddrSpec = strAddress;

        //-----------------------------------------------------------------------------------------

        // Remove LAQUOT/RAQUOT if present
        IMS_SINT32 nLAQUOT = TextParser::GetIndexOfDelimiter(strAddrSpec, TextParser::CHAR_LAQUOT);

        if (nLAQUOT != AString::NPOS)
        {
            strAddrSpec = strAddrSpec.GetSubStr(nLAQUOT + 1);

            IMS_SINT32 nRAQUOT = strAddrSpec.GetIndexOf(TextParser::CHAR_RAQUOT);

            if (nRAQUOT != AString::NPOS)
            {
                strAddrSpec.Truncate(nRAQUOT);
            }
        }

        IMS_CHAR *pszAddrSpec = strAddrSpec.GetStr();

        if (sip_parseAddrSpec(pszAddrSpec, pszAddrSpec + strAddrSpec.GetLength() - 1,
                &pstAddrSpec, SIPStackError()) == SipFail)
        {
            FreeAddrSpec(pstAddrSpec);
            return IMS_NULL;
        }

        return pstAddrSpec;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* DecodeHeader(IN IMS_SINT32 nType, IN CONST AString &strName,
            IN CONST AString &strBody)
    {
        SipHeader *pstHeader = CreateHeader(nType);

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return IMS_NULL;
        }

        if (nType == SipHdrTypeUnknown)
        {
            if (strName.GetLength() == 0)
            {
                FreeHeaderEx(pstHeader);

                SIPStackError(E_INV_PARAM);
                return IMS_NULL;
            }

            SipCommonHeader *pstUnknownHeader
                    = reinterpret_cast<SipCommonHeader*>(pstHeader->pHeader);

            pstUnknownHeader->pStr1 = strName.Duplicate();
            pstUnknownHeader->pStr2 = strBody.Duplicate();
        }
        else
        {
            IMS_UINT32 nBodyLen = strBody.GetLength();
            IMS_CHAR *pszTmpBody = strBody.Duplicate();

            if (pszTmpBody == IMS_NULL)
            {
                FreeHeaderEx(pstHeader);
                return IMS_NULL;
            }

            if (sip_parseHeaderBody(pszTmpBody, (pszTmpBody + nBodyLen - 1),
                    pstHeader->dType, pstHeader, SIPStackError()) == SipFail)
            {
                IMS_TRACE_E(0, "Parsing the header (%d : %s) failed", nType, pszTmpBody, 0);

                Free(pszTmpBody);
                //FreeHeaderEx(pstHeader);
                return IMS_NULL;
            }

            // Process the header type which has an ANY type: Contact, Expires, Retry-After
            sip_equateTypeInSipHeader(pstHeader);

            Free(pszTmpBody);
        }

        return pstHeader;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL DecodeMessage(IN CONST IMS_BYTE *pBuffer, IN IMS_SINT32 nBuffLen,
            IN IMS_SINT32 nOptions, OUT SipMessage *&pstMessage)
    {
        SipOptions stOptions = { static_cast<SIP_U32bit>(nOptions) };
        SipEventContext *pstContext;
        SIP_S8bit *pNextBuffer = IMS_NULL;
        SIP_S8bit *pMsgBuffer = reinterpret_cast<SIP_S8bit*>(const_cast<IMS_BYTE*>(pBuffer));

        //-----------------------------------------------------------------------------------------

        /*
         * Set the decoding options.
         *    SIP_OPT_NOTIMER:        TXN layer is used
         *    SIP_OPT_BADMESSAGE:     Bad message is parsed
         *    SIP_OPT_NOPARSEBODY:    Message body is not parsed
         *    SIP_OPT_CLEN:           Invalid buffer handling
         *
         * Use the NOTIMER option of the core stack. This will
         * disable sip_decodeMessage from directly calling
         * fast_stopTimer. This is because we would like to do some
         * validations on the SIP Message (like whether it arrived
         * on the correct port) and then stop the timer only if our
         * validations succeed.
         */

        // Ensure that the flag SIP_OPT_NOTIMER option is set by default so that
        // the call to sip_decodeMessage does not result in any attempts to stop timers.
        stOptions.dOption |= SIP_OPT_NOTIMER;

        pstContext = CreateEventContext();

        if (pstContext == IMS_NULL)
            return IMS_FALSE;

        if (sip_decodeMessage(pMsgBuffer, &pstMessage,
                &stOptions, nBuffLen, &pNextBuffer, pstContext, SIPStackError()) == SipFail)
        {
            DestroyEventContext(pstContext);
            return IMS_FALSE;
        }

        DestroyEventContext(pstContext);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL DecodeMessageBody(IN SipMessage *pstMessage)
    {
        IMS_UINT32 nBodyCount = 0;

        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        (void) List_SizeOf(&(pstMessage->slMessageBody), &nBodyCount, SIPStackError());

        if (nBodyCount > 1)
        {
            SIPStackError(E_PARSER_ERROR);

            IMS_TRACE_E(0, "There are many SIP message bodies", 0, 0, 0);
            return IMS_FALSE;
        }

        if (nBodyCount == 0)
        {
            IMS_TRACE_D("___ NO SIP MESSAGE BODY ___", 0, 0, 0);
            return IMS_TRUE;
        }

        SipMsgBody *pstMsgBody = IMS_NULL;

        (void) List_GetAt(&(pstMessage->slMessageBody), 0, &pstMsgBody, SIPStackError());

        if (pstMsgBody == IMS_NULL)
        {
            IMS_TRACE_E(0, "Getting SIP message body failed", 0, 0, 0);
            return IMS_FALSE;
        }

        if ((pstMsgBody->dType != SipUnknownBody)
                || (pstMsgBody->u.pUnknownMessage == IMS_NULL))
        {
            IMS_TRACE_E(0, "Message body is not unknown body part", 0, 0, 0);
            return IMS_FALSE;
        }

        //4 Check if the Content-Encoding contains the compression algorithm or not
        //4 As default, consider it to 'gzip'. It needs to be consider other compression algorithm.
        if (IsMessageBodyCompressed(pstMessage))
        {
            ByteArray objBodyPart;
            ByteArray objCompBodyPart;

            objCompBodyPart.Attach(
                    reinterpret_cast<const IMS_BYTE*>(pstMsgBody->u.pUnknownMessage->pBuffer),
                    static_cast<IMS_SINT32>(pstMsgBody->u.pUnknownMessage->dLength));

            if (!IMS_UTIL_ZLIB_Uncompress(objCompBodyPart, objBodyPart))
            {
                IMS_TRACE_E(0, "Uncompressing a body part failed", 0, 0, 0);
                return IMS_FALSE;
            }

            if (IMS_UTIL_SYS_PROP_IS_DEBUG_MODE())
            {
                IMS_TRACE_TEXT("gzip::uncompression",
                        objBodyPart.GetData(), objBodyPart.GetLength());
            }

            // Overwrites the body start / end position from the uncompressed data
            IMS_CHAR *pszBodyStart = reinterpret_cast<IMS_CHAR*>(objBodyPart.GetData());
            IMS_CHAR *pszBodyEnd = pszBodyStart + objBodyPart.GetLength() - 1;
            SipOptions stOptions = { SIP_OPT_NOTIMER | SIP_OPT_BADMESSAGE | SIP_OPT_NOPARSEBODY };

            if (!ParseMIMEBody(pszBodyStart, pszBodyEnd, &stOptions, pstMessage))
            {
                // Rollback??? : Remove all message body except for the first body?

                IMS_TRACE_E(0, "Parsing SIP message body failed", 0, 0, 0);
                return IMS_FALSE;
            }

            // Now, remove the whole buffer (previous buffer) from SIP message
            (void) List_DeleteAt(&(pstMessage->slMessageBody), 0, SIPStackError());
        }
        // Only MIME body will be parsed ...
        else
        {
            SipOptions stOptions = { SIP_OPT_NOTIMER | SIP_OPT_BADMESSAGE | SIP_OPT_NOPARSEBODY };
            SipUnknownMessage *pstBodyPart = pstMsgBody->u.pUnknownMessage;

            if (!ParseMIMEBody(pstBodyPart->pBuffer,
                    (pstBodyPart->pBuffer + pstBodyPart->dLength - 1), &stOptions, pstMessage))
            {
                // Rollback??? : Remove all message body except for the first body?

                IMS_TRACE_E(0, "Parsing SIP message body failed", 0, 0, 0);
                return IMS_FALSE;
            }

            // Now, remove the whole buffer (previous buffer) from SIP message
            (void) List_DeleteAt(&(pstMessage->slMessageBody), 0, SIPStackError());
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL EncodeAddrSpec(IN CONST SipAddrSpec *pstAddrSpec, IN IMS_BOOL bParams,
            OUT AString &strAddrSpec)
    {
        //-----------------------------------------------------------------------------------------

        if (pstAddrSpec == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        AStringBuffer objAddrSpec(128);

        if (!FormAddrSpec(pstAddrSpec, bParams, objAddrSpec))
        {
            strAddrSpec = AString::ConstNull();
            return IMS_FALSE;
        }

        strAddrSpec = static_cast<const AStringBuffer&>(objAddrSpec).GetString();

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL EncodeHeaderBody(IN CONST SipHeader *pstHeader, IN IMS_BOOL bParams,
            OUT AString &strHeaderBody)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        AStringBuffer objStringBuffer(128);

        if (!FormHeaderBody(pstHeader, bParams, objStringBuffer))
        {
            strHeaderBody = AString::ConstNull();
            return IMS_FALSE;
        }

        strHeaderBody = static_cast<const AStringBuffer&>(objStringBuffer).GetString();

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL EncodeMessage(IN SipMessage *pstMessage, IN IMS_SINT32 nOptions,
            OUT IMS_BYTE *&pBuffer, OUT IMS_SINT32 &nBuffLen)
    {
        SipOptions stOptions = { static_cast<SIP_U32bit>(nOptions) };

        //-----------------------------------------------------------------------------------------

        if (sip_formMessage(pstMessage, &stOptions,
                (SIP_S8bit*) pBuffer, (SIP_U32bit*) &nBuffLen, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL EncodePartialMessage(IN SipMessage *pstMessage, IN IMS_SINT32 nOptions,
            OUT ByteArray &objMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if ((nOptions | OPT_ALL) == 0)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        AStringBuffer objMessageBuffer(1024);
        AString strTemp;

        // Check start-line
        if ((nOptions & OPT_START_LINE) == OPT_START_LINE)
        {
            if (pstMessage->dType == SipMessageRequest)
            {
                if (pstMessage->u.pRequest->pRequestLine == IMS_NULL)
                {
                    SIPStackError(E_INV_PARAM);
                    return IMS_FALSE;
                }

                objMessageBuffer.Append(pstMessage->u.pRequest->pRequestLine->pStr1);
                objMessageBuffer.Append(TextParser::CHAR_SP);

                if (pstMessage->u.pRequest->pRequestLine->pAddrSpec != IMS_NULL)
                {
                    FormAddrSpec(pstMessage->u.pRequest->pRequestLine->pAddrSpec,
                            IMS_TRUE, objMessageBuffer);
                }

                objMessageBuffer.Append(TextParser::CHAR_SP);
                objMessageBuffer.Append(pstMessage->u.pRequest->pRequestLine->pStr2);
                objMessageBuffer.Append(TextParser::CHAR_CR);
                objMessageBuffer.Append(TextParser::CHAR_LF);
            }
            else
            {
                if (pstMessage->u.pResponse->pStatusLine == IMS_NULL)
                {
                    SIPStackError(E_INV_PARAM);
                    return IMS_FALSE;
                }

                strTemp.Sprintf("%03d", pstMessage->u.pResponse->pStatusLine->dIntVar1);

                objMessageBuffer.Append(pstMessage->u.pResponse->pStatusLine->pStr1);
                objMessageBuffer.Append(TextParser::CHAR_SP);
                objMessageBuffer.Append(strTemp);
                objMessageBuffer.Append(TextParser::CHAR_SP);
                objMessageBuffer.Append(pstMessage->u.pResponse->pStatusLine->pStr2);
                objMessageBuffer.Append(TextParser::CHAR_CR);
                objMessageBuffer.Append(TextParser::CHAR_LF);
            }
        }

        // Check header parts
        if ((nOptions & OPT_HEADER_PART) == OPT_HEADER_PART)
        {
            IMS_UINT32 nHeaderCount = 0;
            IMS_SINT32 anHeaderCounter[HEADERTYPENUM] = { 0, };

            List_SizeOf(&(pstMessage->slOrderInfo), &nHeaderCount, SIPStackError());

            for (IMS_UINT32 i = 0; i < nHeaderCount; ++i)
            {
                if (!FormEachHeader(pstMessage, i, &anHeaderCounter[0], objMessageBuffer))
                {
                    return IMS_FALSE;
                }
            }
        }

        objMessageBuffer += TextParser::CHAR_CR;
        objMessageBuffer += TextParser::CHAR_LF;

        // To correct the Content-Length header field
        IMS_SINT32 nHeaderLength = objMessageBuffer.GetLength();
        const AString &strMessage
                = static_cast<const AStringBuffer&>(objMessageBuffer).GetString();
        IMS_SINT32 nIndexOfContentLength = strMessage.GetLastIndexOf("\r\nl:");

        if (nIndexOfContentLength == AString::NPOS)
        {
            nIndexOfContentLength = strMessage.GetLastIndexOf("\r\nContent-Length:");
        }

        // Check body parts
        IMS_SINT32 nBodyLength = 0;

        if ((nOptions & OPT_BODY_PART) == OPT_BODY_PART)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeContentType);

            if (pstHeader != IMS_NULL)
            {
#ifdef SIP_INCREMENTAL_PARSING
                if (pstHeader->pHeader != IMS_NULL)
                {
                    SipContentTypeHeader *pstContentType
                            = reinterpret_cast<SipContentTypeHeader*>(pstHeader->pHeader);

                    if (pstContentType->pBuffer != IMS_NULL)
                    {
                        // Parse the content-type header in here ...
                        // TODO::
                    }
                }
#endif // SIP_INCREMENTAL_PARSING

                if (!FormMessageBodys(pstMessage, pstHeader, objMessageBuffer))
                {
                    FreeHeaderEx(pstHeader);
                    return IMS_FALSE;
                }

                nBodyLength = objMessageBuffer.GetLength() - nHeaderLength;

                FreeHeaderEx(pstHeader);
            }
        }

        // Correct the Content-Length header
        if (nBodyLength >= 0)
        {
            AString strContentLength;

            strContentLength.Sprintf("Content-Length: %d\r\n", nBodyLength);

            if (nIndexOfContentLength == AString::NPOS)
            {
                // Concern the empty line (CRLF)
                objMessageBuffer.Insert(nHeaderLength - 2, strContentLength);
            }
            else
            {
                // Skip CRLF...
                const AStringBuffer& objTempBuffer
                        = static_cast<const AStringBuffer&>(objMessageBuffer);
                IMS_SINT32 nOffset = nIndexOfContentLength + 2;
                IMS_SINT32 nEndIndex = objTempBuffer.GetString().GetIndexOf("\r\n", nOffset);

                objMessageBuffer.Replace(nOffset, nEndIndex - nOffset + 2, strContentLength);
            }
        }

        objMessage.Append(reinterpret_cast<const IMS_BYTE*>(objMessageBuffer.GetCharString()),
                objMessageBuffer.GetLength());

        SIPStackError(E_NO_ERROR);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsUnknownHeader(IN_OUT IMS_SINT32 &nType, IN CONST AString & /*strName*/)
    {
        return (nType == ISIPHeader::UNKNOWN);
    }

    /*

    Remarks

    */
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN SipHeader *pstHeader)
    {
        IMSList<SIPParameter*> objParams;
        IMS_UINT32 nPCount = 0;

        //-----------------------------------------------------------------------------------------

        (void) sip_getParamCountFromCommonHdr(pstHeader, &nPCount, SIPStackError());

        if (nPCount > 0)
        {
            SipParam *pstParam;
            SIPParameter *pParameter;
            AString strValue;
            IMS_UINT32 nVCount = 0;
            IMS_CHAR *pszValue;

            for (IMS_UINT32 nP = 0; nP < nPCount; nP++)
            {
                if (sip_getParamAtIndexFromCommonHdr(pstHeader,
                        &pstParam, nP, SIPStackError()) == SipSuccess)
                {
                    strValue = pstParam->pName;
                    pParameter = new SIPParameter(strValue);

                    if (pParameter == IMS_NULL)
                    {
                        sip_freeSipParam(pstParam);
                        continue; // Throw exception ?
                    }

                    (void) sip_getValueCountFromSipParam(pstParam, &nVCount, SIPStackError());

                    for (IMS_UINT32 nV = 0; nV < nVCount; nV++)
                    {
                        if (sip_getValueAtIndexFromSipParam(pstParam,
                                &pszValue, nV, SIPStackError()) == SipSuccess)
                        {
                            strValue = pszValue;
                            pParameter->AddValues(strValue);
                        }
                    }

                    objParams.Append(pParameter);

                    sip_freeSipParam(pstParam);
                }
            }
        }

        return objParams;
    }

    /*

    Remarks

    */
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN SipAddrSpec *pstAddrSpec)
    {
        SipUrl* pstUrl;

        IMSList<SIPParameter*> objParams;
        IMS_UINT32 nPCount = 0;

        //-----------------------------------------------------------------------------------------

        if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError())== SipFail)
        {
            // throw exception ?
            return objParams;
        }

        (void) sip_getUrlParamCountFromUrl(pstUrl, &nPCount, SIPStackError());

        if (nPCount > 0)
        {
            SipParam *pstParam;
            SIPParameter *pParameter;
            AString strValue;
            IMS_UINT32 nVCount = 0;
            IMS_CHAR *pszValue;

            for (IMS_UINT32 nP = 0; nP < nPCount; nP++)
            {
                if (sip_getUrlParamAtIndexFromUrl(pstUrl,
                        &pstParam, nP, SIPStackError()) == SipSuccess)
                {
                    strValue = pstParam->pName;
                    pParameter = new SIPParameter(strValue);

                    if (pParameter == IMS_NULL)
                    {
                        sip_freeSipParam(pstParam);
                        continue; // Throw exception ?
                    }

                    (void) sip_getValueCountFromSipParam(pstParam, &nVCount, SIPStackError());

                    for (IMS_UINT32 nV = 0; nV < nVCount; nV++)
                    {
                        if (sip_getValueAtIndexFromSipParam(pstParam,
                                &pszValue, nV, SIPStackError()) == SipSuccess)
                        {
                            strValue = pszValue;
                            pParameter->AddValues(strValue);
                        }
                    }

                    objParams.Append(pParameter);

                    sip_freeSipParam(pstParam);
                }
            }
        }

        sip_freeSipUrl(pstUrl);

        return objParams;
    }

    /*

    Remarks

    */
    GLOBAL IMSList<SIPParameter*> ExtractParameters(IN CONST AString &strParams, IN IMS_CHAR cSep)
    {
        IMSList<SIPParameter*> objParams;
        AString strTmp = strParams.Trim();

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if ((strTmp.GetLength() == 0) || (strTmp.Equals(cSep)))
            return objParams;

        AStringArray objTokens = strTmp.Split(cSep);

        for (IMS_SINT32 i = 0; i < objTokens.GetCount(); ++i)
        {
            const AString &strToken = objTokens.GetElementAt(i);

            if (strToken.GetLength() == 0)
                continue;

            SIPParameter *pParameter = new SIPParameter();

            if (pParameter == IMS_NULL)
                continue;

            if (!pParameter->Create(strToken))
            {
                IMS_TRACE_E(0, "Creating a SIPParameter failed - token (%s)",
                        strToken.GetStr(), 0, 0);

                delete pParameter;
                continue;
            }

            objParams.Append(pParameter);
        }

        return objParams;
    }

    /*

    Remarks

    */
    GLOBAL void FreeMemBlock(IN void *&pvMemBlock)
    {
        //-----------------------------------------------------------------------------------------

        if (pvMemBlock == IMS_NULL)
            return;

        IMS_MEM_Free(pvMemBlock);
        pvMemBlock = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL void FreeAddrSpec(IN SipAddrSpec *&pstAddrSpec)
    {
        //-----------------------------------------------------------------------------------------

        sip_freeSipAddrSpec(pstAddrSpec);
        pstAddrSpec = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL void FreeHeader(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return;
        }

        sip_freeSipHeader(pstHeader);
        pstHeader->pHeader = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL void FreeHeaderEx(IN SipHeader *&pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return;
        }

        sip_freeSipHeader(pstHeader);
        Free(pstHeader);
    }

    /*

    Remarks

    */
    GLOBAL void FreeMessage(IN SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        sip_freeSipMessage(pstMessage);
        pstMessage = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL void FreeMessageBody(IN SipMsgBody *&pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        sip_freeSipMsgBody(pstMsgBody);
        pstMsgBody = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL IMS_CHAR GetCompactHeaderName(IN IMS_SINT32 nType,
        IN CONST AString &strName /* = AString::ConstNull() */)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        case SipHdrTypeCallId:
            return SIPHeaderName::CF_CALL_ID;
        case SipHdrTypeContactAny:
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
            return SIPHeaderName::CF_CONTACT;
        case SipHdrTypeContentEncoding:
            return SIPHeaderName::CF_CONTENT_ENCODING;
        case SipHdrTypeContentLength:
            return SIPHeaderName::CF_CONTENT_LENGTH;
        case SipHdrTypeContentType:
            return SIPHeaderName::CF_CONTENT_TYPE;
        case SipHdrTypeFrom:
            return SIPHeaderName::CF_FROM;
        case SipHdrTypeSupported:
            return SIPHeaderName::CF_SUPPORTED;
        case SipHdrTypeTo:
            return SIPHeaderName::CF_TO;
        case SipHdrTypeVia:
            return SIPHeaderName::CF_VIA;
#ifdef SIP_IMPP
        case SipHdrTypeEvent:
            return SIPHeaderName::CF_EVENT;
        case SipHdrTypeAllowEvents:
            return SIPHeaderName::CF_ALLOW_EVENTS;
#endif
#ifdef SIP_REFER
        case SipHdrTypeReferTo:
            return SIPHeaderName::CF_REFER_TO;
        case SipHdrTypeReferredBy:
            return SIPHeaderName::CF_REFERRED_BY;
#endif
#ifdef SIP_3GPP
        case SipHdrTypeRequestDisposition:
            return SIPHeaderName::CF_REQUEST_DISPOSITION;
        case SipHdrTypeAcceptContact:
            return SIPHeaderName::CF_ACCEPT_CONTACT;
        case SipHdrTypeRejectContact:
            return SIPHeaderName::CF_REJECT_CONTACT;
#endif
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeSessionExpires:
            return SIPHeaderName::CF_SESSION_EXPIRES;
#endif

        case SipHdrTypeUnknown:
            switch (strName[0])
            {
            case 's':
            case 'S':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SUBJECT))
                    return SIPHeaderName::CF_SUBJECT;
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SUBJECT))
                    return SIPHeaderName::CF_SUBJECT;
                break;

            case 'i':
            case 'I':
                if (strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY))
                    return SIPHeaderName::CF_IDENTITY;
                else if (strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY_INFO))
                    return SIPHeaderName::CF_IDENTITY_INFO;
                break;

            case 'n':
            case 'N':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY_INFO))
                    return SIPHeaderName::CF_IDENTITY_INFO;
                break;

            case 'y':
            case 'Y':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY))
                    return SIPHeaderName::CF_IDENTITY;
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        return '\0';
    }

    /*

    Remarks

    */
    GLOBAL const IMS_CHAR* GetHeaderName(IN IMS_SINT32 nType,
            IN CONST AString &strName /* = AString::ConstNull() */)
    {
        //-----------------------------------------------------------------------------------------

        if ((nType <= ISIPHeader::INVALID) || (nType >= ISIPHeader::ANY))
        {
            return IMS_NULL;
        }

        switch (nType)
        {
        case SipHdrTypeUnknown:
            if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SUBJECT))
                return SIPHeaderName::SUBJECT;
            else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY))
                return SIPHeaderName::IDENTITY;
            else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY_INFO))
                return SIPHeaderName::IDENTITY_INFO;
            else
                return strName.GetStr();

        default:
            break;
        }

        return SIPHeader::NAME[nType];
    }

    /*

    Remarks

    */
    GLOBAL const IMS_CHAR* GetHeaderNameFromType(IN IMS_SINT32 nType)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        case SipHdrTypeAllow:
#ifdef SIP_IMPP
        case SipHdrTypeAllowEvents:
#endif
#ifdef SIP_AUTHENTICATE
        case SipHdrTypeAuthorization:
#endif
        case SipHdrTypeCallId:
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
        case SipHdrTypeContactAny:
        case SipHdrTypeContentDisposition:
        case SipHdrTypeContentEncoding:
        case SipHdrTypeContentLength:
        case SipHdrTypeContentType:
        case SipHdrTypeCseq:
            /* General headers end here. */
#ifdef SIP_IMPP
        case SipHdrTypeEvent:
#endif
#ifdef SIP_DATE
        case SipHdrTypeExpiresDate:
        case SipHdrTypeExpiresSec:
        case SipHdrTypeExpiresAny:
#else
        case SipHdrTypeExpires:
#endif
        case SipHdrTypeAccept:
        case SipHdrTypeMinExpires:
        case SipHdrTypeFrom:
        case SipHdrTypeMaxforwards:
        case SipHdrTypeMimeVersion:
#ifdef SIP_PRIVACY
        case SipHdrTypePrivacy:
        case SipHdrTypePPreferredId:
        case SipHdrTypePAssertId:
#endif
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeMinSE:
#endif
#ifdef SIP_3GPP
        case SipHdrTypePath:
        case SipHdrTypePAssociatedUri:
        case SipHdrTypePCalledPartyId:
        case SipHdrTypePVisitedNetworkId:
        case SipHdrTypePcfAddr:
        case SipHdrTypePanInfo:
        case SipHdrTypePcVector:
        case SipHdrTypeServiceRoute:
        case SipHdrTypeHistoryInfo:
        case SipHdrTypeRequestDisposition:
        case SipHdrTypeAcceptContact:
        case SipHdrTypeRejectContact:
        case SipHdrTypeJoin:
        case SipHdrTypeIfMatch:
        case SipHdrTypeETag:
#endif
#ifdef SIP_AUTHENTICATE
        case SipHdrTypeProxyAuthenticate:
        case SipHdrTypeProxyauthorization:
#endif
#ifdef SIP_RPR
        case SipHdrTypeRAck:
#endif
        case SipHdrTypeRecordRoute:
#ifdef SIP_REFER
        case SipHdrTypeReferredBy:
        case SipHdrTypeReferTo:
#endif
#ifdef SIP_REPLACES
        case SipHdrTypeReplaces:
#endif
        case SipHdrTypeRequire:
        case SipHdrTypeRoute:
#ifdef SIP_RPR
        case SipHdrTypeRSeq:
#endif
#ifdef SIP_SECURITY
        case SipHdrTypeSecurityClient:
        case SipHdrTypeSecurityVerify:
        case SipHdrTypeSecurityServer:
#endif
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeSessionExpires:
#endif
#ifdef SIP_IMPP
        case SipHdrTypeSubscriptionState:
#endif
        case SipHdrTypeSupported:
#ifdef SIP_TIMESTAMP
        case SipHdrTypeTimestamp:
#endif
        case SipHdrTypeTo:
        case SipHdrTypeUnsupported:
        case SipHdrTypeVia:
#ifdef SIP_WARNING
        case SipHdrTypeWarning:
#endif
#ifdef SIP_AUTHENTICATE
        case SipHdrTypeWwwAuthenticate:
#endif
#ifdef SIP_DATE
        case SipHdrTypeRetryAfterDate:
        case SipHdrTypeRetryAfterSec:
        case SipHdrTypeRetryAfterAny:
#else
        case SipHdrTypeRetryAfter:
#endif
#ifdef SIP_PEMEDIA
        case SipHdrTypePEarlyMedia:
#endif
#ifdef SIP_CONGEST
        case SipHdrTypeRsrcPriority:
        case SipHdrTypeAcceptRsrcPriority:
#endif
#if defined(SIP_DATE) && defined(SIP_IMS_DATE)
        case SipHdrTypeDate:
#endif
            return SIPHeader::NAME[nType];

        case SipHdrTypeUnknown:
        default:
            return "";
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetHeaderTypeFromName(IN CONST AString &strName)
    {
        IMS_SINT32 nType = SipHdrTypeUnknown;

        //-----------------------------------------------------------------------------------------

        if (strName.GetLength() == 0)
        {
            return SipHdrTypeAny;
        }

        switch (strName[0])
        {
        case 'a':
        case 'A':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::ALLOW))
                {
                    nType = SipHdrTypeAllow;
                }
#ifdef SIP_IMPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::ALLOW_EVENTS))
                {
                    nType = SipHdrTypeAllowEvents;
                }
#endif
#ifdef SIP_3GPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_ACCEPT_CONTACT))
                {
                    nType = SipHdrTypeAcceptContact;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::ACCEPT_CONTACT))
                {
                    nType = SipHdrTypeAcceptContact;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::ACCEPT))
                {
                    nType = SipHdrTypeAccept;
                }
#ifdef SIP_AUTHENTICATE
                else if (strName.EqualsIgnoreCase(SIPHeaderName::AUTHORIZATION))
                {
                    nType = SipHdrTypeAuthorization;
                }
#endif
#ifdef SIP_CONGEST
                else if (strName.EqualsIgnoreCase(SIPHeaderName::ACCEPT_RESOURCE_PRIORITY))
                {
                    nType = SipHdrTypeAcceptRsrcPriority;
                }
#endif
            }
            break;

        case 'b':
        case 'B':
            {
#ifdef SIP_REFER
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_REFERRED_BY))
                {
                    nType = SipHdrTypeReferredBy;
                }
#endif
            }
            break;

        case 'c':
        case 'C':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_CONTENT_TYPE))
                {
                    nType = SipHdrTypeContentType;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CALL_ID))
                {
                    nType = SipHdrTypeCallId;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CONTACT))
                {
                    nType = SipHdrTypeContactNormal;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CONTENT_TYPE))
                {
                    nType = SipHdrTypeContentType;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CONTENT_LENGTH))
                {
                    nType = SipHdrTypeContentLength;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CONTENT_DISPOSITION))
                {
                    nType = SipHdrTypeContentDisposition;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CONTENT_ENCODING))
                {
                    nType = SipHdrTypeContentEncoding;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CSEQ))
                {
                    nType = SipHdrTypeCseq;
                }
            }
            break;

        case 'd':
        case 'D':
            {
#ifdef SIP_3GPP
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_REQUEST_DISPOSITION))
                {
                    nType = SipHdrTypeRequestDisposition;
                }
#endif
            }
            break;

        case 'e':
        case 'E':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_CONTENT_ENCODING))
                {
                    nType = SipHdrTypeContentEncoding;
                }
#ifdef SIP_IMPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::EVENT))
                {
                    nType = SipHdrTypeEvent;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::EXPIRES))
                {
#ifdef SIP_DATE
                    nType = SipHdrTypeExpiresSec;
#else
                    nType = SipHdrTypeExpires;
#endif
                }
            }
            break;

        case 'f':
        case 'F':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_FROM))
                {
                    nType = SipHdrTypeFrom;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::FROM))
                {
                    nType = SipHdrTypeFrom;
                }
            }
            break;

        case 'h':
        case 'H':
            {
#ifdef SIP_3GPP
                if (strName.EqualsIgnoreCase(SIPHeaderName::HISTORY_INFO))
                {
                    nType = SipHdrTypeHistoryInfo;
                }
#endif
            }
            break;

        case 'i':
        case 'I':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_CALL_ID))
                {
                    nType = SipHdrTypeCallId;
                }
            }
            break;

        case 'j':
        case 'J':
            {
#ifdef SIP_3GPP
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_REJECT_CONTACT))
                {
                    nType = SipHdrTypeRejectContact;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::JOIN))
                {
                    nType = SipHdrTypeJoin;
                }
#endif
            }
            break;

        case 'k':
        case 'K':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SUPPORTED))
                {
                    nType = SipHdrTypeSupported;
                }
            }
            break;

        case 'l':
        case 'L':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_CONTENT_LENGTH))
                {
                    nType = SipHdrTypeContentLength;
                }
            }
            break;

        case 'm':
        case 'M':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_CONTACT))
                {
                    nType = SipHdrTypeContactNormal;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::MAX_FORWARDS))
                {
                    nType = SipHdrTypeMaxforwards;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::MIME_VERSION))
                {
                    nType = SipHdrTypeMimeVersion;
                }
#ifdef SIP_SESSIONTIMER
                else if (strName.EqualsIgnoreCase(SIPHeaderName::MIN_SE))
                {
                    nType = SipHdrTypeMinSE;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::MIN_EXPIRES))
                {
                    nType = SipHdrTypeMinExpires;
                }
            }
            break;

        case 'o':
        case 'O':
            {
#ifdef SIP_IMPP
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_EVENT))
                {
                    nType = SipHdrTypeEvent;
                }
#endif
            }
            break;

        case 'p':
        case 'P':
            {
#ifdef SIP_3GPP
                if (strName.EqualsIgnoreCase(SIPHeaderName::PATH))
                {
                    nType = SipHdrTypePath;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_ASSOCIATED_URI))
                {
                    nType = SipHdrTypePAssociatedUri;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_CALLED_PARTY_ID))
                {
                    nType = SipHdrTypePCalledPartyId;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_VISITED_NETWORK_ID))
                {
                    nType = SipHdrTypePVisitedNetworkId;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_CHARGING_FUNCTION_ADDRESSES))
                {
                    nType = SipHdrTypePcfAddr;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_ACCESS_NETWORK_INFO))
                {
                    nType = SipHdrTypePanInfo;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_CHARGING_VECTOR))
                {
                    nType = SipHdrTypePcVector;
                }
#endif // SIP_3GPP
#ifdef SIP_AUTHENTICATE
                if (strName.EqualsIgnoreCase(SIPHeaderName::PROXY_AUTHENTICATE))
                {
                    nType = SipHdrTypeProxyAuthenticate;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::PROXY_AUTHORIZATION))
                {
                    nType = SipHdrTypeProxyauthorization;
                }
#endif
#ifdef SIP_PRIVACY
                if (strName.EqualsIgnoreCase(SIPHeaderName::PRIVACY))
                {
                    nType = SipHdrTypePrivacy;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_PREFERRED_IDENTITY))
                {
                    nType = SipHdrTypePPreferredId;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::P_ASSERTED_IDENTITY))
                {
                    nType = SipHdrTypePAssertId;
                }
#endif
#ifdef SIP_PEMEDIA
                if (strName.EqualsIgnoreCase(SIPHeaderName::P_EARLY_MEDIA))
                {
                    nType = SipHdrTypePEarlyMedia;
                }
#endif
            }
            break;

        case 'r':
        case 'R':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::REQUIRE))
                {
                    nType = SipHdrTypeRequire;
                }
#ifdef SIP_REFER
                else if (strName.EqualsIgnoreCase(SIPHeaderName::REFERRED_BY))
                {
                    nType = SipHdrTypeReferredBy;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_REFER_TO))
                {
                    nType = SipHdrTypeReferTo;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::REFER_TO))
                {
                    nType = SipHdrTypeReferTo;
                }
#endif
#ifdef SIP_3GPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::REQUEST_DISPOSITION))
                {
                    nType = SipHdrTypeRequestDisposition;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::REJECT_CONTACT))
                {
                    nType = SipHdrTypeRejectContact;
                }
#endif
#ifdef SIP_REPLACES
                else if (strName.EqualsIgnoreCase(SIPHeaderName::REPLACES))
                {
                    nType = SipHdrTypeReplaces;
                }
#endif
#ifdef SIP_RPR
                else if (strName.EqualsIgnoreCase(SIPHeaderName::RACK))
                {
                    nType = SipHdrTypeRAck;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::RECORD_ROUTE))
                {
                    nType = SipHdrTypeRecordRoute;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::ROUTE))
                {
                    nType = SipHdrTypeRoute;
                }
#ifdef SIP_RPR
                else if (strName.EqualsIgnoreCase(SIPHeaderName::RSEQ))
                {
                    nType = SipHdrTypeRSeq;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::RETRY_AFTER))
                {
#ifdef SIP_DATE
                    nType = SipHdrTypeRetryAfterSec;
#else
                    nType = SipHdrTypeRetryAfter;
#endif
                }
#ifdef SIP_CONGEST
                else if (strName.EqualsIgnoreCase(SIPHeaderName::RESOURCE_PRIORITY))
                {
                    nType = SipHdrTypeRsrcPriority;
                }
#endif
            }
            break;

        case 's':
        case 'S':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::SUPPORTED))
                {
                    nType = SipHdrTypeSupported;
                }
#ifdef SIP_3GPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SIP_IF_MATCH))
                {
                    nType = SipHdrTypeIfMatch;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SIP_ETAG))
                {
                    nType = SipHdrTypeETag;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SERVICE_ROUTE))
                {
                    nType = SipHdrTypeServiceRoute;
                }
#endif
#ifdef SIP_SESSIONTIMER
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SESSION_EXPIRES))
                {
                    nType = SipHdrTypeSessionExpires;
                }
#endif
#ifdef SIP_IMPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SUBSCRIPTION_STATE))
                {
                    nType = SipHdrTypeSubscriptionState;
                }
#endif
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SUBJECT))
                {
                    nType = SipHdrTypeUnknown;
                }
#ifdef SIP_SECURITY
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SECURITY_CLIENT))
                {
                    nType = SipHdrTypeSecurityClient;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SECURITY_VERIFY))
                {
                    nType = SipHdrTypeSecurityVerify;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::SECURITY_SERVER))
                {
                    nType = SipHdrTypeSecurityServer;
                }
#endif
            }
            break;

        case 't':
        case 'T':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_TO))
                {
                    nType = SipHdrTypeTo;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::TO))
                {
                    nType = SipHdrTypeTo;
                }
#ifdef SIP_TIMESTAMP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::TIMESTAMP))
                {
                    nType = SipHdrTypeTimestamp;
                }
#endif
            }
            break;

        case 'u':
        case 'U':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::UNSUPPORTED))
                {
                    nType = SipHdrTypeUnsupported;
                }
#ifdef SIP_IMPP
                else if (strName.EqualsIgnoreCase(SIPHeaderName::CF_ALLOW_EVENTS))
                {
                    nType = SipHdrTypeAllowEvents;
                }
#endif
            }
            break;

        case 'v':
        case 'V':
            {
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_VIA))
                {
                    nType = SipHdrTypeVia;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::VIA))
                {
                    nType = SipHdrTypeVia;
                }
            }
            break;

        case 'w':
        case 'W':
            {
#ifdef SIP_WARNING
                if (strName.EqualsIgnoreCase(SIPHeaderName::WARNING))
                {
                    nType = SipHdrTypeWarning;
                }
#endif
#ifdef SIP_AUTHENTICATE
                if (strName.EqualsIgnoreCase(SIPHeaderName::WWW_AUTHENTICATE))
                {
                    nType = SipHdrTypeWwwAuthenticate;
                }
#endif
            }
            break;

        case 'x':
        case 'X':
            {
#ifdef SIP_SESSIONTIMER
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SESSION_EXPIRES))
                {
                    nType = SipHdrTypeSessionExpires;
                }
#endif
            }
            break;

        default:
            break;
        }

        return nType;
    }

    /*

    Remarks

    */
    GLOBAL SipAddrSpec* GetAddrSpec(IN SipHeader *pstHeader)
    {
        SipAddrSpec *pstAddrSpec = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            return IMS_NULL;
        }

        if (pstHeader->dType == SipHdrTypeAny)
        {
            return IMS_NULL;
        }

        if (sip_getAddrSpecFromCommonHdr(pstHeader, &pstAddrSpec, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        return pstAddrSpec;
    }

    /*

    Remarks

    */
    GLOBAL SipAddrSpec* GetAddrSpec(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_UINT32 nIndex /* = 0 */)
    {
        SipHeader    stHeader;
        SipAddrSpec    *pstAddrSpec;

        //-----------------------------------------------------------------------------------------

        if (sip_getHeaderAtIndex(pstMessage, static_cast<en_HeaderType>(nType),
                &stHeader, nIndex, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if (sip_getAddrSpecFromCommonHdr(&stHeader, &pstAddrSpec, SIPStackError()) == SipFail)
        {
            FreeHeader(&stHeader);
            return IMS_NULL;
        }

        FreeHeader(&stHeader);

        return pstAddrSpec;
    }

    /*

    Remarks

    */
    GLOBAL AString GetChallengeScheme(IN SipHeader *pstHeader)
    {
        IMS_CHAR *pszScheme = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if ((pstHeader->dType != SipHdrTypeWwwAuthenticate)
                && (pstHeader->dType != SipHdrTypeProxyAuthenticate))
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        SIPStackError(E_NO_ERROR);

        if (pstHeader->dType == SipHdrTypeWwwAuthenticate)
        {
            if (sip_getSchemeFromWwwAuthenticateHdr(pstHeader,
                    &pszScheme, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }
        }
        else
        {
            if (sip_getSchemeFromProxyAuthenticateHdr(pstHeader,
                    &pszScheme, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }
        }

        return AString(pszScheme);
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetContent(IN SipMsgBody *pstMsgBody, OUT IMS_BYTE *&pContent,
            OUT IMS_SINT32 &nContentLength)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if ((pstMsgBody->dType != SipUnknownBody)
                || (pstMsgBody->u.pUnknownMessage == IMS_NULL))
        {
            IMS_TRACE_E(0, "SIP message body is not unknown body part!!!", 0, 0, 0);

            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        pContent = reinterpret_cast<IMS_BYTE*>(pstMsgBody->u.pUnknownMessage->pBuffer),
        nContentLength = static_cast<IMS_SINT32>(pstMsgBody->u.pUnknownMessage->dLength);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_UINT32 GetCSeqNumber(IN SipMessage *pstMessage)
    {
        IMS_UINT32 nSeqNum = SIPPrivate::INVALID_SEQ_NUM;

        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return SIPPrivate::INVALID_SEQ_NUM;
        }

        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeCseq);

        if (sip_getSeqNumFromCseqHdr(pstHeader, &nSeqNum, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return SIPPrivate::INVALID_SEQ_NUM;
        }

        FreeHeaderEx(pstHeader);

        return nSeqNum;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetRAckHeader(IN SipMessage *pstMessage, OUT IMS_UINT32 &nResponseNum,
            OUT IMS_UINT32 &nCSeqNum, OUT SIPMethod &objMethod)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeRAck);

        if (pstHeader == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if ((pstHeader->dType != SipHdrTypeRAck) || (pstHeader->pHeader == IMS_NULL))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        // Get the response number from RAck header
        if (sip_rpr_getRespNumFromRAckHdr(pstHeader, &nResponseNum, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        // Get the response number from RAck header
        if (sip_rpr_getCseqNumFromRAckHdr(pstHeader, &nCSeqNum, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        IMS_CHAR *pszMethod = IMS_NULL;

        if (sip_rpr_getMethodFromRAckHdr(pstHeader, &pszMethod, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        objMethod = pszMethod;

        FreeHeaderEx(pstHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetDestinationTransport(IN SipMessage *pstMessage)
    {
        SipAddrSpec *pstAddrSpec = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        // Check if the addr-spec which is set in the topmost Route header contains SIPS URI
        pstAddrSpec = GetAddrSpec(pstMessage, SipHdrTypeRoute, 0);

        if (pstAddrSpec != IMS_NULL)
        {
            if ((pstAddrSpec->dType == SipAddrSipSUri)
                    && HasParameter(pstAddrSpec, AString(SIP::STR_LR)))
            {
                FreeAddrSpec(pstAddrSpec);
                return SIP::TRANSPORT_TLS;
            }

            FreeAddrSpec(pstAddrSpec);
            pstAddrSpec = IMS_NULL;
        }

        pstAddrSpec = GetRequestUri(pstMessage);

        if (pstAddrSpec != IMS_NULL)
        {
            if (pstAddrSpec->dType == SipAddrSipSUri)
            {
                FreeAddrSpec(pstAddrSpec);
                return SIP::TRANSPORT_TLS;
            }

            FreeAddrSpec(pstAddrSpec);
        }

        return SIP::TRANSPORT_ANY;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetEventHeader(IN SipMessage *pstMessage, OUT AString &strEvent,
            OUT AString &strEventId)
    {
        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeEvent);

        //-----------------------------------------------------------------------------------------

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        IMS_CHAR *pszEvent = IMS_NULL;

        // Get the event package name & set the event type
        if (sip_getEventTypeFromEventHdr(pstHeader, &pszEvent, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        strEvent = pszEvent;

        // Get "id" parameter from Event header & set the event id
        strEventId = GetParameter(pstHeader, AString("id"));

        FreeHeaderEx(pstHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* GetHeader(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_UINT32 nIndex /* = 0 */)
    {
        SipHeader stHeader;

        //-----------------------------------------------------------------------------------------

        if (sip_getHeaderAtIndex(pstMessage, static_cast<en_HeaderType>(nType),
                &stHeader, nIndex, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        SipHeader *pstHeader = reinterpret_cast<SipHeader*>(SIP_MEM_ALLOC(sizeof(SipHeader)));

        if (pstHeader == IMS_NULL)
        {
            FreeHeader(&stHeader);
            return IMS_NULL;
        }

        pstHeader->dType = stHeader.dType;
        pstHeader->pHeader = stHeader.pHeader;

        return pstHeader;
    }

    /*

    Remarks

    */
    GLOBAL AString GetHeaderAsString(IN SipMessage *pstMessage, IN IMS_SINT32 nType,
            IN IMS_BOOL bParams/* = IMS_FALSE*/, IN IMS_UINT32 nIndex/* = 0*/)
    {
        SipHeader *pstHeader = GetHeader(pstMessage, nType, nIndex);

        //-----------------------------------------------------------------------------------------

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return AString::ConstNull();
        }

        AString strHeaderBody(AString::ConstNull());

        if (!EncodeHeaderBody(pstHeader, bParams, strHeaderBody))
        {
            FreeHeaderEx(pstHeader);
            return AString::ConstNull();
        }

        FreeHeaderEx(pstHeader);

        return strHeaderBody;
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetHeaderCount(IN SipMessage *pstMessage, IN IMS_SINT32 nType)
    {
        IMS_UINT32 nHCount;

        //-----------------------------------------------------------------------------------------

        if (sip_getHeaderCount(pstMessage,
                static_cast<en_HeaderType>(nType), &nHCount, SIPStackError()) == SipFail)
        {
            return 0;
        }

        return nHCount;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetHostAndPort(IN SipAddrSpec *pstAddrSpec, OUT AString &strHost,
            OUT IMS_UINT32 &nPort)
    {
        //-----------------------------------------------------------------------------------------

        nPort = SIP::PORT_UNSPECIFIED;

        if ((pstAddrSpec->dType == SipAddrSipUri)
                || (pstAddrSpec->dType == SipAddrSipSUri))
        {
            SipUrl *pstUrl;

            IMS_UINT16 nTmpPort;
            IMS_CHAR *pszHost = IMS_NULL;

            if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            (void) sip_getHostFromUrl(pstUrl, &pszHost, SIPStackError());

            strHost = pszHost;

            if (sip_getPortFromUrl(pstUrl, &nTmpPort, SIPStackError()) == SipSuccess)
            {
                nPort = nTmpPort;
            }

            sip_freeSipUrl(pstUrl);

            return IMS_TRUE;
        }
        else
        {
            SIPStackError(E_NO_EXIST);
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetHostNPortFromViaHeader(IN SipMessage *pstMessage,
            OUT AString &strHost, OUT IMS_SINT32 &nPort)
    {
        //-----------------------------------------------------------------------------------------

        strHost = AString::ConstNull();
        nPort = SIP::PORT_UNSPECIFIED;

        // Get the topmost Via header from the message
        SipHeader *pstViaHeader = GetHeader(pstMessage, SipHdrTypeVia);

        // Get "sent-by" field value from the Via header
        AString strViaField = GetSentByFromVia(pstViaHeader);

        if (strViaField.IsNULL())
        {
            FreeHeaderEx(pstViaHeader);
            return IMS_FALSE;
        }

        // Parse the host & port information from "sent-by" field
        ParseHostNPort(strViaField, strHost, nPort);

        if (nPort == SIP::PORT_UNSPECIFIED)
        {
            strViaField = GetSentProtocolFromVia(pstViaHeader);

            // "SIP/2.0/" : 8 characters
            strViaField = strViaField.Mid(8);

            if (strViaField.EqualsIgnoreCase(SIP::STR_TLS_CAPS))
                nPort = SIP::PORT_5061;
            else
                nPort = SIP::PORT_5060;
        }

        // Free the local reference
        FreeHeaderEx(pstViaHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SIPMethod GetMethod(IN SipMessage *pstMessage)
    {
        IMS_CHAR *pszMethod = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return SIPMethod();
        }

        if (pstMessage->dType == SipMessageRequest)
        {
            SipReqLine *pstReqLine;

            if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
            {
                return SIPMethod();
            }

            if (sip_getMethodFromReqLine(pstReqLine, &pszMethod, SIPStackError()) == SipFail)
            {
                sip_freeSipReqLine(pstReqLine);
                return SIPMethod();
            }

            sip_freeSipReqLine(pstReqLine);
        }
        else
        {
            SipHeader stHeader;

            if (sip_getHeader(pstMessage, SipHdrTypeCseq, &stHeader, SIPStackError()) == SipFail)
            {
                return SIPMethod();
            }

            if (sip_getMethodFromCseqHdr(&stHeader, &pszMethod, SIPStackError()) == SipFail)
            {
                FreeHeader(&stHeader);
                return SIPMethod();
            }

            FreeHeader(&stHeader);
        }

        return SIPMethod(pszMethod);
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetMessageBodyCount(IN SipMessage *pstMessage)
    {
        IMS_UINT32 nMBCount = 0;

        //-----------------------------------------------------------------------------------------

        if (sip_getMsgBodyCount(pstMessage, &nMBCount, SIPStackError()) == SipFail)
            return 0;

        return nMBCount;
    }

    /*

    Remarks

    */
    GLOBAL SipMsgBody* GetMessageBody(IN SipMessage *pstMessage, IN IMS_SINT32 nIndex /* = 0 */)
    {
        SipMsgBody *pstMsgBody = IMS_NULL;

        //---------------------------------------------------------------------------------------------

        if (sip_getMsgBodyAtIndex(pstMessage, &pstMsgBody, nIndex, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        return pstMsgBody;
    }

    /*

    Remarks

    */
    GLOBAL AString GetMIMEHeader(IN SipMsgBody *pstMsgBody, IN IMS_SINT32 nType,
            IN IMS_SINT32 nIndex /* = 0 */)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

#ifdef SIP_MIME_PARSING
        if (pstMsgBody->pMimeHeader == IMS_NULL)
        {
            SIPStackError(E_NO_EXIST);
            return AString::ConstNull();
        }

        switch (nType)
        {
        case SIPMessageBodyPart::CONTENT_TYPE:
            if (pstMsgBody->pMimeHeader->pContentType != IMS_NULL)
            {
                SipHeader stHeader = { SipHdrTypeContentType, \
                        pstMsgBody->pMimeHeader->pContentType };
                AString strHeader;

                EncodeHeaderBody(&stHeader, IMS_TRUE, strHeader);

                return strHeader;
            }
            break;

        case SIPMessageBodyPart::CONTENT_DISPOSITION:
            if (pstMsgBody->pMimeHeader->pContentDisposition != IMS_NULL)
            {
                SipHeader stHeader = { SipHdrTypeContentDisposition, \
                        pstMsgBody->pMimeHeader->pContentDisposition };
                AString strHeader;

                EncodeHeaderBody(&stHeader, IMS_TRUE, strHeader);

                return strHeader;
            }
            break;

        case SIPMessageBodyPart::CONTENT_TRANSFER_ENCODING:
            return AString(pstMsgBody->pMimeHeader->pContentTransEncoding);

        case SIPMessageBodyPart::CONTENT_ID:
            return AString(pstMsgBody->pMimeHeader->pContentId);

        case SIPMessageBodyPart::CONTENT_DESCRIPTION:
            return AString(pstMsgBody->pMimeHeader->pContentDescription);

        case SIPMessageBodyPart::CONTENT_UNKNOWN:
            {
                IMS_CHAR *pszMIMEHeader = IMS_NULL;

                if (List_GetAt(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                        nIndex, &pszMIMEHeader, SIPStackError()) == SipFail)
                {
                    break;
                }

                return AString(pszMIMEHeader);
            }
            break;

        default:
            break;
        }

        return AString::ConstNull();
#else
        return AString::ConstNull();
#endif // SIP_MIME_PARSING
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetMIMEHeaderCount(IN SipMsgBody *pstMsgBody, IN IMS_SINT32 nType)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return 0;
        }

#ifdef SIP_MIME_PARSING
        if (pstMsgBody->pMimeHeader == IMS_NULL)
        {
            SIPStackError(E_NO_EXIST);
            return 0;
        }

        SIPStackError(E_NO_ERROR);

        switch (nType)
        {
        case SIPMessageBodyPart::CONTENT_TYPE:
            if (pstMsgBody->pMimeHeader->pContentType != IMS_NULL)
                return 1;
            break;

        case SIPMessageBodyPart::CONTENT_DISPOSITION:
            if (pstMsgBody->pMimeHeader->pContentDisposition != IMS_NULL)
                return 1;
            break;

        case SIPMessageBodyPart::CONTENT_TRANSFER_ENCODING:
            if (pstMsgBody->pMimeHeader->pContentTransEncoding != IMS_NULL)
                return 1;
            break;

        case SIPMessageBodyPart::CONTENT_ID:
            if (pstMsgBody->pMimeHeader->pContentId != IMS_NULL)
                return 1;
            break;

        case SIPMessageBodyPart::CONTENT_DESCRIPTION:
            if (pstMsgBody->pMimeHeader->pContentDescription != IMS_NULL)
                return 1;
            break;

        case SIPMessageBodyPart::CONTENT_UNKNOWN:
            {
                IMS_UINT32 nHCount = 0;

                (void) List_SizeOf(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                        &nHCount, SIPStackError());

                return nHCount;
            }
            break;

        default:
            break;
        }

        return 0;
#else
        return 0;
#endif // SIP_MIME_PARSING
    }

    /*

    Remarks

    */
    GLOBAL AString GetParameter(IN SipAddrSpec *pstAddrSpec, IN CONST AString &strName,
            IN IMS_UINT32 nIndex /* = 0 */)
    {
        IMS_UINT32 nPCount = 0;

        //-----------------------------------------------------------------------------------------

        if ((pstAddrSpec->dType == SipAddrSipUri)
                || (pstAddrSpec->dType == SipAddrSipSUri))
        {
            SipUrl        *pstUrl;
            SipParam    *pstParam;

            if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }

            if (sip_getUrlParamCountFromUrl(pstUrl, &nPCount, SIPStackError()) == SipFail)
            {
                sip_freeSipUrl(pstUrl);
                return AString::ConstNull();
            }

            for (IMS_UINT32 i = 0; i < nPCount; i++)
            {
                pstParam = IMS_NULL;

                if (sip_getUrlParamAtIndexFromUrl(pstUrl,
                        &pstParam, i, SIPStackError()) == SipFail)
                {
                    break;
                }

                if (strName.EqualsIgnoreCase(pstParam->pName))
                {
                    IMS_CHAR *pszValue = IMS_NULL;

                    if (sip_getValueAtIndexFromSipParam(pstParam,
                            &pszValue, nIndex, SIPStackError()) == SipFail)
                    {
                        sip_freeSipParam(pstParam);
                        break;
                    }

                    sip_freeSipParam(pstParam);
                    sip_freeSipUrl(pstUrl);

                    return AString(pszValue);
                }

                sip_freeSipParam(pstParam);
            }

            sip_freeSipUrl(pstUrl);
        }
        else
        {
            SIPStackError(E_NO_EXIST);
        }

        return AString::ConstNull();
    }

    /*

    Remarks

    */
    GLOBAL AString GetParameter(IN SipHeader *pstHeader, IN CONST AString &strName,
            IN IMS_UINT32 nIndex /* = 0 */)
    {
        IMS_CHAR *pszValue = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (nIndex == 0)
        {
            if (sip_getParamForNameFromCommonHdr(pstHeader,
                    const_cast<SIP_S8bit*>(strName.GetStr()),
                    &pszValue, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }

            return AString(pszValue);
        }
        else
        {
            SipParam *pstParam;

            IMS_SINT32 nPCount = GetParameterCount(pstHeader);

            for (IMS_SINT32 i = 0; i < nPCount; i++)
            {
                pstParam = IMS_NULL;

                if (sip_getParamAtIndexFromCommonHdr(pstHeader,
                        &pstParam, i, SIPStackError()) == SipFail)
                {
                    break;
                }

                if (strName.EqualsIgnoreCase(pstParam->pName))
                {
                    pszValue = IMS_NULL;

                    if (sip_getValueAtIndexFromSipParam(pstParam,
                            &pszValue, nIndex, SIPStackError()) == SipFail)
                    {
                        sip_freeSipParam(pstParam);
                        break;
                    }

                    sip_freeSipParam(pstParam);

                    return AString(pszValue);
                }

                sip_freeSipParam(pstParam);
            }
        }

        return AString::ConstNull();
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetParameterCount(IN SipAddrSpec *pstAddrSpec)
    {
        IMS_UINT32 nPCount = 0;

        //-----------------------------------------------------------------------------------------

        if ((pstAddrSpec->dType == SipAddrSipUri)
                || (pstAddrSpec->dType == SipAddrSipSUri))
        {
            SipUrl *pstUrl;

            if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError()) == SipFail)
            {
                return 0;
            }

            if (sip_getUrlParamCountFromUrl(pstUrl, &nPCount, SIPStackError()) == SipFail)
            {
                sip_freeSipUrl(pstUrl);
                return 0;
            }

            sip_freeSipUrl(pstUrl);
        }
        else
        {
            SIPStackError(E_NO_EXIST);
        }

        return nPCount;
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetParameterCount(IN SipHeader *pstHeader)
    {
        IMS_UINT32 nPCount = 0;

        //-----------------------------------------------------------------------------------------

        if (sip_getParamCountFromCommonHdr(pstHeader, &nPCount, SIPStackError()) == SipFail)
            return 0;

        return nPCount;
    }

    /*

    Remarks

    */
    GLOBAL SipAddrSpec* GetRequestUri(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage->dType != SipMessageRequest)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        SipReqLine *pstReqLine;
        SipAddrSpec *pstAddrSpec;

        if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        if (sip_getAddrSpecFromReqLine(pstReqLine, &pstAddrSpec, SIPStackError()) == SipFail)
        {
            sip_freeSipReqLine(pstReqLine);
            return IMS_NULL;
        }

        sip_freeSipReqLine(pstReqLine);

        return pstAddrSpec;
    }

    /*

    Remarks

    */
    GLOBAL AString GetSentByFromVia(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if (pstHeader->dType != SipHdrTypeVia)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        IMS_CHAR *pszSentBy = IMS_NULL;

        if (sip_getSentByFromViaHdr(pstHeader, &pszSentBy, SIPStackError()) == SipFail)
        {
            return AString::ConstNull();
        }

        return AString(pszSentBy);
    }

    /*

    Remarks

    */
    GLOBAL AString GetSentProtocolFromVia(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if (pstHeader->dType != SipHdrTypeVia)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        IMS_CHAR *pszSentProtocol = IMS_NULL;

        if (sip_getSentProtocolFromViaHdr(pstHeader,
                &pszSentProtocol, SIPStackError()) == SipFail)
        {
            return AString::ConstNull();
        }

        return AString(pszSentProtocol);
    }

    /*

    Remarks

    */
    GLOBAL AString GetSIPVersion(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if (pstMessage->dType == SipMessageRequest)
        {
            SipReqLine *pstReqLine;

            IMS_CHAR *pszVersion = IMS_NULL;

            if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }

            if (sip_getVersionFromReqLine(pstReqLine, &pszVersion, SIPStackError()) == SipFail)
            {
                sip_freeSipReqLine(pstReqLine);
                return AString::ConstNull();
            }

            AString strVersion(pszVersion);

            sip_freeSipReqLine(pstReqLine);

            return strVersion;
        }
        else
        {
            SipStatusLine *pstStatusLine;

            IMS_CHAR *pszVersion = IMS_NULL;

            if (sip_getStatusLine(pstMessage, &pstStatusLine, SIPStackError()) == SipFail)
            {
                return AString::ConstNull();
            }

            if (sip_getVersionFromStatusLine(pstStatusLine,
                    &pszVersion, SIPStackError()) == SipFail)
            {
                sip_freeSipStatusLine(pstStatusLine);
                return AString::ConstNull();
            }

            AString strVersion(pszVersion);

            sip_freeSipStatusLine(pstStatusLine);

            return strVersion;
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetStatusCode(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return SIPStatusCode::SC_INVALID;
        }

        if (pstMessage->dType != SipMessageResponse)
        {
            SIPStackError(E_INV_PARAM);
            return SIPStatusCode::SC_INVALID;
        }

        SipStatusLine *pstStatusLine;
        IMS_UINT32 nStatusCode = 0;

        if (sip_getStatusLine(pstMessage, &pstStatusLine, SIPStackError()) == SipFail)
        {
            return SIPStatusCode::SC_INVALID;
        }

        if (sip_getStatusCodeNumFromStatusLine(pstStatusLine,
                &nStatusCode, SIPStackError()) == SipFail)
        {
            sip_freeSipStatusLine(pstStatusLine);
            return SIPStatusCode::SC_INVALID;
        }

        sip_freeSipStatusLine(pstStatusLine);

        return static_cast<IMS_SINT32>(nStatusCode);
    }

    /*

    Remarks

    */
    GLOBAL SIPStatusCode GetStatusCodeEx(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return SIPStatusCode();
        }

        if (pstMessage->dType != SipMessageResponse)
        {
            SIPStackError(E_INV_PARAM);
            return SIPStatusCode();
        }

        SipStatusLine *pstStatusLine;

        IMS_UINT32 nStatusCode = 0;
        IMS_CHAR *pszReasonPhrase = IMS_NULL;

        if (sip_getStatusLine(pstMessage, &pstStatusLine, SIPStackError()) == SipFail)
        {
            return SIPStatusCode();
        }

        if (sip_getStatusCodeNumFromStatusLine(pstStatusLine,
                &nStatusCode, SIPStackError()) == SipFail)
        {
            sip_freeSipStatusLine(pstStatusLine);
            return SIPStatusCode();
        }

        // If reason-phrase doesn't exist, it can be NULL.
        sip_getReasonFromStatusLine(pstStatusLine,
                &pszReasonPhrase, SIPStackError());

        sip_freeSipStatusLine(pstStatusLine);

        return SIPStatusCode(static_cast<IMS_SINT32>(nStatusCode), pszReasonPhrase);
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL GetSubscriptionStateHeader(IN SipMessage *pstMessage,
            OUT AString &strSubsState, OUT IMS_SINT32 *pnExpires /* = IMS_NULL */)
    {
        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeSubscriptionState);

        //-----------------------------------------------------------------------------------------

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        // Set the subs-state
        IMS_CHAR *pszSubsState = IMS_NULL;

        if (sip_impp_getSubStateFromSubscriptionStateHdr(pstHeader,
                &pszSubsState, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        strSubsState = pszSubsState;

        // Set the expires parameter
        if (pnExpires != IMS_NULL)
        {
            AString strExpires = GetParameter(pstHeader, AString("expires"));

            if (strExpires.IsNULL())
            {
                (*pnExpires) = (-1);
            }
            else
            {
                IMS_BOOL bOK = IMS_TRUE;

                (*pnExpires) = strExpires.ToInt32(&bOK);

                if (!bOK)
                {
                    (*pnExpires) = (-1);
                }
            }
        }

        FreeHeaderEx(pstHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SipHeader* GetUnknownHeader(IN SipMessage *pstMessage, IN CONST AString &strName,
            IN IMS_UINT32 nIndex /* = 0 */)
    {
        //-----------------------------------------------------------------------------------------

        (void) nIndex;

        IMS_SINT32 nHCount = GetHeaderCount(pstMessage, ISIPHeader::UNKNOWN);

        if (nHCount == 0)
        {
            return IMS_NULL;
        }

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, ISIPHeader::UNKNOWN, i);

            if (pstHeader == IMS_NULL)
            {
                continue;
            }

            SipCommonHeader *pstCommHeader = static_cast<SipCommonHeader*>(pstHeader->pHeader);

            if (pstCommHeader == IMS_NULL)
            {
                FreeHeaderEx(pstHeader);
                continue;
            }

            if (strName.EqualsIgnoreCase(pstCommHeader->pStr1))
            {
                return pstHeader;
            }

            FreeHeaderEx(pstHeader);
        }

        return IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL AString GetUnknownHeaderName(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if ((pstHeader->dType != SipHdrTypeUnknown)
                || (pstHeader->pHeader == IMS_NULL))
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        SIPStackError(E_NO_ERROR);

        return AString((static_cast<SipCommonHeader*>(pstHeader->pHeader))->pStr1);
    }

    /*

    Remarks

    */
    GLOBAL AString GetUnknownHeaderBody(IN SipHeader *pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        if ((pstHeader->dType != SipHdrTypeUnknown)
                || (pstHeader->pHeader == IMS_NULL))
        {
            SIPStackError(E_INV_PARAM);
            return AString::ConstNull();
        }

        SIPStackError(E_NO_ERROR);

        return AString((static_cast<SipCommonHeader*>(pstHeader->pHeader))->pStr2);
    }

    /*

    Remarks

    */
    GLOBAL AString GetViaBranchParameter(IN SipMessage *pstMessage)
    {
        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeVia);

        //-----------------------------------------------------------------------------------------

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return AString::ConstNull();
        }

        AString strViaBranch = GetParameter(pstHeader, SIP::STR_BRANCH);

        FreeHeaderEx(pstHeader);

        return strViaBranch;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL HasParameter(IN SipHeader *pstHeader, IN CONST AString &strName)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        IMS_SINT32 nPCount = GetParameterCount(pstHeader);

        for (IMS_SINT32 i = 0; i < nPCount; ++i)
        {
            SipParam *pstParam = IMS_NULL;

            if (sip_getParamAtIndexFromCommonHdr(pstHeader,
                    &pstParam, i, SIPStackError()) == SipSuccess)
            {
                if (strName.EqualsIgnoreCase(pstParam->pName))
                {
                    sip_freeSipParam(pstParam);
                    return IMS_TRUE;
                }

                sip_freeSipParam(pstParam);
            }
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL HasParameter(IN SipAddrSpec *pstAddrSpec, IN CONST AString &strName)
    {
        //-----------------------------------------------------------------------------------------

        if (pstAddrSpec == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if ((pstAddrSpec->dType == SipAddrSipUri) || (pstAddrSpec->dType == SipAddrSipSUri))
        {
            SipUrl *pstUrl;

            IMS_UINT32 nPCount = 0;

            if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            (void) sip_getUrlParamCountFromUrl(pstUrl, &nPCount, SIPStackError());

            if (nPCount > 0)
            {
                SipParam *pstParam;

                for (IMS_UINT32 i = 0; i < nPCount; ++i)
                {
                    if (sip_getUrlParamAtIndexFromUrl(pstUrl,
                            &pstParam, i, SIPStackError()) == SipFail)
                    {
                        continue;
                    }

                    if (strName.EqualsIgnoreCase(pstParam->pName))
                    {
                        sip_freeSipParam(pstParam);
                        sip_freeSipUrl(pstUrl);
                        return IMS_TRUE;
                    }

                    sip_freeSipParam(pstParam);
                }
            }

            sip_freeSipUrl(pstUrl);
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL HasMIMEMessageBody(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr->pContentTypeHdr == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr->pContentTypeHdr->pStr1 == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (SIP_STRICMPN(pstMessage->pGeneralHdr->pContentTypeHdr->pStr1,
                SIP::STR_MULTIPART, 9) != 0)
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL HasSDPMessageBody(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr->pContentTypeHdr == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (pstMessage->pGeneralHdr->pContentTypeHdr->pStr1 == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (SIP_STRICMP(pstMessage->pGeneralHdr->pContentTypeHdr->pStr1,
                SIP::STR_APPLICATION_SDP) != 0)
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL InsertHeader(IN SipHeader *pstHeader, IN IMS_UINT32 nIndex,
        IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (IsSingleHeader(pstHeader->dType))
        {
            if (sip_setHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }
        else
        {
            if (sip_insertHeaderAtIndex(pstMessage, pstHeader, nIndex, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsCompactHeaderNameSupported(IN IMS_SINT32 nType,
            IN CONST AString &strName /* = AString::ConstNull() */)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        case SipHdrTypeCallId:
        case SipHdrTypeContactAny:
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
        case SipHdrTypeContentEncoding:
        case SipHdrTypeContentLength:
        case SipHdrTypeContentType:
        case SipHdrTypeFrom:
        case SipHdrTypeSupported:
        case SipHdrTypeTo:
        case SipHdrTypeVia:
#ifdef SIP_IMPP
        case SipHdrTypeEvent:
        case SipHdrTypeAllowEvents:
#endif
#ifdef SIP_REFER
        case SipHdrTypeReferTo:
        case SipHdrTypeReferredBy:
#endif
#ifdef SIP_3GPP
        case SipHdrTypeRequestDisposition:
        case SipHdrTypeAcceptContact:
        case SipHdrTypeRejectContact:
#endif
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeSessionExpires:
#endif
            return IMS_TRUE;

        case SipHdrTypeUnknown:
            switch (strName[0])
            {
            case 's':
            case 'S':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_SUBJECT)
                    || strName.EqualsIgnoreCase(SIPHeaderName::SUBJECT))
                {
                    return IMS_TRUE;
                }
                break;

            case 'i':
            case 'I':
                if (strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY))
                {
                    return IMS_TRUE;
                }
                else if (strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY_INFO))
                {
                    return IMS_TRUE;
                }
                break;

            case 'n':
            case 'N':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY_INFO))
                {
                    return IMS_TRUE;
                }
                break;

            case 'y':
            case 'Y':
                if (strName.EqualsIgnoreCase(SIPHeaderName::CF_IDENTITY))
                {
                    return IMS_TRUE;
                }
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsHeaderPresent(IN SipMessage *pstMessage, IN IMS_SINT32 nType)
    {
        SipHeader stHeader = { static_cast<en_HeaderType>(nType), SIP_NULL };

        //-----------------------------------------------------------------------------------------

        if (sip_getHeader(pstMessage, stHeader.dType, &stHeader, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        FreeHeader(&stHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsMessageBodySDP(IN SipMsgBody *pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        SIPStackError(E_NO_ERROR);

#ifdef SIP_MIME_PARSING
        if (pstMsgBody->pMimeHeader == IMS_NULL)
            return IMS_FALSE;

        if (pstMsgBody->pMimeHeader->pContentType == IMS_NULL)
            return IMS_FALSE;

        if (pstMsgBody->pMimeHeader->pContentType->pStr1 == IMS_NULL)
            return IMS_FALSE;

        if (SIP_STRICMP(pstMsgBody->pMimeHeader->pContentType->pStr1,
                SIP::STR_APPLICATION_SDP) == 0)
        {
            return IMS_TRUE;
        }
#endif

        if (pstMsgBody->dType == SipSdpBody)
            return IMS_TRUE;

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsMessageBodyCompressed(IN SipMessage *pstMessage)
    {
        //4 Check if the Content-Encoding contains the compression algorithm or not
        //4 As default, consider it to 'gzip'. It needs to be consider other compression algorithm.
        SipHeader *pstContentEncoding = GetHeader(pstMessage, SipHdrTypeContentEncoding);

        //-----------------------------------------------------------------------------------------

        if (IsValidHeader(pstContentEncoding))
        {
            IMS_CHAR *pszEncoding = IMS_NULL;

            sip_getEncodingFromContentEncodingHdr(
                    pstContentEncoding, &pszEncoding, SIPStackError());

            AString strEncoding(pszEncoding);

            if (strEncoding.EqualsIgnoreCase("gzip"))
            {
                FreeHeaderEx(pstContentEncoding);
                return IMS_TRUE;
            }
        }

        FreeHeaderEx(pstContentEncoding);

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsMessageRPR(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        // If the message is not of type response, return FALSE

        if (pstMessage->dType != SipMessageResponse)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_SINT32 nStatusCode = GetStatusCode(pstMessage);

        // The message has a status code greater than 199; It is a final response.
        if ((nStatusCode <= SIPStatusCode::SC_100)
                || (nStatusCode >= SIPStatusCode::SC_200))
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        // If the message is a provisional response, but does not contain a RSeq header,
        // then return FALSE.
        IMS_SINT32 nRSeqCount = GetHeaderCount(pstMessage, SipHdrTypeRSeq);

        if (nRSeqCount == 0)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_BOOL bOptionTag100rel = IMS_FALSE;
        IMS_CHAR *pszOptionTag = IMS_NULL;
        IMS_SINT32 nHCount = GetHeaderCount(pstMessage, SipHdrTypeRequire);

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeRequire, i);

            if (sip_getTokenFromRequireHdr(pstHeader,
                    &pszOptionTag, SIPStackError()) == SipSuccess)
            {
                if (AString::CompareIgnoreCase(pszOptionTag, SIP::STR_100REL) == 0)
                {
                    bOptionTag100rel = IMS_TRUE;
                    FreeHeaderEx(pstHeader);
                    break;
                }
            }

            FreeHeaderEx(pstHeader);
        }

        SIPStackError(E_NO_ERROR);

        // 101 ~ 199 response, RSeq header, option-tag ("100rel") in Require header
        if ((nRSeqCount > 0) && (bOptionTag100rel))
        {
            return IMS_TRUE;
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsOptionRequired(IN SipMessage *pstMessage, IN CONST AString &strOption)
    {
        //---------------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_SINT32 nCount = GetHeaderCount(pstMessage, SipHdrTypeRequire);

        if (nCount == 0)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_CHAR *pszOptionTag = IMS_NULL;

        for (IMS_SINT32 i = 0; i < nCount; ++i)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeRequire, i);

            if (sip_getTokenFromRequireHdr(pstHeader,
                    &pszOptionTag, SIPStackError()) == SipSuccess)
            {
                if (strOption.EqualsIgnoreCase(pszOptionTag))
                {
                    FreeHeaderEx(pstHeader);
                    SIPStackError(E_NO_ERROR);
                    return IMS_TRUE;
                }
            }

            FreeHeaderEx(pstHeader);
        }

        SIPStackError(E_NO_ERROR);

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsOptionSupported(IN SipMessage *pstMessage, IN CONST AString &strOption)
    {
        //---------------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_SINT32 nCount = GetHeaderCount(pstMessage, SipHdrTypeSupported);

        if (nCount == 0)
        {
            SIPStackError(E_NO_ERROR);
            return IMS_FALSE;
        }

        IMS_CHAR *pszOptionTag = IMS_NULL;

        for (IMS_SINT32 i = 0; i < nCount; ++i)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeSupported, i);

            if (sip_getOptionFromSupportedHdr(pstHeader,
                    &pszOptionTag, SIPStackError()) == SipSuccess)
            {
                if ((AString::Compare(pszOptionTag, "*") == 0)
                        || strOption.EqualsIgnoreCase(pszOptionTag))
                {
                    FreeHeaderEx(pstHeader);
                    SIPStackError(E_NO_ERROR);
                    return IMS_TRUE;
                }
            }

            FreeHeaderEx(pstHeader);
        }

        SIPStackError(E_NO_ERROR);

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsRequestMessage(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        return (pstMessage != IMS_NULL) ? (pstMessage->dType == SipMessageRequest) : IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsSingleHeader(IN IMS_SINT32 nType)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        // General headers
        case SipHdrTypeCallId:
        case SipHdrTypeCseq:
#ifdef SIP_DATE
        case SipHdrTypeExpiresDate:
        case SipHdrTypeExpiresSec:
        case SipHdrTypeExpiresAny:
#else
        case SipHdrTypeExpires:
#endif
        case SipHdrTypeFrom:
#ifdef SIP_TIMESTAMP
        case SipHdrTypeTimestamp:
#endif
        case SipHdrTypeTo:
        case SipHdrTypeContentLength:
        case SipHdrTypeContentType:
        case SipHdrTypeMimeVersion:
#ifdef SIP_DATE
        case SipHdrTypeRetryAfterDate:
        case SipHdrTypeRetryAfterSec:
        case SipHdrTypeRetryAfterAny:
#else
        case SipHdrTypeRetryAfter:
#endif
#ifdef SIP_PRIVACY
        case SipHdrTypePrivacy:
#endif
#ifdef SIP_SESSIONTIMER
        case SipHdrTypeSessionExpires:
        case SipHdrTypeMinSE:
#endif
#ifdef SIP_3GPP
        case SipHdrTypePcfAddr:
        case SipHdrTypePanInfo:
        case SipHdrTypePcVector:
#endif
#if defined(SIP_DATE) && defined(SIP_IMS_DATE)
        case SipHdrTypeDate:
#endif

            // Request headers
#ifdef SIP_RPR
        case SipHdrTypeRAck:
#endif
#ifdef SIP_REFER
        case SipHdrTypeReferTo:
        case SipHdrTypeReferredBy:
#endif
#ifdef SIP_IMPP
        case SipHdrTypeEvent:
        case SipHdrTypeSubscriptionState:
#endif
#ifdef SIP_REPLACES
        case SipHdrTypeReplaces:
#endif
#ifdef SIP_3GPP
        case SipHdrTypePCalledPartyId:
        case SipHdrTypeJoin:
        case SipHdrTypeIfMatch:
#endif
        case SipHdrTypeMaxforwards:

            // Response headers
#ifdef SIP_RPR
        case SipHdrTypeRSeq:
#endif
#ifdef SIP_3GPP
        case SipHdrTypeETag:
#endif
        case SipHdrTypeMinExpires:
            break;

        default:
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsAddressFormatHeader(IN IMS_SINT32 nType, IN CONST AString &strName)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        case SipHdrTypeContactNormal:
        case SipHdrTypeContactWildCard:
        case SipHdrTypeContactAny:
        case SipHdrTypeFrom:
        case SipHdrTypePPreferredId:
        case SipHdrTypePAssertId:
        case SipHdrTypePath:
        case SipHdrTypePAssociatedUri:
        case SipHdrTypePCalledPartyId:
        case SipHdrTypeServiceRoute:
        case SipHdrTypeHistoryInfo:
        case SipHdrTypeRecordRoute:
        case SipHdrTypeReferredBy:
        case SipHdrTypeReferTo:
        case SipHdrTypeRoute:
        case SipHdrTypeTo:
            return IMS_TRUE;

        case SipHdrTypeUnknown:
            if (strName.EqualsIgnoreCase(SIPHeaderName::DIVERSION)
                    || strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY_INFO)
                    || strName.Equals(SIPHeaderName::CF_IDENTITY_INFO)
                    || strName.EqualsIgnoreCase(SIPHeaderName::P_SERVED_USER)
                    || strName.EqualsIgnoreCase(SIPHeaderName::POLICY_CONTACT)
                    || strName.EqualsIgnoreCase(SIPHeaderName::POLICY_ID)
                    || strName.EqualsIgnoreCase(SIPHeaderName::REPLY_TO)
                    || strName.EqualsIgnoreCase(SIPHeaderName::TRIGGER_CONSENT))
            {
                return IMS_TRUE;
            }
            break;

        default:
            break;
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsAQUOTRequiredForAddressFormat(
            IN IMS_SINT32 nType, IN CONST AString &strName)
    {
        //-----------------------------------------------------------------------------------------

        switch (nType)
        {
        case SipHdrTypeUnknown:
            if (strName.EqualsIgnoreCase(SIPHeaderName::IDENTITY_INFO)
                    || strName.Equals(SIPHeaderName::CF_IDENTITY_INFO)
                    || strName.EqualsIgnoreCase(SIPHeaderName::POLICY_CONTACT))
            {
                return IMS_TRUE;
            }
            break;

        default:
            break;
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsUriSchemeAllowed(IN SipHeader *pstHeader)
    {
        SipAddrSpec *pstAddrSpec = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (sip_getAddrSpecFromCommonHdr(pstHeader, &pstAddrSpec, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        if (pstAddrSpec->dType == SipAddrReqUri)
        {
            IMS_BOOL bIsTelUrl = IMS_FALSE;
            IMS_BOOL bIsIMUrl = IMS_FALSE;
            IMS_BOOL bIsPresUrl = IMS_FALSE;

            if (sip_isTelUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                bIsTelUrl = IMS_TRUE;

            if (bIsTelUrl != IMS_TRUE)
            {
                if (sip_isImUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                    bIsIMUrl = IMS_TRUE;
            }

            if ((bIsTelUrl != IMS_TRUE) && (bIsIMUrl != IMS_TRUE))
            {
                if (sip_isPresUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                    bIsPresUrl = IMS_TRUE;
            }

            if ((bIsTelUrl == IMS_TRUE) || (bIsIMUrl == IMS_TRUE) || (bIsPresUrl == IMS_TRUE))
            {
                FreeAddrSpec(pstAddrSpec);
                // Not Allowed
                return IMS_FALSE;
            }
        }

        FreeAddrSpec(pstAddrSpec);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL IsUriSchemeSupported(IN SipHeader *pstHeader)
    {
        SipAddrSpec *pstAddrSpec = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        SIPPrivate::SetLastError(SIPError::NO_ERROR);

        if (sip_getAddrSpecFromCommonHdr(pstHeader, &pstAddrSpec, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        if ((pstAddrSpec->dType != SipAddrSipUri) && (pstAddrSpec->dType != SipAddrSipSUri))
        {
            IMS_BOOL bIsTelUrl = IMS_FALSE;
            IMS_BOOL bIsIMUrl = IMS_FALSE;
            IMS_BOOL bIsPresUrl = IMS_FALSE;

            if (sip_isTelUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                bIsTelUrl = IMS_TRUE;

            if (bIsTelUrl != IMS_TRUE)
            {
                if (sip_isImUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                    bIsIMUrl = IMS_TRUE;
            }

            if ((bIsTelUrl != IMS_TRUE) && (bIsIMUrl != IMS_TRUE))
            {
                if (sip_isPresUrl(pstAddrSpec, SIPStackError()) == SipSuccess)
                    bIsPresUrl = IMS_TRUE;
            }

            if ((bIsTelUrl != IMS_TRUE) && (bIsIMUrl != IMS_TRUE) && (bIsPresUrl != IMS_TRUE))
            {
                FreeAddrSpec(pstAddrSpec);

                SIPPrivate::SetLastError(SIPError::URI_SCHEME_NOT_SUPPORTED);
                return IMS_FALSE;
            }
        }

        FreeAddrSpec(pstAddrSpec);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL OverwriteHeaders(IN SipMessage *pstSrcMessage,
            IN_OUT SipMessage *&pstDestMessage)
    {
        SipHeader stSipHdr;
        SipHeaderOrderInfo *pstOrderInfo;

        IMS_UINT32 nHeaderLineCount = 0;

        //-----------------------------------------------------------------------------------------

        if (sip_getHeaderLineCount(pstSrcMessage, &nHeaderLineCount, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        if (nHeaderLineCount == 0)
        {
            return IMS_TRUE;
        }

        for (IMS_UINT32 i = 0; i < nHeaderLineCount; ++i)
        {
            if (List_GetAt(&(pstSrcMessage->slOrderInfo),
                    i, &pstOrderInfo, SIPStackError()) == SipFail)
            {
                continue;
            }

            // Skip unknown headers if present ...
            if (pstOrderInfo->dType == SipHdrTypeUnknown)
                continue;

            if (pstOrderInfo->dNum == 0)
                continue;

            (void) sip_deleteAllHeaderType(pstDestMessage, pstOrderInfo->dType, SIPStackError());

            for (IMS_UINT32 j = 0; j < pstOrderInfo->dNum; ++j)
            {
                if (sip_getHeaderAtIndex(pstSrcMessage,
                        pstOrderInfo->dType, &stSipHdr, j, SIPStackError()) == SipFail)
                {
                    continue;
                }

                (void) sip_insertHeaderAtIndex(pstDestMessage, &stSipHdr, j, SIPStackError());

                FreeHeader(&stSipHdr);
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL void ParseHostNPort(IN CONST AString &strHostNPort, OUT AString &strHost,
            OUT IMS_SINT32 &nPort)
    {
        IMS_SINT32 nPos;

        //-----------------------------------------------------------------------------------------

        nPort = SIP::PORT_UNSPECIFIED;

        // We need to take care of '[', ']' while extracting the host from IPv6 reference
        if ((nPos = strHostNPort.GetIndexOf(TextParser::CHAR_LSBRACKET)) != AString::NPOS)
        {
            // Strip the '[' , ']' before resolving the address.
            IMS_SINT32 nEndOfHost = strHostNPort.GetIndexOf(TextParser::CHAR_RSBRACKET);

            strHost = strHostNPort.GetSubStr(nPos + 1, nEndOfHost - nPos - 1);

            nPos = strHostNPort.GetIndexOf(TextParser::CHAR_COLON, nEndOfHost + 1);

            if (nPos != AString::NPOS)
            {
                AString strPort = strHostNPort.GetSubStr(nPos + 1, strHostNPort.GetLength() - nPos);

                IMS_BOOL bOK = IMS_FALSE;
                IMS_UINT16 nTmpPort = strPort.ToUInt16(&bOK);

                if (bOK)
                {
                    nPort = nTmpPort;
                }
            }
        }
        else
        {
            nPos = strHostNPort.GetIndexOf(TextParser::CHAR_COLON);

            if (nPos != AString::NPOS)
            {
                strHost = strHostNPort.GetSubStr(0, nPos);
                AString strPort = strHostNPort.GetSubStr(nPos + 1, strHostNPort.GetLength() - nPos);

                IMS_BOOL bOK = IMS_FALSE;
                IMS_UINT16 nTmpPort = strPort.ToUInt16(&bOK);

                if (bOK)
                {
                    nPort = nTmpPort;
                }
            }
            else
            {
                strHost = strHostNPort;
            }
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL RemoveAllMessageBodies(IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (List_DeleteAll(&(pstMessage->slMessageBody), SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL RemoveHeader(IN IMS_SINT32 nType, IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (sip_deleteHeaderAtIndex(pstMessage,
                static_cast<en_HeaderType>(nType), 0, SIPStackError()) == SipFail)
        {
            if (GetLastError() != E_NO_EXIST)
                return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL RemoveParameter(IN CONST AString &strName, IN_OUT SipHeader *&pstHeader)
    {
        //-----------------------------------------------------------------------------------------

        if (sip_deleteParamForNameInCommonHdr(pstHeader,
                const_cast<SIP_S8bit*>(strName.GetStr()), SIPStackError()) == SipFail)
        {
            if (GetLastError() != E_NO_EXIST)
                return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL RemoveParameter(IN CONST AString &strName, IN_OUT SipAddrSpec *&pstAddrSpec)
    {
        IMS_UINT32    nPCount = 0;

        //---------------------------------------------------------------------------------------------

        if ((pstAddrSpec->dType == SipAddrSipUri)
                || (pstAddrSpec->dType == SipAddrSipSUri))
        {
            SipUrl        *pstUrl;
            SipParam    *pstParam;

            if (sip_getUrlFromAddrSpec(pstAddrSpec, &pstUrl, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            if (sip_getUrlParamCountFromUrl(pstUrl, &nPCount, SIPStackError()) == SipFail)
            {
                sip_freeSipUrl(pstUrl);
                return IMS_FALSE;
            }

            for (IMS_UINT32 i = 0; i < nPCount; ++i)
            {
                if (sip_getUrlParamAtIndexFromUrl(pstUrl,
                        &pstParam, i, SIPStackError()) == SipFail)
                {
                    break;
                }

                if (strName.EqualsIgnoreCase(pstParam->pName))
                {
                    (void) sip_deleteUrlParamAtIndexInUrl(pstUrl, i, SIPStackError());

                    sip_freeSipParam(pstParam);
                    sip_freeSipUrl(pstUrl);

                    return IMS_TRUE;
                }

                sip_freeSipParam(pstParam);
            }

            sip_freeSipUrl(pstUrl);
        }
        else
        {
            SIPStackError(E_NO_EXIST);
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL void RemoveUserAndPassword(IN_OUT SipAddrSpec *&pstAddrSpec)
    {
        //---------------------------------------------------------------------------------------------

        if ((pstAddrSpec != IMS_NULL)
                && ((pstAddrSpec->dType == SipAddrSipUri)
                    || (pstAddrSpec->dType == SipAddrSipSUri)))
        {
            Free(pstAddrSpec->u.pSipUrl->pUser);
            Free(pstAddrSpec->u.pSipUrl->pPassword);
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetChallengeScheme(IN CONST AString &strScheme, IN_OUT SipHeader *&pstHeader)
    {
        //---------------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if ((pstHeader->dType != SipHdrTypeAuthorization)
                && (pstHeader->dType != SipHdrTypeProxyauthorization))
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        SIPStackError(E_NO_ERROR);

        IMS_CHAR *pszScheme = strScheme.Duplicate();

        if (pstHeader->dType == SipHdrTypeAuthorization)
        {
            if (sip_setSchemeInAuthorizationHdr(pstHeader, pszScheme, SIPStackError()) == SipFail)
            {
                Free(pszScheme);
                return IMS_FALSE;
            }
        }
        else
        {
            if (sip_setSchemeInProxyAuthorizationHdr(pstHeader,
                    pszScheme, SIPStackError()) == SipFail)
            {
                Free(pszScheme);
                return IMS_FALSE;
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetContent(IN CONST IMS_BYTE *pContent, IN IMS_SINT32 nContentLength,
            IN_OUT SipMsgBody *&pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (pstMsgBody->dType != SipUnknownBody)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        IMS_CHAR *pBuffer = static_cast<IMS_CHAR*>(SIP_MEM_ALLOC(nContentLength + 1));

        if (pBuffer == IMS_NULL)
        {
            SIPStackError(E_NO_MEM);
            return IMS_FALSE;
        }

        IMS_MEM_Memcpy(pBuffer, pContent, nContentLength);
        pBuffer[nContentLength] = '\0';

        if (sip_setBufferInUnknownMessage(pstMsgBody->u.pUnknownMessage,
                pBuffer, nContentLength, SIPStackError()) == SipFail)
        {
            Free(pBuffer);
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetHeader(IN SipHeader *pstHeader, IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        if (IsSingleHeader(pstHeader->dType))
        {
            if (sip_setHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }
        }
        else
        {
            IMS_SINT32 nHCount = GetHeaderCount(pstMessage, pstHeader->dType);

            if (nHCount > 0)
            {
                if (sip_setHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
                {
                    return IMS_FALSE;
                }
            }
            else
            {
                if (sip_insertHeaderAtIndex(pstMessage, pstHeader, 0, SIPStackError()) == SipFail)
                {
                    return IMS_FALSE;
                }
            }
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetMethod(IN CONST SIPMethod &objMethod, IN_OUT SipMessage *&pstMessage)
    {
        IMS_CHAR *pszMethod;

        //-----------------------------------------------------------------------------------------

        if (pstMessage->dType == SipMessageRequest)
        {
            SipReqLine *pstReqLine;

            if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            // Set method
            pszMethod = objMethod.ToString().Duplicate();

            if (sip_setMethodInReqLine(pstReqLine, pszMethod, SIPStackError()) == SipFail)
            {
                Free(pszMethod);
                sip_freeSipReqLine(pstReqLine);
                return IMS_FALSE;
            }

            sip_freeSipReqLine(pstReqLine);
        }
        else
        {
            SipHeader stHeader;

            if (sip_getHeader(pstMessage, SipHdrTypeCseq, &stHeader, SIPStackError()) == SipFail)
            {
                if (GetLastError() == E_NO_EXIST)
                    return IMS_TRUE;

                return IMS_FALSE;
            }

            // Set method
            pszMethod = objMethod.ToString().Duplicate();

            if (sip_setMethodInCseqHdr(&stHeader, pszMethod, SIPStackError()) == SipFail)
            {
                Free(pszMethod);
                FreeHeader(&stHeader);
                return IMS_FALSE;
            }

            FreeHeader(&stHeader);
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetMIMEHeader(IN IMS_SINT32 nType, IN SipHeader *pstHeader,
            IN_OUT SipMsgBody *&pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        if ((pstMsgBody == IMS_NULL)
                || (pstHeader == IMS_NULL))
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

#ifdef SIP_MIME_PARSING
        if (pstMsgBody->pMimeHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        SIPStackError(E_NO_ERROR);

        switch (nType)
        {
        case SIPMessageBodyPart::CONTENT_TYPE:
            {
                if (pstMsgBody->pMimeHeader->pContentType != IMS_NULL)
                    sip_freeSipCommonHeader(pstMsgBody->pMimeHeader->pContentType);

                pstMsgBody->pMimeHeader->pContentType
                        = static_cast<SipCommonHeader*>(pstHeader->pHeader);
                AddReference(pstMsgBody->pMimeHeader->pContentType);
            }
            break;

        case SIPMessageBodyPart::CONTENT_DISPOSITION:
            {
                if (pstMsgBody->pMimeHeader->pContentDisposition != IMS_NULL)
                    sip_freeSipCommonHeader(pstMsgBody->pMimeHeader->pContentDisposition);

                pstMsgBody->pMimeHeader->pContentDisposition
                        = static_cast<SipCommonHeader*>(pstHeader->pHeader);
                AddReference(pstMsgBody->pMimeHeader->pContentDisposition);
            }
            break;

        case SIPMessageBodyPart::CONTENT_TRANSFER_ENCODING:
            {
                SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

                if (pSCH == IMS_NULL)
                    break;

                if (pstMsgBody->pMimeHeader->pContentTransEncoding != IMS_NULL)
                    Free(pstMsgBody->pMimeHeader->pContentTransEncoding);

                pstMsgBody->pMimeHeader->pContentTransEncoding = sip_strdup(pSCH->pStr2, 0);
            }
            break;

        case SIPMessageBodyPart::CONTENT_ID:
            {
                SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

                if (pSCH == IMS_NULL)
                    break;

                if (pstMsgBody->pMimeHeader->pContentId != IMS_NULL)
                    Free(pstMsgBody->pMimeHeader->pContentId);

                pstMsgBody->pMimeHeader->pContentId = sip_strdup(pSCH->pStr2, 0);
            }
            break;

        case SIPMessageBodyPart::CONTENT_DESCRIPTION:
            {
                SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

                if (pSCH == IMS_NULL)
                    break;

                if (pstMsgBody->pMimeHeader->pContentDescription != IMS_NULL)
                    Free(pstMsgBody->pMimeHeader->pContentDescription);

                pstMsgBody->pMimeHeader->pContentDescription = sip_strdup(pSCH->pStr2, 0);
            }
            break;

        case SIPMessageBodyPart::CONTENT_UNKNOWN:
            {
                SipCommonHeader *pSCH = static_cast<SipCommonHeader*>(pstHeader->pHeader);

                if (pSCH == IMS_NULL)
                    break;

                AString strTmp;

                strTmp.Append(pSCH->pStr1);
                strTmp.Append(TextParser::CHAR_COLON);
                strTmp.Append(TextParser::CHAR_SP);
                strTmp.Append(pSCH->pStr2);

                IMS_CHAR *pszMIMEHeader = strTmp.Duplicate();

                if (pszMIMEHeader == IMS_NULL)
                {
                    SIPStackError(E_NO_MEM);
                    return IMS_FALSE;
                }

                if (List_Append(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                        pszMIMEHeader, SIPStackError()) == SipFail)
                {
                    Free(pszMIMEHeader);
                    return IMS_FALSE;
                }
            }
            break;

        default:
            return IMS_FALSE;
        }

        return IMS_TRUE;
#else
        return IMS_FALSE;
#endif // SIP_MIME_PARSING
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetMIMEHeader(IN IMS_SINT32 nType, IN CONST AString &strName,
            IN CONST AString &strBody, IN_OUT SipMsgBody *&pstMsgBody)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMsgBody == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

#ifdef SIP_MIME_PARSING
        if (pstMsgBody->pMimeHeader == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        SIPStackError(E_NO_ERROR);

        switch (nType)
        {
        case SIPMessageBodyPart::CONTENT_TYPE:
            {
                SipHeader *pstHeader = DecodeHeader(SipHdrTypeContentType, strName, strBody);

                if (pstMsgBody->pMimeHeader->pContentType != IMS_NULL)
                    sip_freeSipCommonHeader(pstMsgBody->pMimeHeader->pContentType);

                if (pstHeader != IMS_NULL)
                {
                    pstMsgBody->pMimeHeader->pContentType
                            = static_cast<SipCommonHeader*>(pstHeader->pHeader);
                    AddReference(pstMsgBody->pMimeHeader->pContentType);

                    FreeHeaderEx(pstHeader);
                }
            }
            break;

        case SIPMessageBodyPart::CONTENT_DISPOSITION:
            {
                SipHeader *pstHeader
                        = DecodeHeader(SipHdrTypeContentDisposition, strName, strBody);

                if (pstMsgBody->pMimeHeader->pContentDisposition != IMS_NULL)
                    sip_freeSipCommonHeader(pstMsgBody->pMimeHeader->pContentDisposition);

                if (pstHeader != IMS_NULL)
                {
                    pstMsgBody->pMimeHeader->pContentDisposition
                            = static_cast<SipCommonHeader*>(pstHeader->pHeader);
                    AddReference(pstMsgBody->pMimeHeader->pContentDisposition);

                    FreeHeaderEx(pstHeader);
                }
            }
            break;

        case SIPMessageBodyPart::CONTENT_TRANSFER_ENCODING:
            if (pstMsgBody->pMimeHeader->pContentTransEncoding != IMS_NULL)
                Free(pstMsgBody->pMimeHeader->pContentTransEncoding);

            pstMsgBody->pMimeHeader->pContentTransEncoding = strBody.Duplicate();
            break;

        case SIPMessageBodyPart::CONTENT_ID:
            if (pstMsgBody->pMimeHeader->pContentId != IMS_NULL)
                Free(pstMsgBody->pMimeHeader->pContentId);

            pstMsgBody->pMimeHeader->pContentId = strBody.Duplicate();
            break;

        case SIPMessageBodyPart::CONTENT_DESCRIPTION:
            if (pstMsgBody->pMimeHeader->pContentDescription != IMS_NULL)
                Free(pstMsgBody->pMimeHeader->pContentDescription);

            pstMsgBody->pMimeHeader->pContentDescription = strBody.Duplicate();
            break;

        case SIPMessageBodyPart::CONTENT_UNKNOWN:
            {
                AString strTmp;

                strTmp.Append(strName);
                strTmp.Append(TextParser::CHAR_COLON);
                strTmp.Append(TextParser::CHAR_SP);
                strTmp.Append(strBody);

                IMS_CHAR *pszMIMEHeader = strTmp.Duplicate();

                if (pszMIMEHeader == IMS_NULL)
                {
                    SIPStackError(E_NO_MEM);
                    return IMS_FALSE;
                }

                if (List_Append(&(pstMsgBody->pMimeHeader->slAdditionalMimeHeaders),
                        pszMIMEHeader, SIPStackError()) == SipFail)
                {
                    Free(pszMIMEHeader);
                    return IMS_FALSE;
                }
            }
            break;

        default:
            return IMS_FALSE;
        }

        return IMS_TRUE;
#else
        return IMS_FALSE;
#endif // SIP_MIME_PARSING
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetParameter(IN SipHeader* pstHeader,
            IN CONST AString &strName, IN CONST AString &strValue)
    {
        //-----------------------------------------------------------------------------------------

        if (pstHeader == IMS_NULL)
            return IMS_FALSE;

        if (pstHeader->pHeader == IMS_NULL)
            return IMS_FALSE;

        SipParam *pstParam = IMS_NULL;

        GetParameter(pstHeader, strName, pstParam);

        if (pstParam != IMS_NULL)
        {
            // Remove the first parameter value
            if (strValue.IsNULL())
            {
                (void) sip_deleteValueAtIndexInSipParam(pstParam, 0, SIPStackError());
            }
            else
            {
                IMS_CHAR *pszValue = strValue.Duplicate();
                IMS_UINT32 nValueCount = 0;

                (void) sip_getValueCountFromSipParam(pstParam, &nValueCount, SIPStackError());

                if (nValueCount == 0)
                {
                    if (sip_insertValueAtIndexInSipParam(pstParam,
                            pszValue, 0, SIPStackError()) == SipFail)
                    {
                        Free(pszValue);
                        sip_freeSipParam(pstParam);
                        return IMS_FALSE;
                    }
                }
                else
                {
                    if (sip_setValueAtIndexInSipParam(pstParam,
                            pszValue, 0, SIPStackError()) == SipFail)
                    {
                        Free(pszValue);
                        sip_freeSipParam(pstParam);
                        return IMS_FALSE;
                    }
                }
            }
        }
        else
        {
            if (sip_initSipParam(&pstParam, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            IMS_CHAR *pszName = strName.Duplicate();

            if (sip_setNameInSipParam(pstParam, pszName, SIPStackError()) == SipFail)
            {
                Free(pszName);
                sip_freeSipParam(pstParam);
                return IMS_FALSE;
            }

            if (!strValue.IsNULL())
            {
                IMS_CHAR *pszValue = strValue.Duplicate();
                IMS_UINT32 nValueCount = 0;

                (void) sip_getValueCountFromSipParam(pstParam, &nValueCount, SIPStackError());

                if (nValueCount == 0)
                {
                    if (sip_insertValueAtIndexInSipParam(pstParam,
                            pszValue, 0, SIPStackError()) == SipFail)
                    {
                        Free(pszValue);
                        sip_freeSipParam(pstParam);
                        return IMS_FALSE;
                    }
                }
                else
                {
                    if (sip_setValueAtIndexInSipParam(pstParam,
                            pszValue, 0, SIPStackError()) == SipFail)
                    {
                        Free(pszValue);
                        sip_freeSipParam(pstParam);
                        return IMS_FALSE;
                    }
                }
            }

            IMS_UINT32 nPCount = 0;

            (void) sip_getParamCountFromCommonHdr(pstHeader, &nPCount, SIPStackError());

            if (sip_insertParamAtIndexInCommonHdr(pstHeader,
                    pstParam, nPCount, SIPStackError()) == SipFail)
            {
                sip_freeSipParam(pstParam);
                return IMS_FALSE;
            }
        }

        sip_freeSipParam(pstParam);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetRequestLine(IN CONST AString &strMethod, IN CONST AString &strURI,
            IN_OUT SipMessage *&pstMessage)
    {
        SipReqLine *pstReqLine;
        SipAddrSpec *pstAddrSpec;

        IMS_CHAR *pszMethod;

        //-----------------------------------------------------------------------------------------

        if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        // Set Request method
        pszMethod = strMethod.Duplicate();

        if (sip_setMethodInReqLine(pstReqLine, pszMethod, SIPStackError()) == SipFail)
        {
            Free(pszMethod);
            sip_freeSipReqLine(pstReqLine);
            return IMS_FALSE;
        }

        pstAddrSpec = DecodeAddrSpec(strURI);

        if (sip_setAddrSpecInReqLine(pstReqLine, pstAddrSpec, SIPStackError()) == SipFail)
        {
            FreeAddrSpec(pstAddrSpec);
            sip_freeSipReqLine(pstReqLine);
            return IMS_FALSE;
        }

        FreeAddrSpec(pstAddrSpec);
        sip_freeSipReqLine(pstReqLine);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetRequestUri(IN SipAddrSpec *pstAddrSpec, IN_OUT SipMessage *&pstMessage)
    {
        SipReqLine *pstReqLine;

        //-----------------------------------------------------------------------------------------

        if (sip_getReqLine(pstMessage, &pstReqLine, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        if (sip_setAddrSpecInReqLine(pstReqLine, pstAddrSpec, SIPStackError()) == SipFail)
        {
            sip_freeSipReqLine(pstReqLine);
            return IMS_FALSE;
        }

        sip_freeSipReqLine(pstReqLine);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetStatusLine(IN IMS_SINT32 nStatusCode, IN CONST AString &strReasonPhrase,
            IN_OUT SipMessage *&pstMessage)
    {
        SipStatusLine *pstStatusLine;

        IMS_CHAR *pszReasonPhrase;

        //-----------------------------------------------------------------------------------------

        if (sip_getStatusLine(pstMessage, &pstStatusLine, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        // Set status-code
        if (sip_setStatusCodeNumInStatusLine(pstStatusLine,
                nStatusCode, SIPStackError()) == SipFail)
        {
            sip_freeSipStatusLine(pstStatusLine);
            return IMS_FALSE;
        }

        // Set reason-phrase
        pszReasonPhrase = strReasonPhrase.Duplicate();

        if (sip_setReasonInStatusLine(pstStatusLine, pszReasonPhrase, SIPStackError()) == SipFail)
        {
            Free(pszReasonPhrase);
            sip_freeSipStatusLine(pstStatusLine);
            return IMS_FALSE;
        }

        sip_freeSipStatusLine(pstStatusLine);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SetUnknownHeader(IN SipHeader *pstHeader,
            IN CONST AString &strName, IN_OUT SipMessage *&pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        IMS_SINT32 nHCount = GetHeaderCount(pstMessage, ISIPHeader::UNKNOWN);

        if (nHCount == 0)
        {
            return SetHeader(pstHeader, pstMessage);
        }

        IMS_SINT32 nFirstHeaderIndex = (-1);

        for (IMS_SINT32 i = 0; i < nHCount; ++i)
        {
            SipHeader *pstHeader = GetHeader(pstMessage, ISIPHeader::UNKNOWN, i);

            if (pstHeader == IMS_NULL)
            {
                continue;
            }

            SipCommonHeader *pstCommHeader = static_cast<SipCommonHeader*>(pstHeader->pHeader);

            if (pstCommHeader == IMS_NULL)
            {
                FreeHeaderEx(pstHeader);
                continue;
            }

            if (strName.EqualsIgnoreCase(pstCommHeader->pStr1))
            {
                nFirstHeaderIndex = i;
                FreeHeaderEx(pstHeader);
                break;
            }

            FreeHeaderEx(pstHeader);
        }

        if (nFirstHeaderIndex >= 0)
        {
            if (sip_setHeaderAtIndex(pstMessage,
                    pstHeader, nFirstHeaderIndex, SIPStackError()) == SipFail)
            {
                return IMS_FALSE;
            }

            return IMS_TRUE;
        }

        return AppendHeader(pstHeader, pstMessage);
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL UncompressMessageBody(IN SipMessage *pstMessage)
    {
        IMS_UINT32 nBodyCount = 0;

        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        (void) List_SizeOf(&(pstMessage->slMessageBody), &nBodyCount, SIPStackError());

        if (nBodyCount > 1)
        {
            SIPStackError(E_PARSER_ERROR);

            IMS_TRACE_E(0, "There are many SIP message bodies", 0, 0, 0);
            return IMS_FALSE;
        }

        if (nBodyCount == 0)
        {
            IMS_TRACE_D("___ NO SIP MESSAGE BODY ___", 0, 0, 0);
            return IMS_TRUE;
        }

        SipMsgBody *pstMsgBody = IMS_NULL;

        (void) List_GetAt(&(pstMessage->slMessageBody), 0, &pstMsgBody, SIPStackError());

        if (pstMsgBody == IMS_NULL)
        {
            IMS_TRACE_E(0, "Getting SIP message body failed", 0, 0, 0);
            return IMS_FALSE;
        }

        if ((pstMsgBody->dType != SipUnknownBody)
                || (pstMsgBody->u.pUnknownMessage == IMS_NULL))
        {
            IMS_TRACE_E(0, "Message body is not unknown body part", 0, 0, 0);
            return IMS_FALSE;
        }

        //4 Check if the Content-Encoding contains the compression algorithm or not
        //4 As default, consider it to 'gzip'. It needs to be consider other compression algorithm.
        ByteArray objBodyPart;
        ByteArray objCompBodyPart;

        objCompBodyPart.Attach(
                reinterpret_cast<const IMS_BYTE*>(pstMsgBody->u.pUnknownMessage->pBuffer),
                static_cast<IMS_SINT32>(pstMsgBody->u.pUnknownMessage->dLength));

        if (!IMS_UTIL_ZLIB_Uncompress(objCompBodyPart, objBodyPart))
        {
            IMS_TRACE_E(0, "Uncompressing a body part failed", 0, 0, 0);
            return IMS_FALSE;
        }

        if (IMS_UTIL_SYS_PROP_IS_DEBUG_MODE())
        {
            IMS_TRACE_TEXT("gzip::uncompression",
                    objBodyPart.GetData(), objBodyPart.GetLength());
        }

        SIP_S8bit *pBuffer = pstMsgBody->u.pUnknownMessage->pBuffer;

        pstMsgBody->u.pUnknownMessage->pBuffer
                = reinterpret_cast<SIP_S8bit*>(IMS_MEM_Malloc(objBodyPart.GetLength()));

        IMS_MEM_Memcpy(reinterpret_cast<void*>(pstMsgBody->u.pUnknownMessage->pBuffer),
                reinterpret_cast<const void*>(objBodyPart.GetData()), objBodyPart.GetLength());

        pstMsgBody->u.pUnknownMessage->dLength = objBodyPart.GetLength();

        Free(pBuffer);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL UpdateSentProtocol(IN SipMessage *pstMessage,
            IN CONST AString &strSentProtocol)
    {
        SipHeader stHeader;

        //-----------------------------------------------------------------------------------------

        if (sip_getHeader(pstMessage, SipHdrTypeVia, &stHeader, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        IMS_CHAR *pszTmpVal = strSentProtocol.Duplicate();

        if (sip_setSentProtocolInViaHdr(&stHeader, pszTmpVal, SIPStackError()) == SipFail)
        {
            Free(pszTmpVal);
            FreeHeader(&stHeader);
            return IMS_FALSE;
        }

        FreeHeader(&stHeader);

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL void DisplayBadHeaders(IN SipMessage *pstMessage)
    {
        IMS_SINT32 nCount = GetBadHeaderCount(pstMessage);

        if (nCount > 0)
        {
#ifdef SIP_BADMESSAGE_PARSING
            SipError enError = E_NO_ERROR;

            IMS_TRACE_I("___ SIP bad headers - S ___", 0, 0, 0);

            for (IMS_SINT32 i = 0; i < nCount; ++i)
            {
                SipBadHeader *pstBadHdr = IMS_NULL;

                if (sip_getBadHeaderAtIndex(pstMessage, &pstBadHdr, i, &enError) == SipFail)
                {
                    continue;
                }

                IMS_CHAR *pszName = IMS_NULL;
                IMS_CHAR *pszBody = IMS_NULL;

                sip_getNameFromBadHdr(pstBadHdr, &pszName, &enError);
                sip_getBodyFromBadHdr(pstBadHdr, &pszBody, &enError);

                IMS_TRACE_I("    (%d) %s: %s", i, _TRACE_S_(pszName), _TRACE_S_(pszBody));

                sip_freeSipBadHeader(pstBadHdr);
            }

            IMS_TRACE_I("___ SIP bad headers - E ___", 0, 0, 0);
#else
            (void) pstMessage;
#endif
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 GetBadHeaderCount(IN SipMessage *pstMessage)
    {
        IMS_UINT32 nCount = 0;

#ifdef SIP_BADMESSAGE_PARSING
        SipError enError = E_NO_ERROR;

        sip_getBadHeaderCount(pstMessage, &nCount, &enError);
#else
        (void) pstMessage;
#endif

        return nCount;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL HasMandatoryHeaders(IN SipMessage *pstMessage)
    {
        return (sip_checkMandatory(pstMessage) == SipSuccess) ? IMS_TRUE : IMS_FALSE;
    }

    /*

    Remarks

    */
    /// APIs for SIP authentication
    GLOBAL IMS_BOOL GetEntityBody(IN SipMessage *pstMessage, OUT AString &strEntityBody)
    {
        IMS_UINT32 nMBCount = 0;

        RCPtr<SIPMessageBuffer> pMessageBuffer = SIPMessageBuffer::GetInstance();
        IMS_UINT32 nBuffLen = pMessageBuffer->GetLength();
        IMS_CHAR *pTmpBuffer = reinterpret_cast<IMS_CHAR*>(pMessageBuffer->GetBuffer());

        //-----------------------------------------------------------------------------------------

        if (sip_getMsgBodyCount(pstMessage, &nMBCount, SIPStackError()) == SipFail)
        {
            return IMS_FALSE;
        }

        if (nMBCount == 0)
        {
            SIPStackError(E_NO_EXIST);
            return IMS_TRUE;
        }

        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeContentType);

        if (pstHeader == IMS_NULL)
        {
            return IMS_FALSE;
        }

        if (GetLastError() != E_NO_ERROR)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        IMS_MEM_Memset(pTmpBuffer, 0x00, nBuffLen);

        // Allocate a memory and initialize the entity-body buffer.
        // This will be filled with the entire entity-body and then passed
        // to IMSDigest_CalculateHEntity() to calculate the hash of the entity-body.
        if (sip_formMimeBody(IMS_NULL, pstMessage->slMessageBody,
                static_cast<SipContentTypeHeader*>(pstHeader->pHeader),
                pTmpBuffer, &nBuffLen, SIPStackError()) == SipFail)
        {
            FreeHeaderEx(pstHeader);
            return IMS_FALSE;
        }

        FreeHeaderEx(pstHeader);

        // sip_formMimeBody() returns the initial CRLF(\r\n) in the entity-body.
        // But, this initial CRLF should not be used in computing the hash (Entity).
        AString strTmp(&(pTmpBuffer[2]), nBuffLen - 2);

        strEntityBody = strTmp;

        return IMS_TRUE;
    }

    /// APIs for SIP transaction layer

    /*

    Remarks

    */
    GLOBAL IMS_BOOL AbortTransaction(IN SipTxnKey *pstTxnKey, IN SipTxnContext *pstTxnContext)
    {
        //-----------------------------------------------------------------------------------------

        if (sip_txn_abortTxn(pstTxnKey, pstTxnContext, SIPStackError()) == SipFail)
        {
            SIPStackError(E_TXN_NO_EXIST);
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL SipEventContext* CreateEventContext()
    {
        SipEventContext *pstContext
                = static_cast<SipEventContext*>(IMS_MEM_Malloc(sizeof(SipEventContext)));

        //-----------------------------------------------------------------------------------------

        if (pstContext == IMS_NULL)
        {
            return IMS_NULL;
        }

#ifdef SIP_INCREMENTAL_PARSING
        pstContext->pList = static_cast<SipHdrTypeList*>(IMS_MEM_Malloc(sizeof(SipHdrTypeList)));

        if (pstContext->pList == IMS_NULL)
        {
            IMS_MEM_Free(pstContext);
            return IMS_NULL;
        }
#else
        pstContext->pList = IMS_NULL;
#endif
        pstContext->pTranspAddr = IMS_NULL;
        pstContext->pDirectBuffer = IMS_NULL;
        pstContext->pData = static_cast<void*>(new SIPTxnContextData());
        pstContext->dRemainingLength = 0;
        pstContext->dNextMessageLength = 0;
        pstContext->dOptions.dOption = 0;
#ifndef SIP_TXN_LAYER
        pstContext->dRetransT1 = 0;
        pstContext->dRetransT2 = 0;
        pstContext->dMaxRetransCount = 0;
        pstContext->dMaxInviteRetransCount = 0;
#endif // SIP_TXN_LAYER

        return pstContext;
    }

    /*

    Remarks

    */
    GLOBAL SipTxnContext* CreateTxnContext()
    {
        SipTxnContext *pstContext = IMS_NULL;

        //-----------------------------------------------------------------------------------------

        if (sip_txn_initSipTxnContext(&pstContext, SIPStackError()) == SipFail)
        {
            return IMS_NULL;
        }

        return pstContext;
    }

    /*

    Remarks

    */
    GLOBAL SIPTxnKey* CreateTxnKey(IN SipMessage *pstMessage)
    {
        //-----------------------------------------------------------------------------------------

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        SIPMethod objMethod = GetMethod(pstMessage);
        IMS_SINT32 nStatusCode = 0;
        IMS_UINT32 nCSeq = GetCSeqNumber(pstMessage);

        SipHeader *pstHeader = GetHeader(pstMessage, SipHdrTypeVia);

        if (!IsValidHeader(pstHeader))
        {
            FreeHeaderEx(pstHeader);
            return IMS_NULL;
        }

        AString strViaBranch = GetParameter(pstHeader, SIP::STR_BRANCH);

        FreeHeaderEx(pstHeader);

        if (strViaBranch.GetLength() == 0)
        {
            IMS_TRACE_E(0, "Via branch is null", 0, 0, 0);
            return IMS_NULL;
        }

        if (!IsRequestMessage(pstMessage))
        {
            nStatusCode = GetStatusCode(pstMessage);
        }

        return new SIPTxnKey(objMethod, nStatusCode, strViaBranch, nCSeq);
    }

    /*

    Remarks

    */
    GLOBAL SipTxnKey* CreateTxnKey(IN SipMessage *pstMessage, IN IMS_SINT32 nAPICalled)
    {
        en_SipTxnClass enTxnClass;
        SipTxnInformn stTxnInfo;
        SipTxnKey *pstTxnKey = IMS_NULL;
        en_SipTxnAPICalled enAPICalled = (en_SipTxnAPICalled)nAPICalled;

        //-----------------------------------------------------------------------------------------

        SIPStackError(E_NO_ERROR);

        if (pstMessage == IMS_NULL)
        {
            SIPStackError(E_INV_PARAM);
            return IMS_NULL;
        }

        if (sip_captureTxnInformn(pstMessage,
                IMS_NULL, 0, IMS_NULL, IMS_NULL, 0, &stTxnInfo) == SipFail)
        {
            return IMS_NULL;
        }

        if (sip_getTxnClass(&stTxnInfo, enAPICalled, &enTxnClass) == SipFail)
            return IMS_NULL;

        if (sip_txn_createTxnKey(pstMessage, enTxnClass, &pstTxnKey, SIPStackError()) == SipFail)
            return IMS_NULL;

        return pstTxnKey;
    }

    /*

    Remarks

    */
    GLOBAL SIPTxnKey* CreateTxnKeyFromKey(IN SipTxnKey *pstTxnKey)
    {
        //-----------------------------------------------------------------------------------------

        if (pstTxnKey == IMS_NULL)
        {
            return IMS_NULL;
        }

        SIPMethod objMethod(static_cast<const IMS_CHAR*>(pstTxnKey->pMethod));
        AString strViaBranch(static_cast<const IMS_CHAR*>(pstTxnKey->pViaBranch));

        return new SIPTxnKey(objMethod, pstTxnKey->dRespCodeNum, strViaBranch, pstTxnKey->dCSeq);
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CompareTxnKeys(IN SipTxnKey *pstTxnKey1, IN SipTxnKey *pstTxnKey2)
    {
        //-----------------------------------------------------------------------------------------

        if (sip_txn_compareTxnKeys(pstTxnKey1, pstTxnKey2, SIPStackError()) == SipSuccess)
        {
            return IMS_TRUE;
        }

        return IMS_FALSE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CompareTxnKeysForAck(IN SipTxnKey *pstTxnKey1, IN SipTxnKey *pstTxnKey2)
    {
        //-----------------------------------------------------------------------------------------

        if ((pstTxnKey1 == IMS_NULL) || (pstTxnKey2 == IMS_NULL))
        {
            return IMS_FALSE;
        }

        if ((pstTxnKey1->dCSeq != pstTxnKey2->dCSeq)
                || (IMS_StrCmp(pstTxnKey1->pCallid, pstTxnKey2->pCallid) != 0)
                || (IMS_StrCmp(pstTxnKey1->pFromTag, pstTxnKey2->pFromTag) != 0)
                || (IMS_StrCmp(pstTxnKey1->pToTag, pstTxnKey2->pToTag) != 0)
                || (IMS_StrCmp(pstTxnKey1->pViaBranch, pstTxnKey2->pViaBranch) != 0))
        {
            return IMS_FALSE;
        }

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL CompareTxnKeysForCancel(IN SipTxnKey *pstCancelKey, IN SipTxnKey *pstTxnKey)
    {
        // Compares these values : CSeq number, Call-ID, From-Tag, Via branch parameter

        //-----------------------------------------------------------------------------------------

        if ((pstCancelKey == IMS_NULL) || (pstTxnKey == IMS_NULL))
            return IMS_FALSE;

        // Check Via branch parameter first.
        if (SIP_STRSTR(pstCancelKey->pViaBranch, SIP::STR_BRANCH_MAGIC_COOKIE) != IMS_NULL)
        {
            IMS_TRACE_D("Transaction Matching :: Compliant to RFC 3261", 0, 0, 0);

            // Request was generated by a client transaction compliant to RFC 3261.
            // Therefore, the branch parameter will be unique across all transactions
            // sent by that client.
            if (SIP_STRICMP(pstCancelKey->pViaBranch, pstTxnKey->pViaBranch) != 0)
                return IMS_FALSE;

            // Check 'sent-by' info. - host
            if ((pstCancelKey->pSentBy_Host != IMS_NULL)
                    && (pstTxnKey->pSentBy_Host != IMS_NULL)
                    && (SIP_STRSTR(pstCancelKey->pSentBy_Host, ":") != IMS_NULL)
                    && (SIP_STRSTR(pstTxnKey->pSentBy_Host, ":") != IMS_NULL))
            {
                // Compares IPv6 addresses
                AString strCancelHost(static_cast<const IMS_CHAR*>(pstCancelKey->pSentBy_Host));
                AString strTxnHost(static_cast<const IMS_CHAR*>(pstTxnKey->pSentBy_Host));
                IPAddress objCancelIPA(strCancelHost);
                IPAddress objTxnIPA(strTxnHost);

                if (!objCancelIPA.Equals(objTxnIPA))
                {
                    return IMS_FALSE;
                }
            }
            else
            {
                if (SIP_STRICMP(pstCancelKey->pSentBy_Host, pstTxnKey->pSentBy_Host) != 0)
                    return IMS_FALSE;
            }

            // Check 'sent-by' info. - port
            if (pstCancelKey->nSentBy_Port != pstTxnKey->nSentBy_Port)
                return IMS_FALSE;

            return IMS_TRUE;
        }

        ///// Request-URI, To-Tag, From-Tag, Call-ID, CSeq, top Via header

        // CSeq number
        if (pstCancelKey->dCSeq != pstTxnKey->dCSeq)
            return IMS_FALSE;

        // Call-ID
        if (SIP_STRCMP(pstCancelKey->pCallid, pstTxnKey->pCallid) != 0)
            return IMS_FALSE;

        // From-Tag
        if (SIP_STRCMP(pstCancelKey->pFromTag, pstTxnKey->pFromTag) != 0)
            return IMS_FALSE;

        // Via branch parameter
        if (SIP_STRCMP(pstCancelKey->pViaBranch, pstTxnKey->pViaBranch) != 0)
            return IMS_FALSE;

        return IMS_TRUE;
    }

    /*

    Remarks

    */
    GLOBAL void DestroyEventContext(IN SipEventContext *pstContext)
    {
        //-----------------------------------------------------------------------------------------

        if (pstContext == IMS_NULL)
            return;

#ifdef SIP_INCREMENTAL_PARSING
        if (pstContext->pList != IMS_NULL)
        {
            Free(pstContext->pList);
        }
#endif

        if (pstContext->pData != IMS_NULL)
        {
            SIPTxnContextData *pTxnContextData
                    = static_cast<SIPTxnContextData*>(pstContext->pData);

            if (pTxnContextData != IMS_NULL)
            {
                delete pTxnContextData;
                pstContext->pData = IMS_NULL;
            }
        }

        Free(pstContext);
    }

    /*

    Remarks

    */
    GLOBAL void DestroyTxnContext(IN SipTxnContext *pstContext)
    {
        //-----------------------------------------------------------------------------------------

        if (pstContext == IMS_NULL)
            return;

        Free(pstContext);
    }

    /*

    Remarks

    */
    GLOBAL void DisplayTxnKey(IN CONST SipTxnKey *pstTxnKey)
    {
        static const IMS_CHAR* TXN_CLASS[] =
        {
            "INVITE CLIENT",
            "non-INVITE CLIENT",
            "INVITE SERVER",
            "non-INVITE SERVER"
        };

        //-------------------------------------------------------------------------------------

        if (pstTxnKey == IMS_NULL)
            return;

        IMS_TRACE_D("___ TRANSACTION INFO. - S ___", 0, 0, 0);
        IMS_TRACE_I("\tTxn Class: %s, %s, %d",
                TXN_CLASS[pstTxnKey->dTxnClass], pstTxnKey->pMethod, pstTxnKey->dCSeq);
        IMS_TRACE_I("\tVia Branch: %s, RSeq: %d", pstTxnKey->pViaBranch, pstTxnKey->dRseq, 0);

        IMS_CHAR acCallId[11+1] = { '\0', };

        (void) acCallId;
        IMS_TRACE_I("\tCall-ID: %s", GetLogString(pstTxnKey->pCallid, acCallId, 11, '@'), 0, 0);

        IMS_TRACE_D("___ TRANSACTION INFO. - E ___\r\n", 0, 0, 0);
    }

    /*

    Remarks

    */
    GLOBAL void FreeTxnKey(IN SipTxnKey *&pstTxnKey)
    {
        //-----------------------------------------------------------------------------------------

        sip_freeSipTxnKey(pstTxnKey);
        pstTxnKey = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL void FreeTxnBuffer(IN SipTxnBuffer *&pstTxnBuffer)
    {
        //-----------------------------------------------------------------------------------------

        sip_freeSipTxnBuffer(pstTxnBuffer);
        pstTxnBuffer = IMS_NULL;
    }

    /*

    Remarks

    */
    GLOBAL SIPTxnContextData* GetTxnContextData(IN SipEventContext *pstContext)
    {
        //-----------------------------------------------------------------------------------------

        if (pstContext == IMS_NULL)
            return IMS_NULL;

        return static_cast<SIPTxnContextData*>(pstContext->pData);
    }

    /*

    Remarks

    */
    GLOBAL IMS_SINT32 HandleTxn(IN SipMessage *pstMessage, IN SipTxnContext *pstTxnContext,
            OUT SipTxnKey *&pstTxnKey)
    {
        SipError enSipError = E_NO_ERROR;
        en_SipTxnDecodeResult enDecodeResult = SipTxnNonIgnorable;

        //---------------------------------------------------------------------------------------------

        pstTxnKey = IMS_NULL;

        if (sip_txn_handleTxn(pstMessage,
                pstTxnContext, &enDecodeResult, &pstTxnKey, &enSipError) == SipFail)
        {
            SIPStackError(enSipError);
            IMS_TRACE_D("SIPStack :: handleTxn failed - errorCode=%d", GetLastError(), 0, 0);
            return MESSAGE_FAILURE;
        }

        switch (enDecodeResult)
        {
        case SipTxnNonIgnorable:
            return MESSAGE_SUCCESS;

        case SipTxnIgnorable:
            // Received message is a remote retransmission
            return MESSAGE_RETRANSMISSION;

        case SipTxnStrayMessage:
            // Stray response received
            return MESSAGE_STRAY;

        case SipTxnStrayPrack:
            // PRACK received for which there is no matching RPR transaction.
            return MESSAGE_STRAY_PRACK;

        case SipTxnRetrans:
        case SipTxnConfirmnNeeded:
        case SipTxnQueued:
            /*
             * Queued or confirmation needed are not returned for UA type transactions.
             * These can be ignored in the case-switch.
             * Same of SipTxnRetrans: This is for stateless proxy application using the core stack.
             */
        default:
            return MESSAGE_SUCCESS;
        }
    }

    /*

    Remarks

    */
    GLOBAL IMS_BOOL SendToTxnLayer(IN CONST ByteArray &objBuffer, IN SipMessage *pstMessage,
            IN SipTranspAddr *pstAddr, IN IMS_BYTE byTranspType, IN SipTxnContext *pstTxnContext,
            OUT SipTxnKey *&pstTxnKey)
    {
        SipError enSipError = E_NO_ERROR;
        en_SipTxnClass enTxnClass;
        SipEventContext *pstEventContext = SIP_NULL;
        SipOptions *pstOptions = SIP_NULL;
        SipTxnInformn txnInformn;
        SipBool bResult;
        SipBool bDirectOption = SipFail; // Now, always FALSE
        SIP_S8bit *pBuffer = SIP_NULL;
        SIP_U32bit nBuffLen;

        //-----------------------------------------------------------------------------------------

        SIPDEBUGFN("Entering into SendToTxnLayer (sip_txn_sendMessage)");

        if ( (pstMessage == IMS_NULL) || (pstAddr == IMS_NULL) || (pstTxnContext == IMS_NULL) )
        {
            SIPStackError(E_INV_PARAM);
            return IMS_FALSE;
        }

        // Obtain the event context from the txn event context which needs to
        // be passed to SendMessage
        pstEventContext = pstTxnContext->pEventContext;

        // Obtain the options that will be sent to SendMessage
        pstOptions = &(pstTxnContext->txnOption.dOption);

        // Check if the direct buffer option has been set or not
        // NOTE: For the meantime, the implementation does not apply the direct buffer mechanism.
        //      So, the checking of direct buffer option was missing.
        nBuffLen = objBuffer.GetLength();

        pBuffer = static_cast<SIP_S8bit*>(IMS_MEM_Malloc(nBuffLen));

        if (pBuffer == IMS_NULL)
        {
            Free(pstAddr->pHost);
            Free(pstAddr);

            IMS_TRACE_E(0, "Allocating memory failed", 0, 0, 0);
            SIPStackError(E_NO_MEM);
            return IMS_FALSE;
        }

        (void) IMS_MEM_Memcpy(pBuffer, objBuffer.GetData(), nBuffLen);

        SIPStackError(E_NO_ERROR);

        // The message has been sent now start the transaction level handling on this
        bResult = sip_captureTxnInformn(pstMessage, pBuffer, nBuffLen,
                    pstTxnContext, pstAddr, byTranspType, &txnInformn);

        if (bResult == SipFail)
        {
            Free(pBuffer);
            Free(pstAddr->pHost);
            Free(pstAddr);
            return IMS_FALSE;
        }

        txnInformn.dAPICalled = SipTxnMesgSent;
        txnInformn.dDirectOption = bDirectOption;

        // NOTE: Check the "bypass" option;
        // The implementation does not allow "bypass" option (always no-bypass).

        // Ensure that all the mandatory headers are present
        bResult = sip_checkMandatoryTxnHeaders(pstMessage, pstTxnContext->dTxnType);

        if (bResult == SipFail)
        {
            Free(pBuffer);
            Free(pstAddr->pHost);
            Free(pstAddr);

            SIPStackError(E_TXN_INV_MSG);

            IMS_TRACE_E(0, "Checking mandatory header fields are missed", 0, 0, 0);
            return IMS_FALSE;
        }

        // Obtain the class of the transaction whether INVITE client, non-INVITE client, ...
        bResult = sip_getTxnClass(&txnInformn, SipTxnMesgSent, &enTxnClass);

        if (bResult == SipFail)
        {
            Free(pBuffer);
            Free(pstAddr->pHost);
            Free(pstAddr);

            IMS_TRACE_E(0, "Finding transaction class failed", 0, 0, 0);
            return IMS_FALSE;
        }

        // Also, add the class information into the TxnInformn
        txnInformn.dTxnClass = enTxnClass;
        // Set the pKeyOfCreatedTxn component of TxnInformn to SIP_NULL
        txnInformn.pKeyOfCreatedTxn = SIP_NULL;
        // The message needs to be sent at least once for all success cases
        txnInformn.dMesgSentAtLeastOnce = SipFail;

        // On the basis of the kind of transaction that will get affected call
        // the respective update function
        if (sip_isTxnAnRPR(&txnInformn) == SipSuccess)
        {
            bResult = sip_updateRPRTxnForMesgSent(&txnInformn, &enSipError);

            if (bResult == SipFail)
            {
                Free(pBuffer);

                if (txnInformn.bFreeTransportAddress == SipTrue)
                {
                    Free(pstAddr->pHost);
                    Free(pstAddr);
                }

                SIPStackError(enSipError);
                return IMS_FALSE;
            }
        }
        else
        {
            switch (enTxnClass)
            {
            case SipInviteClient:
                bResult = sip_updateInvClTxnForMesgSent(&txnInformn, &enSipError);
                break;

            case SipNonInviteClient:
                bResult = sip_updateNonInvClTxnForMesgSent(&txnInformn, &enSipError);
                break;

            case SipInviteServer:
                bResult = sip_updateInvSrvTxnForMesgSent(&txnInformn, &enSipError);
                break;

            case SipNonInviteServer:
                bResult = sip_updateNonInvSrvTxnForMesgSent(&txnInformn, &enSipError);
                break;

            default:
                bResult = SipFail;
                break;
            }

            if (bResult == SipFail)
            {
                Free(pBuffer);

                if (txnInformn.bFreeTransportAddress == SipTrue)
                {
                    Free(pstAddr->pHost);
                    Free(pstAddr);
                }

                SIPStackError(enSipError);
                return IMS_FALSE;
            }
        }

        // You come here only for successful operation from the above switch-case.
        // Here, check for two things:
        //    1) If message does not got sent even once it needs to be sent.
        //    2) If the buffer inside txnInformn is non-NULL means nobody owns it apart from
        //      TxnInformn and it needs to be freed

        if (txnInformn.dMesgSentAtLeastOnce == SipFail)
        {
            SIP_S8bit *pTmpBuffer = txnInformn.pMsgBuffer;

            txnInformn.pMsgBuffer = pBuffer;

            bResult = sip_txn_sendBuffer(&txnInformn, SIPStackError());

            if (bResult == SipFail)
            {
                if (txnInformn.pMsgBuffer != SIP_NULL)
                {
                    Free(txnInformn.pMsgBuffer);
                }

                return IMS_FALSE;
            }

            txnInformn.pMsgBuffer = pTmpBuffer;
        }

        if (txnInformn.pMsgBuffer != SIP_NULL)
        {
            Free(txnInformn.pMsgBuffer);
        }

        // Now, check if a transaction got created, this will be indicated by the presence
        // of a non-NULL value in the pKeyOfCreatedTxn of TxnInformn.
        if (txnInformn.pKeyOfCreatedTxn != SIP_NULL)
        {
            pstTxnKey = txnInformn.pKeyOfCreatedTxn;
        }
        else
        {
            pstTxnKey = SIP_NULL;
        }

        SIPDEBUGFN("Exiting from SendToTxnLayer (sip_txn_sendMessage)");

        SIPStackError(E_NO_ERROR);
        return IMS_TRUE;
    }

    GLOBAL void TerminateTransaction(IN SipTxnKey * /*pstTxnKey*/)
    {
        // no-op
    }

    GLOBAL const IMS_CHAR* GetTimerTypeAsString(IN SipTimerType enTimerType)
    {
        static const IMS_CHAR* acTimerType[] =
            {
                "Timer_A_B >> INVITE client",
                "Timer_B >> INVITE client",
                "Timer_C >> INVITE client",
                "Timer_Cr >> INVITE client",
                "Timer_D >> INVITE client",
                "Timer_E_F >> non-INVITE client",
                "Timer_F >> non-INVITE client",
                "Timer_G_H >> INVITE server & ACK receipt",
                "Timer_H >> ACK receipt",
                "Timer_I >> ACK retransmit",
                "Timer_J >> non-INVITE server",
                "Timer_K >> non-INVITE client",
                "Timer_CANCELLED_STATE",
                "Timer_PROXY_UAS_2XX_FWDED",
                "Timer_LAST"
            };

        //-----------------------------------------------------------------------------------------

        return acTimerType[enTimerType];
    }

    GLOBAL const IMS_CHAR* GetTimerTypeAsString(IN SipTxnTimeoutData *pstData)
    {
        //-----------------------------------------------------------------------------------------

        if (pstData == IMS_NULL)
        {
            return "__INVALID__";
        }

        return GetTimerTypeAsString(pstData->dTimerStarted);
    }

    GLOBAL void InvokeTimerCallback(IN SipTimerCallback pfnCallback,
            IN SipTxnTimeoutData *pstData, IN IMS_PVOID pvExtraParam)
    {
        //-----------------------------------------------------------------------------------------

        if (pfnCallback == IMS_NULL)
        {
            return;
        }

        (void) pvExtraParam;

        pfnCallback(pstData);
    }

    GLOBAL void SetTimerValues(IN SIPTimerValues *pTV, IN_OUT SipTxnContext *&pstTxnContext)
    {
        //-----------------------------------------------------------------------------------------

        if (pTV == IMS_NULL)
        {
            return;
        }

        if (!pTV->IsSet(SIPTimerValues::TV_ALL))
        {
            // No timer is set
            return;
        }

        if (pTV->IsSet(SIPTimerValues::TIMER_T1))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_T1;
            pstTxnContext->timeoutValues.dT1 = pTV->GetValue(SIPTimerValues::TIMER_T1);
        }

        if (pTV->IsSet(SIPTimerValues::TIMER_T2))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_T2;
            pstTxnContext->timeoutValues.dT2 = pTV->GetValue(SIPTimerValues::TIMER_T2);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_B))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_B;
            pstTxnContext->timeoutValues.dTimerB = pTV->GetValue(SIPTimerValues::TV_TIMER_B);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_D))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_D;
            pstTxnContext->timeoutValues.dTimerD_T3 = pTV->GetValue(SIPTimerValues::TV_TIMER_D);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_F))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_F;
            pstTxnContext->timeoutValues.dTimerF_T3 = pTV->GetValue(SIPTimerValues::TV_TIMER_F);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_H))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_H;
            pstTxnContext->timeoutValues.dTimerH = pTV->GetValue(SIPTimerValues::TV_TIMER_H);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_I))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_I;
            pstTxnContext->timeoutValues.dTimerI_T4 = pTV->GetValue(SIPTimerValues::TV_TIMER_I);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_J))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_J;
            pstTxnContext->timeoutValues.dTimerJ_T3 = pTV->GetValue(SIPTimerValues::TV_TIMER_J);
        }

        if (pTV->IsSet(SIPTimerValues::TV_TIMER_K))
        {
            pstTxnContext->txnOption.dTimerOption |= SIP_OPT_TIMER_K;
            pstTxnContext->timeoutValues.dTimerK_T4 = pTV->GetValue(SIPTimerValues::TV_TIMER_K);
        }
    }

    GLOBAL void DisplayUnknownHeaders(IN SipMessage *pstMessage)
    {
        SipError stError = E_NO_ERROR;

        IMS_UINT32 nHeaderCount = 0;
        IMS_CHAR acLog[13 + 1] = { '\0', };
        IMS_BOOL bFullLog = IMS_FALSE;

        //-----------------------------------------------------------------------------------------

        IMS_TRACE_D("___ SIP unknown headers - S ___", 0, 0, 0);

        if (pstMessage == IMS_NULL)
            goto EXIT_DisplayUnknownHeaders;

        if (sip_getHeaderCount(pstMessage, SipHdrTypeUnknown, &nHeaderCount, &stError) == SipFail)
            goto EXIT_DisplayUnknownHeaders;

        SipHeader stHeader;

        for (IMS_UINT32 i = 0; i < nHeaderCount; ++i)
        {
            stHeader.dType = SipHdrTypeAny;
            stHeader.pHeader = IMS_NULL;

            bFullLog = IMS_FALSE;

            if (sip_getHeaderAtIndex(pstMessage,
                    SipHdrTypeUnknown, &stHeader, i, &stError) == SipFail)
            {
                continue;
            }

            SipCommonHeader *pUnknown = static_cast<SipCommonHeader*>(stHeader.pHeader);

            if (pUnknown == IMS_NULL)
            {
                sip_freeSipHeader(&stHeader);
                continue;
            }

            acLog[0] = '\0';

            if (pUnknown->pStr1 != IMS_NULL)
            {
                if ((pUnknown->pStr1[0] == 'R') || (pUnknown->pStr1[0] == 'r'))
                {
                    if (IMS_StrICmp(pUnknown->pStr1, SIPHeaderName::REASON) == 0)
                    {
                        bFullLog = IMS_TRUE;
                    }
                }
                else if ((pUnknown->pStr1[0] == 'P') || (pUnknown->pStr1[0] == 'p'))
                {
                    if (IMS_StrICmp(pUnknown->pStr1, "P-SKT-BYE-CAUSE") == 0)
                    {
                        bFullLog = IMS_TRUE;
                    }
                }
            }

            IMS_TRACE_I("\t(U) %s: %s", pUnknown->pStr1,
                    bFullLog ? pUnknown->pStr2 : GetLogString(pUnknown->pStr2, acLog, 13), 0);

            sip_freeSipHeader(&stHeader);
        }

    EXIT_DisplayUnknownHeaders:

        IMS_TRACE_D("___ SIP unknown headers - E ___", 0, 0, 0);
    }

    // Return value: pszOutput (user mode & config-debug-off), pszInput (non-user mode)
    GLOBAL const IMS_CHAR* GetLogString(IN CONST IMS_CHAR *pszInput,
            IN_OUT IMS_CHAR *pszOutput, IN IMS_SINT32 nOutSize /* > 3, excluding null char */,
            IN CONST IMS_CHAR cDelimiter /* = 0 */)
    {
        //-----------------------------------------------------------------------------------------

        if (IMS_UTIL_SYS_PROP_IS_DEBUG_MODE())
        {
            return pszInput;
        }

        if (nOutSize < 3)
        {
            return pszOutput;
        }

        if (pszInput == IMS_NULL)
        {
            pszOutput[0] = 'z';
            pszOutput[1] = 'z';
            pszOutput[2] = 'z';
            pszOutput[3] = '\0';
            return pszOutput;
        }
        else if (pszInput[0] == '\0')
        {
            pszOutput[0] = 'z';
            pszOutput[1] = 'z';
            pszOutput[2] = 'z';
            pszOutput[3] = '\0';
            return pszOutput;
        }

        IMS_SINT32 i = 0;
        IMS_SINT32 nMaxSize = nOutSize - 3;

        while ((pszInput[i] != '\0') && (i < nMaxSize))
        {
            if ((cDelimiter > 0) && (pszInput[i] == cDelimiter))
                break;

            pszOutput[i] = pszInput[i];
            ++i;
        }

        pszOutput[i] = 'x';
        pszOutput[i+1] = 'x';
        pszOutput[i+2] = 'x';
        pszOutput[i+3] = '\0';

        return pszOutput;
    }

} // End of SIP (namespace)
