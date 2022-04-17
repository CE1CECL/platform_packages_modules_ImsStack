package com.android.imsstack.core.config;

import android.content.Context;

public interface IConfigDBLoader {
    /**
     * dbUpdate
     *
     * Update DB based on Operator Configuration xml file.
     *
     * This method check 2 condition
     *   1) is Databases need to update ? <-- if configuration xml file is more recent, db needs update.
     *   2) is Integrity file exist ? <-- this means former db creation is failed, and it need update also.
     *
     */
    public boolean dbUpdate(Context context);


    /**
     * dbUpdateByCompulsion
     *
     * Update DB based on Operator Configuration xml file,
     *
     * This method doesn't check update condition and update database based on configuration xml file by compulsion.
     *
     */
    public boolean dbUpdateByCompulsion(Context context);

    /**
     * Returns the configuration XML loader.
     */
    public ConfigXMLLoader getXmlLoader();
}
