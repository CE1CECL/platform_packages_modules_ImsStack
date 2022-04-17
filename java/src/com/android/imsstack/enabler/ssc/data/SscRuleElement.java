package com.android.imsstack.enabler.ssc.data;

public class SscRuleElement {
    private String key;
    private String value;

    public SscRuleElement(String key, String value) {
        this.key = key;
        this.value = value;
    }

    public String getKey() {
        return key;
    }

    public String getValue() {
        return value;
    }

    public String toString() {
        return "key : " + key + ", value : " + value + "\n";
    }
}
