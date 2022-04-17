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
import android.net.Uri;

import com.android.imsstack.core.config.SmartConfigProviderInterface;
import com.android.imsstack.core.config.SmartConfigXMLLoader;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsPrivateProperties;
import com.android.imsstack.util.Log;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class SmartConfigProvider extends ContentProvider {
    private static final int DATABASE_VERSION = 15;

    private static File dbFile;

    private static final String TAG = "ImsStack_SmartConfigProvider";

    // URI matcher types
    protected static final int URI_SMART_MODIFIED_TIME = 1;
    protected static final int URI_OPERATOR_LIST = 2;
    protected static final int URI_MCCMNC_LIST = 3;
    protected static final int URI_NTCODE_LIST = 4;
    protected static final int URI_SERVICE_ACCEPTANCE_LIST = 5;
    protected static final int URI_NAO_CARRIER_LIST = 6;
    protected static final int URI_SERVICE_ENABLE_LIST_BY_TOTC = 7;
    protected static final int URI_SERVICE_ENABLE_LIST_BY_NTCODE = 8;

    // Projection maps
    protected static final HashMap<String, String> PM_SMART_MODIFIED_TIME;
    protected static final HashMap<String, String> PM_OPERATOR_LIST;
    protected static final HashMap<String, String> PM_MCCMNC_LIST;
    protected static final HashMap<String, String> PM_NTCODE_LIST;
    protected static final HashMap<String, String> PM_SERVICE_ACCEPTANCE_LIST;
    protected static final HashMap<String, String> PM_NAO_CARRIER_LIST;
    protected static final HashMap<String, String> PM_SERVICE_ENABLE_LIST_BY_TOTC;
    protected static final HashMap<String, String> PM_SERVICE_ENABLE_LIST_BY_NTCODE;

    // URI matchers
    protected static final UriMatcher URI_MATCHER;

    // Configuration XML Loader
    protected static SmartConfigXMLLoader sXMLLoader = new SmartConfigXMLLoader();

    // protected SQLiteDatabase DB;
    protected DatabaseHelper mOpenHelper;

    private static ArrayList<String> sDbTableName = null;

    /**
     * This class helps open, create, and upgrade the database file.
     */
    private static class DatabaseHelper extends SQLiteOpenHelper {
        private static Context sContext;

        DatabaseHelper(Context context
                    , String name, CursorFactory factory, int version) {
            super(context, name, factory, version);

            sContext = context;
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            Log.i(TAG, "DatabaseHelper onCreate()");

            createTables(db);
            Log.i(TAG, SmartConfigProviderInterface.DB_NAME + " is created ...");
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.i(TAG, "Upgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");

            dropTables(db);
            onCreate(db);

            ImsPrivateProperties.Persistent.set(sContext,
                    ImsPrivateProperties.Persistent.KEY_DB_SMART_CONFIGURATION_OLD_VERSION,
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
                    ImsPrivateProperties.Persistent.KEY_DB_SMART_CONFIGURATION_OLD_VERSION,
                    String.valueOf(oldVersion), 0);
        }

        // createTables is a core method to create tables in gims_db.
        // Currently, ConfigurationProvider use xml loader to get
        // db table name, column name, and initial values.
        public void createTables(SQLiteDatabase db) {
            String[] dbTableNames = sXMLLoader.getTableNames();

            if ( dbTableNames == null ) {
                Log.e(TAG, "dbTableNames is null");
                return;
            }

            for (int i = 0; i < dbTableNames.length; i++) {
                String[][][] array = sXMLLoader.getTableElements(i);
                if (array == null) {
                    // Error!!!
                    Log.e(TAG, "array[" + i + "] is null !!!");
                    return;
                }
                createTable(db, dbTableNames[i], array);
                insertIntoTable(db, dbTableNames[i], array);
            }
        }

        // Create a table in db.
        // Data will later be inserted into the table.
        private boolean createTable(SQLiteDatabase db, String table, String[][][] content) {
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
            query += content[0][0][0];
            query += " TEXT";

            for (int i = 1; i < content[0].length; ++i) {
                query += ",";
                // '0' - column type, '1' - column name
                query += content[0][i][0];
                query += " TEXT";
            }

            query += ");";

            // Execute "CREATE" query statement
            db.execSQL(query);

            return true;
        }

        // Create a table in db.
        // Data will later be inserted into the table.
        private boolean insertIntoTable(SQLiteDatabase db, String table, String[][][] content) {
            if (table == null) {
                Log.e(TAG, "table is null");
                return false;
            }
            if (content == null || content.length == 0) {
                Log.e(TAG, "content is null");
                return false;
            }
            Log.d(TAG, "content.length=" + content.length
                + ", content[0].length=" + content[0].length
                + ", content[0][0].length=" + content[0][0].length);

            for (int i = 0; i < content.length; i++) {

                // Compose SQL query statement for CREATE
                String query = "INSERT INTO " + table + " VALUES(";

                // '0' - column type, '1' - column name
                query += "'";
                query += content[i][0][1];
                query += "'";

                for (int j = 1; j < content[0].length; j++) {
                    query += ",";
                    // '0' - column type, '1' - column name
                    query += "'";
                    query += content[i][j][1];
                    query += "'";
                }

                query += ");";

                db.execSQL(query);
            }

            return true;
        }

        private void dropTables(SQLiteDatabase db) {
            Log.i(TAG, "");

            getCurrentDBTableName(db);

            if (sDbTableName == null) {
                Log.e(TAG, "DBTableName ArrayList is null");
                return;
            }

            for (String tableName : sDbTableName) {
                db.execSQL("DROP TABLE IF EXISTS " + tableName);
            }
        }
    }

    @Override
    public int delete(Uri arg0, String arg1, String[] arg2) {
        Log.w(TAG, "DELETE query is not implemented");
        return 0;
    }

    @Override
    public String getType(Uri uri) {
        switch (URI_MATCHER.match(uri)) {
            case URI_SMART_MODIFIED_TIME:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.SmartModifiedTime.TABLE_NAME);
            case URI_OPERATOR_LIST:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.OperatorList.TABLE_NAME);
            case URI_MCCMNC_LIST:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.MccMncList.TABLE_NAME);
            case URI_NTCODE_LIST:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.NtCodeList.TABLE_NAME);
            case URI_SERVICE_ACCEPTANCE_LIST:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.ServiceAcceptanceList.TABLE_NAME);
            case URI_NAO_CARRIER_LIST:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.NaoCarrierList.TABLE_NAME);
            case URI_SERVICE_ENABLE_LIST_BY_TOTC:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.ServiceEnableListByToTc.TABLE_NAME);
            case URI_SERVICE_ENABLE_LIST_BY_NTCODE:
                return ("vnd.android.cursor.dir/"
                        + SmartConfigProviderInterface.AUTHORITY
                        + "." + SmartConfigProviderInterface.ServiceEnableListByNtCode.TABLE_NAME);
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
        boolean dbExist = isDBExist();

        sXMLLoader.init(getContext());

        // 1. Check creating or updating
        // 2. If db should be created or updated, then first delete db.
        if (dbExist ) {
            SQLiteDatabase db = DBUtils.DB.open(SmartConfigProviderInterface.DB_FULL_PATH,
                    SQLiteDatabase.OPEN_READONLY);


            boolean result = sXMLLoader.isConfigChanged(db);
            if (db != null) { db.close(); }

            if (result) {
                deleteDB();
                dbExist = false;
            }
        }

        mOpenHelper = new DatabaseHelper(getContext()
                                    , SmartConfigProviderInterface.DB_NAME
                                    , null, DATABASE_VERSION);

        mOpenHelper.getWritableDatabase();

        Log.w(TAG, "DB(" + SmartConfigProviderInterface.DB_NAME + "/"
                     + DATABASE_VERSION + ") is created");

        return true;
    }

    @Override
    public Cursor query(Uri uri,
            String[] projection, String selection, String[] selectionArgs, String sortOrder) {
        SQLiteQueryBuilder queryBuilder = new SQLiteQueryBuilder();
        String table = (uri != null) ? uri.getLastPathSegment() : "__null__";

        Log.i(TAG, "query - table(" + table + ") : " + URI_MATCHER.match(uri)
                + ", queryBuilder:" + queryBuilder);

        switch (URI_MATCHER.match(uri)) {
            case URI_SMART_MODIFIED_TIME:
                queryBuilder.setTables(SmartConfigProviderInterface.SmartModifiedTime.TABLE_NAME);
                break;
            case URI_OPERATOR_LIST:
                queryBuilder.setTables(SmartConfigProviderInterface.OperatorList.TABLE_NAME);
                break;
            case URI_MCCMNC_LIST:
                queryBuilder.setTables(SmartConfigProviderInterface.MccMncList.TABLE_NAME);
                break;
            case URI_NTCODE_LIST:
                queryBuilder.setTables(SmartConfigProviderInterface.NtCodeList.TABLE_NAME);
                break;
            case URI_SERVICE_ACCEPTANCE_LIST:
                queryBuilder.setTables(SmartConfigProviderInterface.ServiceAcceptanceList.TABLE_NAME);
                break;
            case URI_NAO_CARRIER_LIST:
                queryBuilder.setTables(SmartConfigProviderInterface.NaoCarrierList.TABLE_NAME);
                break;
            case URI_SERVICE_ENABLE_LIST_BY_TOTC:
                queryBuilder.setTables(SmartConfigProviderInterface.ServiceEnableListByToTc.TABLE_NAME);
                break;
            case URI_SERVICE_ENABLE_LIST_BY_NTCODE:
                queryBuilder.setTables(SmartConfigProviderInterface.ServiceEnableListByNtCode.TABLE_NAME);
                break;
            default:
                throw new IllegalArgumentException("Unknown URI " + uri);
        }

        Cursor cursor = buildQuery(uri,
                projection, selection, selectionArgs, sortOrder, queryBuilder);

        return cursor;
    }

    @Override
    public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
        String table = (uri != null) ? uri.getLastPathSegment() : "__null__";

        Log.w(TAG, "update from " + table);

        String tableName = null;

        switch (URI_MATCHER.match(uri)) {
            case URI_SMART_MODIFIED_TIME:
                tableName = SmartConfigProviderInterface.SmartModifiedTime.TABLE_NAME;
                break;
            case URI_OPERATOR_LIST:
                tableName = SmartConfigProviderInterface.OperatorList.TABLE_NAME;
                break;
            case URI_MCCMNC_LIST:
                tableName = SmartConfigProviderInterface.MccMncList.TABLE_NAME;
                break;
            case URI_NTCODE_LIST:
                tableName = SmartConfigProviderInterface.NtCodeList.TABLE_NAME;
                break;
            case URI_SERVICE_ACCEPTANCE_LIST:
                tableName = SmartConfigProviderInterface.ServiceAcceptanceList.TABLE_NAME;
                break;
            case URI_NAO_CARRIER_LIST:
                tableName = SmartConfigProviderInterface.NaoCarrierList.TABLE_NAME;
                break;
            case URI_SERVICE_ENABLE_LIST_BY_TOTC:
                tableName = SmartConfigProviderInterface.ServiceEnableListByToTc.TABLE_NAME;
                break;
            case URI_SERVICE_ENABLE_LIST_BY_NTCODE:
                tableName = SmartConfigProviderInterface.ServiceEnableListByNtCode.TABLE_NAME;
                break;
            default:
                throw new IllegalArgumentException("Unknown URI " + uri);
        }

        if (tableName == null) {
            return 0;
        }

        SQLiteDatabase db = mOpenHelper.getWritableDatabase();
        int count = db.update(tableName, values, where, whereArgs);

        if (Log.isDebuggable()) {
            Log.i(TAG, "update :: " + values.toString() + "; count (" + count + ")");
        }

        return count;
    }

    private Cursor buildQuery(Uri uri, String[] projection, String selection,
            String[] selectionArgs, String sortOrder, SQLiteQueryBuilder queryBuilder) {
        Cursor cursor = null;
        SQLiteDatabase db = mOpenHelper.getWritableDatabase();

        try {
            cursor = queryBuilder.query(
                    db, projection, selection, selectionArgs, null, null, sortOrder);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (cursor == null) {
            Log.w(TAG, "Cursor is null");
        }
        return cursor;
    }

    private boolean isDBExist() {
        dbFile = new File(SmartConfigProviderInterface.DB_FULL_PATH);

        return dbFile.exists();
    }

    private void deleteDB() {
        if (isDBExist() != true) {
            return;
        }

        Log.w(TAG, "deleteDB() : " + dbFile.exists());
        dbFile.delete();
    }

    private static void getCurrentDBTableName(SQLiteDatabase db) {
        if (db == null) {
            Log.e(TAG, "db is null");
            return;
        }

        sDbTableName = new ArrayList<String>();
        Cursor cursor = null;

        try {
            cursor = db.rawQuery("SELECT name FROM sqlite_master WHERE type='table'", null);
            if (cursor != null && cursor.moveToFirst()) {
                do {
                    String tableName = cursor.getString(cursor.getColumnIndex("name"));
                    if (!"android_metadata".equals(tableName)) {
                        sDbTableName.add(tableName);
                    }
                } while (cursor.moveToNext());
            }
        } catch (Exception e) {
            e.toString();
            e.printStackTrace();
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    static
    {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                SmartConfigProviderInterface.SmartModifiedTime.TABLE_NAME, URI_SMART_MODIFIED_TIME);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                        SmartConfigProviderInterface.OperatorList.TABLE_NAME, URI_OPERATOR_LIST);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                        SmartConfigProviderInterface.MccMncList.TABLE_NAME, URI_MCCMNC_LIST);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                        SmartConfigProviderInterface.NtCodeList.TABLE_NAME, URI_NTCODE_LIST);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                SmartConfigProviderInterface.ServiceAcceptanceList.TABLE_NAME, URI_SERVICE_ACCEPTANCE_LIST);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                SmartConfigProviderInterface.NaoCarrierList.TABLE_NAME, URI_NAO_CARRIER_LIST);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                SmartConfigProviderInterface.ServiceEnableListByToTc.TABLE_NAME, URI_SERVICE_ENABLE_LIST_BY_TOTC);
        URI_MATCHER.addURI(SmartConfigProviderInterface.AUTHORITY,
                SmartConfigProviderInterface.ServiceEnableListByNtCode.TABLE_NAME, URI_SERVICE_ENABLE_LIST_BY_NTCODE);

        PM_SMART_MODIFIED_TIME = new HashMap<String, String>();
        PM_SMART_MODIFIED_TIME.put(SmartConfigProviderInterface.SmartModifiedTime.XML_FILE,
                        SmartConfigProviderInterface.SmartModifiedTime.XML_FILE);
        PM_SMART_MODIFIED_TIME.put(SmartConfigProviderInterface.SmartModifiedTime.XML_MODIFIED_TIME,
                SmartConfigProviderInterface.SmartModifiedTime.XML_MODIFIED_TIME);
        PM_SMART_MODIFIED_TIME.put(SmartConfigProviderInterface.SmartModifiedTime.SW_VERSION,
                SmartConfigProviderInterface.SmartModifiedTime.SW_VERSION);
        PM_SMART_MODIFIED_TIME.put(SmartConfigProviderInterface.SmartModifiedTime.VERSION,
                SmartConfigProviderInterface.SmartModifiedTime.VERSION);

        PM_OPERATOR_LIST = new HashMap<String, String>();
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.OPERATOR,
                        SmartConfigProviderInterface.OperatorList.OPERATOR);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.COUNTRY,
                        SmartConfigProviderInterface.OperatorList.COUNTRY);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.REGION,
                        SmartConfigProviderInterface.OperatorList.REGION);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.GROUP_ID,
                        SmartConfigProviderInterface.OperatorList.GROUP_ID);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.CATEGORY,
                        SmartConfigProviderInterface.OperatorList.CATEGORY);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.BRAND,
                        SmartConfigProviderInterface.OperatorList.BRAND);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.OPERATOR_BASED_ON,
                        SmartConfigProviderInterface.OperatorList.OPERATOR_BASED_ON);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.ENABLER_TYPE,
                        SmartConfigProviderInterface.OperatorList.ENABLER_TYPE);
        PM_OPERATOR_LIST.put(SmartConfigProviderInterface.OperatorList.SUPPORT_SIM_MOVED,
                        SmartConfigProviderInterface.OperatorList.SUPPORT_SIM_MOVED);

        PM_MCCMNC_LIST = new HashMap<String, String>();
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.MCCMNC,
                        SmartConfigProviderInterface.MccMncList.MCCMNC);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.OPERATOR,
                        SmartConfigProviderInterface.MccMncList.OPERATOR);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.COUNTRY,
                        SmartConfigProviderInterface.MccMncList.COUNTRY);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.GID,
                SmartConfigProviderInterface.MccMncList.GID);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.SPN,
                SmartConfigProviderInterface.MccMncList.SPN);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.IMSI,
                SmartConfigProviderInterface.MccMncList.IMSI);
        PM_MCCMNC_LIST.put(SmartConfigProviderInterface.MccMncList.ENABLED,
                SmartConfigProviderInterface.MccMncList.ENABLED);

        PM_NTCODE_LIST = new HashMap<String, String>();
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.NTCODE,
                        SmartConfigProviderInterface.NtCodeList.NTCODE);
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.OPERATOR,
                        SmartConfigProviderInterface.NtCodeList.OPERATOR);
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.COUNTRY,
                        SmartConfigProviderInterface.NtCodeList.COUNTRY);
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.REGION,
                        SmartConfigProviderInterface.NtCodeList.REGION);
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.CATEGORY,
                        SmartConfigProviderInterface.NtCodeList.CATEGORY);
        PM_NTCODE_LIST.put(SmartConfigProviderInterface.NtCodeList.RESTRICTION,
                        SmartConfigProviderInterface.NtCodeList.RESTRICTION);

        PM_SERVICE_ACCEPTANCE_LIST = new HashMap<String, String>();
        PM_SERVICE_ACCEPTANCE_LIST.put(SmartConfigProviderInterface.ServiceAcceptanceList.OPERATOR,
                        SmartConfigProviderInterface.ServiceAcceptanceList.OPERATOR);
        PM_SERVICE_ACCEPTANCE_LIST.put(SmartConfigProviderInterface.ServiceAcceptanceList.COUNTRY,
                        SmartConfigProviderInterface.ServiceAcceptanceList.COUNTRY);
        PM_SERVICE_ACCEPTANCE_LIST.put(SmartConfigProviderInterface.ServiceAcceptanceList.MCC,
                        SmartConfigProviderInterface.ServiceAcceptanceList.MCC);
        PM_SERVICE_ACCEPTANCE_LIST.put(SmartConfigProviderInterface.ServiceAcceptanceList.MCCMNC,
                        SmartConfigProviderInterface.ServiceAcceptanceList.MCCMNC);

        PM_NAO_CARRIER_LIST = new HashMap<String, String>();
        PM_NAO_CARRIER_LIST.put(SmartConfigProviderInterface.NaoCarrierList.CARRIER,
                        SmartConfigProviderInterface.NaoCarrierList.CARRIER);
        PM_NAO_CARRIER_LIST.put(SmartConfigProviderInterface.NaoCarrierList.OPERATOR,
                        SmartConfigProviderInterface.NaoCarrierList.OPERATOR);
        PM_NAO_CARRIER_LIST.put(SmartConfigProviderInterface.NaoCarrierList.COUNTRY,
                        SmartConfigProviderInterface.NaoCarrierList.COUNTRY);

        PM_SERVICE_ENABLE_LIST_BY_TOTC = new HashMap<String, String>();
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByToTc.TOTC,
                        SmartConfigProviderInterface.ServiceEnableListByToTc.TOTC);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.COUNTRY,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.COUNTRY);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCC,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCC);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.REGION,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.REGION);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.GROUP_ID,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.GROUP_ID);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR_EXEMPT,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR_EXEMPT);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC_EXEMPT,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC_EXEMPT);
        PM_SERVICE_ENABLE_LIST_BY_TOTC.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.INBOUNDING_ROAMING,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.INBOUNDING_ROAMING);

        PM_SERVICE_ENABLE_LIST_BY_NTCODE = new HashMap<String, String>();
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.NTCODE,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.NTCODE);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.COUNTRY,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.COUNTRY);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCC,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCC);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.REGION,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.REGION);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.GROUP_ID,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.GROUP_ID);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR_EXEMPT,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.OPERATOR_EXEMPT);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC_EXEMPT,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.MCCMNC_EXEMPT);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.INBOUNDING_ROAMING,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.INBOUNDING_ROAMING);
        PM_SERVICE_ENABLE_LIST_BY_NTCODE.put(SmartConfigProviderInterface.ServiceEnableListByNtCode.DESC,
                        SmartConfigProviderInterface.ServiceEnableListByNtCode.DESC);
    }
}
