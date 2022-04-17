/**
 * MediaSettings
 * Role
 *         Setting menu factory which is requested by operator.
 *         Send intent to target setting menu based on Operator & Country Information in java layer.
 */

package com.android.imsstack.test.menu;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;

import com.android.imsstack.R;
import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.ImsProperties;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;

public class MediaSetting extends IMSSetting_BASE {
    // Constants--------------------------------------------------

    //// Media
    protected static final String KEY_MEDIA_PREFIX = "";
    protected static final int SESSION_TYPE_AUDIO_VOLTE = 0;
    protected static final int SESSION_TYPE_AUDIO_VT = 1;
    protected static final int SESSION_TYPE_VIDEO_VT = 2;
    protected static final int SESSION_TYPE_TEXT_VOLTE = 3;
    protected static final int SESSION_TYPE_TEXT_VT = 4;

    // Variables--------------------------------------------------
    private PrefixSpecificChangeListener mOnVoIPMediaListener = null;
    private PrefixSpecificChangeListener mOnVoIPAudioListener = null;
    private PrefixSpecificChangeListener mOnVoIPVideoListener = null;
    private PrefixSpecificChangeListener mOnVoIPTextListener = null;

    private MediaAdditionalChangeListener mOnVoIPMediaAdditionalListener = null;

    //test
    private MediaCodecChangeListener mOnCodecAudioVoLTEListener = null;
    private MediaCodecChangeListener mOnCodecAudioVTListener = null;
    private MediaCodecChangeListener mOnCodecVideoVTListener = null;
    private MediaCodecChangeListener mOnCodecTextVoLTEListener = null;
    private MediaCodecChangeListener mOnCodecTextVTListener = null;

    public static final String CODEC_AUDIO_VOLTE = "gims_com_media_audio_codec_volte";
    public static final String CODEC_AUDIO_VT = "gims_com_media_audio_codec_vt";

    private SettingSlot mSettingSlot = null;
    private MediaSettingSlotListener mMediaSettingSlotListener = null;

    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    // Interface implementation methods --------------------------
    @SuppressWarnings("deprecation")
        @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int slotID = getSlotIdFromIntent();
        setSlotID(slotID);

        if (mSlotID < 0) {
            mSettingSlot = new SettingSlot(this, getListView());
            mMediaSettingSlotListener = new MediaSettingSlotListener();
            mSettingSlot.setSlotList(mMediaSettingSlotListener);
        } else {
            initialize();
        }
    }

    public void onDestroy() {

        super.onDestroy();

        if (imsDB != null) {
            imsDB.close();
            imsDB = null;
        }
    }

    protected void initialize(){

        addPreferencesFromResource(R.xml.imsmediasetting);

        mOnVoIPMediaListener = new PrefixSpecificChangeListener(
                imsDB, KEY_MEDIA_PREFIX, ProviderInterface.Media.TABLE_NAME, "Media", getSlotID());
        mOnVoIPAudioListener = new PrefixSpecificChangeListener(
                imsDB, KEY_MEDIA_PREFIX, ProviderInterface.Media.SubTable.AUDIO, "MediaAudio", getSlotID());
        mOnVoIPVideoListener = new PrefixSpecificChangeListener(
                imsDB, KEY_MEDIA_PREFIX, ProviderInterface.Media.SubTable.VIDEO, "MediaVideo", getSlotID());
        mOnVoIPTextListener = new PrefixSpecificChangeListener(
                imsDB, KEY_MEDIA_PREFIX, ProviderInterface.Media.SubTable.TEXT, "MediaText", getSlotID());

        mOnVoIPMediaAdditionalListener = new MediaAdditionalChangeListener(
                imsDB, KEY_MEDIA_PREFIX, ProviderInterface.Media.TABLE_NAME, "Media");

        AudioSettingCommand mediaAudioSettingCommandVoLTE =
                new AudioSettingCommand(ProviderInterface.Media.SubTable.CODEC_AUDIO_VOLTE,
                    "audiocodec", SESSION_TYPE_AUDIO_VOLTE);
        AudioSettingCommand mediaAudioSettingCommandVT =
                new AudioSettingCommand(ProviderInterface.Media.SubTable.CODEC_AUDIO_VT,
                    "audiocodec", SESSION_TYPE_AUDIO_VT);
        VideoSettingCommand mediaVideoSettingCommandVT =
                new VideoSettingCommand(ProviderInterface.Media.SubTable.CODEC_VIDEO_VT,
                    "videocodec", SESSION_TYPE_VIDEO_VT);
        TextSettingCommand mediaTextSettingCommandVoLTE =
                new TextSettingCommand(ProviderInterface.Media.SubTable.CODEC_TEXT_VOLTE,
                    "textcodec", SESSION_TYPE_TEXT_VOLTE);
        TextSettingCommand mediaTextSettingCommandVT =
                new TextSettingCommand(ProviderInterface.Media.SubTable.CODEC_TEXT_VT,
                    "textcodec", SESSION_TYPE_TEXT_VT);

        mOnCodecAudioVoLTEListener = new MediaCodecChangeListener(mediaAudioSettingCommandVoLTE);
        mOnCodecAudioVTListener = new MediaCodecChangeListener(mediaAudioSettingCommandVT);
        mOnCodecVideoVTListener = new MediaCodecChangeListener(mediaVideoSettingCommandVT);
        mOnCodecTextVoLTEListener = new MediaCodecChangeListener(mediaTextSettingCommandVoLTE);
        mOnCodecTextVTListener = new MediaCodecChangeListener(mediaTextSettingCommandVT);

        registerMediaChangeListener(mOnVoIPMediaListener);
        registerMediaAudioVoLTEChangeListener(mOnVoIPAudioListener);
        registerMediaAudioVTChangeListener(mOnVoIPAudioListener);
        registerMediaVideoChangeListener(mOnVoIPVideoListener);
        registerMediaTextVoLTEChangeListener(mOnVoIPTextListener);
        registerMediaTextVTChangeListener(mOnVoIPTextListener);
        registerMediaAdditionalChangeListener(mOnVoIPMediaAdditionalListener);

        registerCodecSettingChangeListener(mOnCodecAudioVoLTEListener);
        registerCodecSettingChangeListener(mOnCodecAudioVTListener);
        registerCodecSettingChangeListener(mOnCodecVideoVTListener);
        registerCodecSettingChangeListener(mOnCodecTextVoLTEListener);
        registerCodecSettingChangeListener(mOnCodecTextVTListener);
    }

     public static Cursor loadTable(SQLiteDatabase imsDB, String tableName, int slotId) {
        if (imsDB == null) {
            return null;
        }

        if (tableName == null) {
            return null;
        }

        Cursor cursor = null;

        try {
            cursor = imsDB.rawQuery("select * from " + tableName + " where id = '" + slotId + "'", null);    // SlotId : where id = 0 or 1
        } catch (SQLiteException e) {
            e.printStackTrace();

            if (cursor != null) {
                cursor.close();
                cursor = null;
            }
        }

        return cursor;
    }

    // Private/Protected methods ---------------------------------
    private void registerMediaChangeListener(PrefixSpecificChangeListener listener) {
        ImsLog.d("");

        ArrayList<String> editPreferenceList;
        ArrayList<String> checkboxPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();
        String prefix = listener.getPrefix();

        checkboxPreferenceList = new ArrayList<String>();
        checkboxPreferenceList.add(getString(R.string.ims_key_media_loopback));
        checkboxPreferenceList.add(getString(R.string.ims_key_media_debug_pcap));
        checkboxPreferenceList.add(getString(R.string.ims_key_media_session_level_bw));

        for (String preference : checkboxPreferenceList) {
            setCheckboxPreference(cursor, prefix, preference, columns, listener);
        }

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_mandatory_nego));
        editPreferenceList.add(getString(R.string.ims_key_media_rtp_analyzer));
        editPreferenceList.add(getString(R.string.ims_key_media_rtp_analyzer_option));
        editPreferenceList.add(getString(R.string.ims_key_media_debug_mmpf_ap_log));
        editPreferenceList.add(getString(R.string.ims_key_media_debug_mmpf_cp_log));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerMediaAudioVoLTEChangeListener(PrefixSpecificChangeListener listener) {
        ImsLog.d("");

        ArrayList<String> editPreferenceList;
        ArrayList<String> neditPreferenceList;
        ArrayList<String> listPreferenceList;
        ArrayList<String> checkboxPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        String prefix = listener.getPrefix();

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_tv_rtp_inactivity));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_jitter_buffer_size));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_telephone_event_duration));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_port_rtp_valueset));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_candidate_priority));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_volte_srtp_master_key_lifetime));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        neditPreferenceList = new ArrayList<String>();
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_volte_bw_as));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_volte_bw_rs));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_volte_bw_rr));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_volte_socket_tos));

        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        listPreferenceList = new ArrayList<String>();
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_rtcp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_send_rtcp_bye));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_sdp_answer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_sdp_reoffer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_sdp_keep_m_line));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_port_rtp_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_as_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_rs_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_rr_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_ptime));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_max_ptime));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_socket_pos));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_rtcpxr_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_empty_fmtp));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_srtp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_srtp_support_capa_nego));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_volte_srtp_crypto_type));

        for (String preference : listPreferenceList) {
            setListPreference(cursor, prefix, preference, columns, listener);
        }

        checkboxPreferenceList = new ArrayList<String>();
        checkboxPreferenceList.add(getString(R.string.ims_key_media_audio_volte_common_rtp_port_per_service));

        for (String preference : checkboxPreferenceList) {
            setCheckboxPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerMediaAudioVTChangeListener(PrefixSpecificChangeListener listener) {

        ArrayList<String> editPreferenceList;
        ArrayList<String> neditPreferenceList;
        ArrayList<String> listPreferenceList;
        ArrayList<String> checkboxPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        String prefix = listener.getPrefix();

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_tv_rtp_inactivity));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_jitter_buffer_size));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_telephone_event_duration));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_port_rtp_valueset));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_candidate_priority));
        editPreferenceList.add(getString(R.string.ims_key_media_audio_vt_srtp_master_key_lifetime));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        neditPreferenceList = new ArrayList<String>();
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_vt_bw_as));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_vt_bw_rs));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_vt_bw_rr));
        neditPreferenceList.add(getString(R.string.ims_key_media_audio_vt_socket_tos));

        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        listPreferenceList = new ArrayList<String>();
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_rtcp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_send_rtcp_bye));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_sdp_answer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_sdp_reoffer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_sdp_keep_m_line));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_port_rtp_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_as_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_rs_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_rr_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_ptime));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_max_ptime));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_socket_pos));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_rtcpxr_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_empty_fmtp));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_srtp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_srtp_support_capa_nego));
        listPreferenceList.add(getString(R.string.ims_key_media_audio_vt_srtp_crypto_type));

        for (String preference : listPreferenceList) {
            setListPreference(cursor, prefix, preference, columns, listener);
        }

        checkboxPreferenceList = new ArrayList<String>();
        checkboxPreferenceList.add(getString(R.string.ims_key_media_audio_vt_common_rtp_port_per_service));

        for (String preference : checkboxPreferenceList) {
            setCheckboxPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerMediaVideoChangeListener(PrefixSpecificChangeListener listener) {

        ArrayList<String> editPreferenceList;
        ArrayList<String> neditPreferenceList;
        ArrayList<String> listPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        String prefix = listener.getPrefix();

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_video_vt_tv_rtp_inactivity));
        editPreferenceList.add(getString(R.string.ims_key_media_video_framerate));
        editPreferenceList.add(getString(R.string.ims_key_media_video_vt_port_rtp_valueset));
        editPreferenceList.add(getString(R.string.ims_key_media_video_vt_candidate_priority));
        editPreferenceList.add(getString(R.string.ims_key_media_video_vt_iframe_interval));

        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_down_interval));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_up_interval));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_loss_threshold));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_min_bitrate_ratio));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_bitrate_level));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_up_level));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr_enforce_vt_downgrade));

        editPreferenceList.add(getString(R.string.ims_key_media_video_marker_option));
        editPreferenceList.add(getString(R.string.ims_key_media_video_vt_srtp_master_key_lifetime));
        editPreferenceList.add(getString(R.string.ims_key_media_video_enable_onscreen_debug_info_video));
        editPreferenceList.add(getString(R.string.ims_key_media_video_file_path));

        editPreferenceList.add(getString(R.string.ims_key_media_video_avp_rate_adaptation_cycle));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avp_rate_adaptation_dev_ratio));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avp_rate_adaptation_threshold));
        editPreferenceList.add(getString(R.string.ims_key_media_video_avp_rate_adaptation_min_bitrate_ratio));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        neditPreferenceList = new ArrayList<String>();
        neditPreferenceList.add(getString(R.string.ims_key_media_video_vt_bw_as));
        neditPreferenceList.add(getString(R.string.ims_key_media_video_vt_bw_rs));
        neditPreferenceList.add(getString(R.string.ims_key_media_video_vt_bw_rr));
        neditPreferenceList.add(getString(R.string.ims_key_media_video_cvo_id));
        neditPreferenceList.add(getString(R.string.ims_key_media_video_vt_socket_tos));

        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        listPreferenceList = new ArrayList<String>();
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_rtcp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_send_rtcp_bye));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_sdp_answer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_sdp_reoffer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_sdp_keep_m_line));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_port_rtp_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_as_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_rs_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_rr_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_framerate_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_send_periodic_sps_pps));
        listPreferenceList.add(getString(R.string.ims_key_media_video_avpf_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_video_avpf_nack));
        listPreferenceList.add(getString(R.string.ims_key_media_video_avpf_tmmbr));
        listPreferenceList.add(getString(R.string.ims_key_media_video_avpf_pli));
        listPreferenceList.add(getString(R.string.ims_key_media_video_avpf_fir));
        listPreferenceList.add(getString(R.string.ims_key_media_video_cvo_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_socket_pos));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_resolution_loose_check));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_profile_level_negotiation));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_bitrate_adapt_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_empty_fmtp));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_srtp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_srtp_support_capa_nego));
        listPreferenceList.add(getString(R.string.ims_key_media_video_vt_srtp_crypto_type));
        listPreferenceList.add(getString(R.string.ims_key_media_video_test_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_video_dump_camera));
        listPreferenceList.add(getString(R.string.ims_key_media_video_dump_encoder_in));
        listPreferenceList.add(getString(R.string.ims_key_media_video_dump_encoder_out));
        listPreferenceList.add(getString(R.string.ims_key_media_video_dump_decoder_in));
        listPreferenceList.add(getString(R.string.ims_key_media_video_dump_decoder_out));

        for (String preference : listPreferenceList) {
            setListPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerMediaTextVoLTEChangeListener(PrefixSpecificChangeListener listener) {
        ImsLog.d("");

        ArrayList<String> editPreferenceList;
        ArrayList<String> neditPreferenceList;
        ArrayList<String> listPreferenceList;
        ArrayList<String> checkboxPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();

        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        String prefix = listener.getPrefix();

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_text_volte_port_rtp_valueset));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        neditPreferenceList = new ArrayList<String>();
        neditPreferenceList.add(getString(R.string.ims_key_media_text_volte_bw_as));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_volte_bw_rs));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_volte_bw_rr));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_volte_socket_tos));

        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        listPreferenceList = new ArrayList<String>();
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_rtcp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_send_rtcp_bye));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_sdp_answer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_sdp_reoffer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_sdp_keep_m_line));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_port_rtp_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_as_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_rs_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_rr_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_socket_pos));
        listPreferenceList.add(getString(R.string.ims_key_media_text_volte_empty_fmtp));

        for (String preference : listPreferenceList) {
            setListPreference(cursor, prefix, preference, columns, listener);
        }

        checkboxPreferenceList = new ArrayList<String>();

        for (String preference : checkboxPreferenceList) {
            setCheckboxPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerMediaTextVTChangeListener(PrefixSpecificChangeListener listener) {

        ArrayList<String> editPreferenceList;
        ArrayList<String> neditPreferenceList;
        ArrayList<String> listPreferenceList;
        ArrayList<String> checkboxPreferenceList;

        Cursor cursor = loadTable(imsDB, listener.getTableName(), getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: " + listener.getTableName() + " is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        // Move the cursor to the first row
        cursor.moveToNext();

        String prefix = listener.getPrefix();

        editPreferenceList = new ArrayList<String>();
        editPreferenceList.add(getString(R.string.ims_key_media_text_vt_port_rtp_valueset));

        for (String preference : editPreferenceList) {
            setEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        neditPreferenceList = new ArrayList<String>();
        neditPreferenceList.add(getString(R.string.ims_key_media_text_vt_bw_as));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_vt_bw_rs));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_vt_bw_rr));
        neditPreferenceList.add(getString(R.string.ims_key_media_text_vt_socket_tos));

        for (String preference : neditPreferenceList) {
            setNumberEditTextPreference(cursor, prefix, preference, columns, listener);
        }

        listPreferenceList = new ArrayList<String>();
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_rtcp_enable));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_send_rtcp_bye));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_sdp_answer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_sdp_reoffer_fullcapa));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_sdp_keep_m_line));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_port_rtp_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_as_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_rs_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_rr_mode));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_socket_pos));
        listPreferenceList.add(getString(R.string.ims_key_media_text_vt_empty_fmtp));

        for (String preference : listPreferenceList) {
            setListPreference(cursor, prefix, preference, columns, listener);
        }

        checkboxPreferenceList = new ArrayList<String>();

        for (String preference : checkboxPreferenceList) {
            setCheckboxPreference(cursor, prefix, preference, columns, listener);
        }

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    private void registerCodecSettingChangeListener(MediaCodecChangeListener listener) {
        ImsLog.d("");

        listener.mCommand.loadDB();
        listener.mCommand.setListener(listener);
    }

    private void registerMediaAdditionalChangeListener(MediaAdditionalChangeListener listener) {
        ImsLog.d("");

        int index;
        boolean bEVSCodecSetted = false;
        boolean bAMRWBCodecSetted  = false;
        String strEVSSupported = "false";
        String strAMRWBSupported = "false";

        Cursor cursor = loadTable(imsDB, CODEC_AUDIO_VOLTE, getSlotID());

        if (cursor == null) {
            ImsLog.e("Cursor :: (" + CODEC_AUDIO_VOLTE + ") is null");
            return;
        }

        String columns[] = cursor.getColumnNames();
        SettingUtil.displayColumns(columns, getSlotID());

        cursor.moveToNext();

        for (int i = 0; i < 9; i++) {
            index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_codec_type");
            String strCodecValue = cursor.getString(index);

            if (strCodecValue == null) {
                continue;
            }

            // check EVS codec
            if ( bEVSCodecSetted == false && strCodecValue.equalsIgnoreCase("EVS") ) {
                index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_network_type");
                String strNetworkType = cursor.getString(index);

                if ( strNetworkType.startsWith("lte") ) {
                    strEVSSupported = "true";
                }
                bEVSCodecSetted = true;
            }

            // check AMR WB codec
            if ( bAMRWBCodecSetted == false && (strCodecValue.equalsIgnoreCase("AMR") ) ) {
                index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_sampling_rate");
                String strSamplingRate = cursor.getString(index);
                if ( strSamplingRate.equalsIgnoreCase("16000") != true ) {
                    continue;
                }

                index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_network_type");
                String strNetworkType = cursor.getString(index);

                if ( strNetworkType.startsWith("lte") ) {
                    strAMRWBSupported = "true";
                }
                bAMRWBCodecSetted = true;
            }
        }
        ImsLog.d("strEVSSupported :: " + strEVSSupported);
        ImsLog.d("strAMRWBSupported :: " + strAMRWBSupported);

        CheckBoxPreference checkbox;

        // Set 'EVS supported' from DB
        checkbox = getCheckBoxPreference(getString(R.string.ims_key_media_evs_codec_mode));
        setListener(checkbox, listener);

        if (ImsProperties.SOC_MODEL.equalsIgnoreCase("sdm845")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8998")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8996")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8937")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8917")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8953")
                || ImsProperties.SOC_MODEL.equalsIgnoreCase("msm8940")) {
            if (strEVSSupported.equalsIgnoreCase("TRUE")) {
                checkbox.setChecked(true);
            } else {
                checkbox.setChecked(false);
            }
        } else {
            checkbox.setChecked(false);
            checkbox.setEnabled(false);
        }

        ImsLog.d("ims_key_media_evs_codec_mode :: " + checkbox.isChecked());

        // Set 'AMR WB supported' from DB
        checkbox = getCheckBoxPreference(getString(R.string.ims_key_media_amrwb_codec_mode));
        setListener(checkbox, listener);

        if (strAMRWBSupported.equalsIgnoreCase("TRUE")) {
            checkbox.setChecked(true);
        } else {
            checkbox.setChecked(false);
        }
        ImsLog.d("ims_key_media_amrwb_codec_mode :: " + checkbox.isChecked());

        // Close the cursor; release all the resource related with the cursor
        cursor.close();
    }

    //---------------------------------------------------------------------------------------------------------

    public class MediaSettingCommand
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        protected String mTableName;
        protected String mTypePrefix;
        protected int mSessionType;
        protected int mMaxCodecNum;
        protected int mCodecNum;
        protected String mCodecTypeName;

        protected IMSMediaCodecInfo mMediaCodecInfo;
        protected ArrayList<String> mCodecKeys;
        protected HashMap<String, String[]> mListCodecAttributes;

        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        public void setCodecAttributes(String key, String[] codecAttributes) {

            mCodecKeys.add(key);

            mListCodecAttributes.put(key, codecAttributes);
        }

        @SuppressWarnings("deprecation")
        public void setListener(MediaCodecChangeListener listener) {

            for (int i = 0; i < mMaxCodecNum; i++) {

                Preference pref = findPreference(mSessionType + "_" + i + "_" + mCodecTypeName);
                if (pref != null) {
                    //set setting
                    pref.setOnPreferenceChangeListener(listener);
                }

                for (int j = 0; j < mCodecKeys.size(); j++) {

                    String[] attributes = mListCodecAttributes.get(mCodecKeys.get(j));

                    if (attributes == null) {
                        continue;
                    }

                    for (int k = 0; k < attributes.length; k++) {
                        pref = findPreference(mSessionType + "_" + i + "_" + attributes[k]);

                        if (pref != null) {
                            pref.setOnPreferenceChangeListener(listener);
                        }
                    }
                }
            }
        }

        public boolean loadDB() {

            for (int i = 0; i < mMaxCodecNum; i++) {
                loadCodecList(i);
            }
            return true;
        }

        protected String load(String tableName, String key) {

            int index;
            Cursor cursor = MediaSetting.loadTable(imsDB, tableName, getSlotID());

            if (cursor == null) {
                ImsLog.e("Cursor :: " + tableName + " is null");
                return null;
            }

            String columns[] = cursor.getColumnNames();

            // Move the cursor to the first row
            cursor.moveToNext();

            //get the DB codec information
            index = SettingUtil.getColumnIndex(columns, key);

            if (index == -1) {
                cursor.close();
                return null;
            }

            String value = cursor.getString(index);

            // WBT - cursor close
            cursor.close();
            return value;
        }

        @SuppressWarnings("deprecation")
        public boolean loadCodecList(int index) {

            //load codec attribute value from DB
            String value = load(mTableName, mTypePrefix + "_" + index + "_" + mCodecTypeName);

            if (value == null || value.equals("")) {
                ImsLog.e("loadCodecList - wrong key : " + mTypePrefix + "_" + index + "_" + mCodecTypeName);
                return false;
            }

            String key = mSessionType + "_" + index + "_" + mCodecTypeName;
            Preference pref = findPreference(key);

            if (pref != null) {
                //set setting
                setValue(pref, value);
                SettingUtil.setSummary(pref, value);
            }
            else {
                ImsLog.e("loadCodecList - wrong key : " + key);
            }

            //Enable / Disable Codec attributes
            for (int i = 0; i < mCodecKeys.size(); i++) {

                String[] attributes = mListCodecAttributes.get(mCodecKeys.get(i));

                if (attributes != null) {

                    if (value.equals("None")) {
                        loadCodecAttribute(mCodecKeys.get(i), index, attributes, false);
                        continue;
                    }
                    //ImsLog.e("TEST:: value: " + value + " mCodecKeys.get(i): " + mCodecKeys.get(i) + " mTypePrefix: " + mTypePrefix);

                    if (mCodecKeys.get(i).equals(value)) {

                        for (int j = 0; j < attributes.length; j++) {
                            attributes[j] = ReplaceKeyForPrefChange(attributes[j], mCodecKeys.get(i));
                        }

                        loadCodecAttribute(value, index, attributes, true);
                    }
                    else if (mCodecKeys.get(i).equals(mTypePrefix)) {
                        loadCodecAttribute(mCodecKeys.get(i), index, attributes, true);
                    }
                    else {
                        loadCodecAttribute(mCodecKeys.get(i), index, attributes, false);
                    }
                }
            }

            return true;
        }

        @SuppressWarnings("deprecation")
        public void loadCodecAttribute(String prefix, int index, String[] attributeArray, boolean isEnabled) {

            for (int j = 0; j < attributeArray.length; j++) {

                Preference pref = findPreference(mSessionType + "_" + index + "_" + attributeArray[j]);

                if (pref != null) {

                    pref.setShouldDisableView(true);

                    if (isEnabled) {
                        //load codec attribute value from DB
                        attributeArray[j] = ReplaceKeyForDBLoad(attributeArray[j]);
                        String query = prefix + "_" + index + "_" + attributeArray[j];
                        attributeArray[j] = ReplaceKeyForPrefChange(attributeArray[j], prefix);

                        //ImsLog.d(TAG, "loadCodecAttribute : " + query);

                        String attributeValue = load(mTableName, query);

                        //ImsLog.d(TAG, "loadCodecAttribute value:" + attributeValue);

                        if (attributeValue == null) {
                            ImsLog.e("loadCodecAttribute : Wrong Key");
                            continue;
                        }
                        setValue(pref, attributeValue);
                        SettingUtil.setSummary(pref, attributeValue);

                        pref.setEnabled(true);
                    }
                    else if (attributeArray[j].equals("codec_type")) {
                        pref.setEnabled(true);
                    }
                    else {
                        pref.setEnabled(false);
                    }
                }
            }
        }

        public String searchCodecPrefix(String attribute) {

            for (int i = 0; i < mCodecKeys.size(); i++) {

                String[] attributes = mListCodecAttributes.get(mCodecKeys.get(i));

                if (attributes == null) {
                    continue;
                }

                for (int j = 0; j < attributes.length; j++) {

                    if (attributes[j].equals(attribute)) {

                        return mCodecKeys.get(i);
                    }
                }
            }
            return null;
        }

        public boolean updateCodec(String query, String inputValue) {

            ContentValues values = new ContentValues();

            values.put(query, inputValue);

            if (updateValue(mTableName, values) == false) {

                ImsLog.e("updateCodec - fail");
                return false;
            }
            return true;
        }

        public boolean updateValue(String tableName, ContentValues values) {

            int affectedRows = 0;

            if (imsDB == null) {
                ImsLog.w("DB table (" + tableName + ") is null");
                return false;
            }

            imsDB.beginTransaction();

            try
            {
                String selection = DBUtils.selectForSlotId(getSlotID());
                affectedRows = imsDB.update(tableName, values, selection, null);
                imsDB.setTransactionSuccessful();
            }
            catch (SQLiteException e) {
                e.printStackTrace();
            }
            finally
            {
                DBUtils.DB.endTransaction(imsDB);
            }

            ImsLog.w(" :: updated row count: " + affectedRows);

            if (affectedRows  <  1) {
                ImsLog.e("Update fails");
                return false;
            }

            return true;
        }

        public boolean preferenceChange(int index, String attribute, String inputValue) {

            String prefix = null;

            if (attribute.equals("network_type")) {

                //fix the MultiSelectionListPreference android API Bug
                inputValue = inputValue.toLowerCase(Locale.ROOT);
                inputValue = inputValue.replace(" ", "");
                inputValue = inputValue.replace("[", "");
                inputValue = inputValue.replace("]", "");

                if (inputValue.equals("")) {
                    inputValue = "";
                } else if ( inputValue.substring(0, 1) != null && (inputValue.substring(0, 1)).equals(",") ) {
                    inputValue = inputValue.substring(1);
                }
            }

            if (attribute.equals(mCodecTypeName)) {
                prefix = mTypePrefix;
            }
            else {
                prefix = searchCodecPrefix(attribute);
                attribute = ReplaceKeyForDBLoad(attribute);
            }

            if (prefix == null) {
                ImsLog.e("cannot find prefix - : " + attribute);
                return false;
            }

            String query = prefix + "_" + index + "_" + attribute;

            ImsLog.d("updateCodec - query : " + query);

            if (updateCodec(query, inputValue)) {

                loadCodecList(index);
            }
            return true;

        }
        // Interface implementation methods --------------------------
        public MediaSettingCommand(String tableName, String typePrefix, int sessionType) {

            mTableName = tableName;
            mTypePrefix = typePrefix;
            mSessionType = sessionType;
            mMaxCodecNum = 10;
            mCodecNum = 8;
            mCodecTypeName = "codec_type";

            //ImsLog.e("MediaSettingCommand TEST:: mTableName: " + tableName + " mTypePrefix: " + mTypePrefix);

            mMediaCodecInfo = new IMSMediaCodecInfo();
            mCodecKeys = new ArrayList<String>();
            mListCodecAttributes = new HashMap<String, String[]>();
        }
        // Private/Protected methods ---------------------------------
        protected String ReplaceKeyForDBLoad(String key) {
            return key;
        }

        protected String ReplaceKeyForPrefChange(String key, String input) {
            return key;
        }
    }

    //---------------------------------------------------------------------------------------------------------

    public class IMSMediaCodecInfo
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        public String[] codecAudioCommonAttribute;
        public String[] codecAudioAMRAttribute;
        // [EVS] jinsun.hwang@ --- start
        public String[] codecAudioEVSAttribute;
        // [EVS] --- end

        public String[] codecVideoCommonAttribute;
        public String[] codecVideoH263Attribute;
        public String[] codecVideoH264Attribute;
        public String[] codecVideoH265Attribute;

        public String[] codecTextCommonAttribute;
        public String[] codecTextREDAttribute;

        public static final int DEFAULT_PAYLOAD_NUM = 100;

        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        // Interface implementation methods --------------------------
        public IMSMediaCodecInfo() {

            codecAudioCommonAttribute = new String[]{    "codec_type", "payload_type", "network_type", "sampling_rate", "priority" };
            codecAudioAMRAttribute = new String[]
            {
                "channel", "octet_align", "mode_set", "default_rtp_modeset", "mode_change_capability", "mode_change_period",
                    "mode_change_neighbor", "max_red", "robust_sorting", "ptime", "max_ptime", "scr"
            };

            // [EVS] jinsun.hwang@ --- start
            codecAudioEVSAttribute = new String[]
            {
                "evs_channel", "evs_ptime", "evs_max_ptime", "evs_dtx", "evs_header_full_only", "evs_mode_switch",
                "evs_max_red", "evs_bitrate", "evs_bandwidth", "evs_cmr", "evs_channel_aware_mode", "evs_mode_set",
                "evs_default_mode_set", "evs_default_bandwidth", "evs_default_bitrate", "evs_send_cmr"
            };
            // [EVS] --- end

            codecVideoCommonAttribute = new String[]
            {
                "codec_type", "network_type", "payload_type", "resolution", "framerate", "bitrate", "AS",
                    "profile_level_id", "chp_profile_level_id", "imageattr", "framesize", "priority"
            };

            codecVideoH263Attribute = new String[]{ "profile", "level", "QCIF" };
            codecVideoH264Attribute = new String[]
            {
                "packetization_mode", "sprop_parameter_sets", "max_mbps", "max_fs",
                    "max_cpb", "max_dpb", "max_br", "redundant_pic_ca"
            };
            codecVideoH265Attribute = new String[]
            {
                "h265_profile", "h265_level", "h265_packetization_mode", "h265_sprop_parameter_sets", "h265_max_mbps",
                    "h265_max_fs", "h265_max_cpb", "h265_max_dpb", "h265_max_br"
            };

            codecTextCommonAttribute = new String[]{    "payload_type", "network_type", "sampling_rate" };
            codecTextREDAttribute = new String[]{    "redundancy", "red_subtype" };

        }
        // Private/Protected methods ---------------------------------
    }

    //---------------------------------------------------------------------------------------------------------

    private class MediaCodecChangeListener implements Preference.OnPreferenceChangeListener
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        private MediaSettingCommand mCommand;
        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        public MediaCodecChangeListener(MediaSettingCommand command) {
            mCommand = command;
            //            mCommand.loadDB();
            //            mCommand.setListener(this);
        }

        public boolean onPreferenceChange(Preference preference, Object newValue) {

            if (mCommand == null) {
                return false;
            }

            String key = preference.getKey();

            int index = Integer.parseInt(key.substring(2, 3));
            String attribute = key.substring(4);

            String inputValue = newValue.toString();

            ImsLog.d("onPreferenceChange - key : " + key);
            ImsLog.d("onPreferenceChange - value : " + inputValue);

            mCommand.preferenceChange(index, attribute, inputValue);

            return true;
        }
    }

    private class MediaAdditionalChangeListener extends PrefixSpecificChangeListener
    {
        // Variables--------------------------------------------------
        public static final String CODEC_AUDIO_VOLTE = "gims_com_media_audio_codec_volte";
        public static final String CODEC_AUDIO_VT = "gims_com_media_audio_codec_vt";

        // Public methods --------------------------------------------
        public MediaAdditionalChangeListener(SQLiteDatabase imsDB, String keyPrefix, String tableName, String logTag) {
            super(imsDB, keyPrefix, tableName, logTag, getSlotID());
        }

        // Interface implementation methods --------------------------
        @Override
        public boolean onPreferenceChange(Preference preference, Object newValue) {
            String key = preference.getKey();

            ImsLog.d(" :: key: " + key + " -> value:" + newValue.toString());

            if (imsDB == null) {
                ImsLog.w("DB table (" + mTableName + ") is null");
                return false;
            }

            if (key.equalsIgnoreCase(getString(R.string.ims_key_media_evs_codec_mode)) ||
                key.equalsIgnoreCase(getString(R.string.ims_key_media_amrwb_codec_mode))) {
                updateCodecDB(CODEC_AUDIO_VOLTE, key, newValue.toString());
                updateCodecDB(CODEC_AUDIO_VT, key, newValue.toString());
            }

            return true;
        }
        public boolean updateCodecDB(String strDBTable, String strKey, String strNewValue) {
            ImsLog.d("updateCodecDB :: key: " + strKey + " -> value:" + strNewValue.toString());
            int index;
            int affectedRows = 0;
            boolean bTEWBDisabled = false;
            ContentValues values = new ContentValues();

            // make a values for updating DB
            Cursor cursor = MediaSetting.loadTable(imsDB, strDBTable, getSlotID());

            if (cursor == null) {
                return false;
            }

            String columns[] = cursor.getColumnNames();
            cursor.moveToNext();

            for (int i = 0; i < 9; i++) {
                index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_codec_type");
                String strCodecValue = cursor.getString(index);

                if (strCodecValue == null) {
                    continue;
                }

                // EVS checked
                if ( strKey.equalsIgnoreCase(getString(R.string.ims_key_media_evs_codec_mode))
                    && strCodecValue.equalsIgnoreCase("EVS")) {

                    if ( strNewValue.equalsIgnoreCase("true") ) {
                        values.put("audiocodec_" + i + "_network_type", "lte,wifi,3g,hspa");
                    } else {
                        values.put("audiocodec_" + i + "_network_type", "");

                        // check WB telephony event disable case
                        CheckBoxPreference checkboxAMRWB;
                        checkboxAMRWB = getCheckBoxPreference(getString(R.string.ims_key_media_amrwb_codec_mode));

                        if (checkboxAMRWB != null && checkboxAMRWB.isChecked() == false) {
                            bTEWBDisabled = true;
                        }
                    }
                }

                //AMR WB checked
                if (strKey.equalsIgnoreCase(getString(R.string.ims_key_media_amrwb_codec_mode))
                    && strCodecValue.equalsIgnoreCase("AMR")) {

                    index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_sampling_rate");
                    String strSamplingRate = cursor.getString(index);
                    if (strSamplingRate.equalsIgnoreCase("16000") != true) {
                        continue;
                    }

                    if ( strNewValue.equalsIgnoreCase("true") ) {
                        values.put("audiocodec_" + i + "_network_type", "lte,wifi,3g,hspa");
                    } else {
                        values.put("audiocodec_" + i + "_network_type", "");

                        // check WB telephony event disable case
                        CheckBoxPreference checkboxEVS;
                        checkboxEVS = getCheckBoxPreference(getString(R.string.ims_key_media_evs_codec_mode));

                        if (checkboxEVS != null && checkboxEVS.isChecked() == false) {
                            bTEWBDisabled = true;
                        }
                    }
                }

                // WB Telephony Event checked
                if (strCodecValue.equalsIgnoreCase("telephone-event")) {

                    index = SettingUtil.getColumnIndex(columns, "audiocodec_" + i + "_sampling_rate");
                    String strSamplingRate = cursor.getString(index);
                    if (strSamplingRate.equalsIgnoreCase("16000") != true) {
                        continue;
                    }

                    if (bTEWBDisabled == true) {
                        values.put("audiocodec_" + i + "_network_type", "");
                    } else {
                        values.put("audiocodec_" + i + "_network_type", "lte,wifi,3g,hspa");
                    }
                }
            }

            if (values.size() > 0) {
                imsDB.beginTransaction();

                try {
                    String selection = DBUtils.selectForSlotId(getSlotID());
                    affectedRows = imsDB.update(strDBTable, values, selection, null);
                    imsDB.setTransactionSuccessful();
                } catch (SQLiteException e) {
                    e.printStackTrace();
                } finally {
                    DBUtils.DB.endTransaction(imsDB);
                }

                ImsLog.w(" :: updated row count: " + affectedRows);

                if (affectedRows != 1) {
                    ImsLog.e("Update fails");
                }
             }
            cursor.close();

            return true;
        }

    }


    //---------------------------------------------------------------------------------------------------------

    public class TextSettingCommand extends MediaSettingCommand
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        public TextSettingCommand(String tableName, String typePrefix, int sessionType) {

            super(tableName, typePrefix, sessionType);
            //set specific codec attributes
            setCodecAttributes(typePrefix, mMediaCodecInfo.codecTextCommonAttribute);
            setCodecAttributes("red", mMediaCodecInfo.codecTextREDAttribute);
        }

        protected int mTextCodecNum = 5;

        @SuppressWarnings("deprecation")
        public boolean loadCodecList(int index) {

            if (index > mTextCodecNum)
            {
               return false;
            }

            super.loadCodecList(index);

            //load codec attribute value from DB
            String value = load(mTableName, mTypePrefix + "_" + index + "_" + mCodecTypeName);

            if (value == null || value.equals("")) {
                return false;
            }

            if (value.equals("None") == false) {

                Preference pref = findPreference(mSessionType + "_" + index + "_" + mCodecTypeName);

                String inputValue = "t140";
                if (pref != null) {
                    if (value.equals("T140") || value.equals("t140")) {
                        inputValue = "t140";
                    }
                    else if (value.equals("RED") || value.equals("red")) {
                        inputValue = "red";
                    }
                    setValue(pref, inputValue);
                    SettingUtil.setSummary(pref, inputValue);
                }

                if (inputValue.equals("red")) {

                EditTextPreference edit = getEditTextPreference(mSessionType + "_" + index + "_redundancy");

                if (edit == null) {
                    return false;
                }
                edit.setDialogMessage(getString(R.string.ims_summary_redundancy));

                edit = getEditTextPreference(mSessionType + "_" + index + "_red_subtype");

                if (edit == null) {
                    return false;
                }
                edit.setDialogMessage(getString(R.string.ims_summary_red_subtype));

                }
            }

            return true;
        }

        public boolean preferenceChange(int index, String attribute, String inputValue) {

            if (attribute.equals(mCodecTypeName)) {
                if (inputValue.equals("red") || inputValue.equals("RED")) {
                    inputValue = "red";
                    EditTextPreference edit = getEditTextPreference(mSessionType + "_" + index + "_redundancy");
                    if (edit == null) {
                        return false;
                    }
                    edit.setDialogMessage(getString(R.string.ims_summary_redundancy));

                    edit = getEditTextPreference(mSessionType + "_" + index + "_red_subtype");
                    if (edit == null) {
                        return false;
                    }
                    edit.setDialogMessage(getString(R.string.ims_summary_red_subtype));
                }
                else {
                    String redundancy = "1";
                    String query = mTypePrefix + "_" + index + "_" + "redundancy";
                    updateCodec(query, redundancy);
                }
            }

            String samplingRate = "1000";

            String query = mTypePrefix + "_" + index + "_" + "sampling_rate";
            updateCodec(query, samplingRate);

            super.preferenceChange(index, attribute, inputValue);

            return true;
        }

    }

    //---------------------------------------------------------------------------------------------------------

    public class VideoSettingCommand extends MediaSettingCommand
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        public VideoSettingCommand(String tableName, String typePrefix, int sessionType) {

            super(tableName, typePrefix, sessionType);
            //set specific codec attributes
            setCodecAttributes(typePrefix, mMediaCodecInfo.codecVideoCommonAttribute);
            setCodecAttributes("H264", mMediaCodecInfo.codecVideoH264Attribute);
            setCodecAttributes("H263", mMediaCodecInfo.codecVideoH263Attribute);
            setCodecAttributes("H265", mMediaCodecInfo.codecVideoH265Attribute);
        }

        public boolean loadCodecList(int index) {

            super.loadCodecList(index);

            //load codec attribute value from DB
            String value = load(mTableName, mTypePrefix + "_" + index + "_" + mCodecTypeName);

            if (value == null || value.equals("")) {
                return false;
            }

            //resolution selections
            ListPreference list = getListPreference(mSessionType + "_" + index + "_resolution");

            if (list == null) {
                return false;
            }

            list.setEntries(getResources().getStringArray(R.array.vt_video_codec_resolution));
            list.setEntryValues(getResources().getStringArray(R.array.sixteen_item_value));

            return true;
        }

        public boolean preferenceChange(int index, String attribute, String inputValue) {

            super.preferenceChange(index, attribute, inputValue);

            if (attribute.equals("codec_type")) {

                ListPreference list = getListPreference(mSessionType + "_" + index + "_resolution");

                if (list == null) {
                    return false;
                }

                list.setEntries(getResources().getStringArray(R.array.vt_video_codec_resolution));
                list.setEntryValues(getResources().getStringArray(R.array.sixteen_item_value));
            }

            return true;
        }

        @Override
        protected String ReplaceKeyForDBLoad(String key) {

            if (key.startsWith("h265_")) {
                // remove pre_fix.
                return key.substring(5);
            }

            return key;
        }

        @Override
        protected String ReplaceKeyForPrefChange(String key, String input) {

            if (input.equals("H265") && !key.startsWith("h265_")) {
                // add pre_fix.
                key = "h265_" + key;
                return key;
            }

            return key;
        }

        // Interface implementation methods --------------------------
        // Private/Protected methods ---------------------------------

    }

    //---------------------------------------------------------------------------------------------------------

    public class AudioSettingCommand extends MediaSettingCommand
    {
        // Constants--------------------------------------------------
        // Variables--------------------------------------------------
        // Static loading materials ----------------------------------
        // Public methods --------------------------------------------
        public AudioSettingCommand(String tableName, String typePrefix, int sessionType) {

            super(tableName, typePrefix, sessionType);
            //set specific codec attributes
            setCodecAttributes(typePrefix, mMediaCodecInfo.codecAudioCommonAttribute);
            setCodecAttributes("AMR", mMediaCodecInfo.codecAudioAMRAttribute);
            setCodecAttributes("EVS", mMediaCodecInfo.codecAudioEVSAttribute);
        }

        @SuppressWarnings("deprecation")
        public boolean loadCodecList(int index) {

            if (index > mCodecNum)
            {
               return false;
            }

            super.loadCodecList(index);

            //load codec attribute value from DB
            String value = load(mTableName, mTypePrefix + "_" + index + "_" + mCodecTypeName);

            if (value == null || value.equals("")) {
                return false;
            }

            if (value.equals("None") == false) {

                Preference pref = findPreference(mSessionType + "_" + index + "_" + mCodecTypeName);

                String inputValue = "AMR";
                if (pref != null) {
                    //set setting
                    String samplingValue = load(mTableName, mTypePrefix + "_" + index + "_" + "sampling_rate");

                    //Null point check
                    if (samplingValue != null) {

                        if (value.equals("EVS")) {
                            inputValue = "EVS";
                        }
                        else if (value.equals("AMR")) {
                            if (samplingValue.equals("16000")) {
                                inputValue = "AMR-WB";
                            }
                        }
                        else if (value.equals("telephone-event")) {
                            if (samplingValue.equals("16000")) {
                                inputValue = "telephone-event 16k";
                            }
                            else {
                                inputValue = "telephone-event 8k";
                            }
                        }
                        setValue(pref, inputValue);
                        SettingUtil.setSummary(pref, inputValue);
                    }
                }

                //mode-set summary
                EditTextPreference edit = getEditTextPreference(mSessionType + "_" + index + "_mode_set");

                if (edit == null) {
                    return false;
                }

                if (inputValue.equals("AMR-WB")) {
                    //amr_wb
                    edit.setDialogMessage(getString(R.string.ims_summary_amr_wb_mode_set));
                }
                else {
                    //amr_nb
                    edit.setDialogMessage(getString(R.string.ims_summary_amr_mode_set));
                }
            }

            return true;
        }

        public boolean preferenceChange(int index, String attribute, String inputValue) {

            if (attribute.equals(mCodecTypeName)) {

                String samplingRate = "8000";
                EditTextPreference edit = getEditTextPreference(mSessionType + "_" + index + "_mode_set");
                if (edit == null) {
                    return false;
                }

                if (inputValue.equals("AMR-WB") || inputValue.equals("telephone-event 16k")) {
                    //amr_wb
                    samplingRate = "16000";
                    edit.setDialogMessage(getString(R.string.ims_summary_amr_wb_mode_set));
                }
                else {
                    //amr_nb
                    edit.setDialogMessage(getString(R.string.ims_summary_amr_mode_set));
                }

                if (inputValue.equals("AMR-WB")) {
                    inputValue = "AMR";
                }
                else if (inputValue.equals("telephone-event 16k") ||  inputValue.equals("telephone-event 8k")) {
                    inputValue = "telephone-event";
                }

                String query = mTypePrefix + "_" + index + "_" + "sampling_rate";
                updateCodec(query, samplingRate);

            }

            super.preferenceChange(index, attribute, inputValue);

            return true;
        }

        @Override
        protected String ReplaceKeyForDBLoad(String key) {

            if (key.startsWith("evs_")) {
                // remove pre_fix.
                return key.substring(4);
            }

            return key;
        }

        @Override
        protected String ReplaceKeyForPrefChange(String key, String input) {

            //ImsLog.e("ReplaceKeyForPrefChange TEST:: key: " + key + " input: " + input);
            if (input.equals("EVS") && !key.startsWith("evs_")) {
                // add pre_fix.
                key = "evs_"+ key;
                return key;
            }

            return key;
        }

    }


    public class CarrierMediaSettingCommand {
        protected HashMap<String, String>[] mDBCodecValues;
        protected IMSMediaCodecInfo mMediaCodecInfo;
        protected String mTableName;
        protected String mTypePrefix;
        protected int mSessionType;

        protected static final int MAX_CODEC_NUM = 9;

        @SuppressWarnings("unchecked")
        public CarrierMediaSettingCommand(String tableName, String typePrefix, int sessionType) {
            //super(tableName, typePrefix, sessionType);

            mTableName = tableName;
            mTypePrefix = typePrefix;
            mSessionType = sessionType;

            mMediaCodecInfo = new IMSMediaCodecInfo();

            mDBCodecValues = new HashMap[MAX_CODEC_NUM];

            for (int i = 0; i < MAX_CODEC_NUM; i++) {
                mDBCodecValues[i] = new HashMap<String, String>();
            }
        }

        public int GetSessionType() {
            return mSessionType;
        }

        public boolean loadDB(String Prefix, String[] strAttributes) {
            String key = null;
            String value = null;

            for (int i = 0; i < MAX_CODEC_NUM; i++) {
                for (int j = 0; j < strAttributes.length; j++) {
                    key = Prefix + "_" + i + "_" + ReplaceKeyForDBLoad(strAttributes[j]);
                    value = load(mTableName, key);

                    // false is "0" or empty
                    if (value == null) {
                        continue;
                    }
                    mDBCodecValues[i].put(strAttributes[j], value);
                }
            }
            return true;
        }

        public int getIndexOfValue(String[] key, String[] value) {
            int validCount = 0;

            if (key.length != value.length) {
                return -1;
            }
            for (int i = 0; i < MAX_CODEC_NUM; i++) {
                validCount = 0;

                for (int j = 0; j < key.length; j++) {
                    //ImsLog.d(TAG + "getIndexOfValue " + "i's key : " + key[j] + " value : " + mDBCodecValues[i].get(key[j]));
                    String tempSearchValue;
                    tempSearchValue = mDBCodecValues[i].get(key[j]);
                    if (tempSearchValue == null) {
                        continue;
                    }

                    if (tempSearchValue.compareToIgnoreCase(value[j]) == 0) {
                        validCount++;
                    }
                    else if (key[j].compareToIgnoreCase("octet_align") == 0
                        && value[j].compareToIgnoreCase("0") == 0 && tempSearchValue.compareToIgnoreCase("") == 0) {
                        validCount++;
                    }
                    else {
                        break;
                    }
                }

                if (validCount == key.length) {
                    return i;
                }
            }

            return -1;
        }

        public boolean updateAllDB(String Prefix, String[] strAttributes) {
            String key = null;
            String value = null;

            for (int i = 0; i < MAX_CODEC_NUM; i++) {
                if (mDBCodecValues[i] == null)
                {
                    continue;
                }
                for (int j = 0; j < strAttributes.length; j++) {
                    key = Prefix + "_" + i + "_" + ReplaceKeyForDBLoad(strAttributes[j]);

                    value = mDBCodecValues[i].get(strAttributes[j]);
                    updateDB(mTableName, key, value);
                }

            }
            return true;
        }

        public boolean updateDB(String table, String query, String inputValue) {

            ContentValues values = new ContentValues();

            values.put(query, inputValue);

            if (updateValue(table, values) == false) {

                ImsLog.e("updateCodec - fail  table : " + table + " query : " + query + " value : " + inputValue);
                return false;
            }
            return true;
        }

        public boolean updateValue(String tableName, ContentValues values) {

            int affectedRows = 0;

            if (imsDB == null) {
                ImsLog.w("DB table (" + tableName + ") is null");
                return false;
            }

            imsDB.beginTransaction();

            try
            {
                String selection = DBUtils.selectForSlotId(getSlotID());
                affectedRows = imsDB.update(tableName, values, selection, null);
                imsDB.setTransactionSuccessful();
            }
            catch (SQLiteException e) {
                e.printStackTrace();
            }
            finally
            {
                DBUtils.DB.endTransaction(imsDB);
            }

            ImsLog.w(" :: updated row count: " + affectedRows);

            if (affectedRows  <  1) {
                ImsLog.e("Update fails");
                return false;
            }

            return true;
        }

        public String load(String tableName, String key) {

            int index;
            Cursor cursor = MediaSetting.loadTable(imsDB, tableName, getSlotID());

            if (cursor == null) {
                ImsLog.e("Cursor :: " + tableName + " is null");
                return null;
            }

            String columns[] = cursor.getColumnNames();

            // Move the cursor to the first row
            cursor.moveToNext();

            //get the DB codec information
            index = SettingUtil.getColumnIndex(columns, key);

            if (index == -1) {
                cursor.close();
                return null;
            }

            String value = cursor.getString(index);

            // WBT - cursor close
            cursor.close();
            return value;
        }

        public boolean loadDBToPreference(String tableName, Preference pref, String key) {

            ImsLog.d("loadDBToPreference : " + "key - " + key);

            String value = load(tableName, key);

            if (value == null) {
                return false;
            }

            if (pref == null) {
                return false;
            }

            if (setValue(pref, value)) {
                SettingUtil.setSummary(pref, value);
                return true;
            }
            return false;
        }

        public boolean loadedDBToPreference(Preference pref, int nCodecIndex, String key) {
            if (nCodecIndex < 0 || nCodecIndex >= MAX_CODEC_NUM) {
                return false;
            }
            String result = mDBCodecValues[nCodecIndex].get(key);
            if (result != null && setValue(pref, result)) {
                SettingUtil.setSummary(pref, result);
            }
            else {
                return false;
            }

            return true;
        }
        protected String ReplaceKeyForDBLoad(String key) {
            return key;
        }

        protected String ReplaceKeyForPrefChange(String key, String input) {
            return key;
        }
    }

    private class MediaSettingSlotListener extends SettingSlot.SlotListener {
        @Override
        public void onSlot(int slotID) {
            setSlotID(slotID);
            initialize();
        }
    }

    // Interface implementation methods --------------------------
    // Private/Protected methods ---------------------------------

}
