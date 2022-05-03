/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SipMethod.h"



PUBLIC GLOBAL
const IMS_CHAR* SipMethod::NAME[] =
{
    "ACK",
    "BYE",
    "CANCEL",
    "INVITE",
    "OPTIONS",
    "REGISTER",
    "PRACK",
    "SUBSCRIBE",
    "NOTIFY",
    "UPDATE",
    "MESSAGE",
    "REFER",
    "PUBLISH",
    "INFO",
    "UNKNOWN"
};

PUBLIC GLOBAL
const SipMethod SipMethod::INVALID_METHOD;



PUBLIC
SipMethod::SipMethod(IN CONST IMS_SINT32 nMethod_ /* = SipMethod::INVALID */)
    : nMethod(nMethod_)
{
    //---------------------------------------------------------------------------------------------

    strMethod = ConvertMethodToString(nMethod_);
}

PUBLIC
SipMethod::SipMethod(IN CONST IMS_CHAR *pszMethod_)
    : strMethod(pszMethod_)
{
    //---------------------------------------------------------------------------------------------

    nMethod = ConvertStringToMethod(strMethod);
}

PUBLIC
SipMethod::SipMethod(IN CONST AString &strMethod_)
    : strMethod(strMethod_)
{
    //---------------------------------------------------------------------------------------------

    nMethod = ConvertStringToMethod(strMethod);
}

PUBLIC
SipMethod::SipMethod(IN CONST SipMethod &objRHS)
    : nMethod(objRHS.nMethod)
    , strMethod(objRHS.strMethod)
{
}

PUBLIC
SipMethod::~SipMethod()
{
}

PUBLIC
SipMethod& SipMethod::operator=(IN CONST SipMethod &objRHS)
{
    //---------------------------------------------------------------------------------------------

    if (this != &objRHS)
    {
        nMethod = objRHS.nMethod;
        strMethod = objRHS.strMethod;
    }

    return (*this);
}

PUBLIC
SipMethod& SipMethod::operator=(IN IMS_SINT32 nMethod_)
{
    //---------------------------------------------------------------------------------------------

    if (nMethod == nMethod_)
        return (*this);

    nMethod = nMethod_;
    strMethod = ConvertMethodToString(nMethod);

    return (*this);
}

PUBLIC
SipMethod& SipMethod::operator=(IN CONST IMS_CHAR *pszMethod_)
{
    //---------------------------------------------------------------------------------------------

    if (strMethod.Equals(pszMethod_))
        return (*this);

    strMethod = pszMethod_;
    nMethod = ConvertStringToMethod(strMethod);

    return (*this);
}

PUBLIC
SipMethod& SipMethod::operator=(IN CONST AString &strMethod_)
{
    //---------------------------------------------------------------------------------------------

    if (strMethod.Equals(strMethod_))
        return (*this);

    strMethod = strMethod_;
    nMethod = ConvertStringToMethod(strMethod);

    return (*this);
}

PUBLIC GLOBAL
const IMS_CHAR* SipMethod::ToName(IN IMS_SINT32 nMethod)
{
    //---------------------------------------------------------------------------------------------

    return ((nMethod > INVALID) && (nMethod < MAX)) ? NAME[nMethod] : "";
}

PRIVATE GLOBAL
IMS_SINT32 SipMethod::ConvertStringToMethod(IN CONST AString &strMethod)
{
    //---------------------------------------------------------------------------------------------

    if (strMethod.IsNULL() || strMethod.IsEmpty())
        return SipMethod::INVALID;
    else
    {
        for (IMS_SINT32 i = (SipMethod::INVALID + 1); i < SipMethod::MAX; i++)
        {
            if (strMethod.Equals(SipMethod::NAME[i]))
            {
                return i;
            }
        }
    }

    return SipMethod::INVALID;
}

PRIVATE GLOBAL
AString SipMethod::ConvertMethodToString(IN IMS_SINT32 nMethod)
{
    //---------------------------------------------------------------------------------------------

    if ((nMethod > SipMethod::INVALID) && (nMethod < SipMethod::MAX))
        return AString(NAME[nMethod]);

    return AString::ConstNull();
}
