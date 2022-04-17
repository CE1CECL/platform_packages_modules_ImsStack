/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101202  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _INTERFACE_IPSEC_SP_H_
#define _INTERFACE_IPSEC_SP_H_

#include "IPAddress.h"

class IIPSecSP
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
    virtual void SetTransportInfo
        (
        IN const IPAddress &objSrcIPA,
        IN IMS_UINT32 nSrcPort,
        IN const IPAddress &objDestIPA,
        IN IMS_UINT32 nDestPort,
        IN IMS_UINT32 nTransProtocol,    // TCP | UDP
        IN IMS_UINT32 nAction,           // APPLY | PERMIT | DROP | BYPASS
        IN IMS_UINT32 nDirection,        // INBOUND | OUTBOUND
        IN IMS_UINT32 nSPI,
        IN IMS_UINT32 nMode              // TRANSPORT | TUNNEL
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
    virtual void SetSecurityAlgorithmInfo
        (
        IN IMS_UINT32 nSecuProto,       // AH | ESP | ESP_AUTH | ESP_NULL
        IN IMS_UINT32 nAuthAlgo,        // HMAC -MD5 | HMAC-SHA-1
        IN IMS_UINT32 nEncrAlgo         // DES | 3DES | AES
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
    virtual void DoneSP() = 0;

};

#endif // _INTERFACE_IPSEC_SP_H_
