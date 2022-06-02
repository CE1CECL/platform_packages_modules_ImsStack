/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140321  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipTxnKey.h"

namespace sipcore
{

PUBLIC
SipTxnKey::SipTxnKey() :
        objMethod(SipMethod::INVALID),
        nStatusCode(0),
        strViaBranch(AString::ConstNull()),
        nCSeq(0),
        nExtraInt(0),
        strExtraString(AString::ConstNull())
{
}

PUBLIC
SipTxnKey::SipTxnKey(IN CONST SipMethod& objMethod_, IN IMS_SINT32 nStatusCode_,
        IN CONST AString& strViaBranch_, IN IMS_UINT32 nCSeq_) :
        objMethod(objMethod_),
        nStatusCode(nStatusCode_),
        strViaBranch(strViaBranch_),
        nCSeq(nCSeq_),
        nExtraInt(0),
        strExtraString(AString::ConstNull())
{
}

PUBLIC
SipTxnKey::SipTxnKey(IN CONST SipTxnKey& objRHS) :
        objMethod(objRHS.objMethod),
        nStatusCode(objRHS.nStatusCode),
        strViaBranch(objRHS.strViaBranch),
        nCSeq(objRHS.nCSeq),
        nExtraInt(objRHS.nExtraInt),
        strExtraString(objRHS.strExtraString)
{
}

PUBLIC
SipTxnKey::~SipTxnKey() {}

PUBLIC
SipTxnKey& SipTxnKey::operator=(IN CONST SipTxnKey& objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        objMethod = objRHS.objMethod;
        nStatusCode = objRHS.nStatusCode;
        strViaBranch = objRHS.strViaBranch;
        nCSeq = objRHS.nCSeq;

        nExtraInt = objRHS.nExtraInt;
        strExtraString = objRHS.strExtraString;
    }

    return (*this);
}

/*

Remarks

*/
PUBLIC
IMS_BOOL SipTxnKey::Equals(IN CONST SipTxnKey* pKey) const
{
    //---------------------------------------------------------------------------------------------

    if (pKey == IMS_NULL)
    {
        return IMS_FALSE;
    }

    if (!objMethod.Equals(pKey->objMethod) || (nCSeq != pKey->nCSeq) ||
            !strViaBranch.Equals(pKey->strViaBranch))
    {
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipTxnKey::GetExtraInt() const
{
    //---------------------------------------------------------------------------------------------

    return nExtraInt;
}

/*

Remarks

*/
PUBLIC
const AString& SipTxnKey::GetExtraString() const
{
    //---------------------------------------------------------------------------------------------

    return strExtraString;
}

/*

Remarks

*/
PUBLIC
const SipMethod& SipTxnKey::GetMethod() const
{
    //---------------------------------------------------------------------------------------------

    return objMethod;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipTxnKey::GetCSeq() const
{
    //---------------------------------------------------------------------------------------------

    return nCSeq;
}

/*

Remarks

*/
PUBLIC
IMS_SINT32 SipTxnKey::GetStatusCode() const
{
    //---------------------------------------------------------------------------------------------

    return nStatusCode;
}

/*

Remarks

*/
PUBLIC
const AString& SipTxnKey::GetViaBranch() const
{
    //---------------------------------------------------------------------------------------------

    return strViaBranch;
}

/*

Remarks

*/
PUBLIC
void SipTxnKey::SetExtraInt(IN IMS_SINT32 nExtraInt)
{
    //---------------------------------------------------------------------------------------------

    this->nExtraInt = nExtraInt;
}

/*

Remarks

*/
PUBLIC
void SipTxnKey::SetExtraString(IN CONST AString& strExtraString)
{
    //---------------------------------------------------------------------------------------------

    this->strExtraString = strExtraString;
}

}  // namespace sipcore
