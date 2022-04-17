package com.android.imsstack.enabler.ssc;

public enum ESsType {

    /**
     * NONE
     */
    NONE (null , null),
    /**
     * Originating Identification Presentation
     */
    OIP ("originating-identity-presentation", "oip_oir.xsd"),
    /**
     * Originating Identification Restriction
     */
    OIR ("originating-identity-presentation-restriction", "oip_oir.xsd"),
    /**
     * Terminating Identification Presentation
     */
    TIP ("terminating-identity-presentation", "tip_tir.xsd"),
    /**
     * Termination Identification Restriction
     */
    TIR ("terminating-identity-presentation-restriction", "tip_tir.xsd"),

    /**
     * Communication Forwarding Default
     */
    CF ("communication-diversion", "cf.xsd"),
    /**
     * Communication Forwarding - Service Capability
     */
    CFSC ("communication-diversion-serv-cap", "cf_serv_cap.xsd"),

    /**
     * Communication Barring
     */
    OCB ("outgoing-communication-barring", "cb.xsd"),
    ICB ("incoming-communication-barring", "cb.xsd"),
    /**
     * FIXME :: To Handle icb for KDDI
     * Rename : ICB --> ICBA
     */
    ICBA ("incoming-communication-barring", "cb.xsd"),
    /**
     * Communication Barring - Service Capability
     */
    CBSC ("communication-barring-serv-cap", "cb_serv_cap.xsd"),

    /**
     * Communication Waiting
     */
    CW ("communication-waiting", "cw.xsd"),

    /**
     * xcap error
     */
    XE ("xcap-error", "xe.xsd");


    private String mSSName = null;
    private String mSchemaFileName = null;

    private ESsType(String name, String xsd) {
        this.mSSName = name;
        this.mSchemaFileName = xsd;
    }

    public String getSSName() {
        return this.mSSName;
    }

    public String getSchema() {
        return this.mSchemaFileName;
    }

    public String toString() {
        return this.name();
    }
}
