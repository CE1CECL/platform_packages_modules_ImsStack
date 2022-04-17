package com.android.imsstack.util;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteFullException;
import android.net.Uri;
import android.text.TextUtils;

import java.util.Locale;

/**
 * This class provides the APIs to access SQLiteDatabase.
 */
public final class DBUtils {
    public static final String DB_FIELD_ID = "id";
    public static final long ERROR_INSERT = -1;

    /**
     * Common methods for Cursor
     */
    public static void closeCursor(Cursor cursor) {
        if (cursor != null) {
            cursor.close();
        }
    }

    public static int getInt(Cursor cursor, String columnName, int defaultValue) {
        if (cursor != null) {
            try {
                int index = cursor.getColumnIndex(columnName);
                int value = (index < 0) ? defaultValue : cursor.getInt(index);

                return value;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }
        }

        return defaultValue;
    }

    public static int getHex(Cursor cursor, String columnName, int defaultValue) {
        if (cursor != null) {
            try {
                int index = cursor.getColumnIndex(columnName);
                int hexValue = 0x00;
                String strValue;
                if (index < 0) {
                    hexValue = defaultValue;
                }
                else {
                    strValue = cursor.getString(index);
                    if (strValue.startsWith("0x")) {
                        strValue = strValue.substring(2);
                    }
                    hexValue = Integer.parseInt(strValue, 16);
                }

                return hexValue;
            }
            catch (NumberFormatException e) {
                loge(e.toString());
                e.printStackTrace();
            }
            catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }
        }

        return defaultValue;
    }

    public static String getString(Cursor cursor, String columnName, String defaultValue) {
        if (cursor != null) {
            try {
                int index = cursor.getColumnIndex(columnName);
                String value = (index < 0) ? defaultValue : cursor.getString(index);

                return value;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }
        }

        return defaultValue;
    }

    public static String selectForSlotId(int slotId) {
        return String.format(Locale.US, "%s='%d'", DB_FIELD_ID, slotId);
    }

    /**
     * Utility methods for SQLiteDatabase
     */
    public static class DB {
        public static SQLiteDatabase open(String path, int flags) {
            log("open :: file=" + Log.firstSubString(Log.lastSubString(path, "/"), ".", true)
                    + "*, flags=" + flags);

            try {
                return SQLiteDatabase.openDatabase(path, null, flags);
            } catch (SQLiteException e) {
                e.printStackTrace();
                return null;
            }
        }

        public static void close(SQLiteDatabase db) {
            if (db != null) {
                db.close();
            }
        }

        public static int getVersion(String path) {
            int version = 0;
            SQLiteDatabase db = open(path, SQLiteDatabase.OPEN_READONLY);

            if (db != null) {
                version = db.getVersion();
                close(db);
            }

            return version;
        }

        public static boolean hasTable(SQLiteDatabase db, String table) {
            boolean hasTable = false;
            String query = "SELECT DISTINCT tbl_name FROM sqlite_master WHERE tbl_name = '"
                    + table + "'";

            Cursor c = getCursor(db, query);

            if (c != null && c.moveToFirst()) {
                if (c.getCount() > 0 && c.getColumnCount() > 0) {
                    String tableInDB = c.getString(0);
                    if (table.equals(tableInDB)) {
                        hasTable = true;
                    }
                }
            }

            DBUtils.closeCursor(c);

            return hasTable;
        }

        public static int getRowCount(SQLiteDatabase db, String table) {
            Cursor c = getCursor(db, table, "");
            int count = (c != null) ? c.getCount() : 0;

            DBUtils.closeCursor(c);

            return count;
        }

        public static int getRowCount(SQLiteDatabase db, String table, String selection) {
            Cursor c = getCursor(db, table, selection);
            int count = (c != null) ? c.getCount() : 0;

            DBUtils.closeCursor(c);

            return count;
        }

        public static int getRowCount(int slotId, SQLiteDatabase db, String table) {
            int count = getRowCount(db, table, selectForSlotId(slotId));
            log("RowCount :: " + slotId
                    + ":" + Log.firstSubString(table, "_", false) + "=" + count);
            return count;
        }

        public static Cursor getCursor(SQLiteDatabase db, String query) {
            if ((db == null) || (query == null)) {
                loge("getCursor :: db=" + db + ", query=" + query);
                return null;
            }

            try {
                return db.rawQuery(query, null);
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }

            return null;
        }

        public static Cursor getCursor(SQLiteDatabase db, String table, String selection) {
            if ((db == null) || (table == null)) {
                loge("getCursor :: db=" + db + ", table=" + table);
                return null;
            }

            String query = "SELECT * FROM " + table;

            if (!TextUtils.isEmpty(selection)) {
                query += " WHERE " + selection;
            }

            return getCursor(db, query);
        }

        public static Cursor getCursor(int slotId, SQLiteDatabase db, String table) {
            return getCursor(db, table, selectForSlotId(slotId));
        }

        public static Cursor getFirstCursor(SQLiteDatabase db, String table, String selection) {
            Cursor c = getCursor(db, table, selection);

            if ((c != null) && c.moveToFirst()) {
                return c;
            }

            closeCursor(c);

            return null;
        }

        public static Cursor getFirstCursor(int slotId, SQLiteDatabase db, String table) {
            return getFirstCursor(db, table, selectForSlotId(slotId));
        }

        public static int getHex(SQLiteDatabase db, String table, String selection,
                String columnName, int defaultValue) {
            Cursor c = getFirstCursor(db, table, selection);

            int value = defaultValue;
            if (c != null) {
                value = DBUtils.getHex(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static int getHex(int slotId, SQLiteDatabase db, String table,
                String columnName, int defaultValue) {
            return getHex(db, table, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static int getInt(SQLiteDatabase db, String table, String selection,
                String columnName, int defaultValue) {
            Cursor c = getFirstCursor(db, table, selection);

            int value = defaultValue;
            if (c != null) {
                value = DBUtils.getInt(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static int getInt(int slotId, SQLiteDatabase db, String table,
                String columnName, int defaultValue) {
            return getInt(db, table, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static String getString(SQLiteDatabase db, String table, String selection,
                String columnName, String defaultValue) {
            Cursor c = getFirstCursor(db, table, selection);

            String value = defaultValue;
            if (c != null) {
                value = DBUtils.getString(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static String getString(int slotId, SQLiteDatabase db, String table,
                String columnName, String defaultValue) {
            return getString(db, table, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static boolean putHex(SQLiteDatabase db, String table,
                String key, int value, String selection) {
            String strValue = String.format("0x%08X", value);

            return putString(db, table, key, strValue, selection);
        }

        public static boolean putHex(int slotId, SQLiteDatabase db, String table,
                String key, int value) {
            return putHex(db, table, key, value, selectForSlotId(slotId));
        }

        public static boolean putInt(SQLiteDatabase db, String table,
                String key, int value, String selection) {
            ContentValues values = new ContentValues();

            values.put(key, value);

            return update(db, table, values, selection, null, true) > 0;
        }

        public static boolean putInt(int slotId, SQLiteDatabase db, String table,
                String key, int value) {
            return putInt(db, table, key, value, selectForSlotId(slotId));
        }

        public static boolean putString(SQLiteDatabase db, String table,
                String key, String value, String selection) {
            ContentValues values = new ContentValues();

            values.put(key, value);

            return update(db, table, values, selection, null, true) > 0;
        }

        public static boolean putString(int slotId, SQLiteDatabase db, String table,
                String key, String value) {
            return putString(db, table, key, value, selectForSlotId(slotId));
        }

        public static boolean createTable(SQLiteDatabase db, String table, String columns) {
            if ((db == null) || (table == null) || (columns == null)) {
                loge("createTable :: db=" + db + ", table=" + table + ", columns=" + columns);
                return false;
            }

            db.beginTransaction();

            try {
                db.execSQL("CREATE TABLE IF NOT EXISTS " + table + " (" + columns + ");");
                db.setTransactionSuccessful();
                return true;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            } finally {
                endTransaction(db);
            }

            return false;
        }

        public static boolean dropTable(SQLiteDatabase db, String table) {
            if ((db == null) || (table == null)) {
                loge("dropTable :: db=" + db + ", table=" + table);
                return false;
            }

            db.beginTransaction();

            try {
                db.execSQL("DROP TABLE IF EXISTS " + table);
                db.setTransactionSuccessful();
                return true;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            } finally {
                endTransaction(db);
            }

            return false;
        }

        /**
         * @return the row ID of the newly inserted rows, or ERROR_INSERT if an error occurred
         */
        public static long insert(SQLiteDatabase db, String table, ContentValues values) {
            if ((db == null) || (table == null) || (values == null)) {
                loge("insert :: db=" + db + ", table=" + table + ", values=" + values);
                return ERROR_INSERT;
            }

            db.beginTransaction();

            try {
                long rowId = db.insertOrThrow(table, null, values);
                db.setTransactionSuccessful();
                return rowId;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            } finally {
                endTransaction(db);
            }

            return ERROR_INSERT;
        }

        /**
         * @return the row ID of the newly inserted rows, or ERROR_INSERT if an error occurred
         */
        public static long insertOrThrow(SQLiteDatabase db, String table, ContentValues values)
                throws SQLiteException {
            if ((db == null) || (table == null) || (values == null)) {
                loge("insertOrThrow :: db=" + db + ", table=" + table + ", values=" + values);
                return ERROR_INSERT;
            }

            db.beginTransaction();

            try {
                long rowId = db.insertOrThrow(table, null, values);
                db.setTransactionSuccessful();
                return rowId;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();

                if (t instanceof SQLiteException) {
                    throw new SQLiteException(t.getMessage(), t.getCause());
                }
            } finally {
                endTransaction(db);
            }

            return ERROR_INSERT;
        }

        public static boolean delete(SQLiteDatabase db, String table,
                String whereClause, String[] whereArgs) {
            if ((db == null) || (table == null)) {
                loge("delete :: db=" + db + ", table=" + table);
                return false;
            }

            db.beginTransaction();

            try {
                db.delete(table, whereClause, whereArgs);
                db.setTransactionSuccessful();
                return true;
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            } finally {
                endTransaction(db);
            }

            return false;
        }

        public static boolean delete(int slotId, SQLiteDatabase db, String table) {
            return delete(db ,table, selectForSlotId(slotId), null);
        }

        /**
         * @return the number of rows updated
         */
        public static int update(SQLiteDatabase db, String table,
                ContentValues values, String whereClause, String[] whereArgs, boolean exclusive) {
            if ((db == null) || (table == null)) {
                loge("update :: db=" + db + ", table=" + table);
                return 0;
            }

            try {
                if (exclusive) {
                    db.beginTransaction();
                } else {
                    db.beginTransactionNonExclusive();
                }

                try {
                    int affectedRows = db.update(table, values, whereClause, whereArgs);
                    db.setTransactionSuccessful();
                    return affectedRows;
                } catch (Throwable t) {
                    loge(t.toString());
                    t.printStackTrace();
                } finally {
                    endTransaction(db);
                }
            } catch (Exception e) {
                loge("Exception :: " + e.toString());
                e.printStackTrace();
            }

            return 0;
        }

        public static int update(int slotId, SQLiteDatabase db, String table,
                ContentValues values, boolean exclusive) {
            return update(db ,table, values, selectForSlotId(slotId), null, exclusive);
        }

        public static boolean execSQL(SQLiteDatabase db, String sql) {
            if ((db == null) || (sql == null)) {
                loge("execSQL :: db=" + db + ", sql=" + sql);
                return false;
            }

            try {
                db.execSQL(sql);
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
                return false;
            }

            return true;
        }

        public static void endTransaction(SQLiteDatabase db) {
            if (db == null) {
                return;
            }

            try {
                db.endTransaction();
            } catch (SQLiteFullException e) {
                loge("SQLiteFullException :: " + e.toString());
                e.printStackTrace();
            } catch (SQLiteException e) {
                loge("SQLiteException :: " + e.toString());
                e.printStackTrace();
            } catch (Exception e) {
                loge("Exception :: " + e.toString());
                e.printStackTrace();
            }
        }

        public static class ImmediateMode {
            public static boolean putHex(SQLiteDatabase db, String table,
                    String key, int value, String selection) {
                String strValue = String.format("0x%08X", value);
                return ImmediateMode.putString(db, table, key, strValue, selection);
            }

            public static boolean putHex(int slotId, SQLiteDatabase db, String table,
                    String key, int value) {
                return ImmediateMode.putHex(db, table, key, value, selectForSlotId(slotId));
            }

            public static boolean putInt(SQLiteDatabase db, String table,
                    String key, int value, String selection) {
                ContentValues values = new ContentValues();

                values.put(key, value);

                return DB.update(db, table, values, selection, null, false) > 0;
            }

            public static boolean putInt(int slotId, SQLiteDatabase db, String table,
                    String key, int value) {
                return ImmediateMode.putInt(db, table, key, value, selectForSlotId(slotId));
            }

            public static boolean putString(SQLiteDatabase db, String table,
                    String key, String value, String selection) {
                ContentValues values = new ContentValues();

                values.put(key, value);

                return DB.update(db, table, values, selection, null, false) > 0;
            }

            public static boolean putString(int slotId, SQLiteDatabase db, String table,
                    String key, String value) {
                return ImmediateMode.putString(db, table, key, value, selectForSlotId(slotId));
            }
        }
    }

    /**
     * Utility methods for ContentProvider
     */
    public static class CP {
        public static Cursor getCursor(ContentResolver cr, Uri contentUri, String selection) {
            if ((cr == null) || (contentUri == null)) {
                loge("getCursor :: cr=" + cr + ", uri=" + contentUri);
                return null;
            }

            try {
                return cr.query(contentUri, null, selection, null, null);
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }

            return null;
        }

        public static Cursor getCursor(int slotId, ContentResolver cr, Uri contentUri) {
            return getCursor(cr, contentUri, selectForSlotId(slotId));
        }

        public static Cursor getFirstCursor(ContentResolver cr, Uri contentUri, String selection) {
            Cursor c = getCursor(cr, contentUri, selection);

            if ((c != null) && c.moveToFirst()) {
                return c;
            }

            closeCursor(c);

            return null;
        }

        public static Cursor getFirstCursor(int slotId, ContentResolver cr, Uri contentUri) {
            return getFirstCursor(cr, contentUri, selectForSlotId(slotId));
        }

        public static int getHex(ContentResolver cr, Uri contentUri, String selection,
                String columnName, int defaultValue) {
            Cursor c = getFirstCursor(cr, contentUri, selection);

            int value = defaultValue;
            if (c != null) {
                value = DBUtils.getHex(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static int getHex(int slotId, ContentResolver cr, Uri contentUri,
                String columnName, int defaultValue) {
            return getHex(cr, contentUri, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static int getInt(ContentResolver cr, Uri contentUri, String selection,
                String columnName, int defaultValue) {
            Cursor c = getFirstCursor(cr, contentUri, selection);

            int value = defaultValue;
            if (c != null) {
                value = DBUtils.getInt(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static int getInt(int slotId, ContentResolver cr, Uri contentUri,
                String columnName, int defaultValue) {
            return getInt(cr, contentUri, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static String getString(ContentResolver cr, Uri contentUri, String selection,
                String columnName, String defaultValue) {
            Cursor c = getFirstCursor(cr, contentUri, selection);

            String value = defaultValue;
            if (c != null) {
                value = DBUtils.getString(c, columnName, defaultValue);
                DBUtils.closeCursor(c);
            }

            return value;
        }

        public static String getString(int slotId, ContentResolver cr, Uri contentUri,
                String columnName, String defaultValue) {
            return getString(cr, contentUri, selectForSlotId(slotId), columnName, defaultValue);
        }

        public static boolean putInt(ContentResolver cr, Uri contentUri,
                String key, int value, String selection) {
            ContentValues values = new ContentValues();

            values.put(key, value);

            return update(cr, contentUri, values, selection, null) > 0;
        }

        public static boolean putInt(int slotId, ContentResolver cr, Uri contentUri,
                String key, int value) {
            return putInt(cr, contentUri, key, value, selectForSlotId(slotId));
        }

        public static boolean putString(ContentResolver cr, Uri contentUri,
                String key, String value, String selection) {
            ContentValues values = new ContentValues();

            values.put(key, value);

            return update(cr, contentUri, values, selection, null) > 0;
        }

        public static boolean putString(int slotId, ContentResolver cr, Uri contentUri,
                String key, String value) {
            return putString(cr, contentUri, key, value, selectForSlotId(slotId));
        }

        /**
         * @return the URL of the newly created now, or null if an error occurred
         */
        public static Uri insert(ContentResolver cr, Uri contentUri, ContentValues values) {
            if ((cr == null) || (contentUri == null)) {
                loge("insert :: cr=" + cr + ", uri=" + contentUri);
                return null;
            }

            try {
                return cr.insert(contentUri, values);
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }

            return null;
        }

        /**
         * @return the number of rows updated
         */
        public static int update(ContentResolver cr, Uri contentUri,
                ContentValues values, String selection, String[] selectionArgs) {
            if ((cr == null) || (contentUri == null)) {
                loge("update :: cr=" + cr + ", uri=" + contentUri);
                return 0;
            }

            try {
                return cr.update(contentUri, values, selection, selectionArgs);
            } catch (Throwable t) {
                loge(t.toString());
                t.printStackTrace();
            }

            return 0;
        }

        public static int update(int slotId, ContentResolver cr, Uri contentUri,
                ContentValues values) {
            return update(cr, contentUri, values, selectForSlotId(slotId), null);
        }
    }

    private static void log(String s) {
        Log.d(Log.TAG, "[DBUtils] " + s);
    }

    private static void loge(String s) {
        Log.e(Log.TAG, "[DBUtils] " + s);
    }
}
