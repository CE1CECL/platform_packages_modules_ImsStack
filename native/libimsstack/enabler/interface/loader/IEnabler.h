/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170221  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_ENABLER_H_
#define _INTERFACE_ENABLER_H_

#include "IMSList.h"

class IEnabler
{
public:
    /*
     Starts an enabler.

    Remarks
     It's called in its own thread execution flow.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void Start() = 0;

    /*
     Stops an enabler.

    Remarks
     It's called in its own thread execution flow.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void Stop() = 0;
};

#endif // _INTERFACE_ENABLER_H_
