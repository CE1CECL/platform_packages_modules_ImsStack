package com.android.imsstack.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteQueryBuilder;
import android.os.Bundle;
import android.net.Uri;
import android.text.TextUtils;

import com.android.imsstack.core.config.ConfigXMLLoader;
import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsPrivateProperties;
import com.android.imsstack.util.Log;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class ConfigurationProvider extends ContentProvider {
    private static final String TAG = "ImsStack_ConfigurationProvider";
    private static final int DATABASE_VERSION = 408;

    // URI matcher types
    // Common
    protected static final int URI_SUBSCRIBER     = 1;
    protected static final int URI_ENGINE         = 2;
    protected static final int URI_SIP            = 3;
    protected static final int URI_SERVICE_SIP    = 4;

    protected static final int URI_AOS            = 20;
    protected static final int URI_AOS_APPLICATION = 21;
    protected static final int URI_AOS_CONDITION  = 22;
    protected static final int URI_AOS_CONNECTION = 23;
    protected static final int URI_AOS_HANDLE     = 24;
    protected static final int URI_AOS_PROVIDER   = 25;
    protected static final int URI_AOS_REG        = 26;

    protected static final int URI_FEATURE        = 40;
    protected static final int URI_SETTING        = 41;
    protected static final int URI_RESOURCE       = 42;
    protected static final int URI_IMS_MO         = 43;

    // VoLTE Service
    protected static final int URI_SESSION_COMMON = 50;
    protected static final int URI_UC_EMERGENCY   = 51;
    protected static final int URI_UC_CONFERENCE  = 52;

    protected static final int URI_SERVICE_SMS    = 62;
    protected static final int URI_SERVICE_EAB    = 63;
    protected static final int URI_SERVICE_MMTEL  = 64;
    protected static final int URI_SESSION_VOIP   = 65;
    protected static final int URI_SESSION_VT     = 66;
    protected static final int URI_SESSION_VZW    = 67;
    // 2016-04-04, Emergency call, red.kim@
    protected static final int URI_COM_EMERGENCY  = 68;
    protected static final int URI_COM_APP_UC     = 69;
    protected static final int URI_UC_APP         = 70;
    //nishit.dhruv@ 2018-06-08 added for MMtel Services
    protected static final int URI_SERVICE_MMTEL_CF  = 71;
    protected static final int URI_SERVICE_MMTEL_CB  = 72;
    protected static final int URI_SERVICE_GBA = 73;
    //Media
    protected static final int URI_MEDIA          = 80;
    protected static final int URI_SERVICE_MEDIA  = 81;
    protected static final int URI_MEDIA_AUDIO    = 82;
    protected static final int URI_MEDIA_AUDIO_CODEC_VOLTE = 83;
    protected static final int URI_MEDIA_AUDIO_CODEC_VT    = 84;
    protected static final int URI_COM_MEDIA_CAPABILITIES  = 85;

    // Common tables
    protected static final int URI_DB_INFO = 1001;
    protected static final int URI_MEDIA_DB_INFO = 1002;

    // Projection maps
    protected static final HashMap<String, String> PM_SUBSCRIBER;
    protected static final HashMap<String, String> PM_ENGINE;
    protected static final HashMap<String, String> PM_SIP;
    protected static final HashMap<String, String> PM_SERVICE_SIP;
    protected static final HashMap<String, String> PM_AOS;
    protected static final HashMap<String, String> PM_AOS_APPLICATION;
    protected static final HashMap<String, String> PM_AOS_CONDITION;
    protected static final HashMap<String, String> PM_AOS_CONNECTION;
    protected static final HashMap<String, String> PM_AOS_HANDLE;
    protected static final HashMap<String, String> PM_AOS_PROVIDER;
    protected static final HashMap<String, String> PM_AOS_REG;
    protected static final HashMap<String, String> PM_COM_APP_UC;
    protected static final HashMap<String, String> PM_UC_APP;
    protected static final HashMap<String, String> PM_MEDIA;
    protected static final HashMap<String, String> PM_SERVICE_MEDIA;
    protected static final HashMap<String, String> PM_COM_MEDIA_CAPABILITIES;
    protected static final HashMap<String, String> PM_SERVICE_VT;
    protected static final HashMap<String, String> PM_SERVICE_SMS;
    protected static final HashMap<String, String> PM_SERVICE_EAB;
    protected static final HashMap<String, String> PM_SERVICE_MMTEL;
    protected static final HashMap<String, String> PM_RESOURCE;
    protected static final HashMap<String, String> PM_FEATURE;

    //nishit.dhruv@ 2018-06-08 added for MMtel Services Projection Map
    protected static final HashMap<String, String> PM_SERVICE_MMTEL_CF;
    protected static final HashMap<String, String> PM_SERVICE_MMTEL_CB;
    protected static final HashMap<String, String> PM_SERVICE_GBA;

    protected static final HashMap<String, String> PM_SETTING;

    protected static final HashMap<String, String> PM_SESSION_COMMON;
    protected static final HashMap<String, String> PM_SESSION_VOIP;
    protected static final HashMap<String, String> PM_SESSION_VT;
    protected static final HashMap<String, String> PM_SESSION_VZW;
    protected static final HashMap<String, String> PM_UC_EMERGENCY;
    protected static final HashMap<String, String> PM_UC_CONFERENCE;

    // 2016-04-04, Emergency call, red.kim@
    protected static final HashMap<String, String> PM_COM_EMERGENCY;
    protected static final HashMap<String, String> PM_IMS_MO;

    // URI matchers
    protected static final UriMatcher URI_MATCHER;
    // Configuration XML Loader
    protected static ConfigXMLLoader sXMLLoader = new ConfigXMLLoader();

    // protected SQLiteDatabase mDB;
    protected DatabaseHelper mOpenHelper;

    /**
     * This class helps open, create, and upgrade the database file.
     */
    private static class DatabaseHelper extends SQLiteOpenHelper {
        private static Context sContext;

        DatabaseHelper(Context context, String name, CursorFactory factory, int version) {
            super(context, name, factory, version);

            // If platform doesn't enable WAL mode automatically,
            // enable the below method to allow multiple read access among threads.
            //setWriteAheadLoggingEnabled(true);

            sContext = context;
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            Log.i(TAG, "onCreate");
            createTables(db);
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.i(TAG, "Upgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");

            dropTables(db);
            onCreate(db);

            ImsPrivateProperties.Persistent.set(sContext,
                    ImsPrivateProperties.Persistent.KEY_DB_IMS_OLD_VERSION,
                    String.valueOf(oldVersion), 0);
        }

        // hwangoo.park@, 2012-04-12
        // If any problem occurred regarding to the lower version of the database,
        // the below code will be enabled.
        @Override
        public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.i(TAG, "Downgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");

            dropTables(db);
            onCreate(db);

            ImsPrivateProperties.Persistent.set(sContext,
                    ImsPrivateProperties.Persistent.KEY_DB_IMS_OLD_VERSION,
                    String.valueOf(oldVersion), 0);
        }

        // createTables is a core method to create tables in gims.db.
        // Currently, ConfigurationProvider use xml loader to get
        // db table name, column name, and initial values.
        public void createTables(SQLiteDatabase db) {
            String[] dbTableNames = sXMLLoader.getTableNames();

            if (dbTableNames == null) {
                Log.e(TAG, "dbTableNames is null");
                return;
            }

            for (int i = 0; i < dbTableNames.length; i++) {
                String[][] array = sXMLLoader.getTableElements(i);
                createTable(db, dbTableNames[i], array);
            }
        }

        // Create a table in db.
        // Data will later be inserted into the table.
        private boolean createTable(SQLiteDatabase db, String table, String[][] content) {

            if (table == null) {
                Log.e(TAG, "table is null");
                return false;
            }
            if (content == null || content.length == 0) {
                Log.e(TAG, "content is null");
                return false;
            }

            // Compose SQL query statement for CREATE
            String query = "CREATE TABLE IF NOT EXISTS " + table + " (";

            // '0' - column type, '1' - column name
            query += content[0][1];
            query += " ";
            query += content[0][0];

            for (int i = 1; i < content.length; ++i) {
                query += ",";
                // '0' - column type, '1' - column name
                query += content[i][1];
                query += " ";
                query += content[i][0];
            }

            query += ");";

            // Execute "CREATE" query statement
            db.execSQL(query);

            // SET Debug Level FFFF in advance...
            if (ProviderInterface.Engine.DEBUG_TABLE_NAME.equals(table)
                    || ProviderInterface.Engine.TABLE_NAME.equals(table)) {
                // Compose SQL query statement for INSERT
                ContentValues values = new ContentValues();
                for (int i = 0; i < content.length; ++i) {
                    // '1' - column name, '2' - column default value
                    values.put(content[i][1], content[i][2]);
                }

                // Execute "INSERT" query statement
                db.beginTransaction();
                try {
                    db.insertOrThrow(table, null, values);
                    db.setTransactionSuccessful();
                }
                catch (SQLiteException e) {
                    Log.e(TAG, "db.insertOrThrow() failed");
                    e.printStackTrace();
                    return false;
                }
                finally {
                    DBUtils.DB.endTransaction(db);
                }
            }

            return true;
        }

        private void dropTables(SQLiteDatabase db) {
            Log.i(TAG, "");

            String[] tables = sXMLLoader.getTableNames();
            if (tables != null) {
                for (String tableName : tables) {
                    db.execSQL("DROP TABLE IF EXISTS " + tableName);
                }
            }
        }
    }

    @Override
    public int delete(Uri arg0, String arg1, String[] arg2) {
        Log.e(TAG, "DELETE query is not implemented");
        return 0;
    }

    @Override
    public String getType(Uri uri) {
        switch (URI_MATCHER.match(uri)) {
            case URI_DB_INFO:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.DBInfo.TABLE_NAME;
            case URI_MEDIA_DB_INFO:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MediaDBInfo.TABLE_NAME;
            case URI_SUBSCRIBER:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.Subscriber.TABLE_NAME;
            case URI_ENGINE:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.Engine.TABLE_NAME;
            case URI_SIP:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SIP.TABLE_NAME;
            case URI_SERVICE_SIP:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SIP.TABLE_NAME_COM;
            case URI_AOS:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AOS.TABLE_NAME;
            case URI_AOS_APPLICATION:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSApplication.TABLE_NAME;
            case URI_AOS_CONDITION:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSCondition.TABLE_NAME;
            case URI_AOS_CONNECTION:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSConnection.TABLE_NAME;
            case URI_AOS_HANDLE:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSHandle.TABLE_NAME;
            case URI_AOS_PROVIDER:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSProvider.TABLE_NAME;
            case URI_AOS_REG:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.AoSReg.TABLE_NAME;
            case URI_SERVICE_MEDIA:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.Media.TABLE_NAME;
            case URI_SERVICE_SMS:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SMS.TABLE_NAME;
            case URI_SERVICE_EAB:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.EAB.TABLE_NAME;
            case URI_SERVICE_MMTEL:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MMTel.TABLE_NAME;
            //nishit.dhruv@ 2018-06-08 added for MMtel Services
            case URI_SERVICE_MMTEL_CF:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MMTel_CF.TABLE_NAME;
            case URI_SERVICE_MMTEL_CB:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MMTel_CB.TABLE_NAME;
            case URI_SERVICE_GBA:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.GBA.TABLE_NAME;
            case URI_RESOURCE:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.Resource.TABLE_NAME;
            case URI_SETTING:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.Setting.TABLE_NAME;
            case URI_UC_APP:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.UC_APP.TABLE_NAME;
            case URI_SESSION_VOIP:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SessionVoIP.TABLE_NAME;
            case URI_SESSION_VT:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SessionVt.TABLE_NAME;
            case URI_SESSION_COMMON:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SessionCommon.TABLE_NAME;
            case URI_UC_EMERGENCY:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.UCEmergency.TABLE_NAME;
            case URI_UC_CONFERENCE:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.UCConference.TABLE_NAME;
            case URI_SESSION_VZW:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.SessionVzw.TABLE_NAME;
            case URI_MEDIA_AUDIO:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MediaAudio.TABLE_NAME;
            case URI_MEDIA_AUDIO_CODEC_VOLTE:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MediaAudioCodecVoLTE.TABLE_NAME;
            case URI_MEDIA_AUDIO_CODEC_VT:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MediaAudioCodecVT.TABLE_NAME;
            case URI_COM_EMERGENCY:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.COM_EMERGENCY.TABLE_NAME;
            case URI_COM_APP_UC :
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.COM_APP_UC.TABLE_NAME;
            case URI_MEDIA:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.MEDIA.TABLE_NAME;
            case URI_COM_MEDIA_CAPABILITIES:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.COM_MEDIA_CAPABILITIES.TABLE_NAME;
            case URI_IMS_MO:
                return "vnd.android.cursor.dir/" + ProviderInterface.AUTHORITY +
                        "." + ProviderInterface.IMS_MO.TABLE_NAME;
            default:
                throw new IllegalArgumentException("Unsupported URI: " + uri);
        }
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        return null;
    }

    @Override
    public boolean onCreate() {
        sXMLLoader.init(getContext());

        mOpenHelper = new DatabaseHelper(getContext(),
                ProviderInterface.DB, null, DATABASE_VERSION);

        try {
            mOpenHelper.getWritableDatabase();
        } catch (SQLiteException e) {
            Log.e(TAG, "Open Database failed : SQLiteException occurred !!!");
            e.printStackTrace();
            return false;
        }

        Log.w(TAG, "DB(" + ProviderInterface.DB + "/" + DATABASE_VERSION + ") is created");

        return true;
    }

    @Override
    public Bundle call(String method, String arg, Bundle extras) {
        Log.i(TAG, "call :: method=" + method + ", arg=" + arg);

        Bundle result = null;

        switch (method) {
            case ProviderInterface.METHOD_GET_TABLE_IN_CONFIG_XML: {
                result = new Bundle();
                result.putInt(ProviderInterface.KEY_RESULT, getTableInConfigXml(arg));
                break;
            }
            case ProviderInterface.METHOD_GET_COLUMN_COUNT_IN_CONFIG_XML: {
                result = new Bundle();
                result.putInt(ProviderInterface.KEY_RESULT, getColumnCountInConfigXml(arg));
                break;
            }
            default: {
                // no-op
                break;
            }
        }

        return result;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        SQLiteQueryBuilder queryBuilder = new SQLiteQueryBuilder();

        switch (URI_MATCHER.match(uri)) {
            case URI_DB_INFO:
                queryBuilder.setTables(ProviderInterface.DBInfo.TABLE_NAME);
                break;
            case URI_MEDIA_DB_INFO:
                queryBuilder.setTables(ProviderInterface.MediaDBInfo.TABLE_NAME);
                break;
            case URI_SUBSCRIBER:
                queryBuilder.setTables(ProviderInterface.Subscriber.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SUBSCRIBER);
                break;
            case URI_ENGINE:
                queryBuilder.setTables(ProviderInterface.Engine.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_ENGINE);
                break;
            case URI_SIP:
                queryBuilder.setTables(ProviderInterface.SIP.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SIP);
                break;
            case URI_SERVICE_SIP:
                queryBuilder.setTables(ProviderInterface.SIP.TABLE_NAME_COM);
                queryBuilder.setProjectionMap(PM_SERVICE_SIP);
                break;
            case URI_AOS:
                queryBuilder.setTables(ProviderInterface.AOS.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS);
                break;
            case URI_AOS_APPLICATION:
                queryBuilder.setTables(ProviderInterface.AoSApplication.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_APPLICATION);
                break;
            case URI_AOS_CONDITION:
                queryBuilder.setTables(ProviderInterface.AoSCondition.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_CONDITION);
                break;
            case URI_AOS_CONNECTION:
                queryBuilder.setTables(ProviderInterface.AoSConnection.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_CONNECTION);
                break;
            case URI_AOS_HANDLE:
                queryBuilder.setTables(ProviderInterface.AoSHandle.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_HANDLE);
                break;
            case URI_AOS_PROVIDER:
                queryBuilder.setTables(ProviderInterface.AoSProvider.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_PROVIDER);
                break;
            case URI_AOS_REG:
                queryBuilder.setTables(ProviderInterface.AoSReg.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_AOS_REG);
                break;
            case URI_SERVICE_MEDIA:
                queryBuilder.setTables(ProviderInterface.Media.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_MEDIA);
                break;
            case URI_SERVICE_SMS:
                queryBuilder.setTables(ProviderInterface.SMS.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_SMS);
                break;
            case URI_SERVICE_EAB:
                queryBuilder.setTables(ProviderInterface.EAB.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_EAB);
                break;
            case URI_SERVICE_MMTEL:
                queryBuilder.setTables(ProviderInterface.MMTel.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_MMTEL);
                break;
             //nishit.dhruv@ 2018-06-08 added for MMtel Services
            case URI_SERVICE_MMTEL_CF:
                queryBuilder.setTables(ProviderInterface.MMTel_CF.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_MMTEL_CF);
                break;
            case URI_SERVICE_MMTEL_CB:
                queryBuilder.setTables(ProviderInterface.MMTel_CB.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_MMTEL_CB);
                break;
            case URI_SERVICE_GBA:
                queryBuilder.setTables(ProviderInterface.GBA.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SERVICE_GBA);
                break;
            case URI_RESOURCE:
                queryBuilder.setTables(ProviderInterface.Resource.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_RESOURCE);
                break;
            case URI_FEATURE:
                queryBuilder.setTables(ProviderInterface.FEATURE.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_FEATURE);
                break;
            case URI_SETTING:
                queryBuilder.setTables(ProviderInterface.Setting.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SETTING);
                break;
            case URI_UC_APP:
                queryBuilder.setTables(ProviderInterface.UC_APP.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_UC_APP);
                break;
            case URI_SESSION_VOIP:
                queryBuilder.setTables(ProviderInterface.SessionVoIP.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SESSION_VOIP);
                break;
            case URI_SESSION_VT:
                queryBuilder.setTables(ProviderInterface.SessionVt.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SESSION_VT);
                break;
            case URI_SESSION_COMMON:
                queryBuilder.setTables(ProviderInterface.SessionCommon.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SESSION_COMMON);
                break;
            case URI_UC_EMERGENCY:
                queryBuilder.setTables(ProviderInterface.UCEmergency.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_UC_EMERGENCY);
                break;
            case URI_UC_CONFERENCE:
                queryBuilder.setTables(ProviderInterface.UCConference.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_UC_CONFERENCE);
                break;
            case URI_SESSION_VZW:
                queryBuilder.setTables(ProviderInterface.SessionVzw.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_SESSION_VZW);
                break;
            case URI_MEDIA_AUDIO:
                queryBuilder.setTables(ProviderInterface.MediaAudio.TABLE_NAME);
                break;
            case URI_MEDIA_AUDIO_CODEC_VOLTE:
                queryBuilder.setTables(ProviderInterface.MediaAudioCodecVoLTE.TABLE_NAME);
                break;
            case URI_MEDIA_AUDIO_CODEC_VT:
                queryBuilder.setTables(ProviderInterface.MediaAudioCodecVT.TABLE_NAME);
                break;
            case URI_COM_EMERGENCY:
                queryBuilder.setTables(ProviderInterface.COM_EMERGENCY.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_COM_EMERGENCY);
                break;
            case URI_COM_APP_UC:
                queryBuilder.setTables(ProviderInterface.COM_APP_UC.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_COM_APP_UC);
                break;
            case URI_MEDIA:
                queryBuilder.setTables(ProviderInterface.MEDIA.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_MEDIA);
                break;
            case URI_COM_MEDIA_CAPABILITIES:
                queryBuilder.setTables(ProviderInterface.COM_MEDIA_CAPABILITIES.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_COM_MEDIA_CAPABILITIES);
                break;
            case URI_IMS_MO:
                queryBuilder.setTables(ProviderInterface.IMS_MO.TABLE_NAME);
                queryBuilder.setProjectionMap(PM_IMS_MO);
                break;
            default:
                throw new IllegalArgumentException("Unknown URI " + uri);
        }

        Cursor cursor = buildQuery(uri, projection, selection, selectionArgs, queryBuilder);

        return cursor;
    }

    @Override
    public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
        if (Log.isDebuggable()) {
            Log.w(TAG, "update from " + uri);
        }

        String tableName = null;
        int uriMatcher = URI_MATCHER.match(uri);

        switch (uriMatcher) {
            case URI_DB_INFO:
                tableName = ProviderInterface.DBInfo.TABLE_NAME;
                break;
            case URI_MEDIA_DB_INFO:
                tableName = ProviderInterface.MediaDBInfo.TABLE_NAME;
                break;
            case URI_SUBSCRIBER:
                tableName = ProviderInterface.Subscriber.TABLE_NAME;
                break;
            case URI_ENGINE:
                tableName = ProviderInterface.Engine.TABLE_NAME;
                break;
            case URI_SIP:
                tableName = ProviderInterface.SIP.TABLE_NAME;
                break;
            case URI_SERVICE_SIP:
                tableName = ProviderInterface.SIP.TABLE_NAME_COM;
                break;
            case URI_AOS:
                tableName = ProviderInterface.AOS.TABLE_NAME;
                break;
            case URI_AOS_APPLICATION:
                tableName = ProviderInterface.AoSApplication.TABLE_NAME;
                break;
            case URI_AOS_CONDITION:
                tableName = ProviderInterface.AoSCondition.TABLE_NAME;
                break;
            case URI_AOS_CONNECTION:
                tableName = ProviderInterface.AoSConnection.TABLE_NAME;
                break;
            case URI_AOS_HANDLE:
                tableName = ProviderInterface.AoSHandle.TABLE_NAME;
                break;
            case URI_AOS_PROVIDER:
                tableName = ProviderInterface.AoSProvider.TABLE_NAME;
                break;
            case URI_AOS_REG:
                tableName = ProviderInterface.AoSReg.TABLE_NAME;
                break;
            case URI_SERVICE_MEDIA:
                tableName = ProviderInterface.Media.TABLE_NAME;
                break;
            case URI_SERVICE_SMS:
                tableName = ProviderInterface.SMS.TABLE_NAME;
                break;
            case URI_SERVICE_EAB:
                tableName = ProviderInterface.EAB.TABLE_NAME;
                break;
            case URI_SERVICE_MMTEL:
                tableName = ProviderInterface.MMTel.TABLE_NAME;
                break;
            //nishit.dhruv@ 2018-06-08 added for MMtel Services
            case URI_SERVICE_MMTEL_CF:
                tableName = ProviderInterface.MMTel_CF.TABLE_NAME;
                break;
            case URI_SERVICE_MMTEL_CB:
                tableName = ProviderInterface.MMTel_CB.TABLE_NAME;
                break;
            case URI_SERVICE_GBA:
                tableName = ProviderInterface.GBA.TABLE_NAME;
                break;
            case URI_RESOURCE:
                tableName = ProviderInterface.Resource.TABLE_NAME;
                break;
            case URI_FEATURE:
                tableName = ProviderInterface.FEATURE.TABLE_NAME;
                break;
            case URI_SETTING:
                tableName = ProviderInterface.Setting.TABLE_NAME;
                break;
            case URI_UC_APP:
                tableName = ProviderInterface.UC_APP.TABLE_NAME;
                break;
            case URI_SESSION_VOIP:
                tableName = ProviderInterface.SessionVoIP.TABLE_NAME;
                break;
            case URI_SESSION_VT:
                tableName = ProviderInterface.SessionVt.TABLE_NAME;
                break;
            case URI_SESSION_COMMON:
                tableName = ProviderInterface.SessionCommon.TABLE_NAME;
                break;
            case URI_UC_EMERGENCY:
                tableName = ProviderInterface.UCEmergency.TABLE_NAME;
                break;
            case URI_UC_CONFERENCE:
                tableName = ProviderInterface.UCConference.TABLE_NAME;
                break;
            case URI_SESSION_VZW:
                tableName = ProviderInterface.SessionVzw.TABLE_NAME;
                break;
            case URI_MEDIA_AUDIO:
                tableName = ProviderInterface.MediaAudio.TABLE_NAME;
                break;
            case URI_MEDIA_AUDIO_CODEC_VOLTE:
                tableName = ProviderInterface.MediaAudioCodecVoLTE.TABLE_NAME;
                break;
            case URI_MEDIA_AUDIO_CODEC_VT:
                tableName = ProviderInterface.MediaAudioCodecVT.TABLE_NAME;
                break;
            case URI_COM_EMERGENCY:
                tableName = ProviderInterface.COM_EMERGENCY.TABLE_NAME;
                break;
            case URI_COM_APP_UC:
                tableName = ProviderInterface.COM_APP_UC.TABLE_NAME;
                break;
            case URI_MEDIA:
                tableName = ProviderInterface.MEDIA.TABLE_NAME;
                break;
            case URI_COM_MEDIA_CAPABILITIES:
                tableName = ProviderInterface.COM_MEDIA_CAPABILITIES.TABLE_NAME;
                break;
            case URI_IMS_MO:
                tableName = ProviderInterface.IMS_MO.TABLE_NAME;
                break;
            default:
                throw new IllegalArgumentException("Unknown URI " + uri);
        }

        if (tableName == null) {
            return 0;
        }

        SQLiteDatabase db = mOpenHelper.getWritableDatabase();
        int count = 0;

        db.beginTransactionNonExclusive();

        try {
            count = db.update(tableName, values, where, whereArgs);
            db.setTransactionSuccessful();
        } finally {
            DBUtils.DB.endTransaction(db);
        }

        if (Log.isDebuggable()) {
            Log.i(TAG, "update :: " + values.toString() + "; count (" + count + ")");
        }

        if (count > 0) {
            if (uriMatcher == URI_IMS_MO) {
                getContext().getContentResolver().notifyChange(uri, null);
            }
        }

        return count;
    }

    private Cursor buildQuery(Uri uri, String[] projection, String selection,
            String[] selectionArgs, SQLiteQueryBuilder queryBuilder) {
        Cursor cursor = null;
        SQLiteDatabase db = mOpenHelper.getWritableDatabase();

        try {
            cursor = queryBuilder.query(
                    db, projection, selection, selectionArgs, null, null, null);

            if (Log.isDebuggable()) {
                if (uri != null) {
                    Log.i(TAG, "query : " + uri.getLastPathSegment());
                } else {
                    Log.i(TAG, "query");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (cursor == null) {
            Log.w(TAG, "Cursor is null");
        }
        return cursor;
    }

    private String[] getTableNames() {
        String[] tableNames = sXMLLoader.getTableNamesFromCache();

        if (tableNames == null || tableNames.length == 0) {
            tableNames = sXMLLoader.getTableNames();
        }

        return tableNames;
    }

    private int getColumnCountInConfigXml(String tableName) {
        if (TextUtils.isEmpty(tableName)) {
            return 0;
        }

        String[] tableNames = getTableNames();

        if (tableNames == null || tableNames.length == 0) {
            return 0;
        }

        for (int i = 0; i < tableNames.length; i++) {
            if (tableName.equals(tableNames[i])) {
                String[][] contents = sXMLLoader.getTableElements(i);
                return (contents != null) ? contents.length : 0;
            }
        }

        return 0;
    }

    private int getTableInConfigXml(String tableName) {
        if (TextUtils.isEmpty(tableName)) {
            return 0;
        }

        String[] tableNames = getTableNames();

        if (tableNames == null || tableNames.length == 0) {
            return 0;
        }

        for (int i = 0; i < tableNames.length; i++) {
            if (tableName.equals(tableNames[i])) {
                return 1;
            }
        }

        return 0;
    }

    static
    {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);
        // Common tables
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.DBInfo.TABLE_NAME, URI_DB_INFO);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MediaDBInfo.TABLE_NAME, URI_MEDIA_DB_INFO);

        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.Subscriber.TABLE_NAME, URI_SUBSCRIBER);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.Engine.TABLE_NAME, URI_ENGINE);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SIP.TABLE_NAME, URI_SIP);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SIP.TABLE_NAME_COM, URI_SERVICE_SIP);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AOS.TABLE_NAME, URI_AOS);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSApplication.TABLE_NAME, URI_AOS_APPLICATION);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSCondition.TABLE_NAME, URI_AOS_CONDITION);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSConnection.TABLE_NAME, URI_AOS_CONNECTION);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSHandle.TABLE_NAME, URI_AOS_HANDLE);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSProvider.TABLE_NAME, URI_AOS_PROVIDER);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSReg.TABLE_NAME, URI_AOS_REG);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSProvider.TABLE_NAME, URI_AOS_PROVIDER);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.AoSCondition.TABLE_NAME, URI_AOS_CONDITION);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.Media.TABLE_NAME, URI_SERVICE_MEDIA);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SMS.TABLE_NAME, URI_SERVICE_SMS);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.EAB.TABLE_NAME, URI_SERVICE_EAB);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MMTel.TABLE_NAME, URI_SERVICE_MMTEL);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.Resource.TABLE_NAME, URI_RESOURCE);
        //nishit.dhruv@ 2018-06-08 added for MMtel Services
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MMTel_CF.TABLE_NAME, URI_SERVICE_MMTEL_CF);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MMTel_CB.TABLE_NAME, URI_SERVICE_MMTEL_CB);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.GBA.TABLE_NAME, URI_SERVICE_GBA);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.FEATURE.TABLE_NAME, URI_FEATURE);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.Setting.TABLE_NAME, URI_SETTING);
        //20150601, Add URIs for VZW VoLTE
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SessionVoIP.TABLE_NAME, URI_SESSION_VOIP);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SessionVt.TABLE_NAME, URI_SESSION_VT);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SessionCommon.TABLE_NAME, URI_SESSION_COMMON);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.UCEmergency.TABLE_NAME, URI_UC_EMERGENCY);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.UCConference.TABLE_NAME, URI_UC_CONFERENCE);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.SessionVzw.TABLE_NAME, URI_SESSION_VZW);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MediaAudio.TABLE_NAME, URI_MEDIA_AUDIO);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MediaAudioCodecVoLTE.TABLE_NAME, URI_MEDIA_AUDIO_CODEC_VOLTE);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MediaAudioCodecVT.TABLE_NAME, URI_MEDIA_AUDIO_CODEC_VT);
        // 2016-04-04, Emergency call, red.kim@
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.COM_EMERGENCY.TABLE_NAME, URI_COM_EMERGENCY);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.COM_APP_UC.TABLE_NAME, URI_COM_APP_UC);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.UC_APP.TABLE_NAME, URI_UC_APP);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.MEDIA.TABLE_NAME, URI_MEDIA);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.COM_MEDIA_CAPABILITIES.TABLE_NAME, URI_COM_MEDIA_CAPABILITIES);
        URI_MATCHER.addURI(ProviderInterface.AUTHORITY,
                ProviderInterface.IMS_MO.TABLE_NAME, URI_IMS_MO);


        PM_SUBSCRIBER = new HashMap<String, String>();
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.FEATURES,
                ProviderInterface.Subscriber.FEATURES);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF,
                ProviderInterface.Subscriber.PCSCF);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.SERVICES,
                ProviderInterface.Subscriber.SERVICES);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_0,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_0);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_0,
                ProviderInterface.Subscriber.PCSCF_PORT_0);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_1,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_1);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_1,
                ProviderInterface.Subscriber.PCSCF_PORT_1);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_2,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_2);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_2,
                ProviderInterface.Subscriber.PCSCF_PORT_2);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_3,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_3);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_3,
                ProviderInterface.Subscriber.PCSCF_PORT_3);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_4,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_4);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_4,
                ProviderInterface.Subscriber.PCSCF_PORT_4);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_5,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_5);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_5,
                ProviderInterface.Subscriber.PCSCF_PORT_5);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_6,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_6);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_6,
                ProviderInterface.Subscriber.PCSCF_PORT_6);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_7,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_7);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_7,
                ProviderInterface.Subscriber.PCSCF_PORT_7);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_8,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_8);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_8,
                ProviderInterface.Subscriber.PCSCF_PORT_8);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_ADDRESS_9,
                ProviderInterface.Subscriber.PCSCF_ADDRESS_9);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PCSCF_PORT_9,
                ProviderInterface.Subscriber.PCSCF_PORT_9);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.SCSCF_ADDRESS,
                ProviderInterface.Subscriber.SCSCF_ADDRESS);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.HOME_DOMAIN_NAME,
                ProviderInterface.Subscriber.HOME_DOMAIN_NAME);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPI,
                ProviderInterface.Subscriber.IMPI);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPU_VALIDATION,
                ProviderInterface.Subscriber.IMPU_VALIDATION);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPU_COUNT,
                ProviderInterface.Subscriber.IMPU_COUNT);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPU_0,
                ProviderInterface.Subscriber.IMPU_0);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPU_1,
                ProviderInterface.Subscriber.IMPU_1);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.IMPU_2,
                ProviderInterface.Subscriber.IMPU_2);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.PHONE_CONTEXT,
                ProviderInterface.Subscriber.PHONE_CONTEXT);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.AUTH_USERNAME,
                ProviderInterface.Subscriber.AUTH_USERNAME);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.AUTH_PASSWORD,
                ProviderInterface.Subscriber.AUTH_PASSWORD);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.AUTH_REALM,
                ProviderInterface.Subscriber.AUTH_REALM);
        PM_SUBSCRIBER.put(ProviderInterface.Subscriber.AUTH_ALGORITHM,
                ProviderInterface.Subscriber.AUTH_ALGORITHM);

        PM_ENGINE = new HashMap<String, String>();
        PM_ENGINE.put(ProviderInterface.Engine.TRACE_OPTION,
                ProviderInterface.Engine.TRACE_OPTION);
        PM_ENGINE.put(ProviderInterface.Engine.TRACE_MODULE,
                ProviderInterface.Engine.TRACE_MODULE);

        PM_SIP = new HashMap<String, String>();
        PM_SIP.put(ProviderInterface.SIP.REG_EXPIRATION,
                ProviderInterface.SIP.REG_EXPIRATION);
        PM_SIP.put(ProviderInterface.SIP.REG_SUBSCRIPTION,
                ProviderInterface.SIP.REG_SUBSCRIPTION);
        PM_SIP.put(ProviderInterface.SIP.REG_SUB_EXPIRATION,
                ProviderInterface.SIP.REG_SUB_EXPIRATION);

        PM_SERVICE_SIP = new HashMap<String, String>();
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_T1, ProviderInterface.SIP.TV_T1);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_T2, ProviderInterface.SIP.TV_T2);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_T4, ProviderInterface.SIP.TV_T4);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TA, ProviderInterface.SIP.TV_TA);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TB, ProviderInterface.SIP.TV_TB);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TC, ProviderInterface.SIP.TV_TC);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TD, ProviderInterface.SIP.TV_TD);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TE, ProviderInterface.SIP.TV_TE);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TF, ProviderInterface.SIP.TV_TF);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TG, ProviderInterface.SIP.TV_TG);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TH, ProviderInterface.SIP.TV_TH);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TI, ProviderInterface.SIP.TV_TI);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TJ, ProviderInterface.SIP.TV_TJ);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TV_TK, ProviderInterface.SIP.TV_TK);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.TARGET_SCHEME,
                ProviderInterface.SIP.TARGET_SCHEME);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.PREFERRED_ID,
                ProviderInterface.SIP.PREFERRED_ID);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.SERVICE_VERSION,
                ProviderInterface.SIP.SERVICE_VERSION);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.FEATURE_TAGS,
                ProviderInterface.SIP.FEATURE_TAGS);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.ST_REFRESHER,
                ProviderInterface.SIP.ST_REFRESHER);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.ST_METHOD,
                ProviderInterface.SIP.ST_METHOD);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.ST_MINSE,
                ProviderInterface.SIP.ST_MINSE);
        PM_SERVICE_SIP.put(ProviderInterface.SIP.ST_SESSION_EXPIRES,
                ProviderInterface.SIP.ST_SESSION_EXPIRES);

        PM_AOS = new HashMap<String, String>();
        PM_AOS.put(ProviderInterface.AOS.CONF, ProviderInterface.AOS.CONF);

        PM_AOS_APPLICATION = new HashMap<String, String>();
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.UNIQUENESS,
                ProviderInterface.AoSApplication.UNIQUENESS);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_0,
                ProviderInterface.AoSApplication.ID_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_1,
                ProviderInterface.AoSApplication.ID_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_2,
                ProviderInterface.AoSApplication.ID_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_3,
                ProviderInterface.AoSApplication.ID_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_4,
                ProviderInterface.AoSApplication.ID_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.ID_5,
                ProviderInterface.AoSApplication.ID_5);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_0,
                ProviderInterface.AoSApplication.FEATURES_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_1,
                ProviderInterface.AoSApplication.FEATURES_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_2,
                ProviderInterface.AoSApplication.FEATURES_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_3,
                ProviderInterface.AoSApplication.FEATURES_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_4,
                ProviderInterface.AoSApplication.FEATURES_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.FEATURES_5,
                ProviderInterface.AoSApplication.FEATURES_5);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_0,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_1,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_2,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_3,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_4,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.STATE_START_INTERVAL_5,
                ProviderInterface.AoSApplication.STATE_START_INTERVAL_5);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_0,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_1,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_2,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_3,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_4,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_5,
                ProviderInterface.AoSApplication.RECONFIG_GUARD_INTERVAL_5);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_0,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_1,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_2,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_3,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_4,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_5,
                ProviderInterface.AoSApplication.REG_STOP_WAITING_INTERVAL_5);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_0,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_0);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_1,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_1);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_2,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_2);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_3,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_3);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_4,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_4);
        PM_AOS_APPLICATION.put(ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_5,
                ProviderInterface.AoSApplication.REG_TERMINATED_TRYING_INTERVAL_5);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_0,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_0);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_1,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_1);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_2,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_2);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_3,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_3);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_4,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_4);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_5,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_COUNT_5);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_0,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_0);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_1,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_1);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_2,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_2);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_3,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_3);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_4,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_4);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_5,
                ProviderInterface.AoSApplication.UNEXPECTED_ERROR_REG_TRYING_MAX_INTERVAL_5);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_0,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_0);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_1,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_1);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_2,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_2);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_3,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_3);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_4,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_4);
        PM_AOS_APPLICATION.put(
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_5,
                ProviderInterface.AoSApplication.PDN_RELEASE_DELAY_ON_TERMINATION_5);

        PM_AOS_CONDITION = new HashMap<String, String>();
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_0,
                ProviderInterface.AoSCondition.ID_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_1,
                ProviderInterface.AoSCondition.ID_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_2,
                ProviderInterface.AoSCondition.ID_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_3,
                ProviderInterface.AoSCondition.ID_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_4,
                ProviderInterface.AoSCondition.ID_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ID_5,
                ProviderInterface.AoSCondition.ID_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_0,
                ProviderInterface.AoSCondition.FEATURES_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_1,
                ProviderInterface.AoSCondition.FEATURES_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_2,
                ProviderInterface.AoSCondition.FEATURES_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_3,
                ProviderInterface.AoSCondition.FEATURES_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_4,
                ProviderInterface.AoSCondition.FEATURES_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.FEATURES_5,
                ProviderInterface.AoSCondition.FEATURES_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_0,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_1,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_2,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_3,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_4,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_5,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_IMPU_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_0,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_1,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_2,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_3,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_4,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_5,
                ProviderInterface.AoSCondition.ISIM_INDEX_FOR_PCSCF_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_0,
                ProviderInterface.AoSCondition.PCSCF_PORT_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_1,
                ProviderInterface.AoSCondition.PCSCF_PORT_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_2,
                ProviderInterface.AoSCondition.PCSCF_PORT_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_3,
                ProviderInterface.AoSCondition.PCSCF_PORT_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_4,
                ProviderInterface.AoSCondition.PCSCF_PORT_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_PORT_5,
                ProviderInterface.AoSCondition.PCSCF_PORT_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_0,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_1,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_2,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_3,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_4,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_5,
                ProviderInterface.AoSCondition.MULTIPLE_DISCOVERY_SCHEME_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_0,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_1,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_2,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_3,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_4,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_5,
                ProviderInterface.AoSCondition.PCSCF_CHANGE_CONTROL_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_0,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_1,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_2,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_3,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_4,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_5,
                ProviderInterface.AoSCondition.NETWORK_SUSPEND_CONDITION_5);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_0,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_0);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_1,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_1);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_2,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_2);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_3,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_3);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_4,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_4);
        PM_AOS_CONDITION.put(ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_5,
                ProviderInterface.AoSCondition.SERVICE_COUNCIL_BLOCK_REASONS_5);

        PM_AOS_CONNECTION = new HashMap<String, String>();
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_0,
                ProviderInterface.AoSConnection.PROFILE_NAME_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_1,
                ProviderInterface.AoSConnection.PROFILE_NAME_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_2,
                ProviderInterface.AoSConnection.PROFILE_NAME_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_3,
                ProviderInterface.AoSConnection.PROFILE_NAME_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_4,
                ProviderInterface.AoSConnection.PROFILE_NAME_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PROFILE_NAME_5,
                ProviderInterface.AoSConnection.PROFILE_NAME_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_0,
                ProviderInterface.AoSConnection.ACCESS_POLICY_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_1,
                ProviderInterface.AoSConnection.ACCESS_POLICY_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_2,
                ProviderInterface.AoSConnection.ACCESS_POLICY_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_3,
                ProviderInterface.AoSConnection.ACCESS_POLICY_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_4,
                ProviderInterface.AoSConnection.ACCESS_POLICY_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.ACCESS_POLICY_5,
                ProviderInterface.AoSConnection.ACCESS_POLICY_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_0,
                ProviderInterface.AoSConnection.IP_VERSION_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_1,
                ProviderInterface.AoSConnection.IP_VERSION_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_2,
                ProviderInterface.AoSConnection.IP_VERSION_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_3,
                ProviderInterface.AoSConnection.IP_VERSION_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_4,
                ProviderInterface.AoSConnection.IP_VERSION_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IP_VERSION_5,
                ProviderInterface.AoSConnection.IP_VERSION_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_0,
                ProviderInterface.AoSConnection.FEATURES_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_1,
                ProviderInterface.AoSConnection.FEATURES_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_2,
                ProviderInterface.AoSConnection.FEATURES_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_3,
                ProviderInterface.AoSConnection.FEATURES_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_4,
                ProviderInterface.AoSConnection.FEATURES_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.FEATURES_5,
                ProviderInterface.AoSConnection.FEATURES_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_0,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_1,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_2,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_3,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_4,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_5,
                ProviderInterface.AoSConnection.IPV6_DELAY_INTERVAL_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_0,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_1,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_2,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_3,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_4,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_5,
                ProviderInterface.AoSConnection.RAT_GUARD_INTERVAL_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_0,
                ProviderInterface.AoSConnection.EPDG_SCHEME_0);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_1,
                ProviderInterface.AoSConnection.EPDG_SCHEME_1);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_2,
                ProviderInterface.AoSConnection.EPDG_SCHEME_2);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_3,
                ProviderInterface.AoSConnection.EPDG_SCHEME_3);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_4,
                ProviderInterface.AoSConnection.EPDG_SCHEME_4);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.EPDG_SCHEME_5,
                ProviderInterface.AoSConnection.EPDG_SCHEME_5);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.MPDN,
                ProviderInterface.AoSConnection.MPDN);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.PS_SUPPORTED,
                ProviderInterface.AoSConnection.PS_SUPPORTED);
        PM_AOS_CONNECTION.put(ProviderInterface.AoSConnection.DCN_SERVICE_IN_TIME,
                ProviderInterface.AoSConnection.DCN_SERVICE_IN_TIME);

        PM_AOS_HANDLE = new HashMap<String, String>();
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_0, ProviderInterface.AoSHandle.ID_0);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_1, ProviderInterface.AoSHandle.ID_1);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_2, ProviderInterface.AoSHandle.ID_2);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_3, ProviderInterface.AoSHandle.ID_3);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_4, ProviderInterface.AoSHandle.ID_4);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.ID_5, ProviderInterface.AoSHandle.ID_5);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_0,
                ProviderInterface.AoSHandle.FEATURES_0);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_1,
                ProviderInterface.AoSHandle.FEATURES_1);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_2,
                ProviderInterface.AoSHandle.FEATURES_2);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_3,
                ProviderInterface.AoSHandle.FEATURES_3);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_4,
                ProviderInterface.AoSHandle.FEATURES_4);
        PM_AOS_HANDLE.put(ProviderInterface.AoSHandle.FEATURES_5,
                ProviderInterface.AoSHandle.FEATURES_5);

        PM_AOS_PROVIDER = new HashMap<String, String>();
        PM_AOS_PROVIDER.put(ProviderInterface.AoSProvider.COMMON_FEATURES,
                ProviderInterface.AoSProvider.COMMON_FEATURES);

        PM_AOS_REG = new HashMap<String, String>();
        PM_AOS_REG.put(ProviderInterface.AoSReg.GBA, ProviderInterface.AoSReg.GBA);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_INTERVAL_0,
                ProviderInterface.AoSReg.RETRY_INTERVAL_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_INTERVAL_1,
                ProviderInterface.AoSReg.RETRY_INTERVAL_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_INTERVAL_2,
                ProviderInterface.AoSReg.RETRY_INTERVAL_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_INTERVAL_3,
                ProviderInterface.AoSReg.RETRY_INTERVAL_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_0,
                ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_1,
                ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_2,
                ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_3,
                ProviderInterface.AoSReg.RETRY_REPEAT_INTERVAL_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_0, ProviderInterface.AoSReg.IPSEC_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_1, ProviderInterface.AoSReg.IPSEC_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_2, ProviderInterface.AoSReg.IPSEC_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_3, ProviderInterface.AoSReg.IPSEC_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_SPI_3GPP_0,
                ProviderInterface.AoSReg.IPSEC_SPI_3GPP_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_SPI_3GPP_1,
                ProviderInterface.AoSReg.IPSEC_SPI_3GPP_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_SPI_3GPP_2,
                ProviderInterface.AoSReg.IPSEC_SPI_3GPP_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_SPI_3GPP_3,
                ProviderInterface.AoSReg.IPSEC_SPI_3GPP_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_0_ALGS,
                ProviderInterface.AoSReg.IPSEC_0_ALGS);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_1_ALGS,
                ProviderInterface.AoSReg.IPSEC_1_ALGS);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_2_ALGS,
                ProviderInterface.AoSReg.IPSEC_2_ALGS);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_3_ALGS,
                ProviderInterface.AoSReg.IPSEC_3_ALGS);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_BASE_TIME_0,
                ProviderInterface.AoSReg.RETRY_BASE_TIME_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_BASE_TIME_1,
                ProviderInterface.AoSReg.RETRY_BASE_TIME_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_BASE_TIME_2,
                ProviderInterface.AoSReg.RETRY_BASE_TIME_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_BASE_TIME_3,
                ProviderInterface.AoSReg.RETRY_BASE_TIME_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_MAX_TIME_0,
                ProviderInterface.AoSReg.RETRY_MAX_TIME_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_MAX_TIME_1,
                ProviderInterface.AoSReg.RETRY_MAX_TIME_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_MAX_TIME_2,
                ProviderInterface.AoSReg.RETRY_MAX_TIME_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_MAX_TIME_3,
                ProviderInterface.AoSReg.RETRY_MAX_TIME_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.FEATURES_0, ProviderInterface.AoSReg.FEATURES_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.FEATURES_1, ProviderInterface.AoSReg.FEATURES_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.FEATURES_2, ProviderInterface.AoSReg.FEATURES_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.FEATURES_3, ProviderInterface.AoSReg.FEATURES_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_0,
                ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_1,
                ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_2,
                ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_3,
                ProviderInterface.AoSReg.NORMAL_REG_FLOW_ID_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_0,
                ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_1,
                ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_2,
                ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_3,
                ProviderInterface.AoSReg.AUTHENTICATION_MAX_COUNT_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_0,
                ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_1,
                ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_2,
                ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_3,
                ProviderInterface.AoSReg.REG_DEFAULT_RETRY_AFTER_INTERVAL_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SCHEME_0, ProviderInterface.AoSReg.SCHEME_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SCHEME_1, ProviderInterface.AoSReg.SCHEME_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SCHEME_2, ProviderInterface.AoSReg.SCHEME_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SCHEME_3, ProviderInterface.AoSReg.SCHEME_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_0,
                ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_1,
                ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_2,
                ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_3,
                ProviderInterface.AoSReg.SUB_DEFAULT_RETRY_AFTER_INTERVAL_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_0,
                ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_1,
                ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_2,
                ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_3,
                ProviderInterface.AoSReg.IPSEC_GUARD_LIFE_TIME_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_0,
                ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_1,
                ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_2,
                ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_3,
                ProviderInterface.AoSReg.IPSEC_PORT_INTERVAL_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_0,
                ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_1,
                ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_2,
                ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_3,
                ProviderInterface.AoSReg.REGINFO_CONTACT_MATCH_RULE_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_0,
                ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_1,
                ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_2,
                ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_3,
                ProviderInterface.AoSReg.RETRY_PCSCF_COUNT_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_0,
                ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_1,
                ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_2,
                ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_3,
                ProviderInterface.AoSReg.RETRY_RESPONSE_CODES_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.BLOCK_CONDITIONS_0,
                ProviderInterface.AoSReg.BLOCK_CONDITIONS_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.BLOCK_CONDITIONS_1,
                ProviderInterface.AoSReg.BLOCK_CONDITIONS_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.BLOCK_CONDITIONS_2,
                ProviderInterface.AoSReg.BLOCK_CONDITIONS_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.BLOCK_CONDITIONS_3,
                ProviderInterface.AoSReg.BLOCK_CONDITIONS_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.USERINFO_POLICY_0,
                ProviderInterface.AoSReg.USERINFO_POLICY_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.USERINFO_POLICY_1,
                ProviderInterface.AoSReg.USERINFO_POLICY_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.USERINFO_POLICY_2,
                ProviderInterface.AoSReg.USERINFO_POLICY_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.USERINFO_POLICY_3,
                ProviderInterface.AoSReg.USERINFO_POLICY_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_0,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_1,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_2,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_3,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_OPTION_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_0,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_1,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_2,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_3,
                ProviderInterface.AoSReg.TCP_KEEPALIVE_VALUE_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_0,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_1,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_2,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_3,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV4_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_0,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_1,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_2,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_3,
                ProviderInterface.AoSReg.TCP_CRITERION_LENGTH_IPV6_3);
        PM_AOS_REG.put(ProviderInterface.AoSReg.DSCP_VALUE_0,
                ProviderInterface.AoSReg.DSCP_VALUE_0);
        PM_AOS_REG.put(ProviderInterface.AoSReg.DSCP_VALUE_1,
                ProviderInterface.AoSReg.DSCP_VALUE_1);
        PM_AOS_REG.put(ProviderInterface.AoSReg.DSCP_VALUE_2,
                ProviderInterface.AoSReg.DSCP_VALUE_2);
        PM_AOS_REG.put(ProviderInterface.AoSReg.DSCP_VALUE_3,
                ProviderInterface.AoSReg.DSCP_VALUE_3);

        PM_SERVICE_MEDIA = new HashMap<String, String>();
        PM_SERVICE_MEDIA.put(ProviderInterface.Media.LOOPBACK,
                ProviderInterface.Media.LOOPBACK);
        PM_SERVICE_MEDIA.put(ProviderInterface.Media.SESSION_LEVEL_BW,
                ProviderInterface.Media.SESSION_LEVEL_BW);
        PM_SERVICE_MEDIA.put(ProviderInterface.Media.RTP_ANALYZER,
                ProviderInterface.Media.RTP_ANALYZER);

        PM_SERVICE_VT = new HashMap<String, String>();
        PM_SERVICE_VT.put(ProviderInterface.Media.LOOPBACK, ProviderInterface.Media.LOOPBACK);

        PM_SERVICE_SMS = new HashMap<String, String>(); //KDDI VZW
        PM_SERVICE_SMS.put(ProviderInterface.SMS.SMS_FORMAT, ProviderInterface.SMS.SMS_FORMAT);
        PM_SERVICE_SMS.put(ProviderInterface.SMS.SMS_OVER_IP_NETWORK,
                ProviderInterface.SMS.SMS_OVER_IP_NETWORK);
        PM_SERVICE_SMS.put(ProviderInterface.SMS.PSI, ProviderInterface.SMS.PSI);

        PM_SERVICE_EAB = new HashMap<String, String>();
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FLAG,
                ProviderInterface.EAB.EAB_SERVICE_FLAG);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_STATUS,
                ProviderInterface.EAB.EAB_SERVICE_STATUS);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_ALLOW_VT_REGRADLESS_OF_EAB,
                ProviderInterface.EAB.EAB_ALLOW_VT_REGRADLESS_OF_EAB);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_VIDEO_CAPABILITY_SHARING,
                ProviderInterface.EAB.EAB_VIDEO_CAPABILITY_SHARING);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_PUBLISH_489_RETRY_TIMER,
                ProviderInterface.EAB.EAB_PUBLISH_489_RETRY_TIMER);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_PUBLISH_ERROR_RETRY_TIMER,
                ProviderInterface.EAB.EAB_PUBLISH_ERROR_RETRY_TIMER);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_PUBLISH_REFRESH_RATIO,
                ProviderInterface.EAB.EAB_PUBLISH_REFRESH_RATIO);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SUBSCRIBE_PER_SECOND,
                ProviderInterface.EAB.EAB_SUBSCRIBE_PER_SECOND);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_POLLING_PERIOD,
                ProviderInterface.EAB.EAB_POLLING_PERIOD);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_POLLING_RATE,
                ProviderInterface.EAB.EAB_POLLING_RATE);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_POLLING_RATE_PERIOD,
                ProviderInterface.EAB.EAB_POLLING_RATE_PERIOD);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_MAX_NUMBER_OFSUBSCRIPTIONS_INPRESENCE_LIST,
                ProviderInterface.EAB.EAB_MAX_NUMBER_OFSUBSCRIPTIONS_INPRESENCE_LIST);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_CAPABILITY_INFO_EXPIRY,
                ProviderInterface.EAB.EAB_CAPABILITY_INFO_EXPIRY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_NON_RCS_CAPABILITY_INFO_EXPIRY,
                ProviderInterface.EAB.EAB_NON_RCS_CAPABILITY_INFO_EXPIRY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_AVAILABILITY_INFO_EXPIRY,
                ProviderInterface.EAB.EAB_AVAILABILITY_INFO_EXPIRY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_POLLING_PERIOD_RANDOMIZATION,
                ProviderInterface.EAB.EAB_POLLING_PERIOD_RANDOMIZATION);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_POLLING_PERIOD_INITIALIZATION,
                ProviderInterface.EAB.EAB_POLLING_PERIOD_INITIALIZATION);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_ALWAYS_AVAILABILITY_FETCH,
                ProviderInterface.EAB.EAB_ALWAYS_AVAILABILITY_FETCH);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_USE_EAB_VALUE_FROM_AC,
                ProviderInterface.EAB.EAB_USE_EAB_VALUE_FROM_AC);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_DISABLE_INITIAL_ADDRESS_BOOK_SCAN,
                ProviderInterface.EAB.EAB_DISABLE_INITIAL_ADDRESS_BOOK_SCAN);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SUBSCIRBE_INDEPENDENT_OF_PUBLISH,
                ProviderInterface.EAB.EAB_SUBSCIRBE_INDEPENDENT_OF_PUBLISH);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_CHECK_VALID_NUMBER,
                ProviderInterface.EAB.EAB_CHECK_VALID_NUMBER);
        PM_SERVICE_EAB.put(
                ProviderInterface.EAB.EAB_SERVICE_CAPABILITY_DISCOVERY_VIA_PRESENCE_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CAPABILITY_DISCOVERY_VIA_PRESENCE_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_IP_CALL_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_IP_CALL_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_STANDALONE_MESSAGING_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_STANDALONE_MESSAGING_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_CHAT_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CHAT_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_IM_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_IM_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_GROUP_CHAT_FULL_STAND_FWD_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_GROUP_CHAT_FULL_STAND_FWD_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_THUMBNAIL_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_THUMBNAIL_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_STORE_FORWARD_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_STORE_FORWARD_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_HTTP_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_HTTP_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_GEOLOCATION_PUSH_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_GEOLOCATION_PUSH_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_EXPIRE_VALUE_IN_PUBLISH,
                ProviderInterface.EAB.EAB_EXPIRE_VALUE_IN_PUBLISH);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_EXTENDED_EXPIRE_VALUE_IN_PUBLISH,
                ProviderInterface.EAB.EAB_EXTENDED_EXPIRE_VALUE_IN_PUBLISH);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_THROTTLE_TIME_IN_PUBLISH,
                ProviderInterface.EAB.EAB_THROTTLE_TIME_IN_PUBLISH);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_EXPIRE_VALUE_IN_LIST_SUBSCRIBE,
                ProviderInterface.EAB.EAB_EXPIRE_VALUE_IN_LIST_SUBSCRIBE);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SUPPORT_GZIP,
                ProviderInterface.EAB.EAB_SUPPORT_GZIP);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_CAPABILITY_DISCOVERY_ALLOWED_PREFIXES,
                ProviderInterface.EAB.EAB_CAPABILITY_DISCOVERY_ALLOWED_PREFIXES);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_SHARED_MAP_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_SHARED_MAP_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_SHARED_SKETCH_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_SHARED_SKETCH_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_CALL_COMPOSER_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CALL_COMPOSER_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_POST_CALL_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_POST_CALL_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_CHATBOT_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CHATBOT_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_CHATBOT_STANDALONE_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CHATBOT_STANDALONE_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_CHATBOT_EXTEND_MSG_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_CHATBOT_EXTEND_MSG_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_GEOLOCATION_PUSH_VIA_SMS_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_GEOLOCATION_PUSH_VIA_SMS_POLICY);
        PM_SERVICE_EAB.put(ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_VIA_SMS_POLICY,
                ProviderInterface.EAB.EAB_SERVICE_FILE_TRANSFER_VIA_SMS_POLICY);

        PM_SERVICE_MMTEL = new HashMap<String, String>();
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_PDN,
                ProviderInterface.MMTel.MMTEL_PDN);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_ADDRESS_TYPE,
                ProviderInterface.MMTel.MMTEL_ADDRESS_TYPE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_TLS,
                ProviderInterface.MMTel.MMTEL_TLS);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SERVER_IP,
                ProviderInterface.MMTel.MMTEL_SERVER_IP);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SERVER_PORT,
                ProviderInterface.MMTel.MMTEL_SERVER_PORT);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_PROVISIONING,
                ProviderInterface.MMTel.MMTEL_PROVISIONING);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SERVICE_VERSION,
                ProviderInterface.MMTel.MMTEL_SERVICE_VERSION);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_CONTROL_PREFERENCE,
                ProviderInterface.MMTel.MMTEL_CONTROL_PREFERENCE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_CHECK_REG,
                ProviderInterface.MMTel.MMTEL_CHECK_REG);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_AUTH_TYPE,
                ProviderInterface.MMTel.MMTEL_AUTH_TYPE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_AUTH_USERNAME,
                ProviderInterface.MMTel.MMTEL_AUTH_USERNAME);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_AUTH_PASSWORD,
                ProviderInterface.MMTel.MMTEL_AUTH_PASSWORD);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_NAMESPACE,
                ProviderInterface.MMTel.MMTEL_NAMESPACE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_TARGET,
                ProviderInterface.MMTel.MMTEL_TARGET);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SOFTINDIA_WIFI_TEST,
                ProviderInterface.MMTel.MMTEL_SOFTINDIA_WIFI_TEST);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_XUI_PRIORITY,
                ProviderInterface.MMTel.MMTEL_XUI_PRIORITY);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_AUTO_CS_RETRY,
                ProviderInterface.MMTel.MMTEL_AUTO_CS_RETRY);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_RULEID_FROM_RESPONSE,
                ProviderInterface.MMTel.MMTEL_RULEID_FROM_RESPONSE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SRV_RECORD,
                ProviderInterface.MMTel.MMTEL_SRV_RECORD);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_USSD_OVER_IMS,
                ProviderInterface.MMTel.MMTEL_USSD_OVER_IMS);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_DOMAIN_SETTING,
                ProviderInterface.MMTel.MMTEL_DOMAIN_SETTING);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SUPPORT_ERASURE,
                ProviderInterface.MMTel.MMTEL_SUPPORT_ERASURE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_ACCESS_POLICY,
                ProviderInterface.MMTel.MMTEL_ACCESS_POLICY);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_SUPPORT_SERVICE,
                ProviderInterface.MMTel.MMTEL_SUPPORT_SERVICE);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_PROVISIONING_VOWIFI,
                ProviderInterface.MMTel.MMTEL_PROVISIONING_VOWIFI);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_URL_QUERYPREFIX,
                ProviderInterface.MMTel.MMTEL_URL_QUERYPREFIX);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_QUERY_ON_BOOTUP,
                ProviderInterface.MMTel.MMTEL_QUERY_ON_BOOTUP);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_TRUSTALLHOSTS,
                ProviderInterface.MMTel.MMTEL_TRUSTALLHOSTS);
        PM_SERVICE_MMTEL.put(ProviderInterface.MMTel.MMTEL_URLADDR_INCLUDE_URLPORT,
                ProviderInterface.MMTel.MMTEL_URLADDR_INCLUDE_URLPORT);

        PM_SERVICE_MMTEL_CF = new HashMap<String, String>();
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_SUPPORT_CF_RULES,
                ProviderInterface.MMTel_CF.MMTEL_SUPPORT_CF_RULES);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_TARGET_NUMBER_PREFIX,
                ProviderInterface.MMTel_CF.MMTEL_CF_TARGET_NUMBER_PREFIX);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_GET_REQUEST_EACH_RULE,
                ProviderInterface.MMTel_CF.MMTEL_CF_GET_REQUEST_EACH_RULE);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_UNCONDITIONAL,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_UNCONDITIONAL);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_BUSY,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_BUSY);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NO_REPLY,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NO_REPLY);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_REACHABLE,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_REACHABLE);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_LOGGED_IN,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_LOGGED_IN);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_UNCONDITIONAL_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_UNCONDITIONAL_VIDEO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_BUSY_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_BUSY_VIDEO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NO_REPLY_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NO_REPLY_VIDEO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_REACHABLE_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_REACHABLE_VIDEO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_LOGGED_IN_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_RULE_ID_NOT_LOGGED_IN_VIDEO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_TIMER_IN_NO_ANSWER,
                ProviderInterface.MMTel_CF.MMTEL_TIMER_IN_NO_ANSWER);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_MEDIA_AUDIO,
                ProviderInterface.MMTel_CF.MMTEL_CF_MEDIA_AUDIO);
        PM_SERVICE_MMTEL_CF.put(ProviderInterface.MMTel_CF.MMTEL_CF_MEDIA_VIDEO,
                ProviderInterface.MMTel_CF.MMTEL_CF_MEDIA_VIDEO);

        PM_SERVICE_MMTEL_CB = new HashMap<String, String>();
        PM_SERVICE_MMTEL_CB.put(ProviderInterface.MMTel_CB.MMTEL_SUPPORT_CB_RULES,
                ProviderInterface.MMTel_CB.MMTEL_SUPPORT_CB_RULES);
        PM_SERVICE_MMTEL_CB.put(ProviderInterface.MMTel_CB.MMTEL_CB_MEDIA_AUDIO,
                ProviderInterface.MMTel_CB.MMTEL_CB_MEDIA_AUDIO);
        PM_SERVICE_MMTEL_CB.put(ProviderInterface.MMTel_CB.MMTEL_CB_MEDIA_VIDEO,
                ProviderInterface.MMTel_CB.MMTEL_CB_MEDIA_VIDEO);
        PM_SERVICE_MMTEL_CB.put(ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_INCOMING_CALL,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_INCOMING_CALL);
        PM_SERVICE_MMTEL_CB.put(ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.
                    MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY,
                ProviderInterface.MMTel_CB.
                    MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_INCOMING_CALL_VIDEO,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_INCOMING_CALL_VIDEO);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL_VIDEO,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_ALL_OUTGOING_CALL_VIDEO);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_VIDEO,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_VIDEO);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.
                    MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY_VIDEO,
                ProviderInterface.MMTel_CB.
                    MMTEL_CB_RULE_ID_OUTGOING_INTERNATIONAL_CALL_EX_HOME_COUNTRY_VIDEO);
        PM_SERVICE_MMTEL_CB.put(
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING_VIDEO,
                ProviderInterface.MMTel_CB.MMTEL_CB_RULE_ID_INCOMING_CALL_WHEN_ROAMING_VIDEO);

        PM_SERVICE_GBA = new HashMap<String, String>();
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_ENABLED,
                ProviderInterface.GBA.GBA_ENABLED);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_TYPE,
                ProviderInterface.GBA.GBA_TYPE);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_REQUEST_URI,
                ProviderInterface.GBA.GBA_REQUEST_URI);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_HOST,
                ProviderInterface.GBA.GBA_HOST);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_USERNAME,
                ProviderInterface.GBA.GBA_USERNAME);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_REALM,
                ProviderInterface.GBA.GBA_REALM);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_BSF_DNS_NAME,
                ProviderInterface.GBA.GBA_BSF_DNS_NAME);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_PDN,
                ProviderInterface.GBA.GBA_PDN);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_BSF_PORT,
                ProviderInterface.GBA.GBA_BSF_PORT);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_SCHEMA,
                ProviderInterface.GBA.GBA_SCHEMA);
        PM_SERVICE_GBA.put(ProviderInterface.GBA.GBA_BSF_SELECTION,
                ProviderInterface.GBA.GBA_BSF_SELECTION);

        PM_RESOURCE = new HashMap<String, String>();
        PM_RESOURCE.put(ProviderInterface.Resource.VOLTE_ICON_INDICATOR_ONOFF,
                ProviderInterface.Resource.VOLTE_ICON_INDICATOR_ONOFF);
        PM_RESOURCE.put(ProviderInterface.Resource.VOLTE_ICON_INDICATOR_RESOURCE,
                ProviderInterface.Resource.VOLTE_ICON_INDICATOR_RESOURCE);
        PM_RESOURCE.put(ProviderInterface.Resource.VOWIFI_ICON_INDICATOR_ONOFF,
                ProviderInterface.Resource.VOWIFI_ICON_INDICATOR_ONOFF);
        PM_RESOURCE.put(ProviderInterface.Resource.VOWIFI_ICON_INDICATOR_RESOURCE,
                ProviderInterface.Resource.VOWIFI_ICON_INDICATOR_RESOURCE);

        PM_FEATURE = new HashMap<String, String>();
        PM_FEATURE.put(ProviderInterface.FEATURE.FEATURE_IPSEC,
                ProviderInterface.FEATURE.FEATURE_IPSEC);
        PM_FEATURE.put(ProviderInterface.FEATURE.FEATURE_SMS,
                ProviderInterface.FEATURE.FEATURE_SMS);
        PM_FEATURE.put(ProviderInterface.FEATURE.FEATURE_UCE,
                ProviderInterface.FEATURE.FEATURE_UCE);

        PM_SETTING = new HashMap<String, String>();
        PM_SETTING.put(ProviderInterface.Setting.VIDEO_SETTING,
                ProviderInterface.Setting.VIDEO_SETTING);
        PM_SETTING.put(ProviderInterface.Setting.VOIP_SETTING,
                ProviderInterface.Setting.VOIP_SETTING);
        PM_SETTING.put(ProviderInterface.Setting.MOBILE_DATA_SETTING,
                ProviderInterface.Setting.MOBILE_DATA_SETTING);
        PM_SETTING.put(ProviderInterface.Setting.IRAT_TRANSITION_TIME,
                ProviderInterface.Setting.IRAT_TRANSITION_TIME);
        PM_SETTING.put(ProviderInterface.Setting.TEST_MASK,
                ProviderInterface.Setting.TEST_MASK);
        PM_SETTING.put(ProviderInterface.Setting.DEVICE_MODE,
                ProviderInterface.Setting.DEVICE_MODE);
        PM_SETTING.put(ProviderInterface.Setting.MULTIDEVICE_ENABLE,
                ProviderInterface.Setting.MULTIDEVICE_ENABLE);
        PM_SETTING.put(ProviderInterface.Setting.VOWIFI_ENABLE,
                ProviderInterface.Setting.VOWIFI_ENABLE);
        PM_SETTING.put(ProviderInterface.Setting.IMS_APN_NAME,
                ProviderInterface.Setting.IMS_APN_NAME);
        PM_SETTING.put(ProviderInterface.Setting.ACTIVE_CALL_HO,
                ProviderInterface.Setting.ACTIVE_CALL_HO);

        PM_UC_APP = new HashMap<String, String>();
        PM_UC_APP.put(ProviderInterface.UC_APP.CNAP_TYPE,
                ProviderInterface.UC_APP.CNAP_TYPE);
        PM_UC_APP.put(ProviderInterface.UC_APP.LOCALNUMBER_TYPE,
                ProviderInterface.UC_APP.LOCALNUMBER_TYPE);

        PM_SESSION_VOIP = new HashMap<String, String>();
        PM_SESSION_VOIP.put(ProviderInterface.SessionVoIP.CONF_URI,
                ProviderInterface.SessionVoIP.CONF_URI);
        PM_SESSION_VOIP.put(ProviderInterface.SessionVoIP.LOCAL_FEATURE,
                ProviderInterface.SessionVoIP.LOCAL_FEATURE);
        PM_SESSION_VOIP.put(ProviderInterface.SessionVoIP.TIME_ACTIVE_MEDIA_TH,
                ProviderInterface.SessionVoIP.TIME_ACTIVE_MEDIA_TH);
        PM_SESSION_VOIP.put(ProviderInterface.SessionVoIP.TIME_HOLD_MEDIA_TH,
                ProviderInterface.SessionVoIP.TIME_HOLD_MEDIA_TH);

        PM_SESSION_VT = new HashMap<String, String>();
        PM_SESSION_VT.put(ProviderInterface.SessionVt.CONF_URI,
                ProviderInterface.SessionVt.CONF_URI);
        PM_SESSION_VT.put(ProviderInterface.SessionVt.LOCAL_FEATURE,
                ProviderInterface.SessionVt.LOCAL_FEATURE);
        PM_SESSION_VT.put(ProviderInterface.SessionVt.TIME_ACTIVE_MEDIA_TH,
                ProviderInterface.SessionVt.TIME_ACTIVE_MEDIA_TH);
        PM_SESSION_VT.put(ProviderInterface.SessionVt.TIME_HOLD_MEDIA_TH,
                ProviderInterface.SessionVt.TIME_HOLD_MEDIA_TH);

        PM_SESSION_COMMON = new HashMap<String, String>();
        PM_SESSION_COMMON.put(ProviderInterface.SessionCommon.TIMER_MO_NOANSWER,
                ProviderInterface.SessionCommon.TIMER_MO_NOANSWER);
        PM_SESSION_COMMON.put(ProviderInterface.SessionCommon.TIMER_MT_ALERTING,
                ProviderInterface.SessionCommon.TIMER_MT_ALERTING);

        PM_UC_EMERGENCY = new HashMap<String, String>();
        PM_UC_EMERGENCY.put(ProviderInterface.UCEmergency.USINGEPDN_LTE,
                ProviderInterface.UCEmergency.USINGEPDN_LTE);
        PM_UC_EMERGENCY.put(ProviderInterface.UCEmergency.USINGEPDN_WIFI,
                ProviderInterface.UCEmergency.USINGEPDN_WIFI);

        PM_UC_CONFERENCE = new HashMap<String, String>();
        PM_UC_CONFERENCE.put(ProviderInterface.UCConference.CONF_FEATURES,
                ProviderInterface.UCConference.CONF_FEATURES);

        PM_SESSION_VZW = new HashMap<String, String>();
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.SILENT_REDIAL,
                ProviderInterface.SessionVzw.SILENT_REDIAL);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.TIMER_VZW,
                ProviderInterface.SessionVzw.TIMER_VZW);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.TIMER_LTE911_FAIL,
                ProviderInterface.SessionVzw.TIMER_LTE911_FAIL);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.CONF_SUBS,
                ProviderInterface.SessionVzw.CONF_SUBS);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.CONF_RES_LIST,
                ProviderInterface.SessionVzw.CONF_RES_LIST);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.VICE_SYNC,
                ProviderInterface.SessionVzw.VICE_SYNC);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.VICE_PULL,
                ProviderInterface.SessionVzw.VICE_PULL);
        PM_SESSION_VZW.put(ProviderInterface.SessionVzw.CANID,
                ProviderInterface.SessionVzw.CANID);

        PM_COM_EMERGENCY = new HashMap<String, String>();
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.SUPPORT_OVER_LTE,
                ProviderInterface.COM_EMERGENCY.SUPPORT_OVER_LTE);
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.SUPPORT_OVER_WIFI,
                ProviderInterface.COM_EMERGENCY.SUPPORT_OVER_WIFI);
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.CONTROL_BY_VOLTE_SETTING,
                ProviderInterface.COM_EMERGENCY.CONTROL_BY_VOLTE_SETTING);
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.CONTROL_BY_IMS_REG,
                ProviderInterface.COM_EMERGENCY.CONTROL_BY_IMS_REG);
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.REQUIRE_NORMAL_CALL_END,
                ProviderInterface.COM_EMERGENCY.REQUIRE_NORMAL_CALL_END);
        PM_COM_EMERGENCY.put(ProviderInterface.COM_EMERGENCY.WIFI_ECALL_BLOCK_FEATURE,
                ProviderInterface.COM_EMERGENCY.WIFI_ECALL_BLOCK_FEATURE);

        PM_COM_APP_UC = new HashMap<String, String>();
        PM_COM_APP_UC.put(ProviderInterface.COM_APP_UC.CONF, ProviderInterface.COM_APP_UC.CONF);

        PM_MEDIA = new HashMap<String, String>();
        PM_MEDIA.put(ProviderInterface.MEDIA.CONF, ProviderInterface.MEDIA.CONF);

        PM_COM_MEDIA_CAPABILITIES = new HashMap<String, String>();
        PM_COM_MEDIA_CAPABILITIES.put(ProviderInterface.COM_MEDIA_CAPABILITIES.CONF,
                ProviderInterface.COM_MEDIA_CAPABILITIES.CONF);

        PM_IMS_MO = new HashMap<String, String>();
        PM_IMS_MO.put(ProviderInterface.IMS_MO.VOICE_DOMAIN_PREFERENCE_EUTRAN,
                ProviderInterface.IMS_MO.VOICE_DOMAIN_PREFERENCE_EUTRAN);
    }
}
