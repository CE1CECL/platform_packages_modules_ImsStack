package com.android.imsstack.core.agents.fqdn;

import com.android.imsstack.util.ImsLog;

public class Address {

    private String strProtocol;
    private String strAddress;
    private String strPort;
    private int nPriority;
    private int nWeight;
    private int nIdenx;
    private int nMax;

    public Address (String _protocol, String _addr, String _port, int _priority, int _weight
        ,int _index, int _max ) {

        ImsLog.i("" + _protocol + "/" + _addr + "/" + _port + "/" + _priority + "/" + _weight +"/"
            +_index + "/" + _max);
        strProtocol = _protocol;
        strAddress = _addr;
        strPort = _port;
        nPriority = _priority;
        nWeight = _weight;
        nIdenx = _index;
        nMax = _max;
    }

    public String getProtocol() {
        return strProtocol;
    }

    public String getAddress() {
        return strAddress;
    }

    public String getPort() {
        return strPort;
    }

    public int getPriority() {
        return nPriority;
    }

    public int getWeight() {
        return nWeight;
    }

    public int getIndex() {
        return nIdenx;
    }

    public int getMax() {
        return nMax;
    }

    @Override
    public String toString() {
        return "Protocol : "+strProtocol +
                "/Addr :" + strAddress +
                "/Port" + strPort +
                "/Priorty :" + nPriority +
                "/Weight" + nWeight +
                "/Index " + nIdenx +
                "/MAx" +nMax;
    }
}
