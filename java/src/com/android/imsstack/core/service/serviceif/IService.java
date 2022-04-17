package com.android.imsstack.core.service.serviceif;

import android.content.Context;

public interface IService {

    boolean start(IVoLteService voLteService);
    void cleanup(Context context);
    void update(Context context);
}
