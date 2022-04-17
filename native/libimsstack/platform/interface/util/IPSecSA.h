
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101202  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _IPSEC_SA_H_
#define _IPSEC_SA_H_

#include "IMSIPSecType.h"

class SAInfoP;

class IPSecSA
{
public:
    IPSecSA();
    ~IPSecSA();

private:
    IPSecSA(IN const IPSecSA& objRHS);
    IPSecSA& operator=(IN const IPSecSA& objRHS);

public:
    // Upper layer Protocol
    enum
    {
        TRANS_PROTOCOL_UDP = 0,
        TRANS_PROTOCOL_TCP,
        TRANS_PROTOCOL_BOTH
    };

    void SetSA
        (
        IN IMS_UINT32 nTransProtocol,
        IN IMS_UINT32 nUEPort,
        IN IMS_UINT32 nServerPort,
        IN IMS_UINT32 nSPI,
        IN IMS_UINT32 nDirection
        );

    // specific purpose data related IPSec Lib
    void SetContext(IN IMS_PVOID pContext);

    IMS_UINT32 GetTransProtocol();
    IMS_UINT32 GetUePort();
    IMS_UINT32 GetServerPort();
    IMS_UINT32 GetSPI();
    IMS_UINT32 GetDirection();

    void GetSAInfo
        (
        OUT IMS_UINT32 &nTransProtocol,
        OUT IMS_UINT32 &nUEPort,
        OUT IMS_UINT32 &nServerPort,
        OUT IMS_UINT32 &nSPI,
        OUT IMS_UINT32 &nDirection
        );

    // specific purpose data related IPSec Lib
    IMS_PVOID GetContext();

private:
    SAInfoP* pSAInfoP;
};

#endif // _IPSEC_SA_H_
