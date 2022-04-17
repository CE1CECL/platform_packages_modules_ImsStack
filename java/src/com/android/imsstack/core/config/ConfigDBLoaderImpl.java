package com.android.imsstack.core.config;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.net.Uri;
import android.os.Bundle;
import android.os.SystemClock;
import android.text.TextUtils;

import com.android.imsstack.util.AppContext;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsConstants;
import com.android.imsstack.util.ImsUtils;
import com.android.imsstack.util.Log;
import com.android.imsstack.util.MSimUtils;

import java.util.Locale;

public class ConfigDBLoaderImpl implements IConfigDBLoader {
    private static final String TAG = "ImsStack_ConfigDBLoaderImpl";
    private static final String DB_INTEGRITY_FILE_NAME = "gims_integrity";
    private static final int MAX_ALL_TABLE_RETRY_COUNT = 3;
    private static final int MAX_TABLE_RETRY_COUNT = 5;

    private static final int NO_ERROR = 0;
    private static final int ERROR_UNSPECIFIED = 1;
    private static final int ERROR_UNKNOWN_TABLE = 2;
    private static final int ERROR_UNKNOWN_COLUMN = 3;

    private final int mSlotId;
    private final ConfigXMLLoader mXMLLoader;
    private SQLiteDatabase mImsDb = null;
    private int mLastError = NO_ERROR;

    public ConfigDBLoaderImpl(int slotId) {
        mSlotId = slotId;
        mXMLLoader = new ConfigXMLLoader(mSlotId);
    }

    @Override
    public boolean dbUpdate(Context context) {
        Log.i(TAG, "dbUpdate - slotId=" + mSlotId);

        if (init(context) != true) {
            release();
            Log.e(TAG, "init failed");
            return false;
        }

        String[] dbTableNames = mXMLLoader.getTableNames();

        if (dbTableNames == null || dbTableNames.length == 0) {
            Log.e(TAG, "No table names");
            release();
            return false;
        }

        boolean hasIntegrityFile = mXMLLoader.checkDBIntegrityFile(DB_INTEGRITY_FILE_NAME);
        boolean isConfigChanged = mXMLLoader.isConfigChanged(mImsDb);
        boolean dbUpdateRequired = hasIntegrityFile || isConfigChanged;

        Log.d(TAG, "dbUpdate - hasIntegrityFiled=" + hasIntegrityFile
                + ", isConfigChanged=" + isConfigChanged);

        // RecoveryOnRebootOrImsRestart : Row-count check
        if (!dbUpdateRequired) {
            if (!checkConfigValueIntegrity(dbTableNames)) {
                dbUpdateRequired = true;
            }
        }

        boolean dbRecreationRequired = false;

        if (dbUpdateRequired) {
            mXMLLoader.createDBIntegrityFile(DB_INTEGRITY_FILE_NAME);

            int retryCount = 0;

            while (true) {
                if (dbProcess(dbTableNames)) {
                    break;
                } else if (isDbRecreationRequired()) {
                    downgradeDbVersion();
                    dbRecreationRequired = true;
                    break;
                }

                // RecoveryOnException : Row-delete or Row-insert
                retryCount++;

                if (retryCount > MAX_ALL_TABLE_RETRY_COUNT) {
                    break;
                }

                SystemClock.sleep(200);

                Log.i(TAG, "dbUpdate :: retry=" + retryCount);
            }

            if (retryCount > MAX_ALL_TABLE_RETRY_COUNT) {
                release();
                Log.e(TAG, "dbUpdate - failed");
                return false;
            }

            mXMLLoader.deleteDBIntegrityFile(DB_INTEGRITY_FILE_NAME);
        }

        release();

        if (dbRecreationRequired) {
            Log.w(TAG, "dbUpdate :: ims-restart");
            ImsUtils.killProcess(0);
        }

        Log.i(TAG, "dbUpdate - completed; slotId=" + mSlotId);

        return true;
    }

    // This method is triggered by hidden menu.
    @Override
    public boolean dbUpdateByCompulsion(Context context) {
        Log.i(TAG, "dbUpdateByCompulsion - slotId=" + mSlotId);

        if (init(context) != true) {
            release();
            Log.e(TAG, "init failed");
            return false;
        }

        String[] dbTableNames = mXMLLoader.getTableNames();

        if (dbTableNames == null || dbTableNames.length == 0) {
            Log.e(TAG, "No table names");
            release();
            return false;
        }

        mXMLLoader.createDBIntegrityFile(DB_INTEGRITY_FILE_NAME);

        if (dbProcess(dbTableNames) != true) {
            release();
            return false;
        }

        mXMLLoader.deleteDBIntegrityFile(DB_INTEGRITY_FILE_NAME);

        release();

        Log.i(TAG, "dbUpdateByCompulsion - completed");

        return true;
    }

    @Override
    public ConfigXMLLoader getXmlLoader() {
        return mXMLLoader;
    }

    private boolean init(Context context) {
        mXMLLoader.init(context);

        mImsDb = DBUtils.DB.open(ProviderInterface.DBFP, SQLiteDatabase.OPEN_READWRITE);

        return (mImsDb != null);
    }

    private void release() {
        DBUtils.DB.close(mImsDb);
        mImsDb = null;
    }

    private boolean dbProcess(String[] dbTableNames) {
        Log.i(TAG, "dbProcess");

        // "gims_engine" table will be update for only one table based on the build type.
        // So, the initial value should be 1. (gims_engine_debug)
        int dbTableCount = 1;
        boolean logDisabled = !ImsConstants.DBG;
        String whereClause = String.format(Locale.US, "%s=%d", ProviderInterface.ID, mSlotId);

        mImsDb.beginTransaction();

        try {
            for (int i = 0; i < dbTableNames.length; i++) {
                String[][] array = mXMLLoader.getTableElements(i);

                if (array == null) {
                    Log.e(TAG, "Content is null");
                    return false;
                }

                String dbTableName = dbTableNames[i];

                if (dbTableName.contains(ProviderInterface.Engine.TABLE_NAME)) {
                    // Setting log level for release version
                    if (logDisabled) {
                        if (ProviderInterface.Engine.TABLE_NAME.equals(dbTableName)) {
                            // Do update for Engine table
                        } else {
                            // Update "gims_engine_debug" table
                            if (MSimUtils.isMultiSimEnabled()) {
                                updateConfigValueWithRetry(dbTableName, array, whereClause);
                            }

                            dbTableName = null;
                        }
                    } else {
                        // For userdebug, eng, or etc...
                        if (ProviderInterface.Engine.DEBUG_TABLE_NAME.equals(dbTableName)) {
                            // Update "gims_engine_debug" table
                            if (MSimUtils.isMultiSimEnabled()) {
                                updateConfigValueWithRetry(dbTableName, array, whereClause);
                            }

                            dbTableName = ProviderInterface.Engine.TABLE_NAME;
                        } else {
                            dbTableName = null;
                        }
                    }
                }

                if (dbTableName == null) {
                    continue;
                }

                if (updateConfigValueWithRetry(dbTableName, array, whereClause)) {
                    dbTableCount++;
                } else if (isDbRecreationRequired()) {
                    Log.w(TAG, "dbProcess - stopped; DB re-creation required.");
                    return false;
                }
            }

            if (dbTableNames.length != dbTableCount) {
                Log.e(TAG, "dbProcess :: failed - tables=" + dbTableNames.length
                        + ", updatedTables=" + dbTableCount);
                return false;
            }

            mImsDb.setTransactionSuccessful();

            Log.i(TAG, "dbProcess - completed; tables=" + dbTableNames.length);
            return true;
        } finally {
            DBUtils.DB.endTransaction(mImsDb);
        }
    }

    private boolean checkConfigValueIntegrity(String[] dbTableNames) {
        boolean dbValid = true;
        String whereClause = String.format(Locale.US, "%s=%d", ProviderInterface.ID, mSlotId);

        for (int i = 0; i < dbTableNames.length; i++) {
            // Check whether the content values are present or not
            if (DBUtils.DB.getRowCount(mImsDb, dbTableNames[i], whereClause) == 0) {
                if (getTableInConfigXml(dbTableNames[i]) != 0) {
                    Log.w(TAG, "checkConfigValueIntegrity :: wrong-table=" + dbTableNames[i]);
                    dbValid = false;
                }
            }
        }

        Log.i(TAG, "checkConfigValueIntegrity :: " + (dbValid ? "OK" : "Not OK"));

        return dbValid;
    }

    private void downgradeDbVersion() {
        if (mImsDb != null) {
            int dbVersion = mImsDb.getVersion();
            mImsDb.setVersion(dbVersion - 1);

            Log.i(TAG, "DB-Version :: " + dbVersion + " >> " + mImsDb.getVersion());
        }
    }

    private boolean updateConfigValueWithRetry(String dbTable,
            String[][] contents, String whereClause) {
        int retryCount = 0;

        while (true) {
            if (updateConfigValue(dbTable, contents, whereClause)) {
                return true;
            } else if (isDbRecreationRequired()) {
                return false;
            }

            // RecoveryOnException : Row-delete or Row-insert
            retryCount++;

            if (retryCount > MAX_TABLE_RETRY_COUNT) {
                break;
            }

            SystemClock.sleep(100);

            Log.i(TAG, "updateConfigValueWithRetry :: retry=" + retryCount);
        }

        return false;
    }

    private boolean updateConfigValue(String dbTable, String[][] contents, String whereClause) {
        Log.i(TAG, "updateConfigValue - " + subString(dbTable,
                !ImsConstants.DBG ? null : "_", 4, true));

        if (!deleteRow(mImsDb, dbTable, whereClause)) {
            Log.e(TAG, "updateConfigValue :: delete failed");
            return false;
        }

        if (!insertRow(mImsDb, dbTable, contents)) {
            Log.e(TAG, "updateConfigValue :: insert failed");
            return false;
        }

        return true;
    }

    private boolean insertRow(SQLiteDatabase db, String table, String[][] contents) {
        if (table == null || contents == null || contents.length == 0) {
            setLastError(ERROR_UNSPECIFIED);
            return false;
        }

        // Prepare content values
        ContentValues values = new ContentValues();

        for (int i = 0; i < contents.length; ++i) {
            // '1' - column name, '2' - column default value
            values.put(contents[i][1], contents[i][2]);
        }

        try {
            long rowId = db.insertOrThrow(table, null, values);

            if (rowId >= 0) {
                setLastError(NO_ERROR);
            } else {
                setLastError(ERROR_UNSPECIFIED);
            }
        } catch (Exception e) {
            setLastError(ERROR_UNSPECIFIED);

            String errorMsg = e.getMessage();

            if (e instanceof SQLiteException) {
                if (errorMsg.startsWith("no such table")) {
                    setLastError(ERROR_UNKNOWN_TABLE);

                    if (getTableInConfigXml(table) == 0) {
                        // Do not validate this table because it's only for this operator.
                        // So, this table will not be created.
                        Log.w(TAG, "No table in Config-XML; table=" + table);
                        setLastError(NO_ERROR);
                    }
                } else if (errorMsg.contains("no column named")) {
                    setLastError(ERROR_UNKNOWN_COLUMN);

                    int colCountInConfigXml = getColumnCountInConfigXml(table);

                    Log.i(TAG, "No column named :: columnCountInConfigXml=" + colCountInConfigXml
                            + ", currentColumnCount=" + contents.length);

                    if (colCountInConfigXml < contents.length) {
                        // Do not validate this column because it's only for this operator.
                        // So, it will try to insert a row using the columns in DB.
                        Log.w(TAG, "No column in Config-XML; " + subString(errorMsg, ":", 0, false));

                        if (insertRowUsingDBColumns(db, table, contents)) {
                            setLastError(NO_ERROR);
                        }
                    }
                }
            }

            if (getLastError() != NO_ERROR) {
                Log.w(TAG, "Exception :: " + errorMsg);
                e.printStackTrace();
            }
        }

        return getLastError() == NO_ERROR;
    }

    private boolean insertRowUsingDBColumns(SQLiteDatabase db,
            String table, String[][] contents) {
        Cursor cursor = null;

        try {
            cursor = db.rawQuery("SELECT * FROM " + table, null);
        } catch (Exception e) {
            return false;
        }

        if (cursor == null) {
            return false;
        }

        // Prepare content values
        ContentValues values = new ContentValues();

        for (int i = 0; i < contents.length; ++i) {
            if (cursor.getColumnIndex(contents[i][1]) >= 0) {
                // '1' - column name, '2' - column default value
                values.put(contents[i][1], contents[i][2]);
            }
        }

        cursor.close();
        cursor = null;

        try {
            long rowId = db.insertOrThrow(table, null, values);
            return (rowId >= 0);
        } catch (Exception e) {
            Log.w(TAG, "Exception :: " + e.getMessage());
        }

        return false;
    }

    private boolean deleteRow(SQLiteDatabase db, String table, String where) {
        if (table == null || where == null) {
            setLastError(ERROR_UNSPECIFIED);
            return false;
        }

        // Execute "DELETE" query statement
        try {
            db.delete(table, where, null);
            setLastError(NO_ERROR);
        } catch (Exception e) {
            setLastError(ERROR_UNSPECIFIED);

            String errorMsg = e.getMessage();

            if (e instanceof SQLiteException) {
                if (errorMsg.startsWith("no such table")) {
                    setLastError(ERROR_UNKNOWN_TABLE);

                    if (getTableInConfigXml(table) == 0) {
                        // Do not validate this table because it's only for this operator.
                        // So, it will not be created at this moment.
                        Log.w(TAG, "No table in Config-XML; table=" + table);
                        setLastError(NO_ERROR);
                    }
                }
            }

            if (getLastError() != NO_ERROR) {
                Log.w(TAG, "Exception :: " + errorMsg);
                e.printStackTrace();
            }
        }

        return getLastError() == NO_ERROR;
    }

    public int getColumnCountInConfigXml(String tableName) {
        ContentResolver cr = AppContext.get().getContentResolver();
        Uri contentUri = Uri.parse("content://" + ProviderInterface.AUTHORITY);

        try {
            Bundle result = cr.call(contentUri,
                    ProviderInterface.METHOD_GET_COLUMN_COUNT_IN_CONFIG_XML, tableName, null);
            return (result != null) ? result.getInt(ProviderInterface.KEY_RESULT, 0) : 0;
        } catch (NullPointerException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }

        return 0;
    }

    public int getTableInConfigXml(String tableName) {
        ContentResolver cr = AppContext.get().getContentResolver();
        Uri contentUri = Uri.parse("content://" + ProviderInterface.AUTHORITY);

        try {
            Bundle result = cr.call(contentUri,
                    ProviderInterface.METHOD_GET_TABLE_IN_CONFIG_XML, tableName, null);
            return (result != null) ? result.getInt(ProviderInterface.KEY_RESULT, 0) : 0;
        } catch (NullPointerException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }

        return 0;
    }

    private boolean isDbRecreationRequired() {
        int lastError = getLastError();
        return (lastError == ERROR_UNKNOWN_COLUMN) || (lastError == ERROR_UNKNOWN_TABLE);
    }

    private int getLastError() {
        return mLastError;
    }

    private void setLastError(int error) {
        if (mLastError != error) {
            Log.i(TAG, "setLastError :: " + mLastError + " >> " + error);
            mLastError = error;
        }
    }

    private static String subString(String s, String delimiter,
            int prefixSizeForFromLast, boolean fromLast) {
        if (s == null) {
            return "(null)";
        } else if (s.isEmpty()) {
            return "(empty)";
        } else {
            if (fromLast) {
                int index = (delimiter == null) ? (-1) : s.lastIndexOf(delimiter);

                if (index >= prefixSizeForFromLast) {
                    index -= prefixSizeForFromLast;
                }

                return (index < 0) ? s : s.substring(index + 1, s.length());
            } else {
                int index = (delimiter == null) ? (-1) : s.indexOf(delimiter);
                return (index < 0) ? s : s.substring(0, index);
            }
        }
    }
}
