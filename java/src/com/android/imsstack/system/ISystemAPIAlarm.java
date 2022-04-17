package com.android.imsstack.system;

public interface ISystemAPIAlarm {
    int startAlarm4Sys(long duration, long tid);
    int killAlarm4Sys(long tid);
}
