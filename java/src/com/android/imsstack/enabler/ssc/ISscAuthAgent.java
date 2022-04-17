package com.android.imsstack.enabler.ssc;

public interface ISscAuthAgent {
    boolean calculateResponse(String method, String uri, String body);
    String getCredentialInfoString();

    void setGbaKeys(String username, String password);
    String getCipherSuite();
    void setCipherSuite(String cipherSuite);
    String getETag();
    void setETag(String tag);
    String getNafFqdnFromRealm();

    void parse(String wwwAuthenticate);

    boolean isCredentialInfoUpdated();
    void setIsCredentialInfoUpdated(boolean updated);

    String getRealm();
}
