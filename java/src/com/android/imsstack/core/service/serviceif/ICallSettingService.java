package com.android.imsstack.core.service.serviceif;

import com.android.imsstack.core.agents.agentif.IAgent;
import com.android.imsstack.core.agents.agentif.ICallSetting;
import com.android.imsstack.system.ISystemAPIWifiCalling;

public interface ICallSettingService extends IService, ICallSetting, ISystemAPIWifiCalling, IAgent {

}
