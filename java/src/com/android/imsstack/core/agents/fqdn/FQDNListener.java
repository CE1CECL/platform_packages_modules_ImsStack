package com.android.imsstack.core.agents.fqdn;

import java.util.List;

public interface FQDNListener {

    final int RESULT_FQDN_SUCCESS = 1;
    final int RESULT_FQDN_FAIL = 2;

    void onResolved(int nResult, List<Address> objAddressList);
}
