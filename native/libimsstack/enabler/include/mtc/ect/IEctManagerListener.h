/*
 * author : hyunbin.shin@
 * version : 2.0
 * date : 201503
 * brief :
 */

#ifndef INTERFACE_UCECTMNGR_LISTENER_H_
#define INTERFACE_UCECTMNGR_LISTENER_H_

class IUCECTMngrListener
{
public:
    virtual void ECTMngr_Started(IN IMS_UINTP nParam) = 0;
    virtual void ECTMngr_StartFailed(IN IMS_UINTP nParam) = 0;
    virtual void ECTMngr_SessionStartFailed(IN IMS_UINTP nParam) = 0;
    virtual void ECTMngr_SessionUpdateFailed(IN IMS_UINTP nParam) = 0;
    virtual void ECTMngr_SessionNOTIFYDeliveryFailed(IN IMS_UINTP nParam) = 0;
    virtual void ECTMngr_SessionTerminated(IN IMS_UINTP nParam) = 0;
};

#endif // INTERFACE_UCECTMNGR_LISTENER_H_
