/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20111015  hwangoo.park@             Created
    </table>

    Description
*/

#ifndef _SIP_WAKE_LOCK_H_
#define _SIP_WAKE_LOCK_H_

#include "SipMethod.h"

class SipWakeLock
{
private:
    SipWakeLock();

private:
    SipWakeLock(IN CONST SipWakeLock& objRHS);
    SipWakeLock& operator=(IN CONST SipWakeLock& objRHS);

public:
    static void Acquire(IN CONST SipMethod& objMethod, IN IMS_UINT32 nTimeout = 0);
    static IMS_BOOL IsSupported();
    static void Release();
};

#endif  // _SIP_WAKE_LOCK_H_
