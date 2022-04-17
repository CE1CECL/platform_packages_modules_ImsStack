package com.android.imsstack;

import android.content.Intent;

public interface IStateInfoChangedObserver {

    void notifyStateInfoChanged(Intent intent);
}
