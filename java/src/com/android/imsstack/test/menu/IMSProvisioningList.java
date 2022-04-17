package com.android.imsstack.test.menu;

import android.content.Context;

import com.android.imsstack.R;
import com.android.imsstack.core.ImsGlobal;

import java.util.ArrayList;

public class IMSProvisioningList {

    Context mContext;
    int mSlotID = 0;
    SettingFeature mSubscriberFeature;
    SettingFeature mMMTELServerInfoFeature;
    SettingFeature mMMTELGBAInfoFeature;
    SettingFeature mFakeSubscriberFeature;
    SettingFeature mAoSApplicationFeature;
    SettingFeature mAoSConditionFeature;
    SettingFeature mAoSConnectionFeature;
    SettingFeature mAoSHandleFeature;
    SettingFeature mAoSProviderFeature;
    SettingFeature mAoSRegFeature;
    SettingFeature mEngineFeature;
    SettingFeature mSIPFeature;
    SettingFeature mComSIPFeature;
    SettingFeature mSMSFeature;
    SettingFeature mSMSSIPFeature;
    SettingFeature mIMSCommonEngineFeature;
    SettingFeature mUCAppFeature;
    SettingFeature mUCServiceFeature;
    SettingFeature mUCSessionFeature;
    SettingFeature mUCSessionCommonFeature;
    SettingFeature mUCEmergencyFeature;
    SettingFeature mUCConferenceFeature;
    SettingFeature mComEmergencyFeature;
    SettingFeature mMMTELServerInfoCFFeature;
    SettingFeature mMMTELServerInfoCBFeature;

    public IMSProvisioningList(Context context, int slotID) {
        mContext = context;
        mSlotID = slotID;
        //
        // Following the order of Hidden menu items... (from IMSProvisioning xml structure)
        //

        mSubscriberFeature = new SubscriberFeature();
        mFakeSubscriberFeature = new FakeSubscriberFeature();
        mAoSApplicationFeature = new AoSApplicationFeature();
        mAoSConditionFeature = new AoSConditionFeature();
        mAoSConnectionFeature = new AoSConnectionFeature();
        mAoSHandleFeature = new AoSHandleFeature();
        mAoSProviderFeature = new AoSProviderFeature();
        mAoSRegFeature = new AoSRegFeature();
        mEngineFeature = new EngineFeature();
        mSIPFeature = new SIPFeature();
        mComSIPFeature = new ComSIPFeature();
        mSMSFeature = new SMSFeature();
        mSMSSIPFeature = new SMSSIPFeature();
        mIMSCommonEngineFeature = new IMSCommonEngineFeature();
        mUCAppFeature = new UCAppFeature();
        mUCServiceFeature = new UCServiceFeature();
        mUCSessionFeature = new UCSessionFeature();
        mUCSessionCommonFeature = new UCSessionCommonFeature();
        mUCEmergencyFeature = new UCEmergencyFeature();
        mUCConferenceFeature = new UCConferenceFeature();
        mMMTELServerInfoFeature = new MMTELServerInfoFeature();
        mMMTELGBAInfoFeature = new MMTELGBAInfoFeature();
        mComEmergencyFeature = new ComEmergencyFeature();
        mMMTELServerInfoCFFeature = new MMTELServerInfoCFFeature();
        mMMTELServerInfoCBFeature = new MMTELServerInfoCBFeature();
    }

    public class MMTELServerInfoFeature extends SettingFeature {
        private MMTELServerInfoFeature() {
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_provisioning));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_check_reg));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_tls));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_namespace));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_softindia_wifi_test));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_response_from_ruleid));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_ussd_over_ims));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_auto_cs_retry));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_srv_record));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_support_erasure));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_url_queryPrefix));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_query_on_bootup));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_trustAllHosts));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_urlAddr_include_urlPort));

            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_server_ip));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_server_port));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_server_password));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_support_services));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_service_version));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_access_policy));

            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_pdn));
            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_address));
            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_target));
            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_auth_username));
            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_xui_priority));
        }
    }

    public class MMTELServerInfoCFFeature extends SettingFeature {
        private MMTELServerInfoCFFeature() {
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_support_cf_rules));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_rule_id_unconditional));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_rule_id_busy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_rule_id_no_reply));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_rule_id_not_reachable));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_rule_id_not_logged_in));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_target_number_prefix));

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_timer_in_no_answer));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_media_audio));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_media_video));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cf_get_request_each_rule));
        }
    }

    public class MMTELServerInfoCBFeature extends SettingFeature {
        private MMTELServerInfoCBFeature() {
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_support_cb_rules));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_rule_id_aic));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_rule_id_aoc));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_rule_id_oic));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_rule_id_oic_exhc));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_rule_id_icr));

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_media_audio));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_cb_media_video));
        }
    }

    public class MMTELGBAInfoFeature extends SettingFeature
    {

        private MMTELGBAInfoFeature() {
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_enabled));

            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_type));
            listPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_bsf_selection));

            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_request_uri));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_host));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_username));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_realm));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_bsf_dns_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_bsf_port));
            editPreferenceList.add(mContext.getString(R.string.ims_key_mmtel_gba_schema));
        }
    }

    public class UCAppFeature extends SettingFeature
    {

        private UCAppFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_nCNAPType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_nLocalNumberType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nRoutingRejectCode));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nSuspendTime));
        }
    }

    public class UCServiceFeature extends SettingFeature
    {

        private UCServiceFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_tServiceID));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_tContactH));
            listPreferenceList.add(mContext.getString(R.string.ims_key_uc_eType));
        }
    }

    public class UCSessionCommonFeature extends SettingFeature
    {

        private UCSessionCommonFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bSRVCCCheckFeatureCaps));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bPR_Answer));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bUse180RPR));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bIgnoreOfferAfterNego));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bKeepAlive));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bPreconditionSupportWithoutQoS));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bQosNotiDisableAfterEnable));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bUseFastStopTimerWaitQoS));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConvertWithEarlyUpdate));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bSSAC));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bDialogEventPackage));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bTIP));

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nSRVCCSupportedType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nOfferResCode));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nInitialCapaMode));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MO_1XX_WAIT));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MO_NOANSWER));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MT_ALERTING));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MT_PRACKWAIT));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MO_UPDATE));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_MT_UPDATE));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_SRVCC_TERMINATED));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_GLARECONDITION));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_WAIT_NETWORK_RBT));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_WAIT_NETWORK_HOLD_TONE));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_QOS_WAIT));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_QOS_FORCE_FAKE));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_QOS_INACTIVE_GUARD));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_QOS_NOTI_PARTIAL));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_UDP_KEEP_ALIVE));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_SESS_WAIT_TERMINATED));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_aStrExLang));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_aStrCustSessionConfig));

            listPreferenceList.add(mContext.getString(R.string.ims_key_uc_nCW));
            listPreferenceList.add(mContext.getString(R.string.ims_key_uc_eDTMFType));
        }
    }

    public class UCSessionFeature extends SettingFeature
    {

        private UCSessionFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bIs));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bECT));

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nBlock_LowBattery));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMaxSess));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_tAcceptContactH));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_tConfURI));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nQoSInitOfferDir));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdAProtocol));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdAType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdATime));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdHProtocol));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdHType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMediaThresholdHTime));
        }
    }

    public class UCEmergencyFeature extends SettingFeature
    {

        private UCEmergencyFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_bUsingEPDN_LTE));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_bUsingEPDN_WIFI));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bECM));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bESNUEDIgnore));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bESNUEDIgnorePort));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bESCheckSOS));

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_LTE_OPEN));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_LTE_START));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_LTE_RINGBACK));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_WIFI_OPEN));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_WIFI_START));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_E911_WIFI_RINGBACK));
        }
    }

    public class UCConferenceFeature extends SettingFeature
    {

        private UCConferenceFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bReferSub));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConfSub));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConfSubInDialog));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bReferredBy));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConfSubSupportAlone));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConfSupportLocal));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bConfKeepDisconnected));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_uc_bCheckConfDocVers));

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nUETIMER_CONF_SUB_WAIT_NOTIFY));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfType_start));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfType_expand));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfType_merge));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nReferType_start));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nReferType_expand));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nReferType_merge));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nReferType_join));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nReferType_drop));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nMultipleReferSendType));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfSubWaitTerminated));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfSubExpireTime));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_nConfSubWaitTerminated));

            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_timer_wait_initiation));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_timer_wait_notify_active));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_timer_wait_notify_terminated));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_timer_wait_sipfrag));
            editPreferenceList.add(mContext.getString(R.string.ims_key_uc_refer_type));
        }
    }

    public class IMSCommonEngineFeature extends SettingFeature
    {

        private IMSCommonEngineFeature() {

            listPreferenceList.add(mContext.getString(R.string.log_key_enabled));
        }
    }

    public class SMSSIPFeature extends SettingFeature
    {

        private SMSSIPFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_service_version));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_t1));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_t2));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tf));

            listPreferenceList = new ArrayList<String>();
            listPreferenceList.add(mContext.getString(R.string.ims_key_preferred_id));
            listPreferenceList.add(mContext.getString(R.string.ims_key_target_number_format));
            listPreferenceList.add(mContext.getString(R.string.ims_key_target_scheme));
        }
    }

    public class SMSFeature extends SettingFeature
    {

        private SMSFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_sms_psi));

            listPreferenceList.add(mContext.getString(R.string.ims_key_sms_format));
            listPreferenceList.add(mContext.getString(R.string.ims_key_sms_over_ip_network));
        }
    }

    public class ComSIPFeature extends SettingFeature
    {

        private ComSIPFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_service_version));
            editPreferenceList.add(mContext.getString(R.string.ims_key_session_st_headers));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_t1));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_t2));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_t4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_ta));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tb));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tc));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_td));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_te));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tg));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_th));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_ti));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tj));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_sip_timer_tk));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_session_st_minse));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_session_st_session_expires));

            listPreferenceList.add(mContext.getString(R.string.ims_key_preferred_id));
            listPreferenceList.add(mContext.getString(R.string.ims_key_target_number_format));
            listPreferenceList.add(mContext.getString(R.string.ims_key_target_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_session_st_refresher));
            listPreferenceList.add(mContext.getString(R.string.ims_key_session_st_method));
        }
    }

    public class SIPFeature extends SettingFeature
    {

        private SIPFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_sip_compact_form));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_registration_subscription));

            editPreferenceList.add(mContext.getString(R.string.ims_key_sip_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_sip_tcp_criterion_len));
            editPreferenceList.add(mContext.getString(R.string.ims_key_sip_tcp_connection_timer));
            editPreferenceList.add(mContext.getString(R.string.ims_key_sip_tcp_keepalive_timer));
            editPreferenceList.add(mContext.getString(R.string.ims_key_sip_tcp_wouldblock_timer));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_registration_duration));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_reg_sub_expiration));

            listPreferenceList.add(mContext.getString(R.string.ims_key_sip_device_id));
        }
    }

    public class EngineFeature extends SettingFeature
    {

        private EngineFeature() {
            // trace option
            listPreferenceList.add(mContext.getString(R.string.ims_key_log_selection));
        }
    }

    public class AoSRegFeature extends SettingFeature
    {

        private AoSRegFeature() {

            // REG_GBA
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_gba));

            // REG_0
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ipsec_enable));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_refresh_3gpp_standard));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ipsec_spi_3gpp));
            if ((ImsGlobal.isOperatorCountry(mSlotID, "TMO", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "MPCS", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "TRF", "US"))) {
                checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ims_indicator_onoff));
            }

            // REG_1
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ipsec_enable));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_refresh_3gpp_standard));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ipsec_spi_3gpp));
            if ((ImsGlobal.isOperatorCountry(mSlotID, "TMO", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "MPCS", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "TRF", "US"))) {
                checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ims_indicator_onoff));
            }

            // REG_2
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ipsec_enable));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_refresh_3gpp_standard));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ipsec_spi_3gpp));
            if ((ImsGlobal.isOperatorCountry(mSlotID, "TMO", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "MPCS", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "TRF", "US"))) {
                checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ims_indicator_onoff));
            }

            // REG_3
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ipsec_enable));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_refresh_3gpp_standard));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ipsec_spi_3gpp));
            if ((ImsGlobal.isOperatorCountry(mSlotID, "TMO", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "MPCS", "US"))
                    || (ImsGlobal.isOperatorCountry(mSlotID, "TRF", "US"))) {
                checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ims_indicator_onoff));
            }

            // REG_0
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_type));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_flow_id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_repeat_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ipsec_algs));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_response_codes));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_block_conditions));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_userinfo_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_tcp_keepalive_option));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_base_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_max_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_normal_reg_flow_id));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_authentication_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_reg_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_sub_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ipsec_guard_life_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_ipsec_port_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_retry_pcscf_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_tcp_keepalive_value));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_tcp_criterion_length_ipv4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_tcp_criterion_length_ipv6));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_dscp_value));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_0_reginfo_contact_match_rule));

            // REG_1
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_type));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_flow_id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_repeat_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ipsec_algs));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_response_codes));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_block_conditions));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_userinfo_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_tcp_keepalive_option));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_base_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_max_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_normal_reg_flow_id));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_authentication_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_reg_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_sub_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ipsec_guard_life_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_ipsec_port_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_retry_pcscf_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_tcp_keepalive_value));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_tcp_criterion_length_ipv4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_tcp_criterion_length_ipv6));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_dscp_value));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_1_reginfo_contact_match_rule));

            // REG_2
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_type));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_flow_id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_repeat_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ipsec_algs));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_response_codes));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_block_conditions));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_userinfo_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_tcp_keepalive_option));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_base_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_max_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_normal_reg_flow_id));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_authentication_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_reg_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_sub_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ipsec_guard_life_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_ipsec_port_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_retry_pcscf_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_tcp_keepalive_value));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_tcp_criterion_length_ipv4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_tcp_criterion_length_ipv6));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_dscp_value));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_2_reginfo_contact_match_rule));

            // REG_3
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_type));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_flow_id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_repeat_interval));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ipsec_algs));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_response_codes));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_block_conditions));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_userinfo_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_tcp_keepalive_option));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_base_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_max_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_normal_reg_flow_id));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_authentication_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_reg_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_sub_default_retry_after_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ipsec_guard_life_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_ipsec_port_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_retry_pcscf_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_tcp_keepalive_value));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_tcp_criterion_length_ipv4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_tcp_criterion_length_ipv6));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_dscp_value));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_reg_3_reginfo_contact_match_rule));
        }
    }

    public class AoSProviderFeature extends SettingFeature
    {

        private AoSProviderFeature() {
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_provider_common_features));
        }
    }

    public class AoSHandleFeature extends SettingFeature
    {

        private AoSHandleFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_0_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_1_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_2_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_3_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_4_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_5_Id));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_0_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_1_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_2_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_3_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_4_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_handle_5_features));
        }
    }

    public class AoSConnectionFeature extends SettingFeature
    {

        private AoSConnectionFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_mpdn));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_ps_supported));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_imsapn_onoff));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_profile_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_profile_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_profile_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_profile_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_profile_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_profile_name));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_access_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_access_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_access_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_access_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_access_policy));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_access_policy));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_features));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_service_in_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_service_in_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_service_in_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_service_in_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_service_in_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_service_in_time));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_service_out_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_service_out_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_service_out_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_service_out_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_service_out_time));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_service_out_time));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_ipv6_delay_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_ipv6_delay_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_ipv6_delay_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_ipv6_delay_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_ipv6_delay_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_ipv6_delay_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_rat_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_rat_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_rat_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_rat_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_rat_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_rat_guard_interval));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_ip_version));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_ip_version));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_ip_version));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_ip_version));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_ip_version));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_ip_version));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_0_epdg_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_1_epdg_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_2_epdg_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_3_epdg_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_4_epdg_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_connection_5_epdg_scheme));
        }
    }


    public class AoSConditionFeature extends SettingFeature
    {

        private AoSConditionFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_Id));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_features));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_service_council_block_reasons));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_service_council_block_reasons));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_service_council_block_reasons));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_service_council_block_reasons));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_service_council_block_reasons));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_service_council_block_reasons));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_uniqueness));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_isim_index_for_impu));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_isim_index_for_impu));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_isim_index_for_impu));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_isim_index_for_impu));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_isim_index_for_impu));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_isim_index_for_impu));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_isim_index_for_pcscf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_isim_index_for_pcscf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_isim_index_for_pcscf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_isim_index_for_pcscf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_isim_index_for_pcscf));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_isim_index_for_pcscf));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_pcscf_port));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_pcscf_port));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_pcscf_port));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_pcscf_port));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_pcscf_port));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_pcscf_port));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_multiple_discoverty_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_multiple_discoverty_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_multiple_discoverty_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_multiple_discoverty_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_multiple_discoverty_scheme));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_multiple_discoverty_scheme));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_pcscf_changed_control));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_pcscf_changed_control));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_pcscf_changed_control));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_pcscf_changed_control));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_pcscf_changed_control));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_pcscf_changed_control));

            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_0_network_suspend_condition));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_1_network_suspend_condition));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_2_network_suspend_condition));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_3_network_suspend_condition));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_4_network_suspend_condition));
            listPreferenceList.add(mContext.getString(R.string.ims_key_aos_condition_5_network_suspend_condition));
        }
    }

    public class AoSApplicationFeature extends SettingFeature
    {

        private AoSApplicationFeature() {
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_Id));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_Id));

            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_features));
            editPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_features));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_uniqueness));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_state_start_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_state_start_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_state_start_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_state_start_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_state_start_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_state_start_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_connected_state_checking_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_connected_state_checking_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_connected_state_checking_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_connected_state_checking_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_connected_state_checking_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_connected_state_checking_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_reconfig_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_reconfig_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_reconfig_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_reconfig_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_reconfig_guard_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_reconfig_guard_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_reg_stop_waiting_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_reg_stop_waiting_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_reg_stop_waiting_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_reg_stop_waiting_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_reg_stop_waiting_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_reg_stop_waiting_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_reg_terminated_trying_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_reg_terminated_trying_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_reg_terminated_trying_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_reg_terminated_trying_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_reg_terminated_trying_interval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_reg_terminated_trying_interval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_unexpected_error_reg_trying_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_unexpected_error_reg_trying_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_unexpected_error_reg_trying_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_unexpected_error_reg_trying_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_unexpected_error_reg_trying_max_count));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_unexpected_error_reg_trying_max_count));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_unexpected_error_reg_trying_max_internval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_unexpected_error_reg_trying_max_internval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_unexpected_error_reg_trying_max_internval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_unexpected_error_reg_trying_max_internval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_unexpected_error_reg_trying_max_internval));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_unexpected_error_reg_trying_max_internval));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_0_pdn_release_delay_on_termination));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_1_pdn_release_delay_on_termination));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_2_pdn_release_delay_on_termination));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_3_pdn_release_delay_on_termination));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_4_pdn_release_delay_on_termination));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_aos_application_5_pdn_release_delay_on_termination));
        }
    }

    public class FakeSubscriberFeature extends SettingFeature
    {

        private FakeSubscriberFeature() {

            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_0));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_1));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_2));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_3));
            editPreferenceList.add(mContext.getString(R.string.ims_key_admin_pcscf));
            editPreferenceList.add(mContext.getString(R.string.ims_key_home_domain_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impi));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_0));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_1));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_2));
            editPreferenceList.add(mContext.getString(R.string.ims_key_phone_context));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_0));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_1));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_2));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_3));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_impu_primary_ref_index));
        }
    }

    public class SubscriberFeature extends SettingFeature
    {

        private SubscriberFeature() {

            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_ims));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_isim));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_usim));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_debug));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_testmode));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_admin_testmode_gcf));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_impu_validation));
            checkboxPreferenceList.add(mContext.getString(R.string.ims_key_auth_realm_leniency));

            editPreferenceList.add(mContext.getString(R.string.ims_key_home_domain_name));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impi));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_0));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_1));
            editPreferenceList.add(mContext.getString(R.string.ims_key_impu_2));
            editPreferenceList.add(mContext.getString(R.string.ims_key_phone_context));
            editPreferenceList.add(mContext.getString(R.string.ims_key_auth_username));
            editPreferenceList.add(mContext.getString(R.string.ims_key_auth_realm));
            editPreferenceList.add(mContext.getString(R.string.ims_key_auth_password));
            editPreferenceList.add(mContext.getString(R.string.ims_key_auth_algorithm));
            editPreferenceList.add(mContext.getString(R.string.ims_key_scscf));
            editPreferenceList.add(mContext.getString(R.string.ims_key_admin_services));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_0));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_1));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_2));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_3));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_4));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_5));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_6));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_7));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_8));
            editPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_address_9));

            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_0));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_1));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_2));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_3));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_4));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_5));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_6));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_7));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_8));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_pcscf_port_9));
            neditPreferenceList.add(mContext.getString(R.string.ims_key_impu_primary_ref_index));

            listPreferenceList.add(mContext.getString(R.string.ims_key_admin_pcscf));
        }
    }

    public class SettingFeature
    {
        ArrayList<String> checkboxPreferenceList = new ArrayList<String>();
        ArrayList<String> editPreferenceList = new ArrayList<String>();
        ArrayList<String> neditPreferenceList = new ArrayList<String>();
        ArrayList<String> listPreferenceList = new ArrayList<String>();
    }

    public class ComEmergencyFeature extends SettingFeature
    {
        private ComEmergencyFeature() {
            checkboxPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_support_lte));
            checkboxPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_support_wifi));
            checkboxPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_control_by_volte_setting));
            checkboxPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_control_by_ims_reg));
            checkboxPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_require_normal_call_end));
            editPreferenceList.add(
                    mContext.getString(R.string.ims_key_emergency_wifi_ecall_block_features));
        }
    }
}
