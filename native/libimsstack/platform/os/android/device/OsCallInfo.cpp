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
#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "device/OsCallInfo.h"
#include "system-intf/SystemConstants.h"
#include "system-intf/System.h"

__IMS_TRACE_TAG_ADAPT__;

class OsCallInfoPrivate
{
public:
    OsCallInfoPrivate();
    ~OsCallInfoPrivate();

    OsCallInfoPrivate(IN const OsCallInfoPrivate&) = delete;
    OsCallInfoPrivate& operator=(IN const OsCallInfoPrivate&) = delete;

public:
    inline IMS_UINT32 GetNumberOfCall() const
    { return m_nNumOfCall; }
    inline ICSCallInfo::CALLMODE_ENTYPE GetCallMode() const
    { return m_eCallMode; }
    inline OsCallNode* GetCurrentCallNode() const
    { return m_pCallNode; }

    inline void IncrementCall()
    { ++m_nNumOfCall; }
    inline void DecrementCall()
    {
        if (m_nNumOfCall == 0)
        {
            return;
        }

        --m_nNumOfCall;
    }
    inline void SetCallInfo(IN ICSCallInfo* piCallInfo)
    { m_piCallInfo = piCallInfo; }
    inline void SetCallMode(IN ICSCallInfo::CALLMODE_ENTYPE eMode)
    { m_eCallMode = eMode; }
    inline void SetCurrentCallNode(IN OsCallNode* pCallNode)
    {
        if (m_pCallNode != IMS_NULL)
        {
            delete m_pCallNode;
            m_pCallNode = IMS_NULL;
        }

        m_pCallNode = pCallNode;
    }

    inline void NotifyCallEvent(IN ICSCallInfoListener::CALLEVENT_ENTYPE eCallEvent,
            IN IMS_SINT32 nSlotId)
    {
        if (m_piCallInfo != IMS_NULL)
        {
            m_piCallInfo->PostMsgRegisteredThread(eCallEvent, m_pCallNode, nSlotId);
        }
    }

private:
    ICSCallInfo* m_piCallInfo;
    IMS_UINT32 m_nNumOfCall;
    ICSCallInfo::CALLMODE_ENTYPE m_eCallMode;
    OsCallNode* m_pCallNode;
};

PUBLIC
OsCallInfoPrivate::OsCallInfoPrivate()
    : m_piCallInfo(IMS_NULL)
    , m_nNumOfCall(0)
    , m_eCallMode(ICSCallInfo::CALLMODE_NONE)
    , m_pCallNode(IMS_NULL)
{
}

PUBLIC
OsCallInfoPrivate::~OsCallInfoPrivate()
{
    if (m_pCallNode != IMS_NULL)
    {
        delete m_pCallNode;
        m_pCallNode = IMS_NULL;
    }
}



PUBLIC
OsCallInfo::OsCallInfo(IN IMS_SINT32 nSlotId)
    : IMSSlot(nSlotId)
    , m_pCallInfoP(new OsCallInfoPrivate())
{
    m_pCallNode = new OsCallNode();
    m_eCallMode = ICSCallInfo::CALLMODE_NONE;

    System::GetInstance()->AddListener(
            SystemConstants::CATEGORY_CALL, this, GetSlotId());
}

PUBLIC VIRTUAL
OsCallInfo::~OsCallInfo()
{
    System::GetInstance()->RemoveListener(
            SystemConstants::CATEGORY_CALL, this, GetSlotId());

    if (m_pCallNode != IMS_NULL)
    {
        delete m_pCallNode;
    }

    if (m_pCallInfoP != IMS_NULL)
    {
        delete m_pCallInfoP;
    }
}

PUBLIC
IMS_BOOL OsCallInfo::IsIdleCall(IN OsCallNode* pCallNode)
{
    if (pCallNode != IMS_NULL)
    {
        if ((pCallNode->GetCsCallState() == PHONE_IDLE)
                || (pCallNode->GetCsCallState() == PHONE_DISCONNECTED))
        {
            return IMS_TRUE;
        }
        else if (pCallNode->GetCsCallState() == CALL_IDLE)
        {
            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}

PUBLIC
void OsCallInfo::UpdatePhoneIncoming(IN IMS_CHAR* pszPeerNumber)
{
    (void) pszPeerNumber;
}

PUBLIC
void OsCallInfo::UpdatePhoneHolding()
{
}

PUBLIC
void OsCallInfo::UpdatePhoneAlerting()
{
}

PUBLIC
void OsCallInfo::UpdatePhoneWaiting()
{
}

PUBLIC
void OsCallInfo::UpdatePhoneIdle()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if (pCurCallNode != IMS_NULL)
    {
        if (ICSCallNode::CALLINIT_MO == pCurCallNode->GetCSCallInitiator())
        {
            if ((pCurCallNode->GetCsCallState() == PHONE_ACTIVE)
                    || (pCurCallNode->GetCsCallState() == PHONE_DIALING))
            {
                OsCallNode *pCallNode = new OsCallNode();
                if (pCallNode != IMS_NULL)
                {
                    pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_NONE);
                    pCallNode->SetCsCallStatus(ICSCallNode::CALLSTATUS_DISCONNECTED);
                    pCallNode->SetCsCallState(PHONE_IDLE);

                    m_pCallInfoP->DecrementCall();
                    m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_NONE);
                    m_pCallInfoP->SetCurrentCallNode(pCallNode);

                    m_pCallInfoP->NotifyCallEvent(
                            ICSCallInfoListener::CALLEVENT_CALLENDED, GetSlotId());
                }
            }
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdatePhoneDialing()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if ((pCurCallNode == IMS_NULL) || IsIdleCall(pCurCallNode))
    {
        OsCallNode* pCallNode = new OsCallNode();

        if (pCallNode != IMS_NULL)
        {
            pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_MO);
            pCallNode->SetCsCallState(PHONE_DIALING);

            m_pCallInfoP->IncrementCall();
            m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_SINGLE);
            m_pCallInfoP->SetCurrentCallNode(pCallNode);
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdatePhoneActive()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if (pCurCallNode != IMS_NULL)
    {
        if (pCurCallNode->GetCSCallInitiator() == ICSCallNode::CALLINIT_MO)
        {
            OsCallNode *pCallNode = new OsCallNode();

            if (pCallNode != IMS_NULL)
            {
                pCallNode->SetCsCallInitiator( ICSCallNode::CALLINIT_MO );
                pCallNode->SetCsCallStatus(ICSCallNode::CALLSTATUS_CONNECTED);
                pCallNode->SetCsCallState(PHONE_ACTIVE);

                m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_SINGLE);
                m_pCallInfoP->SetCurrentCallNode(pCallNode);
            }

            m_pCallInfoP->NotifyCallEvent(ICSCallInfoListener::CALLEVENT_NEWCALL, GetSlotId());
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdatePhoneDisconnected()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if (pCurCallNode != IMS_NULL)
    {
        if (pCurCallNode->GetCSCallInitiator() == ICSCallNode::CALLINIT_MO)
        {
            if ((pCurCallNode->GetCsCallState() == PHONE_ACTIVE)
                    || (pCurCallNode->GetCsCallState() == PHONE_DIALING))
            {
                OsCallNode* pCallNode = new OsCallNode();
                if (pCallNode != IMS_NULL)
                {
                    pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_NONE);
                    pCallNode->SetCsCallStatus(ICSCallNode::CALLSTATUS_DISCONNECTED);
                    pCallNode->SetCsCallState(PHONE_DISCONNECTED);

                    m_pCallInfoP->DecrementCall();
                    m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_NONE);
                    m_pCallInfoP->SetCurrentCallNode(pCallNode);

                    m_pCallInfoP->NotifyCallEvent(
                            ICSCallInfoListener::CALLEVENT_CALLENDED, GetSlotId());
                }
            }
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdatePhoneStatus(IN IMS_UINT32 nState)
{
    switch (nState)
    {
        case PHONE_IDLE:
            UpdatePhoneIdle( );
            break;

        case PHONE_ACTIVE:
            UpdatePhoneActive();
            break;

        case PHONE_HOLDING:
            //UpdatePhoneHolding();
            break;

        case PHONE_DIALING:
            UpdatePhoneDialing();
            break;

        case PHONE_ALERTING:
            //UpdatePhoneAlerting();
            break;

        case PHONE_INCOMING:
            //UpdatePhoneIncoming();
            break;

        case PHONE_WAITING:
            //UpdatePhoneWaiting();
            break;

        case PHONE_DISCONNECTED:
            UpdatePhoneDisconnected();
            break;

        default:
            // no-op
            break;
    }

}

PUBLIC
void OsCallInfo::UpdateCallIdle()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if (pCurCallNode != IMS_NULL)
    {
        if (pCurCallNode->GetCSCallInitiator() == ICSCallNode::CALLINIT_MT)
        {
            if ((pCurCallNode->GetCsCallState() == CALL_RINGING)
                    || (pCurCallNode->GetCsCallState() == CALL_OFFHOOK))
            {
                OsCallNode *pCallNode = new OsCallNode();

                if (pCallNode != IMS_NULL)
                {
                    pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_NONE);
                    pCallNode->SetCsCallStatus(ICSCallNode::CALLSTATUS_DISCONNECTED);
                    pCallNode->SetCsCallState(CALL_IDLE);

                    m_pCallInfoP->DecrementCall();
                    m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_NONE);
                    m_pCallInfoP->SetCurrentCallNode(pCallNode);

                    m_pCallInfoP->NotifyCallEvent(
                            ICSCallInfoListener::CALLEVENT_CALLENDED, GetSlotId());
                }
            }
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdateCallOffhook()
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if (pCurCallNode != IMS_NULL)
    {
        if (pCurCallNode->GetCSCallInitiator() == ICSCallNode::CALLINIT_MT)
        {
            OsCallNode* pCallNode = new OsCallNode();

            if (pCallNode != IMS_NULL)
            {
                pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_MT);
                pCallNode->SetCsCallStatus(ICSCallNode::CALLSTATUS_CONNECTED);
                pCallNode->SetCsCallState(CALL_OFFHOOK);

                m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_SINGLE);
                m_pCallInfoP->SetCurrentCallNode(pCallNode);

                m_pCallInfoP->NotifyCallEvent(ICSCallInfoListener::CALLEVENT_NEWCALL,
                        GetSlotId());
            }
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdateCallRinging(IN IMS_CHAR* pszPeerNumber)
{
    if (m_pCallInfoP == IMS_NULL)
    {
        return;
    }

    m_objMutex.Lock();

    OsCallNode* pCurCallNode = m_pCallInfoP->GetCurrentCallNode();

    if ((pCurCallNode == IMS_NULL) || IsIdleCall(pCurCallNode))
    {
        OsCallNode* pCallNode = new OsCallNode();

        if (pCallNode != IMS_NULL)
        {
            pCallNode->SetCsCallInitiator(ICSCallNode::CALLINIT_MT);
            pCallNode->SetCsCallState(CALL_RINGING);
            pCallNode->SetPeerMdn(pszPeerNumber);

            m_pCallInfoP->IncrementCall();
            m_pCallInfoP->SetCallMode(ICSCallInfo::CALLMODE_SINGLE);
            m_pCallInfoP->SetCurrentCallNode(pCallNode);
        }
    }

    m_objMutex.Unlock();
}

PUBLIC
void OsCallInfo::UpdateCallStatus(IN IMS_UINT32 nState, IN IMS_CHAR* pszPeerNumber)
{
    IMS_UINT32 nLocalState = nState + CALL_IDLE;

    switch (nLocalState)
    {
        case CALL_IDLE:
            UpdateCallIdle();
            break;
        case CALL_RINGING:
            UpdateCallRinging(pszPeerNumber);
            break;
        case CALL_OFFHOOK:
            UpdateCallOffhook();
            break;
        default:
            // no-op
            break;
    }
}

PUBLIC VIRTUAL
void OsCallInfo::System_NotifyEvent(IN IMS_UINT32 nEvent,
        IN IMS_UINTP nWParam, IN IMS_UINTP nLParam)
{
    IMS_TRACE_D("CallInfo :: event=%d, wp=%" PFLS_d ", lp=%" PFLS_d, nEvent, nWParam, nLParam);

    switch (nEvent)
    {
        case IMS_SYSTEM_CSCALL_STATE_CHANGED: {
            IMS_UINT32 nState = LONG_TO_INT(nWParam);
            IMS_CHAR* pszPeerNumber = reinterpret_cast<IMS_CHAR*>(nLParam);

            IMS_TRACE_D("CS call state changed - state=%d, number=%s",
                    nState, _TRACE_S_(pszPeerNumber), 0);

            UpdateCallStatus(nState, pszPeerNumber);
            break;
        }
        case IMS_SYSTEM_PHONE_STATE_CHANGED: {
            IMS_TRACE_D("Phone state changed - state=%" PFLS_u, nWParam, 0, 0);
            UpdatePhoneStatus(nWParam);
            break;
        }
        default:
            // no-op
            break;
    }
}

PUBLIC VIRTUAL
IMS_UINT32 OsCallInfo::GetHowManyCallExist() const
{
    return m_pCallInfoP->GetNumberOfCall();
}

PUBLIC VIRTUAL
ICSCallNode* OsCallInfo::GetCurrentCallNode() const
{
    return m_pCallInfoP->GetCurrentCallNode();
}

PUBLIC VIRTUAL
ICSCallInfo::CALLMODE_ENTYPE OsCallInfo::GetCSCallMode() const
{
    return m_pCallInfoP->GetCallMode();
}

PUBLIC VIRTUAL
IMS_SINT32 OsCallInfo::GetCSCallState()
{
    return System::GetInstance()->GetCallState(GetSlotId());
}
