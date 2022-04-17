/**
 * SettingMenu
 * Role
 *         SettingMenu class is a abstract class which is need to implements each setting menu.
 *         This class contain basic methods that are using all over the setting menu.
 */

package com.android.imsstack.test.menu;

import android.database.Cursor;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.MultiSelectListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.view.inputmethod.EditorInfo;

import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.test.menu.SettingUtil;
import com.android.imsstack.util.ImsLog;
import com.android.imsstack.util.ImsProperties;

import java.util.HashSet;
import java.util.Locale;

public abstract class SettingMenu extends PreferenceActivity {

    protected CheckBoxPreference getCheckBoxPreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return null;
        }

        return (CheckBoxPreference)root.findPreference(key);
    }

    protected ListPreference getListPreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return null;
        }

        return (ListPreference)root.findPreference(key);
    }

    protected MultiSelectListPreference getMultiSelectListPreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return null;
        }

        return (MultiSelectListPreference)root.findPreference(key);
    }

    protected EditTextPreference getEditTextPreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return null;
        }

        return (EditTextPreference)root.findPreference(key);
    }

    protected Preference getPreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return null;
        }

        return root.findPreference(key);
    }

    protected void removePreference(String key) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return;
        }

        PreferenceScreen preference = (PreferenceScreen)root.findPreference(key);

        if (preference == null) {
            return;
        }

        root.removePreference(preference);
    }

    protected void removePreference(String upperpreference, String lowerpreference) {
        @SuppressWarnings("deprecation")
        PreferenceScreen root = getPreferenceScreen();

        if (root == null) {
            return;
        }

        PreferenceScreen upperPreference = (PreferenceScreen)root.findPreference(upperpreference);

        if (upperPreference == null) {
            return;
        }

        PreferenceScreen lowerPreference = (PreferenceScreen)root.findPreference(lowerpreference);

        if (lowerPreference == null) {
            return;
        }

        upperPreference.removePreference(lowerPreference);
    }

    protected void setEditTextAsNumberOnly(EditTextPreference edit) {
        if (edit == null) {
            return;
        }

        edit.getEditText().setInputType(EditorInfo.TYPE_CLASS_NUMBER);
    }

    protected void setListener(Preference preference,
            Preference.OnPreferenceChangeListener listener) {
        if (preference == null) {
            return;
        }

        preference.setOnPreferenceChangeListener(listener);
    }

    protected void setValue(Cursor cursor, int index, CheckBoxPreference checkbox) {
        if (checkParameterForSetValue(cursor, index, checkbox) == false) {
            return;
        }

        String value = cursor.getString(index);

        if (value == null) {
            return;
        }

        if (value.equalsIgnoreCase("true") || value.equalsIgnoreCase("1")) {
            checkbox.setChecked(true);
            checkbox.setSummary("true");
        } else {
            checkbox.setChecked(false);
            checkbox.setSummary("false");
        }
    }

    protected void setValue(Cursor cursor, int index, EditTextPreference edit) {
        if (checkParameterForSetValue(cursor, index, edit) == false) {
            return;
        }

        String value = cursor.getString(index);

        if (value == null) {
            return;
        }

        edit.setText(value);
        edit.setSummary(value);
    }

    protected void setValue(Cursor cursor, int index, ListPreference list) {
        if (checkParameterForSetValue(cursor, index, list) == false) {
            return;
        }

        String value = cursor.getString(index);

        if (value == null) {
            return;
        }

        int valueIndex = list.findIndexOfValue(value);

        if (valueIndex < 0) {
            if (value.contains(",")) {
                String[] tokens = value.split(",");

                if (tokens != null && tokens.length > 0) {
                    valueIndex = list.findIndexOfValue(tokens[0]);
                }
            }
        }

        if (valueIndex < 0) {
            ImsLog.d("index < 0 :: list - key=" + list.getKey() + ", value=" + value);
            return;
        }

        list.setValueIndex(valueIndex);
        list.setSummary(list.getEntry());
    }

    protected boolean setValue(Preference pref, String value) {
        if (pref instanceof EditTextPreference) {
            ((EditTextPreference)pref).setText(value);
        } else if (pref instanceof ListPreference) {
            int listIndex = ((ListPreference)pref).findIndexOfValue(value);

            if (listIndex == -1) {
                listIndex = 0;
            }

            ((ListPreference)pref).setValueIndex(listIndex);
        } else if (pref instanceof CheckBoxPreference) {
            if (value.compareToIgnoreCase("true") == 0) {
                ((CheckBoxPreference)pref).setChecked(true);
            } else {
                ((CheckBoxPreference)pref).setChecked(false);
            }
        } else if ( pref instanceof MultiSelectListPreference ) {
            String[] tokens = value.split(",");
            HashSet<String> values = new HashSet<String>();

            if (tokens.length == 0) {
                return false;
            }

            for (int i = 0; i < tokens.length; i++) {
                String newString = tokens[i].toUpperCase(Locale.ROOT);
                values.add(newString);
            }

            ((MultiSelectListPreference)pref).setValues(values);
        }

        return true;
    }

    protected void setValueForAdminFeatures(Cursor cursor,
            int index, CheckBoxPreference checkbox, int feature) {
        if (checkParameterForSetValue(cursor, index, checkbox) == false) {
            return;
        }

        int value = cursor.getInt(index);

        if (value < 0) {
            return;
        }

        if ((value & feature) == feature) {
            checkbox.setChecked(true);
            checkbox.setSummary("true");
        } else {
            checkbox.setChecked(false);
            checkbox.setSummary("false");
        }
    }

    protected void setCheckboxPreference(Cursor cursor, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        CheckBoxPreference checkbox;
        int index = 0;

        checkbox = getCheckBoxPreference(preference);
        setListener(checkbox, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, checkbox);
    }

    protected void setCheckboxPreferenceForAdminFeatures(Cursor cursor,
            String preference, int feature, String[] columns,
            Preference.OnPreferenceChangeListener listener) {
        CheckBoxPreference checkbox = getCheckBoxPreference(preference);
        setListener(checkbox, listener);

        int index = SettingUtil.getColumnIndex(columns, ProviderInterface.Subscriber.FEATURES);
        setValueForAdminFeatures(cursor, index, checkbox, feature);
    }

    protected void setListPreference(Cursor cursor, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        ListPreference list;
        int index = 0;

        list = getListPreference(preference);
        setListener(list, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, list);
    }

    protected void setEditTextPreference(Cursor cursor, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        EditTextPreference edit;
        int index = 0;

        edit = getEditTextPreference(preference);
        setListener(edit, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, edit);
    }

    protected void setNumberEditTextPreference (Cursor cursor, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        EditTextPreference edit = getEditTextPreference(preference);
        setEditTextPreference(cursor, preference, columns, listener);
        setEditTextAsNumberOnly(edit);
    }

    protected void setCheckboxPreference(Cursor cursor, String prefix, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        CheckBoxPreference checkbox;
        int index = 0;

        checkbox = getCheckBoxPreference(prefix + preference);
        setListener(checkbox, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, checkbox);
    }

    protected void setListPreference(Cursor cursor, String prefix, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        ListPreference list;
        int index = 0;

        list = getListPreference(prefix + preference);
        setListener(list, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, list);
    }

    protected void setEditTextPreference(Cursor cursor,  String prefix, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        EditTextPreference edit;
        int index = 0;

        edit = getEditTextPreference(prefix + preference);
        setListener(edit, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, edit);
    }

    protected void setNumberEditTextPreference (Cursor cursor,  String prefix, String preference,
            String[] columns, Preference.OnPreferenceChangeListener listener) {
        EditTextPreference edit;
        int index = 0;

        edit = getEditTextPreference(prefix + preference);
        setListener(edit, listener);

        index = SettingUtil.getColumnIndex(columns, preference);
        setValue(cursor, index, edit);

        setEditTextAsNumberOnly(edit);
    }

    protected boolean getAtCmdSupported() {
        return false;
    }

    protected void setAtCmdSupported(boolean value) {
        // no-op
    }

    protected int getImsRegistrationState() {
        return 0;
    }

    private boolean checkParameterForSetValue(Cursor cursor, int index, Preference preference) {
        if ((preference == null) || (index == (-1)) || (cursor == null)) {
            return false;
        }

        return true;
    }
}
