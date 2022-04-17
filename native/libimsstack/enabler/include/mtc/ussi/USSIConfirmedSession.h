/*
 * author : aromi.kwak@
 * version : 1.0
 * date :
 * brief : Create USSIConfirmedSession
 */


#ifndef USSI_CONFIRMEDSESSION_H_
#define USSI_CONFIRMEDSESSION_H_

#include "call/IMtcCall.h"

class ISIPConnection;
class ISIPClientConnection;
class ISIPServerConnection;

class USSIConfirmedSession
    // TODO, MTC BUILD
    // : public ConfirmedSession
{

/* ------------------------------------------------------------------------------------------------
    STATE
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
    Constructor, Destructor, Operator Overloading
------------------------------------------------------------------------------------------------ */
public:
    USSIConfirmedSession(IN IMtcCall* pSession);
    virtual ~USSIConfirmedSession();


/* ------------------------------------------------------------------------------------------------
    inheritance & listener
------------------------------------------------------------------------------------------------ */
public:
protected:
public:

/* ------------------------------------------------------------------------------------------------
    State Machine METHODS
------------------------------------------------------------------------------------------------ */
protected:

/* ------------------------------------------------------------------------------------------------
    VIRTUAL
------------------------------------------------------------------------------------------------ */
// TODO, MTC BUILD
// public:
//     virtual IMS_BOOL Control(IN IMS_UINT32 nCmdType,
//             IN IMS_UINTP nInParam, OUT IMS_UINTP *pnOutParam);
//     virtual void SetUSSType(IN IMS_UINT32 nUSSType);

// protected:
//     virtual IMS_BOOL OnMessage(IN IMSMSG &objMSG);
//     virtual IMS_BOOL IsUSSIInfoReceived(IN ISIPMessage* pSIPMessage);

//     virtual IMS_BOOL StateCONVERSATION_SS_Terminated(IN IMSMSG &objMsg);
//     virtual IMS_BOOL StateXXX_SS_TransactionReceived(IN IMSMSG &objMsg);
//     virtual IMS_BOOL StateXXX_SS_SendTransaction(IN IMSMSG &objMsg);

//     virtual void LoadConfig();

//     virtual void SendUSSITransactionToListn(IN ISIPMessage* pISIPMessage);

//     virtual void ClientConnection_NotifyResponse(IN ISIPClientConnection *piSCC,
//             IN ISIPClientConnection * piForkedSCC /* = IMS_NULL */);
//     virtual void Error_NotifyError(IN ISIPConnection *piSC, IN IMS_SINT32 nCode,
//             IN const AString &strMessage);
//     virtual void SendTransactionWithErrorCode(IN IMS_UINT32 nErrorCode);

// private:
//     virtual void SendTransactionResponse(IN IMS_UINT32 nResponseCode,
//             IN const AString& strPhrase = AString::ConstEmpty());
//     virtual IMS_BOOL SetUSSIBody(IN ISIPMessage* pISIPMessage, IN const AString& strUSSDStr,
//             IN IMS_UINT32 nErrorCode);
// ------------------------------------------------------------------------------------------------
// Variable
// ------------------------------------------------------------------------------------------------
public:
enum
{
    CMD_HANDLING_RESULT = 0
};

enum
{
    // TODO, MTC BUILD
    USSI_CONFIRMED_SEND_ERROR_INFO = 0
    // USSI_CONFIRMED_SEND_ERROR_INFO = CONFIRMED_COM_DEFAULT + 1
};

protected:
    ISIPClientConnection*    m_pISIPClientConnection;
    ISIPServerConnection*    m_pISIPServerConnection;

    IMS_UINT32 m_nUSSType;
};

#endif /*  USSI_CONFIRMEDSESSION_H_ */
