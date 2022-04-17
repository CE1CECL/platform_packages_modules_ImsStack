/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20140203  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _SIP_CONNECTION_FACTORY_H_
#define _SIP_CONNECTION_FACTORY_H_

#include "util/IDialogMethod.h"
#include "util/ICancellableMethod.h"
#include "ISIPConnectionFactory.h"
#include "EngineActivity.h"

class Service;



class SIPConnectionFactory
    : public EngineActivity
    , public IDialogMethod
    , public ICancellableMethod
    , public ISIPConnectionFactory
{
public:
    SIPConnectionFactory(IN Service *pService_);
    SIPConnectionFactory(IN Service *pService_, IN ISIPServerConnection *piSSC);
    virtual ~SIPConnectionFactory();

public:
    // EngineActivity class
    virtual IMS_BOOL DispatchMessage(IN IMSMSG &objMSG);

    // IDialogMethod class
    virtual IMS_BOOL Dialog_Compare(IN ISIPServerConnection *piSSC) const;
    virtual IMS_BOOL Dialog_NotifyRequest(IN ISIPServerConnection *piSSC);

    // ICancellableMethod class
    virtual IMS_BOOL Cancellable_Compare(IN ISIPServerConnection *piSSC_CANCEL) const;
    virtual IMS_BOOL Cancellable_NotifyRequest(IN ISIPServerConnection *piSSC_CANCEL);

    // IMethod class
    virtual void Destroy();
    // SIP_MESSAGE_MEDIATOR
    virtual void SetMessageMediator(IN IMessageMediator *piMediator);

    // ISIPConnectionFactory class
    virtual ISIPClientConnection* CreateClientConnection(IN CONST SIPMethod &objMethod,
            IN CONST SIPAddress *pFrom, IN CONST SIPAddress *pTo);
    virtual ISIPClientConnection* CreateClientConnection(IN ISIPDialog *piDialog,
            IN CONST SIPMethod &objMethod);
    virtual IMS_BOOL CreateResponse(IN_OUT ISIPServerConnection *piSSC,
            IN IMS_SINT32 nStatusCode, IN CONST AString &strPhrase = AString::ConstNull());
    virtual ISIPServerConnection* GetNewServerConnection();
    virtual void SetDialog(IN ISIPDialog *piDialog);
    virtual void SetListener(IN ISIPConnectionFactoryListener *piListener);
    virtual void SetSSCForCANCEL(IN ISIPServerConnection *piSSC);

private:
    enum
    {
        AMSG_SSC_FOR_MID_DIALOG_RECEIVED = AMSG_USER
    };

    Service *pService;
    ISIPDialog *piDialog;
    ISIPConnectionFactoryListener *piListener;
    // It is only maintained for a new incoming request
    ISIPServerConnection *piInitialSSC;
    ISIPServerConnection *piInviteSSC;
};

#endif // _SIP_CONNECTION_FACTORY_H_
