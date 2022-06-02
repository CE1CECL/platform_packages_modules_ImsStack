/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100323  hwangoo.park@             Created
    </table>

    Description
     This class defines an extended SIP dialog.
    It has a dialog usage to support a multiple dialog usage in SIP.
*/

#ifndef _SIP_DIALOG_EX_H_
#define _SIP_DIALOG_EX_H_

#include "SipDialogBase.h"

class SipDialogUsage;

class SipDialogEx : public SipDialogBase
{
public:
    explicit SipDialogEx(IN SipDialogState* pDState_);
    SipDialogEx(IN CONST SipDialogEx& objRHS);
    virtual ~SipDialogEx();

private:
    SipDialogEx();

public:
    SipDialogEx& operator=(IN CONST SipDialogEx& objRHS);

public:
    // For an initial requests
    IMS_BOOL InitDialog(IN CONST SipMethod& objMethod);
    // For a dialog request or incoming requests
    IMS_BOOL InitDialog(IN CONST SipMessageInfo& objMInfo);
    IMS_BOOL InitDialogWithDelay(IN CONST SipMessageInfo& objMInfo);
    IMS_BOOL CompareTo(IN CONST SipMessageInfo& objMInfo) const;
    IMS_BOOL Equals(IN SipDialogEx* pDialogEx) const;
    IMS_BOOL IsInviteUsage() const;
    IMS_BOOL IsDialogTerminated() const;
    void TerminateDialogUsage();
    IMS_SINT32 UpdateDialogDetails(IN CONST SipMessageInfo& objMInfo);

    static SipDialogEx* CreateDialog(IN CONST SipMethod& objMethod);
    static SipDialogEx* CreateDialog(IN SipDialogState* pDState, IN CONST SipMethod& objMethod);
    static SipDialogEx* CreateDialog(IN SipDialogState* pDState, IN CONST SipMessageInfo& objMInfo);

protected:
    // SipDialogBase class
    virtual IMS_BOOL OnInit();
    virtual void OnTerminated();
    virtual IMS_SINT32 OnUpdateDialogDetails(IN CONST SipMessageInfo& objMInfo,
            IN IMS_SINT32 nUsage, IN IMS_SINT32 nAction, IN IMS_SINT32 nTrigger);

private:
    IMS_BOOL bFlag_PermanentDialog;
    IMS_BOOL bFlag_InitWithDelay;
    IMS_BOOL bFlag_IsDialogTerminated;

    SipDialogUsage* pDialogUsage;
};

#endif  // _SIP_DIALOG_EX_H_
