#ifndef MTC_APP_H_
#define MTC_APP_H_

#include "IMSApp.h"

#include "MtcImsEventReceiver.h"
#include "helper/CallStateProxy.h"
#include "IMtcApp.h"
#include "call/IMtcCallManager.h"
#include "IMtcContext.h"
#include "IMtcService.h"
#include "call/MtcCallController.h"
#include "call/MtcCallManager.h"
#include "configuration/MtcConfigurationProxy.h"
#include "dialingplan/MtcDialingPlan.h"
#include "vonr/MtcVonrManager.h"
#include "helper/sipinterfaceholder/MtcSipInterfaceFactory.h"


class MtcService;

class MtcApp :
        public IMSApp,
        public IMtcApp,
        public IMtcContext
{
public:
    MtcApp(IN IMS_SINT32 nSlotId);
    virtual ~MtcApp();
    MtcApp(IN const MtcApp&) = delete;
    MtcApp& operator=(IN const MtcApp&) = delete;

    // IMtcApp implementation
    virtual void Start() override;
    virtual void Stop() override;

    // IMtcContext implementation
    inline virtual IMS_SINT32 GetSlotId() override { return m_nSlotId; }
    virtual IMtcService* GetServiceByType(IN ServiceType eServiceType) override;
    inline virtual MtcDialingPlan& GetDialingPlan() override { return m_objDialingPlan; }
    inline virtual MtcCallController& GetCallController() override { return m_objCallController; }
    inline virtual IMtcCallManager& GetCallManager() override { return m_objCallManager; }
    inline virtual MtcVonrManager& GetVonrManager() override { return m_objVonrManager; }
    inline virtual MtcConfigurationProxy& GetConfigurationProxy() override
    { return m_objConfigurationProxy; }
    inline virtual CallStateProxy& GetCallStateProxy() override { return m_objCallStateProxy; }
    inline MtcImsEventReceiver& GetImsEventReceiver() override { return m_objImsEventReceiver; }
    virtual MtcAosConnector* GetAosConnector(IN ServiceType eServiceType) override;
    virtual MtcSipInterfaceFactory& GetSipInterfaceFactory() override
    { return m_objSipInterfaceFactory; }

private:
    void InitConfiguration();
    void CreateServices();
    void InitCallManager();
    void DestroyServices();

private:
    IMS_SINT32                  m_nSlotId;
    AString                     m_strJniServiceName;
    MtcConfigurationProxy       m_objConfigurationProxy;
    IMSList<MtcService*>        m_lstServices;
    MtcDialingPlan              m_objDialingPlan;
    MtcCallManager              m_objCallManager;
    MtcCallController           m_objCallController;
    MtcVonrManager              m_objVonrManager;
    CallStateProxy              m_objCallStateProxy;
    MtcImsEventReceiver         m_objImsEventReceiver;
    MtcSipInterfaceFactory      m_objSipInterfaceFactory;
};

#endif
