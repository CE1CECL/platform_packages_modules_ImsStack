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
#ifndef OS_CALL_INFO_H_
#define OS_CALL_INFO_H_

#include "IMSSlot.h"
#include "IPhoneInfoCSCall.h"
#include "OsMutex.h"
#include "system-intf/ISystemListener.h"

class OsCallInfoPrivate;
class OsCallNode;

typedef enum
{
    PHONE_IDLE = 0,
    PHONE_ACTIVE = 1,
    PHONE_HOLDING = 2,
    PHONE_DIALING = 3,
    PHONE_ALERTING = 4,
    PHONE_INCOMING = 5,
    PHONE_WAITING = 6,
    PHONE_DISCONNECTED = 7,
} PHONE_STATE_TYPE;

typedef enum
{
    CALL_IDLE = 10,
    CALL_RINGING = 11,
    CALL_OFFHOOK = 12,
} CALL_STATE_TYPE;

class OsCallInfo
    : public IMSSlot
    , public ICSCallInfo
    , public ISystemListener
{
public:
    OsCallInfo(IN IMS_SINT32 nSlotId);
    virtual ~OsCallInfo();

    OsCallInfo(IN const OsCallInfo&) = delete;
    OsCallInfo& operator=(IN const OsCallInfo&) = delete;

public:
    // ICSCallInfo class
    IMS_UINT32 GetHowManyCallExist() const override;
    ICSCallNode* GetCurrentCallNode() const override;
    CALLMODE_ENTYPE GetCSCallMode() const override;
    IMS_SINT32 GetCSCallState() override;

    // ISystemListener class
    void System_NotifyEvent(IN IMS_UINT32 nEvent, IN IMS_UINTP nWParam,
            IN IMS_UINTP nLParam) override;

    IMS_BOOL IsIdleCall(IN OsCallNode* pCallNode);

    // MT
    void UpdateCallIdle();
    void UpdateCallRinging(IN IMS_CHAR* pszPeerNumber);
    void UpdateCallOffhook();

    void UpdateCallStatus(IN IMS_UINT32 nState, IN IMS_CHAR* pszPeerNumber);

    // MO
    void UpdatePhoneIdle();
    void UpdatePhoneIncoming(IN IMS_CHAR* pszPeerNumber);
    void UpdatePhoneActive();
    void UpdatePhoneHolding();
    void UpdatePhoneAlerting();
    void UpdatePhoneWaiting();
    void UpdatePhoneDialing();
    void UpdatePhoneDisconnected();

    void UpdatePhoneStatus(IN IMS_UINT32 nState);

private:
    OsCallInfoPrivate* m_pCallInfoP;
    OsCallNode* m_pCallNode;
    ICSCallInfo::CALLMODE_ENTYPE m_eCallMode;
    OsMutex m_objMutex;
};



class OsCallNode
    : public ICSCallNode
{
public:
    inline OsCallNode()
        : m_eStatus(CALLSTATUS_DISCONNECTED)
        , m_eType(CALLTYPE_1XCALL)
        , m_eInitiator(CALLINIT_NONE)
        , m_strPeerMdn(AString::ConstNull())
        , m_nCallId(0)
        , m_nState(CALL_IDLE)
    {}
    inline virtual ~OsCallNode()
    {}

public:
    inline CALLSTATUS_ENTYPE GetCSCallStatus() const override
    { return m_eStatus; }

    inline CALLTYPE_ENTYPE GetCSCallType() const override
    { return m_eType; }

    inline CALLINIT_ENTYPE GetCSCallInitiator() const override
    { return m_eInitiator; }

    inline const AString& GetCSCallPeerMDN() const override
    { return m_strPeerMdn; }

    inline IMS_UINT32 GetCsCallState() const
    { return m_nState; }

    inline void SetCsCallStatus(IN CALLSTATUS_ENTYPE eStatus)
    { m_eStatus = eStatus; }

    inline void SetCsCallType(IN CALLTYPE_ENTYPE eType)
    { m_eType = eType; }

    inline void SetCsCallInitiator(IN CALLINIT_ENTYPE eInitiator)
    { m_eInitiator = eInitiator; }

    inline void SetPeerMdn(IN CONST IMS_CHAR* pszPeerMdn)
    { m_strPeerMdn = pszPeerMdn; }

    inline void SetCallId(IN IMS_UINT32 nCallId)
    { m_nCallId = nCallId; }

    inline void SetCsCallState(IN IMS_UINT32 nState)
    { m_nState = nState; }

private:
    CALLSTATUS_ENTYPE m_eStatus;
    CALLTYPE_ENTYPE m_eType;
    CALLINIT_ENTYPE m_eInitiator;
    AString m_strPeerMdn;
    IMS_UINT32 m_nCallId;

    IMS_UINT32 m_nState;
};

#endif
