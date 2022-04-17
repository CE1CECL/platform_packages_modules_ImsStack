package com.android.imsstack.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.ContentUris;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteQueryBuilder;
import android.net.Uri;
import android.text.TextUtils;

import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.Log;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class ImsStateProvider extends ContentProvider {
    private static final String TAG = "ImsStateProvider";

    // URI matcher types
    protected static final int URI_REG_STATE = 1;
    protected static final int URI_REG_STATE_SINGLE = 2;
    protected static final int URI_VOLTE_STATE = 3;
    protected static final int URI_VOLTE_STATE_SINGLE = 4;
    protected static final int URI_CALL_STATE = 5;
    protected static final int URI_CALL_STATE_SINGLE = 6;
    protected static final int URI_ROAMING_STATE = 7;
    protected static final int URI_ROAMING_STATE_SINGLE = 8;
    protected static final int URI_VOWIFI_ACTIVATION_STATE = 9;
    protected static final int URI_VOWIFI_ACTIVATION_STATE_SINGLE = 10;
    // URI matchers
    protected static final HashMap<String, String> PM_REG_STATE;
    protected static final HashMap<String, String> PM_VOLTE_STATE;
    protected static final HashMap<String, String> PM_CALL_STATE;
    protected static final HashMap<String, String> PM_ROAMING_STATE;
    protected static final HashMap<String, String> PM_VOWIFI_ACTIVATION_STATE;
    // URI matchers
    protected static final UriMatcher URI_MATCHER;

    private DatabaseHelper mDBHelper;

    /**
     * This class helps open, create, and upgrade the database file.
     */
    private static class DatabaseHelper extends SQLiteOpenHelper {
        DatabaseHelper(Context context, String name, CursorFactory factory, int version) {
            super(context, name, factory, version);
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            if (Log.isDebuggable()) {
                Log.d(TAG, ImsState.DATABASE_NAME
                        + "(v=" + ImsState.DATABASE_VERSION + ") is created");
            }

            createTables(db);
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.d(TAG, "Upgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");

            List<ImsState.PersistentValues> valuesList = getPersistentValues(db);

            dropTables(db);
            onCreate(db);

            if (valuesList != null) {
                setPersistentValues(db, valuesList);
            }
        }

        // hwangoo.park@, 2012-04-12
        // If any problem occurred regarding to the lower version of the database,
        // the below code will be enabled.
        @Override
        public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.d(TAG, "Downgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");

            dropTables(db);
            onCreate(db);
        }

        public void checkIntegrityAndCreateTablesIfNotPresent(SQLiteDatabase db) {
            Log.d(TAG, "checkIntegrityAndCreateTablesIfNotPresent :: "
                    + Log.pii(ImsState.DATABASE_NAME));

            if (db == null) {
                Log.e(TAG, "Database is null");
                return;
            }

            String[] tables = ImsState.getTables();

            if ((tables == null) || (tables.length == 0)) {
                return;
            }

            for (int i = 0; i < tables.length; ++i) {
                String table = tables[i];
                String[][] contents = ImsState.getTableContents(table);

                if ((contents == null) || (contents.length == 0)) {
                    continue;
                }

                if (DBUtils.DB.hasTable(db, table)) {
                    if (!ImsState.hasDefaultValues(table)) {
                        // No need to insert default values
                    } else if (DBUtils.DB.getRowCount(db, table) <= 0) {
                        Log.d(TAG, "Add a default row :: " + table);

                        int contentCount = ImsState.getTableContentCount();

                        try {
                            for (int j = 0; j < contentCount; ++j) {
                                insertValue(db, table, contents);
                            }
                        } catch (SQLiteException e) {
                            e.printStackTrace();
                        }
                    }
                    continue;
                }

                Log.d(TAG, "Create a table :: " + table);

                if (createTable(db, table, contents)) {
                    if (ImsState.hasDefaultValues(table)) {
                        int contentCount = ImsState.getTableContentCount();

                        try {
                            for (int j = 0; j < contentCount; ++j) {
                                insertValue(db, table, contents);
                            }
                        } catch (SQLiteException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        }

        private boolean createTable(SQLiteDatabase db, String table, String[][] contents) {
            // Compose SQL query statement for CREATE
            StringBuilder sb = new StringBuilder();

            sb.append("CREATE TABLE " + table + " (");

            sb.append(contents[0][ImsState.Columns.I_NAME]);
            sb.append(" ");
            sb.append(contents[0][ImsState.Columns.I_TYPE]);

            for (int i = 1; i < contents.length; ++i) {
                sb.append(",");
                sb.append(contents[i][ImsState.Columns.I_NAME]);
                sb.append(" ");
                sb.append(contents[i][ImsState.Columns.I_TYPE]);
            }

            sb.append(");");

            if (Log.isDebuggable()) {
                Log.d(TAG, "createTable :: " + sb.toString());
            }

            // Execute "CREATE" query statement
            db.execSQL(sb.toString());

            return true;
        }

        private void createTables(SQLiteDatabase db) {
            String[] tables = ImsState.getTables();

            if ((tables == null) || (tables.length == 0)) {
                return;
            }

            for (int i = 0; i < tables.length; ++i) {
                String table = tables[i];
                String[][] contents = ImsState.getTableContents(table);

                if ((contents == null) || (contents.length == 0)) {
                    continue;
                }

                if (createTable(db, table, contents)) {
                    if (ImsState.hasDefaultValues(table)) {
                        int contentCount = ImsState.getTableContentCount();

                        for (int j = 0; j < contentCount; ++j) {
                            insertValue(db, table, contents);
                        }
                    }
                }
            }
        }

        private void dropTables(SQLiteDatabase db) {
            String[] tables = ImsState.getTables();

            if ((tables == null) || (tables.length == 0)) {
                return;
            }

            for (int i = 0; i < tables.length; ++i) {
                db.execSQL("DROP TABLE IF EXISTS " + tables[i]);
            }
        }

        private boolean insertValue(SQLiteDatabase db, String table, String[][] contents) {
            // Compose SQL query statement for INSERT
            ContentValues values = new ContentValues();

            // "_id" will not be included for automatic increment.
            for (int i = ImsState.Columns.I_VALUE_BASE; i < contents.length; ++i) {
                values.put(contents[i][ImsState.Columns.I_NAME],
                    contents[i][ImsState.Columns.I_VALUE]);
            }

            // Execute "INSERT" query statement
            db.insertOrThrow(table, null, values);

            return true;
        }

        private ImsState.PersistentValues createPersistentValues(Cursor c,
                String table, String[] cols) {
            int colIndex = c.getColumnIndex(ImsState.Columns._ID);
            int id = -1;

            if (colIndex >= 0) {
                id = c.getInt(colIndex);

                // phone-id(slot-id) only considered
                if (id >= ImsState.Columns.PRIMARY_KEY_BASE) {
                    id = id - ImsState.Columns.PRIMARY_KEY_BASE;
                }
            }

            ImsState.PersistentValues values = new ImsState.PersistentValues(table, id);

            for (int i = 0; i < cols.length; i++) {
                colIndex = c.getColumnIndex(cols[i]);

                if (colIndex >= 0) {
                    int type = c.getType(colIndex);

                    // INTEGER / TEXT only considered
                    if (type == Cursor.FIELD_TYPE_INTEGER) {
                        values.putInt(cols[i], c.getInt(colIndex));
                    } else if (type == Cursor.FIELD_TYPE_STRING) {
                        values.putString(cols[i], c.getString(colIndex));
                    }
                }
            }

            if (Log.isDebuggable()) {
                Log.d(TAG, "createPersistentValues: " + values.toString());
            }

            return values;
        }

        private List<ImsState.PersistentValues> getPersistentValues(SQLiteDatabase db) {
            List<ImsState.PersistentValues> persistentValues
                    = new ArrayList<ImsState.PersistentValues>();
            String[] tables = ImsState.getTables();

            if ((tables == null) || (tables.length == 0)) {
                return persistentValues;
            }

            for (int i = 0; i < tables.length; ++i) {
                String table = tables[i];
                String[] persistentCols = ImsState.getPersistentColumns(table);

                if ((persistentCols == null) || (persistentCols.length == 0)) {
                    continue;
                }

                int contentCount = ImsState.getTableContentCount();

                for (int j = 0; j < contentCount; j++) {
                    String where = ImsStateController.selectForPhoneId(j);
                    Cursor c = DBUtils.DB.getFirstCursor(db, table, where);

                    if (c != null) {
                        persistentValues.add(createPersistentValues(c, table, persistentCols));
                        DBUtils.closeCursor(c);
                    }
                }
            }

            return persistentValues;
        }

        private void setPersistentValues(SQLiteDatabase db,
                List<ImsState.PersistentValues> valuesList) {
            for (int i = 0; i < valuesList.size(); i++) {
                ImsState.PersistentValues values = valuesList.get(i);

                if (values.getId() < 0 || values.isEmpty()) {
                    continue;
                }

                String where = ImsStateController.selectForPhoneId(values.getId());
                int count = DBUtils.DB.update(db, values.getTableName(),
                        values.getValues(), where, null, false);

                if (Log.isDebuggable()) {
                    Log.d(TAG, "setPersistentValues :: " + ((count == 1) ? "OK" : "NOK")
                            + " " + values.toString());
                }
            }
        }
    }

    @Override
    public int delete(Uri uri, String where, String[] whereArgs) {
        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        int count;

        switch (URI_MATCHER.match(uri)) {
        case URI_REG_STATE:
            count = db.delete(ImsState.RegState.TABLE_NAME, where, whereArgs);
            break;
        case URI_REG_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.delete(ImsState.RegState.TABLE_NAME,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_VOLTE_STATE:
            count = db.delete(ImsState.VoLteState.TABLE_NAME, where, whereArgs);
            break;
        case URI_VOLTE_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.delete(ImsState.VoLteState.TABLE_NAME,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_CALL_STATE:
            count = db.delete(ImsState.CallState.TABLE_NAME, where, whereArgs);
            break;
        case URI_CALL_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.delete(ImsState.CallState.TABLE_NAME,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_ROAMING_STATE:
            count = db.delete(ImsState.RoamingState.TABLE_NAME, where, whereArgs);
            break;
        case URI_ROAMING_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.delete(ImsState.RoamingState.TABLE_NAME,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_VOWIFI_ACTIVATION_STATE:
            count = db.delete(ImsState.VoWiFiActivationState.TABLE_NAME, where, whereArgs);
            break;
        case URI_VOWIFI_ACTIVATION_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.delete(ImsState.VoWiFiActivationState.TABLE_NAME,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        default:
            throw new IllegalArgumentException("Unknown URI " + uri);
        }

        getContext().getContentResolver().notifyChange(uri, null);

        return count;
    }

    @Override
    public String getType(Uri uri) {
        switch (URI_MATCHER.match(uri)) {
        case URI_REG_STATE:
            return ImsState.RegState.CONTENT_TYPE;
        case URI_REG_STATE_SINGLE:
            return ImsState.RegState.CONTENT_ITEM_TYPE;
        case URI_VOLTE_STATE:
            return ImsState.VoLteState.CONTENT_TYPE;
        case URI_VOLTE_STATE_SINGLE:
            return ImsState.VoLteState.CONTENT_ITEM_TYPE;
        case URI_CALL_STATE:
            return ImsState.CallState.CONTENT_TYPE;
        case URI_CALL_STATE_SINGLE:
            return ImsState.CallState.CONTENT_ITEM_TYPE;
        case URI_ROAMING_STATE:
            return ImsState.RoamingState.CONTENT_TYPE;
        case URI_ROAMING_STATE_SINGLE:
            return ImsState.RoamingState.CONTENT_ITEM_TYPE;
        case URI_VOWIFI_ACTIVATION_STATE:
            return ImsState.VoWiFiActivationState.CONTENT_TYPE;
        case URI_VOWIFI_ACTIVATION_STATE_SINGLE:
            return ImsState.VoWiFiActivationState.CONTENT_ITEM_TYPE;
        default:
            throw new IllegalArgumentException("Unsupported URI: " + uri);
        }
    }

    @Override
    public Uri insert(Uri uri, ContentValues initialValues) {
        switch (URI_MATCHER.match(uri)) {
        case URI_REG_STATE:
            return insertRegState(uri, initialValues);
        case URI_VOLTE_STATE:
            return insertVoLteState(uri, initialValues);
        case URI_CALL_STATE:
            return insertCallState(uri, initialValues);
        case URI_ROAMING_STATE:
            return insertRoamingState(uri, initialValues);
        case URI_VOWIFI_ACTIVATION_STATE:
            return insertVoWiFiActivationState(uri, initialValues);
        default:
            throw new IllegalArgumentException("Unknown URI " + uri);
        }
    }

    @Override
    public boolean onCreate() {
        if (Log.isDebuggable()) {
            Log.d(TAG, "ImsStateProvider is created ...");
        }

        mDBHelper = new DatabaseHelper(getContext(),
                ImsState.DATABASE_NAME, null, ImsState.DATABASE_VERSION);

        return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        SQLiteQueryBuilder qb = new SQLiteQueryBuilder();

        switch (URI_MATCHER.match(uri)) {
        case URI_REG_STATE:
            qb.setTables(ImsState.RegState.TABLE_NAME);
            qb.setProjectionMap(PM_REG_STATE);
            break;
        case URI_REG_STATE_SINGLE:
            qb.setTables(ImsState.RegState.TABLE_NAME);
            qb.setProjectionMap(PM_REG_STATE);
            qb.appendWhere(ImsState.Columns._ID + "=" + uri.getPathSegments().get(1));
            break;
        case URI_VOLTE_STATE:
            qb.setTables(ImsState.VoLteState.TABLE_NAME);
            qb.setProjectionMap(PM_VOLTE_STATE);
            break;
        case URI_VOLTE_STATE_SINGLE:
            qb.setTables(ImsState.VoLteState.TABLE_NAME);
            qb.setProjectionMap(PM_VOLTE_STATE);
            qb.appendWhere(ImsState.Columns._ID + "=" + uri.getPathSegments().get(1));
            break;
        case URI_CALL_STATE:
            qb.setTables(ImsState.CallState.TABLE_NAME);
            qb.setProjectionMap(PM_CALL_STATE);
            break;
        case URI_CALL_STATE_SINGLE:
            qb.setTables(ImsState.CallState.TABLE_NAME);
            qb.setProjectionMap(PM_CALL_STATE);
            qb.appendWhere(ImsState.Columns._ID + "=" + uri.getPathSegments().get(1));
            break;
        case URI_ROAMING_STATE:
            qb.setTables(ImsState.RoamingState.TABLE_NAME);
            qb.setProjectionMap(PM_ROAMING_STATE);
            break;
        case URI_ROAMING_STATE_SINGLE:
            qb.setTables(ImsState.RoamingState.TABLE_NAME);
            qb.setProjectionMap(PM_ROAMING_STATE);
            qb.appendWhere(ImsState.Columns._ID + "=" + uri.getPathSegments().get(1));
            break;
        case URI_VOWIFI_ACTIVATION_STATE:
            qb.setTables(ImsState.VoWiFiActivationState.TABLE_NAME);
            qb.setProjectionMap(PM_VOWIFI_ACTIVATION_STATE);
            break;
        case URI_VOWIFI_ACTIVATION_STATE_SINGLE:
            qb.setTables(ImsState.VoWiFiActivationState.TABLE_NAME);
            qb.setProjectionMap(PM_VOWIFI_ACTIVATION_STATE);
            qb.appendWhere(ImsState.Columns._ID + "=" + uri.getPathSegments().get(1));
            break;
        default:
            throw new IllegalArgumentException("Unknown URI " + uri);
        }

        SQLiteDatabase db = mDBHelper.getReadableDatabase();
        Cursor c = null;

        try {
            c = qb.query(db, projection, selection, selectionArgs, null, null, sortOrder);
        } catch (SQLiteException e) {
            Log.e(TAG, "query :: " + e.getMessage());
            e.printStackTrace();

            if (e.getMessage().startsWith("no such table")) {
                mDBHelper.checkIntegrityAndCreateTablesIfNotPresent(
                        mDBHelper.getWritableDatabase());
            }
        }

        if (c != null) {
            c.setNotificationUri(getContext().getContentResolver(), uri);
        }

        if (Log.isDebuggable()) {
            Log.d(TAG, "query :: " + ((uri != null) ? uri.getLastPathSegment() : "null")
                    + "::" + selection);
        }

        return c;
    }

    @Override
    public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        int count = 0;

        switch (URI_MATCHER.match(uri)) {
        case URI_REG_STATE:
            count = db.update(ImsState.RegState.TABLE_NAME, values, where, whereArgs);
            break;
        case URI_REG_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.update(ImsState.RegState.TABLE_NAME, values,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_VOLTE_STATE:
            count = db.update(ImsState.VoLteState.TABLE_NAME, values, where, whereArgs);
            break;
        case URI_VOLTE_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.update(ImsState.VoLteState.TABLE_NAME, values,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_CALL_STATE:
            count = db.update(ImsState.CallState.TABLE_NAME, values, where, whereArgs);
            break;
        case URI_CALL_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.update(ImsState.CallState.TABLE_NAME, values,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_ROAMING_STATE:
            count = db.update(ImsState.RoamingState.TABLE_NAME, values, where, whereArgs);
            break;
        case URI_ROAMING_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.update(ImsState.RoamingState.TABLE_NAME, values,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        case URI_VOWIFI_ACTIVATION_STATE:
            count = db.update(ImsState.VoWiFiActivationState.TABLE_NAME, values, where, whereArgs);
            break;
        case URI_VOWIFI_ACTIVATION_STATE_SINGLE: {
            String rowId = uri.getPathSegments().get(1);
            count = db.update(ImsState.VoWiFiActivationState.TABLE_NAME, values,
                    ImsState.Columns._ID + "=" + rowId
                    + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "''"),
                    whereArgs);
            break;
        }
        default:
            throw new IllegalArgumentException("Unknown URI " + uri);
        }

        if (count > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }

        return count;
    }

    @Override
    public void dump(FileDescriptor fd, PrintWriter writer, String[] args) {
        SQLiteDatabase db = mDBHelper.getReadableDatabase();

        if (db == null) {
            writer.println("No database");
            return;
        }

        String[] tables = ImsState.getTables();

        if (tables == null) {
            writer.println("No tables");
            return;
        }

        writer.println("ImsState {");

        StringBuilder sb = new StringBuilder();

        for (int i = 0; i < tables.length; i++) {
            if (i != 0) {
                sb.append("\n");
            }

            Cursor c = null;

            try {
                c = db.rawQuery("SELECT * FROM " + tables[i], null);

                int index = tables[i].indexOf('_');

                sb.append("\t");

                if (index > 0) {
                    sb.append(tables[i].substring(0, index));
                } else {
                    sb.append(tables[i]);
                }

                sb.append(" {\n");

                if (c != null && c.moveToFirst()) {
                    do {
                        sb.append("\t  ");
                        int colCount = c.getColumnCount();

                        for (int j = 0; j < colCount; j++) {
                            if (j != 0) {
                                if ((j % 6) == 0) {
                                    sb.append(",\n\t\t");
                                } else {
                                    sb.append(", ");
                                }
                            }

                            sb.append(c.getColumnName(j)).append("=");
                            sb.append(c.getString(j));
                        }
                        sb.append("\n");
                    } while (c.moveToNext());
                }

                sb.append("\t}");
            } catch (SQLiteException e) {
                Log.e(TAG, "rawQuery :: " + e.getMessage());
                e.printStackTrace();
            } finally {
                if (c != null) {
                    c.close();
                }
            }
        }

        writer.println(sb.toString());

        writer.println("}");
    }

    private Uri insertRegState(Uri uri, ContentValues initialValues) {
        ContentValues values;

        if (initialValues != null) {
            values = new ContentValues(initialValues);
        } else {
            values = new ContentValues();
        }

        putValueIfNotPresent(values, ImsState.Columns.SUB_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.STATE, "0");
        putValueIfNotPresent(values, ImsState.Columns.SERVICE_STATUS, "0");
        putValueIfNotPresent(values, ImsState.Columns.NETWORK_TYPE, "0");

        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        long rowId = db.insert(ImsState.RegState.TABLE_NAME, null, values);

        if (rowId > 0) {
            Uri insertedUri = ContentUris.withAppendedId(ImsState.RegState.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(insertedUri, null);
            return insertedUri;
        }

        throw new SQLiteException("Failed to insert row into " + uri);
    }

    private Uri insertVoLteState(Uri uri, ContentValues initialValues) {
        ContentValues values;

        if (initialValues != null) {
            values = new ContentValues(initialValues);
        } else {
            values = new ContentValues();
        }

        putValueIfNotPresent(values, ImsState.Columns.SUB_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.SERVICE_STATUS, "0");
        putValueIfNotPresent(values, ImsState.Columns.CALL_STATE, "0");
        putValueIfNotPresent(values, ImsState.Columns.VOLTE_PROVISIONED, "0");
        putValueIfNotPresent(values, ImsState.Columns.VT_PROVISIONED, "0");
        putValueIfNotPresent(values, ImsState.Columns.WFC_PROVISIONED, "0");
        putValueIfNotPresent(values, ImsState.Columns.DEVICE_VOLTE_AVAILABLE, "0");
        putValueIfNotPresent(values, ImsState.Columns.DEVICE_VT_AVAILABLE, "0");
        putValueIfNotPresent(values, ImsState.Columns.DEVICE_WFC_AVAILABLE, "0");

        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        long rowId = db.insert(ImsState.VoLteState.TABLE_NAME, null, values);

        if (rowId > 0) {
            Uri insertedUri = ContentUris.withAppendedId(ImsState.VoLteState.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(insertedUri, null);
            return insertedUri;
        }

        throw new SQLiteException("Failed to insert row into " + uri);
    }

    private Uri insertCallState(Uri uri, ContentValues initialValues) {
        ContentValues values;

        if (initialValues != null) {
            values = new ContentValues(initialValues);
        } else {
            values = new ContentValues();
        }

        putValueIfNotPresent(values, ImsState.Columns.SUB_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.STATE, "0");
        putValueIfNotPresent(values, ImsState.Columns.CONNECTED_CALL_ON_WIFI, "0");

        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        long rowId = db.insert(ImsState.CallState.TABLE_NAME, null, values);

        if (rowId > 0) {
            Uri insertedUri = ContentUris.withAppendedId(ImsState.CallState.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(insertedUri, null);
            return insertedUri;
        }

        throw new SQLiteException("Failed to insert row into " + uri);
    }

    private Uri insertRoamingState(Uri uri, ContentValues initialValues) {
        ContentValues values;

        if (initialValues != null) {
            values = new ContentValues(initialValues);
        } else {
            values = new ContentValues();
        }

        putValueIfNotPresent(values, ImsState.Columns.SUB_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.VOLTE_ROAMING, "0");
        putValueIfNotPresent(values, ImsState.Columns.VT_ROAMING, "0");

        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        long rowId = db.insert(ImsState.RoamingState.TABLE_NAME, null, values);

        if (rowId > 0) {
            Uri insertedUri = ContentUris.withAppendedId(ImsState.RoamingState.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(insertedUri, null);
            return insertedUri;
        }

        throw new SQLiteException("Failed to insert row into " + uri);
    }

    private Uri insertVoWiFiActivationState(Uri uri, ContentValues initialValues) {
        ContentValues values;

        if (initialValues != null) {
            values = new ContentValues(initialValues);
        } else {
            values = new ContentValues();
        }

        putValueIfNotPresent(values, ImsState.Columns.SUB_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.SLOT_ID, "-1");
        putValueIfNotPresent(values, ImsState.Columns.OPERATOR, "unknown");
        putValueIfNotPresent(values, ImsState.Columns.STATE, "0");
        putValueIfNotPresent(values, ImsState.Columns.RESERVED, "");

        SQLiteDatabase db = mDBHelper.getWritableDatabase();
        long rowId = db.insert(ImsState.VoWiFiActivationState.TABLE_NAME, null, values);

        if (rowId > 0) {
            if (Log.isDebuggable()) {
                Log.d(TAG, "VoWiFiActivationState: " + values.toString());
            }

            Uri insertedUri = ContentUris.withAppendedId(
                    ImsState.VoWiFiActivationState.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(insertedUri, null);
            return insertedUri;
        }

        throw new SQLiteException("Failed to insert row into " + uri);
    }

    private static void putValueIfNotPresent(ContentValues values, String key, String value) {
        if (!values.containsKey(key)) {
            values.put(key, value);
        }
    }

    static {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.RegState.TABLE_NAME, URI_REG_STATE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.RegState.TABLE_NAME + "/#", URI_REG_STATE_SINGLE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.VoLteState.TABLE_NAME, URI_VOLTE_STATE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.VoLteState.TABLE_NAME + "/#", URI_VOLTE_STATE_SINGLE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.CallState.TABLE_NAME, URI_CALL_STATE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.CallState.TABLE_NAME + "/#", URI_CALL_STATE_SINGLE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.RoamingState.TABLE_NAME, URI_ROAMING_STATE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.RoamingState.TABLE_NAME + "/#", URI_ROAMING_STATE_SINGLE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.VoWiFiActivationState.TABLE_NAME, URI_VOWIFI_ACTIVATION_STATE);
        URI_MATCHER.addURI(ImsState.AUTHORITY,
            ImsState.VoWiFiActivationState.TABLE_NAME + "/#", URI_VOWIFI_ACTIVATION_STATE_SINGLE);

        PM_REG_STATE = new HashMap<String, String>();
        PM_REG_STATE.put(ImsState.Columns._ID, ImsState.Columns._ID);
        PM_REG_STATE.put(ImsState.Columns.SUB_ID, ImsState.Columns.SUB_ID);
        PM_REG_STATE.put(ImsState.Columns.STATE, ImsState.Columns.STATE);
        PM_REG_STATE.put(ImsState.Columns.SERVICE_STATUS, ImsState.Columns.SERVICE_STATUS);
        PM_REG_STATE.put(ImsState.Columns.NETWORK_TYPE, ImsState.Columns.NETWORK_TYPE);

        PM_VOLTE_STATE = new HashMap<String, String>();
        PM_VOLTE_STATE.put(ImsState.Columns._ID, ImsState.Columns._ID);
        PM_VOLTE_STATE.put(ImsState.Columns.SUB_ID, ImsState.Columns.SUB_ID);
        PM_VOLTE_STATE.put(ImsState.Columns.SERVICE_STATUS, ImsState.Columns.SERVICE_STATUS);
        PM_VOLTE_STATE.put(ImsState.Columns.CALL_STATE, ImsState.Columns.CALL_STATE);
        PM_VOLTE_STATE.put(ImsState.Columns.VOLTE_PROVISIONED, ImsState.Columns.VOLTE_PROVISIONED);
        PM_VOLTE_STATE.put(ImsState.Columns.VT_PROVISIONED, ImsState.Columns.VT_PROVISIONED);
        PM_VOLTE_STATE.put(ImsState.Columns.WFC_PROVISIONED, ImsState.Columns.WFC_PROVISIONED);
        PM_VOLTE_STATE.put(
            ImsState.Columns.DEVICE_VOLTE_AVAILABLE, ImsState.Columns.DEVICE_VOLTE_AVAILABLE);
        PM_VOLTE_STATE.put(
            ImsState.Columns.DEVICE_VT_AVAILABLE, ImsState.Columns.DEVICE_VT_AVAILABLE);
        PM_VOLTE_STATE.put(
            ImsState.Columns.DEVICE_WFC_AVAILABLE, ImsState.Columns.DEVICE_WFC_AVAILABLE);

        PM_CALL_STATE = new HashMap<String, String>();
        PM_CALL_STATE.put(ImsState.Columns._ID, ImsState.Columns._ID);
        PM_CALL_STATE.put(ImsState.Columns.SUB_ID, ImsState.Columns.SUB_ID);
        PM_CALL_STATE.put(ImsState.Columns.STATE, ImsState.Columns.STATE);
        PM_CALL_STATE.put(
            ImsState.Columns.CONNECTED_CALL_ON_WIFI, ImsState.Columns.CONNECTED_CALL_ON_WIFI);

        PM_ROAMING_STATE = new HashMap<String, String>();
        PM_ROAMING_STATE.put(ImsState.Columns._ID, ImsState.Columns._ID);
        PM_ROAMING_STATE.put(ImsState.Columns.SUB_ID, ImsState.Columns.SUB_ID);
        PM_ROAMING_STATE.put(ImsState.Columns.VOLTE_ROAMING, ImsState.Columns.VOLTE_ROAMING);
        PM_ROAMING_STATE.put(ImsState.Columns.VT_ROAMING, ImsState.Columns.VT_ROAMING);

        PM_VOWIFI_ACTIVATION_STATE = new HashMap<String, String>();
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns._ID, ImsState.Columns._ID);
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns.SUB_ID, ImsState.Columns.SUB_ID);
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns.SLOT_ID, ImsState.Columns.SLOT_ID);
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns.OPERATOR, ImsState.Columns.OPERATOR);
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns.STATE, ImsState.Columns.STATE);
        PM_VOWIFI_ACTIVATION_STATE.put(ImsState.Columns.RESERVED, ImsState.Columns.RESERVED);
    }
}
