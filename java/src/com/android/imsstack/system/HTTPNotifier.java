package com.android.imsstack.system;

public interface HTTPNotifier {

    public void updateStatusOnRequested(long connectionId);

    public void updateStatusOnProgress(long connectionId, long progress);

    public void updateStatusOnCompleted(long connectionId, int responseCode);

    public void updateStatusOnResult(long connectionId, String result);

    public void updateStatusOnAborted(final long connectionId, final int errorCode);

    public void updateResponseHeader(long connectionId, String header, String value);

    public void setWwwAuthHeader(String wwwAuth, long connectionId);

}
