
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101202  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _INTERFACE_IPSEC_POLICY_LISTENER_H_
#define _INTERFACE_IPSEC_POLICY_LISTENER_H_

#include "IMSTypeDef.h"

class IIPSecPolicy;

class IIPSecPolicyListener
{
public:
    /*
    Notify SA lifetime expiration

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
    virtual void ExpiredSAs(IN IIPSecPolicy* piNetIPSec) = 0;
};

#endif // _INTERFACE_IPSEC_POLICY_LISTENER_H_
