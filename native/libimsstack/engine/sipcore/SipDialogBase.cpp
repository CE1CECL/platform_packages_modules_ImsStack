/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100323  hwangoo.park@             Created
    </table>

    Description
     This class defines a base class for SIP dialog class.
*/

#include "SipDialogBase.h"

PUBLIC
SipDialogBase::SipDialogBase(IN SipDialogState* pDState_) :
        RCObject(),
        pDState(pDState_)
{
}

PUBLIC
SipDialogBase::SipDialogBase(IN CONST SipDialogBase& objRHS) :
        RCObject(objRHS),
        pDState(objRHS.pDState)
{
}

PUBLIC VIRTUAL SipDialogBase::~SipDialogBase() {}

/*

Remarks

*/
PUBLIC
SipDialogBase& SipDialogBase::operator=(IN CONST SipDialogBase& objRHS)
{
    //---------------------------------------------------------------------------------------------

    RCObject::operator=(objRHS);

    return (*this);
}

/*

Remarks

*/
PUBLIC
SipDialogState* SipDialogBase::GetDialogState() const
{
    //---------------------------------------------------------------------------------------------

    return pDState.Get();
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipDialogBase::GetState() const
{
    //---------------------------------------------------------------------------------------------

    return pDState->GetState();
}

/*

Remarks

*/
PUBLIC GLOBAL IMS_BOOL SipDialogBase::IsDialogCreatable(IN CONST SipMethod& objMethod)
{
    //---------------------------------------------------------------------------------------------

    if (objMethod.Equals(SipMethod::INVALID))
        return IMS_FALSE;

    // Check if the current method can create a dialog or not
    if (!objMethod.Equals(SipMethod::INVITE) && !objMethod.Equals(SipMethod::SUBSCRIBE) &&
            !objMethod.Equals(SipMethod::REFER)
            /* && !objMethod.Equals(SipMethod::NOTIFY) */)
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}
