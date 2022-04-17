/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20180323  sukhwan.mun@              Created
    </table>

    Description

*/

#ifndef _INTERFACE_TRM_TIMER_H_
#define _INTERFACE_TRM_TIMER_H_

class ITRMTimerListener
{
public:
    /*
        Notifies the application that the service timer is expired.
    */
    virtual void ITRMTimer_Expired(IN IMS_UINT32 nSlotId, IN IMS_UINT32 nType) = 0;
};

class ITRMTimer
{
public:
    /*
    */
    virtual void SetListener(IN ITRMTimerListener *piListener) = 0;

    /*
    */
    virtual void Start() = 0;

    /*
    */
    virtual void Stop() = 0;
};

#endif // _INTERFACE_TRM_TIMER_H_
