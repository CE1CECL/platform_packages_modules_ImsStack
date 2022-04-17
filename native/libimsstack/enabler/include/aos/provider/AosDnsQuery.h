#ifndef _AOS_DNS_QUERY_H_
#define _AOS_DNS_QUERY_H_

#include "IMSActivityEx.h"
#include "IMSList.h"
#include "IPAddress.h"
#include "interface/AosInternalMsgDef.h"

class INetConnection;
class IAosDnsQueryListener;
class AosDnsQueryPrivate;

class AosDnsQuery
    : public IMSActivityEx
{
public:
    AosDnsQuery();
    virtual ~AosDnsQuery();

private:
    AosDnsQuery(IN CONST AosDnsQuery &objRHS);
    AosDnsQuery& operator=(IN CONST AosDnsQuery &objRHS);

public:
    void SetListener(IN IAosDnsQueryListener *piListener_);
    void Request  (IN AString &strDomainName_,  IN INetConnection *piConnection_);

    // Delete myself
    void Destroy();

    void DnsQueryPrivate_Ready();
    void DnsQueryPrivate_Done(IN IMS_BOOL bResult, IN IMSList<IPAddress> objIPs);
    void DnsQueryPrivate_Terminated();

private:
    // IMSActivityEx
    IMS_BOOL OnMessage(IN IMSMSG &objMSG);

    enum
    {
        MSG_READY = AOSMSG_SERVICE_INTERNAL,
        MSG_REQUEST,
        MSG_DONE,
        MSG_DESTROY,
        MSG_TERMINATED
    };

public:
    static IMS_UINT32 nIdentity;

private:
    AosDnsQueryPrivate *pPrivate;
    IAosDnsQueryListener *piListener;
    AString strDomainName;
    INetConnection *piConnection;
    IMSList<IPAddress> objIPAs;
};

class IAosDnsQueryListener
{
public:
    virtual void DnsQuery_Ready() = 0;
    virtual void DnsQuery_Done(IN IMS_BOOL bResult, IN IMSList<IPAddress> objIPs) = 0;
};

#endif // _AOS_DNS_QUERY_H_
