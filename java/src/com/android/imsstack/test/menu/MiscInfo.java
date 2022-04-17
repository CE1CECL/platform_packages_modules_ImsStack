package com.android.imsstack.test.menu;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.android.imsstack.R;
import com.android.imsstack.core.config.ProviderInterface;
import com.android.imsstack.core.config.SmartConfigProviderInterface;
import com.android.imsstack.util.DBUtils;
import com.android.imsstack.util.ImsPrivateProperties;

public class MiscInfo extends Activity {
    private TextView mObjContents;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setTitle("Misc");
        setContentView(R.xml.miscinfo);

        setFindView();
        setDataFromPreference();
        setVisibility();
    }

    @Override
    public void onStop() {
        super.onStop();
        this.finish();
    }

    private void setFindView() {
        mObjContents = (TextView)findViewById(R.id.MiscTextContent);
    }

    private void setDataFromPreference() {
        StringBuilder sb = new StringBuilder();

        sb.append("# DB");
        sb.append(System.lineSeparator());
        sb.append("gims.db :: "
                + ImsPrivateProperties.Persistent.getInt(
                        ImsPrivateProperties.Persistent.KEY_DB_IMS_OLD_VERSION,
                        0, 0)
                + " >> "
                + DBUtils.DB.getVersion(ProviderInterface.DBFP));
        sb.append(System.lineSeparator());
        sb.append("smart_configuration.db :: "
                + ImsPrivateProperties.Persistent.getInt(
                        ImsPrivateProperties.Persistent.KEY_DB_SMART_CONFIGURATION_OLD_VERSION,
                        0, 0)
                + " >> "
                + DBUtils.DB.getVersion(SmartConfigProviderInterface.DB_FULL_PATH));
        sb.append(System.lineSeparator());

        mObjContents.setText(sb.toString());
    }

    private void setVisibility() {
        mObjContents.setVisibility(View.VISIBLE);
    }
}
