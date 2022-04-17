/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101202  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _INTERFACE_IPSEC_SA_H_
#define _INTERFACE_IPSEC_SA_H_

#include "IPAddress.h"

class IIPSecSA
{
public:
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
    virtual void SetSA
        (
        IN const IPAddress &objSrcIPA,
        IN IMS_UINT32 nSrcPort,
        IN const IPAddress &objDestIPA,
        IN IMS_UINT32 nDestPort,
        IN IMS_UINT32 nSecuProto,
        IN IMS_UINT32 nSPI,
        IN IMS_UINT32 nMode,
        IN IMS_UINT32 nAuthAlgo,
        IN IMS_UINT32 nEncrAlgo,
        IN const ByteArray &objAuthKey,
        IN const ByteArray &objEncrKey
        ) = 0;

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
    virtual void DoneSA() = 0;

};

#endif // _INTERFACE_IPSEC_SA_H_
