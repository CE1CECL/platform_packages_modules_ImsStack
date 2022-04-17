package com.android.imsstack.system;

public interface ISystemAPISIM {

    String getISIMState4Sys();
    int readISIMFileAttributes4Sys(int field);
    int readISIMRecord4Sys(int field, int index);
    int requestISIMAuthentication4Sys(String nonce, long owner);
    int requestUSIMAuthentication4Sys(String nonce, long owner);
}
