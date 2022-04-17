/**
 * MsgProcInterface
 *     Role
 *     MsgProcInterface is a interface to handler message through hashmap.
 *  Switch case statement increases Cyclometic Complexity,
 *  so if case statements exists over then 9 cases to handle,
 *  use hashmap & this interface to decrease complexity.
 */

package com.android.imsstack.core.agents.agentif;

import android.os.Message;

public interface MsgProcInterface {

    public void procMsg(Message msg);

}
