/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INTERFACE_MESSAGE_UTILS_H_
#define INTERFACE_MESSAGE_UTILS_H_

#include "AString.h"
#include "ImsList.h"
#include "ImsTypeDef.h"
#include "MtcDef.h"
#include "call/IMtcCall.h"

class IMessage;
class Ims3gpp;
class ISession;
struct ConfUser;
struct ReasonHeaderValue;

class IMessageUtils
{
public:
    virtual ~IMessageUtils() {}

    virtual IMessage* GetPreviousResponse(IN const ISession* piSession,
            IN IMS_SINT32 eServiceMethod, IN IMS_SINT32 nResponseIndex = -1) = 0;
    virtual IMessage* GetRemotePreviousMessage(IN ISession* piSession, IN IMS_SINT32 eServiceMethod,
            IN IMS_BOOL bIsUac, IN IMS_SINT32 nResponseIndex = -1) = 0;
    virtual IMS_SINT32 GetResponseStatusCode(IN ISession* piSession, IN IMS_SINT32 eServiceMethod,
            IN IMS_SINT32 nResponseIndex = -1) = 0;
    virtual ImsList<AString> GetRemoteUris(IN ISession* piSession, IN PeerType ePeerType) = 0;
    virtual AString GetRemoteUri(IN ISession* piSession, IN PeerType ePeerType) = 0;
    virtual AString GetSessionId(IN ISession* piSession) = 0;
    virtual ImsList<AString> GetHeaders(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetHeader(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetHeaderValue(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_SINT32 GetHeaderValueInt(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetParameterValue(IN const IMessage* piMessage,
            IN const AString& strParameterName, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual ImsList<AString> GetUserParts(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetUserPart(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual ImsList<AString> GetUserIds(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetUserId(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual ImsList<AString> GetDisplayNames(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetDisplayName(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual ImsList<AString> GetHosts(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetHost(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetParameterValueFromUri(IN IMessage* piMessage,
            IN const AString& strParameterName, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual ImsList<AString> GetUris(IN IMessage* piMessage, IN IMS_BOOL bWithParameters,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GetUri(IN IMessage* piMessage, IN IMS_BOOL bWithParameters,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_SINT32 GetFeatures(IN IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_SINT32 GetSosTypeFromServiceUrn(IN const IMessage* piMessage,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_SINT32 GetCauseFromReasonHeader(
            IN const IMessage* piMessage, IN const AString& strProtocol = AString::ConstNull()) = 0;
    virtual ReasonHeaderValue GetCauseAndTextFromReasonHeader(
            IN const IMessage* piMessage, IN const AString& strProtocol = AString::ConstNull()) = 0;
    virtual IMS_SINT32 GetSupportedFeatures(IN IMessage* piMessage) = 0;
    virtual IMS_SINT32 GetRequireFeatures(IN IMessage* piMessage) = 0;
    virtual Ims3gpp& GetIms3gppFromBody(IN const IMessage* piMessage, OUT Ims3gpp& objIms3gpp) = 0;
    virtual IMS_SINT32 GetStatusCodeInNotify(IN IMessage* piMessage) = 0;
    virtual IMS_BOOL HasSdp(IN const IMessage* piMessage) = 0;
    virtual IMS_BOOL IsFocusConf(IN const IMessage* piMessage) = 0;
    virtual IMS_BOOL IsInitialRegistrationRequired(IN const IMessage* piMessage) = 0;
    virtual IMS_BOOL ContainsValue(IN IMessage* piMessage, IN const AString& strValue,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_BOOL HasValue(IN const IMessage* piMessage, IN const AString& strValue,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_BOOL IsHeaderPresent(IN const IMessage* piMessage, IN IMS_SINT32 eHeaderType,
            IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_BOOL ContainsTag(IN const AString& strHeader, IN const AString& strTag) = 0;
    virtual IMS_BOOL ContainsAddressInPaid(
            IN const IMessage* piMessage, IN const AString& strAddress) = 0;
    virtual IMS_RESULT SetHeader(IN IMessage* piMessage, IN const AString& strValue,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual IMS_RESULT AddValueIfNotExists(IN IMessage* piMessage, IN const AString& strValue,
            IN IMS_SINT32 eHeaderType, IN const AString& strHeaderName = AString::ConstNull()) = 0;
    virtual AString GenerateContentId(
            IN const AString& strHost, IN IMS_BOOL bAngleQuote = IMS_FALSE) = 0;
    virtual IMS_RESULT SetResourceListByConfUser(IN_OUT IMessage* piMessage,
            IN const AString& strContentId, IN ImsList<ConfUser*>& lstConfUser,
            IN IMS_BOOL bMultiPart, IN IMS_BOOL bCopyControl = IMS_TRUE) = 0;
    virtual IMS_RESULT SetResourceListByEntryUri(IN_OUT IMessage* piMessage,
            IN const AString& strContentId, IN ImsList<AString>& lstEntryUri,
            IN IMS_BOOL bMultiPart, IN IMS_BOOL bCopyControl = IMS_TRUE) = 0;
    virtual IMS_BOOL IsVideoFeatureIncluded(IN const IMessage* piMessage) = 0;
    virtual IMS_BOOL IsTextFeatureIncluded(IN const IMessage* piMessage) = 0;
    virtual CallType GetCallType(
            IN const IMessage* piMessage, IN ISession* piSession, IN IMS_BOOL bPeerView) = 0;
    virtual CallType GetCallTypeFromSdp(IN ISession* piSession, IN IMS_BOOL bNegoSdp,
            IN IMS_BOOL bPeerView,
            IN IMS_BOOL bCheckPort = IMS_TRUE) = 0;  // TODO: change name of bPeerView
    virtual IMS_BOOL IsResponseExist(IN ISession* piSession, IN IMS_SINT32 nStatusCode) = 0;
};

struct ReasonHeaderValue
{
public:
    inline ReasonHeaderValue() :
            nCause(-1),
            strText(AString::ConstNull())
    {
    }
    inline ~ReasonHeaderValue() {}
    inline ReasonHeaderValue(IN const ReasonHeaderValue& objRhs) :
            nCause(objRhs.nCause),
            strText(objRhs.strText)
    {
    }

    IMS_SINT32 nCause;
    AString strText;
};

#endif
