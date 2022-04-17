/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090831  yhrhee@                   Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_PHONE_INFO_CS_CALL_H_
#define _INTERFACE_IMS_PHONE_INFO_CS_CALL_H_

#include "IMSMsgDef.h"
#include "ServiceThread.h"
#include "ServiceMSG.h"

class ICSCallInfo;

class ICSCallNode
{
public:
    typedef enum
    {
        CALLSTATUS_CONNECTED,
        CALLSTATUS_DISCONNECTED,

        CALLSTATUS_NOTUSED,
    } CALLSTATUS_ENTYPE;

    typedef enum
    {
        CALLTYPE_1XCALL,
        CALLTYPE_1XDATA,
        CALLTYPE_1XSMS,

        CALLTYPE_NONE,

    } CALLTYPE_ENTYPE;

    typedef enum
    {
        CALLINIT_MO,
        CALLINIT_MT,

        CALLINIT_NONE

    } CALLINIT_ENTYPE;

    /*
    Remarks
        Checks if CS call is connected or not.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>
    : Mobile Terminated

    </table>
    */
    virtual CALLSTATUS_ENTYPE GetCSCallStatus() const = 0;

    /*
    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    CALLINIT_ENTYPE         CALLINIT_MO : Mobile Oriented
                            CALLINIT_MT : Mobile Terminated
                            CALLINIT_NONE : Disconnected

    </table>
    */
    virtual CALLTYPE_ENTYPE GetCSCallType() const = 0;

    /*
    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    CALLINIT_ENTYPE         CALLINIT_MO : Mobile Oriented
                            CALLINIT_MT : Mobile Terminated
                            CALLINIT_NONE : Disconnected

    </table>
    */
    virtual CALLINIT_ENTYPE GetCSCallInitiator() const = 0;

    /*
    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strPeerMDN              Phone number of remote endpoint excluding '+'
                            eg. +8210xxxx -> 8210xxxx
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Remote's phone number of the current call
    </table>
    */
    virtual const AString& GetCSCallPeerMDN() const = 0;
};

/*
ICSCallInfoListener
    Monitors the call state information of CS call.
*/
class ICSCallInfoListener
{
public:
    typedef enum
    {
        CALLEVENT_NEWCALL,         /* New call */
        CALLEVENT_CALLENDED,       /* Current call is ended (but another call can be remained) */

        CALLEVENT_MODECHANED,      /* Call's state is changed by me or other party */

        CALLEVENT_NOTUSED,
    } CALLEVENT_ENTYPE;

public:
    virtual void NotifyCSCallStatus(IN ICSCallInfo *piCallInfo, IN CALLEVENT_ENTYPE eEvt,
            IN ICSCallNode *piCallNode) = 0;
};

class ICSCallInfo
{
public:
    typedef enum
    {
        CALLMODE_SINGLE,          /* 1-to-1 call exists */
        CALLMODE_FORWARDED,       /* call forwarding is in progress */
        CALLMODE_NOTSINGLE,       /* conference call exists */
        CALLMODE_HOLD,            /* call is in hold */
        CALLMODE_SWAP,            /* In multiple calls, call is swapped */

        CALLMODE_NONE

    } CALLMODE_ENTYPE;

private:
    class CallEvent
    {
    public:
        inline CallEvent(IN ICSCallInfoListener::CALLEVENT_ENTYPE enEvent_, IN ICSCallNode *piNode_)
            : enEvent(enEvent_)
            , piNode(piNode_)
        {}

    public:
        ICSCallInfoListener::CALLEVENT_ENTYPE enEvent;
        ICSCallNode *piNode;
    };

public:
    /*
    Remarks

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>
    */
    inline void SubscribeCSCallStatusListener(IN ICSCallInfoListener *piListener)
    {
        IThread *piThread = ThreadService::GetThreadService()->GetCurrentThread();

        for (IMS_UINT32 i = 0; i < objObserverLists.GetSize(); i++)
        {
            ObserverList *pObserverList = objObserverLists.GetAt(i);

            if (pObserverList == IMS_NULL)
            {
                continue;
            }

            if (*pObserverList == piThread)
            {
                pObserverList->objListeners.Append(piListener);
                return;
            }
        }

        objObserverLists.Append(new ObserverList(piListener));
    }

    inline void UnsubscribeCSCallStatusListener(IN ICSCallInfoListener *piListener)
    {
        IThread *piThread = ThreadService::GetThreadService()->GetCurrentThread();

        for (IMS_UINT32 i = 0; i < objObserverLists.GetSize(); i++)
        {
            ObserverList *pObserverList = objObserverLists.GetAt(i);

            if (pObserverList == IMS_NULL)
            {
                continue;
            }

            if (*pObserverList == piThread)
            {
                for (IMS_UINT32 j = 0; j < pObserverList->objListeners.GetSize(); j++)
                {
                    ICSCallInfoListener *objListener = pObserverList->objListeners.GetAt(j);

                    if (piListener == objListener)
                    {
                        pObserverList->objListeners.RemoveAt(j);
                        break;
                    }
                }
                break;
            }
        }
    }

public:
    inline void PostMsgRegisteredThread(IN ICSCallInfoListener::CALLEVENT_ENTYPE eEvt,
            IN ICSCallNode *piCallNode, IN IMS_SINT32 nSlotId)
    {
        for (IMS_UINT32 i = 0; i < objObserverLists.GetSize(); ++i)
        {
            ObserverList *pObserverList = objObserverLists.GetAt(i);

            if (pObserverList == IMS_NULL)
            {
                continue;
            }

            CallEvent *pEvent = new CallEvent(eEvt, piCallNode);

            IMS_MSG_CreateNPostThreadMessage(
                    pObserverList->piOwnerThread, IMS_MSG_CS_CALL_STATUS, nSlotId, pEvent);
        }
    }

public:
    inline void ProcessNotify(IN IMSMSG &objMSG)
    {
        CallEvent *pEvent = reinterpret_cast<CallEvent*>(objMSG.nLparam);

        if (pEvent == IMS_NULL)
        {
            return;
        }

        ICSCallInfoListener::CALLEVENT_ENTYPE eEvt = pEvent->enEvent;
        ICSCallNode *piCallNode = pEvent->piNode;

        delete pEvent;

        IThread *piThread = ThreadService::GetThreadService()->GetCurrentThread();

        for (IMS_UINT32 i = 0; i < objObserverLists.GetSize(); ++i)
        {
            ObserverList *pObserverList = objObserverLists.GetAt(i);

            if (pObserverList == IMS_NULL)
            {
                continue;
            }

            if (*pObserverList == piThread)
            {
                for (IMS_UINT32 j = 0; j < pObserverList->objListeners.GetSize(); ++j)
                {
                    ICSCallInfoListener* piListener = pObserverList->objListeners.GetAt(j);

                    if (piListener != IMS_NULL)
                    {
                        piListener->NotifyCSCallStatus(this, eEvt, piCallNode);
                    }
                }
                break;
            }
        }
    }

private:
    class ObserverList
    {
        public:
            inline ObserverList(IN ICSCallInfoListener *piListener)
            {
                piOwnerThread = ThreadService::GetThreadService()->GetCurrentThread();

                objListeners.Append(piListener);
            }

            inline IMS_BOOL operator==(IN IThread *piThread)
            {
                return piThread == piOwnerThread;
            }

        public:
            IThread *piOwnerThread;
            IMSList<ICSCallInfoListener*> objListeners;
    };

public:
    virtual IMS_UINT32 GetHowManyCallExist() const = 0;
    virtual ICSCallNode* GetCurrentCallNode() const = 0;
    virtual CALLMODE_ENTYPE GetCSCallMode() const = 0;
    virtual IMS_SINT32 GetCSCallState() = 0;

private:
    IMSList<ObserverList*> objObserverLists;
};

#endif // _INTERFACE_IMS_PHONE_INFO_CS_CALL_H_
