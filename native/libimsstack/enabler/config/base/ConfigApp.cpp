/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170717  hwangoo.park@             Changed from AppConfigurableParameterManager
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "ServiceTrace.h"
#include "ServiceEvent.h"
#include "IConfigurable.h"
#include "Configuration.h"
#include "ImsConfig.h"
#include "base/ConfigApp.h"

__IMS_TRACE_TAG_USER_DECL__("ConfigApp");

PUBLIC
ConfigApp::ConfigApp(IN const AString &strAppName)
    : IMSApp(strAppName)
{
}

PUBLIC VIRTUAL
ConfigApp::~ConfigApp()
{
    IMS_EVENT_RemoveListenerForSlotId(IMS_EVENT_CONFIG_UPDATE, this, GetSlotId());
}

PUBLIC
void ConfigApp::Start()
{
    PostMessage(AMSG_START, 0, 0);
}

PROTECTED VIRTUAL
IMS_BOOL ConfigApp::OnPreprocess(IN IMSMSG & /* objMSG */)
{
    return IMS_FALSE;
}

PROTECTED VIRTUAL
IMS_BOOL ConfigApp::OnMessage(IN IMSMSG &objMSG)
{
    switch (objMSG.GetName())
    {
    case AMSG_START:
        IMS_EVENT_AddListenerForSlotId(IMS_EVENT_CONFIG_UPDATE, this, GetSlotId());
        break;

    default:
        return IMS_FALSE;
    }

    return IMS_TRUE;
}

PROTECTED VIRTUAL
IMS_BOOL ConfigApp::OnPostprocess(IN IMSMSG & /* objMSG */)
{
    return IMS_FALSE;
}

PROTECTED VIRTUAL
void ConfigApp::Event_NotifyEvent(IN IMS_SINT32 /*nEvent*/,
        IN IMS_UINT32 /*nWParam*/, IN IMS_UINT32 /*nLParam*/)
{
    // Subclass MAY implement this method to handle the external events.
}

PROTECTED VIRTUAL
void ConfigApp::UpdateAllForHidden(IN IMS_SINT32 nItem, IN IMS_SINT32 nParam)
{
    (void) nParam;

    IMS_BOOL bSubscriberChanged = ((nItem & ImsConfig::FLAG_IMS_SUBSCRIBER) != 0);
    IMS_BOOL bSipConfigChanged = ((nItem & ImsConfig::FLAG_IMS_SIP) != 0);
    IMS_BOOL bSipConfigVChanged = ((nItem & ImsConfig::FLAG_IMS_COM_SIP) != 0);

    IMS_TRACE_I("UpdateAllForHidden :: subscriber=%s, sip=%s, sip-v=%s",
            _TRACE_B_(bSubscriberChanged), _TRACE_B_(bSipConfigChanged),
            _TRACE_B_(bSipConfigVChanged));

    Configuration *pConfiguration = Configuration::GetInstance();

    // SUBSCRIBER
    if (bSubscriberChanged)
    {
        const ISubscriberConfig *piSubConfig = pConfiguration->GetSubscriberConfig(GetSlotId());

        if (piSubConfig != IMS_NULL)
        {
            IConfigurable* piConfigurable = piSubConfig->GetConfigurable();

            if (piConfigurable != IMS_NULL)
            {
                piConfigurable->Update(IConfigurable::CP_I_SUBSCRIBER_ALL);
            }
        }
    }

    // SIP configuration
    if (bSipConfigChanged)
    {
        const ISipConfig *piSipConfig = pConfiguration->GetSipConfig(GetSlotId());

        if (piSipConfig != IMS_NULL)
        {
            IConfigurable* piConfigurable = piSipConfig->GetConfigurable();

            if (piConfigurable != IMS_NULL)
            {
                piConfigurable->Update(IConfigurable::CP_I_SIP_FEATURES);
                piConfigurable->Update(IConfigurable::CP_I_TCP_CRITERION_LENGTH);
                piConfigurable->Update(IConfigurable::CP_I_REG_EXPIRES);
                piConfigurable->Update(IConfigurable::CP_I_REG_SUB);
                piConfigurable->Update(IConfigurable::CP_I_REG_SUB_EXPIRES);
            }
        }
    }

    // SIP configuration V
    if (bSipConfigVChanged)
    {
        const ISipConfig *piSipConfig = pConfiguration->GetSipConfig(GetSlotId());

        if (piSipConfig != IMS_NULL)
        {
            const ISipConfigV *piSipConfigV = piSipConfig->GetSipConfigV();

            if (piSipConfigV != IMS_NULL)
            {
                IConfigurable* piConfigurable = piSipConfigV->GetConfigurable();

                if (piConfigurable != IMS_NULL)
                {
                    piConfigurable->Update(IConfigurable::CP_I_SIP_ALL);
                }
            }
        }
    }
}

PROTECTED VIRTUAL
void ConfigApp::UpdateAllForDM(IN IMS_SINT32 nItem, IN IMS_SINT32 nParam)
{
    (void) nParam;

    IMS_BOOL bSipConfigChanged = ((nItem & ImsConfig::FLAG_IMS_SIP) != 0);
    IMS_BOOL bSipConfigVChanged = ((nItem & ImsConfig::FLAG_IMS_COM_SIP) != 0);

    IMS_TRACE_I("UpdateAllForDM :: sip=%s, sip-v=%s",
            _TRACE_B_(bSipConfigChanged), _TRACE_B_(bSipConfigVChanged), 0);

    Configuration *pConfiguration = Configuration::GetInstance();

    // SIP configuration
    if (bSipConfigChanged)
    {
        const ISipConfig *piSipConfig = pConfiguration->GetSipConfig(GetSlotId());

        if (piSipConfig != IMS_NULL)
        {
            IConfigurable* piConfigurable = piSipConfig->GetConfigurable();

            if (piConfigurable != IMS_NULL)
            {
                piConfigurable->Update(IConfigurable::CP_I_SIP_FEATURES);
                piConfigurable->Update(IConfigurable::CP_I_TCP_CRITERION_LENGTH);
                piConfigurable->Update(IConfigurable::CP_I_REG_EXPIRES);
                piConfigurable->Update(IConfigurable::CP_I_REG_SUB);
                piConfigurable->Update(IConfigurable::CP_I_REG_SUB_EXPIRES);
            }
        }
    }

    // SIP configuration V
    if (bSipConfigVChanged)
    {
        const ISipConfig *piSipConfig = pConfiguration->GetSipConfig(GetSlotId());

        if (piSipConfig != IMS_NULL)
        {
            const ISipConfigV *piSipConfigV = piSipConfig->GetSipConfigV();

            if (piSipConfigV != IMS_NULL)
            {
                IConfigurable* piConfigurable = piSipConfigV->GetConfigurable();

                if (piConfigurable != IMS_NULL)
                {
                    piConfigurable->Update(IConfigurable::CP_I_SIP_ALL);
                }
            }
        }
    }
}

PROTECTED VIRTUAL
void ConfigApp::UpdateItemForPST(IN IMS_UINT32 nItem, IN IMS_UINT32 nValue)
{
    IMS_TRACE_I("UpdateItemForPST :: item=%d, value=%d", nItem, nValue, 0);

    switch (nItem)
    {
        case ImsConfig::PST_I_PCSCF_ADDRESS:
            UpdateSubscriberConfig(IConfigurable::CP_I_PCSCF_ADDRESS_0);
            break;

        case ImsConfig::PST_I_PCSCF_PORT:
            UpdateSubscriberConfig(IConfigurable::CP_I_PCSCF_PORT_0);
            break;

        case ImsConfig::PST_I_TV_T1:
            UpdateSipConifgV(IConfigurable::CP_I_TV_T1);
            break;

        case ImsConfig::PST_I_TV_T2:
            UpdateSipConifgV(IConfigurable::CP_I_TV_T2);
            break;

        case ImsConfig::PST_I_TV_TF:
            UpdateSipConifgV(IConfigurable::CP_I_TV_TF);
            break;

        case ImsConfig::PST_I_SESSION_TIMER:
            UpdateSipConifgV(IConfigurable::CP_I_SESSION_EXPIRES);
            break;

        case ImsConfig::PST_I_MIN_SE:
            UpdateSipConifgV(IConfigurable::CP_I_SESSION_MINSE);
            break;

        default:
            break;
    }
}

PROTECTED VIRTUAL
void ConfigApp::UpdateItemForSDM(IN IMS_UINT32 nItem, IN IMS_UINT32 nValue)
{
    IMS_TRACE_I("UpdateItemForSDM :: item=%d, value=%d", nItem, nValue, 0);

    switch (nItem)
    {
        case ImsConfig::SDM_I_HOME_DOMAIN_NAME:
            UpdateSubscriberConfig(IConfigurable::CP_I_HOME_DOMAIN_NAME);
            break;

        case ImsConfig::SDM_I_TV_T1:
            UpdateSipConifgV(IConfigurable::CP_I_TV_T1);
            break;

        case ImsConfig::SDM_I_TV_T2:
            UpdateSipConifgV(IConfigurable::CP_I_TV_T2);
            break;

        case ImsConfig::SDM_I_TV_TF:
            UpdateSipConifgV(IConfigurable::CP_I_TV_TF);
            break;

        case ImsConfig::SDM_I_SIP_SESSION_TIMER:
            UpdateSipConifgV(IConfigurable::CP_I_SESSION_EXPIRES);
            break;

        case ImsConfig::SDM_I_MIN_SE:
            UpdateSipConifgV(IConfigurable::CP_I_SESSION_MINSE);
            break;

        default:
            break;
    }
}

PROTECTED
IMS_BOOL ConfigApp::UpdateSipConifgV(IN IMS_SINT32 nCPI,
        IN const AString& /*strServiceId = AString::ConstNull()*/)
{
    const ISipConfig *piSipConfig = Configuration::GetInstance()->GetSipConfig(GetSlotId());

    if (piSipConfig != IMS_NULL)
    {
        const ISipConfigV *piSipConfigV = piSipConfig->GetSipConfigV();

        if (piSipConfigV != IMS_NULL)
        {
            IConfigurable *piConfigurable = piSipConfigV->GetConfigurable();

            if (piConfigurable != IMS_NULL)
            {
                if (!piConfigurable->Update(nCPI))
                {
                    IMS_TRACE_E(0, "UpdateSipConifgV :: [%d] failed", nCPI, 0, 0);
                    return IMS_FALSE;
                }

                return IMS_TRUE;
            }
        }
    }

    return IMS_FALSE;
}

PROTECTED
IMS_BOOL ConfigApp::UpdateSubscriberConfig(IN IMS_SINT32 nCPI)
{
    const ISubscriberConfig *piSubsConfig = Configuration::GetInstance(
            )->GetSubscriberConfig(GetSlotId());

    if (piSubsConfig != IMS_NULL)
    {
        IConfigurable *piConfigurable = piSubsConfig->GetConfigurable();

        if (piConfigurable != IMS_NULL)
        {
            if (!piConfigurable->Update(nCPI))
            {
                IMS_TRACE_E(0, "UpdateSubscriberConfig :: [%d] failed", nCPI, 0, 0);
                return IMS_FALSE;
            }

            return IMS_TRUE;
        }
    }

    return IMS_FALSE;
}
