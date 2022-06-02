/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100317  hwangoo.park@             Created
    </table>

    Description
     This class defines a shared dialog state.
    SipDialogState class MUST have this class as its member.
*/

#ifndef _SIP_DIALOG_SHARED_STATE_H_
#define _SIP_DIALOG_SHARED_STATE_H_

#include "IMSList.h"
#include "SipMessageInfo.h"

class SipDialogEx;

class SipDialogSharedState
{
public:
    SipDialogSharedState();
    ~SipDialogSharedState();

private:
    SipDialogSharedState(IN CONST SipDialogSharedState& objRHS);
    SipDialogSharedState& operator=(IN CONST SipDialogSharedState& objRHS);

private:
    IMS_BOOL AddDialog(IN SipDialogEx* pDialogEx);
    void RemoveDialog(IN SipDialogEx* pDialogEx);
    SipDialogEx* GetDialog(IN CONST SipMessageInfo& objMInfo);
    IMS_BOOL HasMultipleDialogUsages() const;
    inline IMS_BOOL IsShared() const { return (nSharedState == SHARED_STATE_ACTIVE); }

private:
    friend class SipDialogState;

    enum
    {
        SHARED_STATE_INIT = 0,
        SHARED_STATE_ACTIVE,
        SHARED_STATE_TERMINATED
    };

    IMS_SINT32 nSharedState;

    // References of dialog state
    IMSList<SipDialogEx*> objDialogExs;
};

#endif  // _SIP_DIALOG_SHARED_STATE_H_
