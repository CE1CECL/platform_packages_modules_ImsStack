/*
    Author
    <table>
    Date        Author                        Description
    --------      -----------------            ------------------
    20111203    hwangoo.park@               Created

    </table>

    Description

*/

#ifndef _IU_IMS_H_
#define _IU_IMS_H_

class IUIMS
{
public:
    // IMS application identifiers
    enum
    {
        APP_BASE = 0,

        APP_MTC = 1,
        APP_MTS = 2,
        APP_UCE = 3,
        APP_SIP_DELEGATE = 4,

        APP_MAX = 5,
        APP_UNKNOWN = APP_MAX + 1
    };

    // System interface
    enum
    {
        // FIXME: Is it required to define? how about singleton for system interface....?
        // getSystemInterface() instead of getInterface(...).
        // (releaseInterface can be shared between System and UI interfaces)
        // The assigned number may be changed later...
        //
        // This is reserved number for system interface;
        // Refer to SystemConstants.java / SystemConstants.h
        SYSTEM_INTERFACE = 51
    };

    // IMS_SERVICE_AOS
    enum
    {
        AOS_SERVICE = 61
    };

    // IMS_CALL_MTC
    enum
    {
        MTC_CALL = 91,

    };

    // SMS specific application identifiers
    enum
    {
        MTS_BASE = 400,

        // SMS - Service
        MTS_EMERGENCY_SERVICE = (MTS_BASE + 1),
    };


    //// Bitmask to composite each services
    enum
    {
        M_APP_UC = 0x00000001,
        M_APP_SMS = 0x00000002,
        M_APP_VT = 0x00000004,
        M_SERVICE_ALL = 0xFFFFFFFF
    };
};
#endif // _IU_IMS_H_
