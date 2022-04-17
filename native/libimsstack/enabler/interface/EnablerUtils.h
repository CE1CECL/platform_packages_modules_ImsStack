/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170515  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _ENABLER_UTILS_H_
#define _ENABLER_UTILS_H_

#include "AString.h"

class EnablerUtils
{
public:
    /** Returns the prefix of enabler thread name */
    static const IMS_CHAR* GetPrefixForEnablerThread();
    /** Forms the enabler's thread name with the specified slot id */
    static AString GetEnablerThreadName(IN IMS_SINT32 nSlotId);
};

#endif // _ENABLER_UTILS_H_
