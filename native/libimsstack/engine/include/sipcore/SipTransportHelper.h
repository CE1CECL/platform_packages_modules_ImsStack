/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090326  toastops@                 Created
    </table>

    Description

*/

#ifndef _SIP_TRANSPORT_HELPER_H_
#define _SIP_TRANSPORT_HELPER_H_

// TCP active connection MUST be created at the start time of raw SIP message transmission.
#include "IMSMap.h"
#include "EngineActivity.h"
#include "ISipTransportHelper.h"
#include "ISipDatagramSocketListener.h"
#include "ISipStreamSocketListener.h"
#include "SipTransportAddress.h"
#include "SipSocketAddress.h"

class ISipSocketListener;
class ISipTransportListener;
class ISipLocalDnsQueryListener;

class SipTransportHelper :
        public EngineActivity,
        public ISipTransportHelper,
        public ISipStreamSocketListener,
        public ISipDatagramSocketListener
{
public:
    SipTransportHelper();
    virtual ~SipTransportHelper();

private:
    SipTransportHelper(IN CONST SipTransportHelper& objRHS);
    SipTransportHelper& operator=(IN CONST SipTransportHelper& objRHS);

public:
    // EngineActivity class
    virtual IMS_BOOL DispatchMessage(IN IMSMSG& objMSG);

    // Only SipConnectionNotifier
    void Clear();
    SipSocket* Create(IN CONST SipSocketAddress& objSA);
    SipSocket* CreateStreamSocket(
            IN CONST SipSocketAddress& objSA, IN CONST SipSocketAddress& objSA_FarEnd);
    void Destroy(IN SipSocket*& pSocket, IN ISipSocketListener* piListener);
    void DestroyStreamSocket(
            IN CONST SipSocketAddress& objSA, IN CONST SipSocketAddress& objSA_FarEnd);
    SipSocket* Open(IN CONST SipSocketAddress& objSA);
    SipSocket* OpenStreamSocket(
            IN CONST SipSocketAddress& objSA, IN CONST SipSocketAddress& objSA_FarEnd);
    void SetListener(IN ISipTransportListener* piListener);

    // MULTI_REG_TRANSPORT
    IMS_SINT32 AttachClientInitiatedConnection(IN SipSocket* pSocket);
    void DetachClientInitiatedConnection(IN SipSocket* pSocket);
    IMS_BOOL IsClientInitiatedConnection(IN SipSocket* pSocket) const;

    // LOCAL_DNS_QUERY
    IMS_BOOL GetHostByName(IN CONST IPAddress& objLocalIP, IN CONST AString& strHostname,
            OUT IPAddress& objHostIP);

private:
    // ISipTransportHelper
    virtual void ApplyIpSecForServerSockets();
    virtual void DestroyAllSockets(
            IN IMS_SINT32 nMethod = 0, IN CONST IPAddress& objLocalIP = IPAddress::NONE);
    virtual void DestroyTcpSocket(IN CONST IPAddress& objSrcIP, IN IMS_UINT32 nSrcPort,
            IN CONST IPAddress& objDestIP, IN IMS_UINT32 nDestPort,
            IN IMS_BOOL bIsConnectionByPeer = IMS_FALSE);
    virtual void SetIpQos(IN SipRtConfig::IpQos* pIPQoS);
    virtual void SetKeepAlivePolicy(IN CONST IPAddress& objSrcIP, IN IMS_UINT32 nSrcPort,
            IN CONST IPAddress& objDestIP, IN IMS_UINT32 nDestPort,
            IN IMS_SINT32 nPolicy = (-1) /* default */);
    // LOCAL_DNS_QUERY
    virtual void SetLocalDnsQueryListener(IN ISipLocalDnsQueryListener* piListener);

    // ISipDatagramSocketListener
    virtual void DatagramSocket_DataReceived(IN SipSocket* pSocket, IN CONST ByteArray& objBuffer,
            IN CONST IPAddress& objIPA, IN IMS_SINT32 nPort);
    // ISipStreamSocketListener
    virtual void StreamSocket_ConnectionReceived(IN SipSocket* pSocket);
    virtual void StreamSocket_DataReceived(IN SipSocket* pSocket, IN_OUT ByteArray& objBuffer);
    virtual void StreamSocket_KeepAliveExpired(IN SipSocket* pSocket);
    virtual void StreamSocket_PassiveClosed(IN SipSocket* pSocket);

    IMS_BOOL AttachSocket(IN SipSocket* pSocket);
    IMS_BOOL IsSocketPresent(IN SipSocket* pSocket) const;
    SipSocket* LookupSocket(IN CONST SipSocketAddress& objSA, IN IMS_BOOL bDetach = IMS_FALSE);
    SipSocket* LookupSocket(IN CONST SipSocket& objSocket, IN IMS_BOOL bDetach = IMS_FALSE);
    SipSocket* LookupStreamSocket(IN CONST SipSocketAddress& objSA);
    SipSocket* LookupStreamSocket(
            IN CONST SipSocketAddress& objSA, IN CONST SipSocketAddress& objSA_FarEnd);

private:
    // Event for message processing
    enum
    {
        AMSG_PROCESS_MESSAGE = AMSG_USER,
        AMSG_DESTROY_ALL_SOCKETS
    };

    // Result values for the completeness of a raw SIP message (TCP only)
    enum
    {
        MESSAGE_ERROR = (-1),
        MESSAGE_COMPLETE = 0,
        MESSAGE_INCOMPLETE
    };

    // Result values for SipComp module
    enum
    {
        SIGCOMP_NO_COMP = (-3),
        SIGCOMP_INCOMPLETE = (-2),
        SIGCOMP_ERROR = (-1),
        SIGCOMP_SUCCESS = 0
    };

    class TransportBuffer
    {
        friend class SipTransportHelper;

        ByteArray objData;
        // Destination information of a message (on the basis of sender)
        SipTransportAddress objTA_NearEnd;
        // Source information of a message (on the basis of sender)
        SipTransportAddress objTA_FarEnd;

        inline TransportBuffer() {}
        inline TransportBuffer(IN CONST TransportBuffer& objRHS) :
                objData(objRHS.objData),
                objTA_NearEnd(objRHS.objTA_NearEnd),
                objTA_FarEnd(objRHS.objTA_FarEnd)
        {
        }
        inline ~TransportBuffer() {}
        inline TransportBuffer& operator=(IN CONST TransportBuffer& objRHS)
        {
            if (this != &objRHS)
            {
                objData = objRHS.objData;
                objTA_NearEnd = objRHS.objTA_NearEnd;
                objTA_FarEnd = objRHS.objTA_FarEnd;
            }

            return (*this);
        }

        // DEBUG message
        void DisplayMessage(IN IMS_SINT32 nSlotId);
    };

    IMSList<SipSocket*> objSockets;
    IMSList<TransportBuffer*> objBuffers;
    ISipTransportListener* piListener;

    // MULTI_REG_TRANSPORT :: <Socket object's pointer, count>
    IMSMap<IMS_UINTP, IMS_SINT32> objClientInitiatedConnections;

    // LOCAL_DNS_QUERY
    // 1) Test purpose
    // 2) Using the application layer's DNS query result
    ISipLocalDnsQueryListener* piDnsQueryListener;
};

#endif  // _SIP_TRANSPORT_HELPER_H_
