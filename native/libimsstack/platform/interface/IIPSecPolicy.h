
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101202  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _INTERFACE_IPSEC_POLICY_H_
#define _INTERFACE_IPSEC_POLICY_H_

#include "IMSTypeDef.h"
#include "IMSList.h"

class IIPSecSA;
class IIPSecSP;
class IIPSecPolicyListener;

class IIPSecPolicy
{
public:
    /*
     Returns an Identifier of this IIPSecPolicy.
    */
    virtual IMS_SINT32 GetId() const = 0;

    /*

    Creates SP Configuration

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IIPSecSAD*              Pointer to IIPSecSAD
    </table>

    */
    virtual IIPSecSP* CreateSP() = 0;

    /*

    Destroy SP Configuration

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>
    IIPSecSP*               Pointer to IIPSecSP

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>

    */
    virtual void DestroySP(IN IIPSecSP* piSP) = 0;

    /*

    Creates SA Configuration

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IIPSecSA*               Pointer to IIPSecSA
    </table>

    */
    virtual IIPSecSA* CreateSA() = 0;

    /*

    Destroy SA Configuration

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>
    IIPSecSA*               Pointer to IIPSecSA

    Returns
    <table>
    return                  description
    ----------              ----------

    </table>

    */
    virtual void DestroySA(IN IIPSecSA* piSA) = 0;

    /*

    Manage SAs lifetime

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
    virtual void ManageLifetime(IN IMS_UINT32 nDuration) = 0;

    /*

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
    virtual void SetListener(IN IIPSecPolicyListener *piListener) = 0;

};

#endif // _INTERFACE_IPSEC_POLICY_H_
