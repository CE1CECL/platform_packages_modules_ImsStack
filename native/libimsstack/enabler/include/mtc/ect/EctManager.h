/*
 * author : seyoon0802.jung@
 * version : 0.1
 * date : 201507
 * brief :
 */


#ifndef UC_ECT_MNGR_H_
#define UC_ECT_MNGR_H_

#include "IMSStateMachine.h"
#include "IMSActivityEx.h"
#include "IIMSActivityControl.h"

#include "call/IMtcCallManager.h"

#include "ect/IEctManagerListener.h"
#include "ect/EctReference.h"
#include "helper/IMtcTimerListener.h"
#include "helper/MtcTimerWrapper.h"
#include "IMtcApp.h"

class ISession;
class IMtcCall;
class IMtcService;
class IMessage;
class ISIPClientConnection;

class UCECTMngr
    : public IMSStateMachine
    , public IMSActivityEx
    , public IIMSActivityControl
    // TODO, MTC BUILD
    // , public IUCSessionListener
    , public IUCECTReferenceListener
    , public IMtcTimerListener
{

/* ------------------------------------------------------------------------------------------------
STATE
------------------------------------------------------------------------------------------------ */
// TODO, MTC BUILD
// DECLARE_STATE_MAP()
// DECLARE_STATE_MSG_MAP(IDLE)
// DECLARE_STATE_MSG_MAP(NOTIFYING)
// DECLARE_STATE_MSG_MAP(INHR)
// DECLARE_STATE_MSG_MAP(RINGBACK)
// DECLARE_STATE_MSG_MAP(RINGING)
// DECLARE_STATE_MSG_MAP(REFERRING)
// DECLARE_STATE_MSG_MAP(WAITINGNOTIFY)
// DECLARE_STATE_MSG_MAP(WAITINGBYE)
// DECLARE_STATE_MSG_MAP(TERMINATING)


public:
    UCECTMngr(IN IMtcApp* pApp, IN IMtcCall* pTransfereeSession,
            IN IMtcCall* pTransferTargetSession, IN IMtcCall* pTransferredSession);
    virtual ~UCECTMngr();

private:
    UCECTMngr(IN CONST UCECTMngr &objRHS);
    UCECTMngr& operator=(IN CONST UCECTMngr &objRHS);

public:
    virtual IMS_BOOL Control(IN IMS_UINT32 nCmdType, IN IMS_UINTP nInParam,
            OUT IMS_UINTP *pnOutParam);
    virtual IIMSActivityControl* GetController(); // IMSActivity class
    virtual void UCTimer_Expired(IN IMS_SINT32 eType);

    // TODO, MTC BUILD
    // virtual void RingBack(IN IMS_UINTP nParam);
    // virtual void Ringing(IN IMS_UINTP nParam);
    // virtual void Started(IN IMS_UINTP nParam);
    // virtual void StartFailed(IN IMS_UINTP nParam);
    // virtual void Updated(IN IMS_UINTP nParam);
    // virtual void UpdateFailed(IN IMS_UINTP nParam);
    // virtual void Terminated(IN IMS_UINTP nParam);
    // virtual void Destroyed(IN IMS_UINTP nParam);
    // virtual void ReferenceReceived(IN IMS_UINTP nParam);

    virtual void SetListener(IN IUCECTMngrListener* pListener);

    // IGlobalECTReferenceListener
    virtual void Refer_Delivered(IN IMS_UINTP nParam);
    virtual void Refer_DeliveryFailed(IN IMS_UINTP nParam);
    virtual void Refer_Notify(IN IMS_UINTP nParam);
    virtual void Refer_Terminated(IN IMS_UINTP nParam);
    virtual void Refer_Failed(IN IMS_UINTP nParam);
    virtual void Refer_Notify_Delivered(IN IMS_UINTP nParam);
    virtual void Refer_Notify_DeliveryFailed(IN IMS_UINTP nParam);

    void Init(IN IMtcCall* pTransfereeSession, IN IMtcCall* pTransferTargetSession,
            IN IMtcCall* pTransferredSession);

    void DeInit();
    void Destroy();

protected:
    virtual IMS_BOOL OnMessage(IN IMSMSG &objMSG);
    // TODO, MTC BUILD
    // virtual IMS_BOOL StateIDLE_ECTStart(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateIDLE_ECTStartBlind(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateIDLE_ReferenceReceived(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateIDLE_IncomingSession(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateIDLE_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateNOTIFYING_NOTIFYDelivered(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateNOTIFYING_NOTIFYDeliveryFailed(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateNOTIFYING_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateINHR_Held(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateINHR_HoldFailed(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateINHR_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateRINGBACK_Started(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateRINGBACK_StartFailed(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateRINGBACK_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateRINGING_Started(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateRINGING_StartFailed(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateRINGING_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateREFERRING_REFERDelivered(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateREFERRING_REFERDeliverFailed(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateREFERRING_REFERNotify(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateREFERRING_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateWAITINGNOTIFY_REFERNotify(IN IMSMSG &objMsg);
    // virtual IMS_BOOL StateWAITINGNOTIFY_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateWAITINGBYE_Terminated(IN IMSMSG &objMsg);

    // virtual IMS_BOOL StateTERMINATING_Terminated(IN IMSMSG &objMsg);
    // virtual AString GetReferToURI(IN AString aStrReplaceURI);
    // virtual IMtcCall* CreateComSession(IN IMtcService* pService, IN AString aStrUIKey,
    //         IN IMS_UINTP nIMSKey);
    // virtual UCECTReference* CreateComReference(IN IMtcCall* pSession);

    // IMS_BOOL CreateRefer();
    // IMS_BOOL SendRefer();
    // IMtcCall* CreateSession(IN IMtcService* pService, IN AString UIKey, IN IMS_UINTP IMSKey);
    // IMS_BOOL UpdateService(IN IMtcCall* pSession, IN IMtcService* pService,
    //         IN IMS_UINT32 eLocalServiceType);

    // AString GetTransferTargetURI();
    // void SetState(IN IMS_UINT32 eState);
    // void SendResumedByToUI(IN IMtcCall* pExistSession);
    // void SendECTStartedByToUI(IN IMtcCall* pExistSession);
    // void SendECTResultToUI(IN IMS_BOOL bResult, IN IMS_SINT32 eReason, IN IMS_SINT32 eCode = -1);
    // IMS_BOOL CreateECTSession();
    // void SetBlindECT(IN IMS_BOOL bBlindECT);
    const IMS_CHAR* PrintState(IN IMS_SINT32 eState = -1);

    // IMtcCall* GetSession(IMS_SINTP nSessionKey);

private:
    // TODO, MTC BUILD
    // IMS_BOOL DestroyRefer();
    // IMS_BOOL SendHold();
    // IMS_BOOL HandleTransfereeWaitBYE();
    // IMS_BOOL SetSessionListn();
    // void ResetSessionListn();
    // IMS_BOOL IsBlindECT();
    // IMS_BOOL IsECTTerminateRequired(IN IMS_SINTP nDestroyedSessionKey); // naming...

public:
    virtual IMS_UINT32 GetState();
    virtual IMS_UINT32 GetOldState();


    enum
    {
        IDLE,
        NOTIFYING,
        INHR,
        RINGBACK,
        RINGING,
        REFERRING,
        WAITINGNOTIFY,
        WAITINGBYE,
        TERMINATING
    };

    enum
    {
        UCECTMNGR_BASE_DEFAULT = MTC_INTERNAL_MSG::ECT_MSG_BASE,

        UCECTMNGR_ECT_START,
        UCECTMNGR_REFERENCE_RECEIVED,
        UCECTMNGR_INVITE_RECEIVED,
        UCECTMNGR_HOLD,

        UCECTMNGR_REFERENCE_DELIVERED,
        UCECTMNGR_REFERENCE_DELIVERFAILED,
        UCECTMNGR_REFERENCE_TERMINATED,
        UCECTMNGR_NOTIFY_RECEIVED,

        UCECTMNGR_HR_HELD,
        UCECTMNGR_HR_HOLDFAILED,

        UCECTMNGR_NOTIFY_DELIVERED,
        UCECTMNGR_NOTIFY_DELIVERYFAILED,

        UCECTMNGR_SS_STARTED,
        UCECTMNGR_SS_STARTFAILED,
        UCECTMNGR_SS_TERMINATED,

        UCECTMNGR_ECT_START_BLIND,

        UCECTMNGR_COM_DEFAULT = UCECTMNGR_BASE_DEFAULT + 50
    };

    protected:
    enum
    {
        /* Base Timer */
        TIMER_BASE_DEFAULT = 0,
        TIMER_TRANSFEREE_WAIT_BYE = 1,

        TIMER_COM_DEFAULT = 20

    }; /*TIMERTYPE */

private:

protected:
    IMS_UINT32                  m_eState;
    IMS_UINT32                  m_eOldState;

    IMtcApp*                     m_pApp;
    IMS_SINT32                  m_nSlotID;

    IMS_SINTP                 m_pTransfereeSessKey;
    IMS_SINTP                 m_pTransferTargetSessKey;
    IMS_SINTP                 m_pTransferredSessKey;

    AString                     m_strUIKey;

    IUCECTMngrListener*         m_pListener;

    IMtcCallManager*                m_pUCSessList;
    AString                     m_strTransferTargetURI;
    UCECTReference*             m_pECTReference;

    MtcTimerWrapper*               m_pTimer;

    IMS_BOOL                    m_bBlindECT;

private:
    static const IMS_SINT32     ECT_WAIT_BYE_TIME = 3000; //ms
    static const IMS_CHAR       STR_USER_PHONE[];
    static const IMS_CHAR       STR_REFER_REFERTOEX[];
};

/* --------------------------------------------------------------------------------------------- */
class UCECTMngrBaseParam
{
public:
    inline UCECTMngrBaseParam()
        : pSessList(IMS_NULL)
        , pMsgParam(IMS_NULL)
    {}
    inline virtual ~UCECTMngrBaseParam()
    {}

private:
    UCECTMngrBaseParam(IN const UCECTMngrBaseParam &objRHS);
    UCECTMngrBaseParam& operator=(IN const UCECTMngrBaseParam &objRHS);

public:
    IMtcCallManager*        pSessList;
    void*               pMsgParam;
};

// ------------------------------------------------------------------------------------------------
class UCECTMngrStartParam
    : public UCECTMngrBaseParam
{
public:
    inline UCECTMngrStartParam()
        : UCECTMngrBaseParam()
        , aStrTarget(AString::ConstNull())
    {}
    inline virtual ~UCECTMngrStartParam()
    {}

private:
    UCECTMngrStartParam(IN const UCECTMngrStartParam &objRHS);
    UCECTMngrStartParam& operator=(IN const UCECTMngrStartParam &objRHS);

public:
    AString aStrTarget;
};

#endif // UC_ECT_MNGR_H_
