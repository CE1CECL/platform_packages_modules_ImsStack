/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20090715  toastops@                 Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "SIPError.h"
#include "SIPPrivate.h"



/*
 Returns an SIP last error code

Remarks

*/
PUBLIC GLOBAL
IMS_SINT32 SIPError::GetLastError()
{
    //---------------------------------------------------------------------------------------------

    return SIPPrivate::GetLastError();
}
